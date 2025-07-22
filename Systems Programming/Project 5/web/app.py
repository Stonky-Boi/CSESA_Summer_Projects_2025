import dash
from dash import dcc, html, Input, Output, State, no_update
import dash_bootstrap_components as dbc
import subprocess
import json
import os
import pandas as pd

# Path to the C++ executable
# Assumes the executable is in the same directory as this script, as per CMakeLists.txt
EXECUTABLE_PATH = os.path.join(os.path.dirname(__file__), 'mips_simulator')

# --- Dash App Initialization ---
app = dash.Dash(__name__, external_stylesheets=[dbc.themes.CYBORG], suppress_callback_exceptions=True)
app.title = "MIPS Simulator"
server = app.server

# --- Helper Functions ---
def run_simulator(code, mode, state=None):
    """Runs the C++ simulator and returns the JSON output."""
    temp_asm_file = "temp.asm"
    with open(temp_asm_file, "w") as f:
        f.write(code)

    command = [EXECUTABLE_PATH, temp_asm_file, mode]
    
    try:
        result = subprocess.run(command, capture_output=True, text=True, check=True, timeout=5)
        os.remove(temp_asm_file)
        return json.loads(result.stdout)
    except subprocess.CalledProcessError as e:
        os.remove(temp_asm_file)
        try:
            error_json = json.loads(e.stderr)
            return {"error": f"Simulator Error: {error_json.get('error', 'Unknown')}"}
        except json.JSONDecodeError:
            return {"error": f"Simulator execution failed: {e.stderr}"}
    except FileNotFoundError:
        return {"error": f"Simulator executable not found at '{EXECUTABLE_PATH}'. Please build the C++ project first."}
    except Exception as e:
        if os.path.exists(temp_asm_file):
            os.remove(temp_asm_file)
        return {"error": f"An unexpected error occurred: {str(e)}"}


def create_register_table(register_data):
    """Creates a Dash DataTable from register data."""
    if not register_data:
        return dbc.Table()
        
    # Split registers into two columns
    reg_names = list(register_data.keys())
    reg_values = list(register_data.values())
    
    df = pd.DataFrame({
        'Reg': reg_names[:16],
        'Value': reg_values[:16],
        'Reg_2': reg_names[16:],
        'Value_2': reg_values[16:]
    })

    table_header = [
        html.Thead(html.Tr([html.Th("Register"), html.Th("Value"), html.Th("Register"), html.Th("Value")]))
    ]
    table_body = [html.Tbody(
        [html.Tr([
            html.Td(df.iloc[i]['Reg']), html.Td(f"0x{df.iloc[i]['Value']:08x}"),
            html.Td(df.iloc[i]['Reg_2']), html.Td(f"0x{df.iloc[i]['Value_2']:08x}")
         ]) for i in range(16)]
    )]
    
    return dbc.Table(table_header + table_body, bordered=True, striped=True, hover=True, size='sm')

# --- App Layout ---
initial_code = """.data
    myMessage: .asciiz "Hello World!\\n"
    anotherMessage: .asciiz "MIPS simulation complete.\\n"

.text
main:
    # Print Hello World
    li $v0, 4
    la $a0, myMessage
    syscall
    
    # Simple calculation
    li $t0, 5
    li $t1, 10
    add $t2, $t0, $t1 # t2 should be 15
    
    # Print completion message
    li $v0, 4
    la $a0, anotherMessage
    syscall
    
    # Exit
    li $v0, 10
    syscall
"""

app.layout = dbc.Container(fluid=True, children=[
    dbc.Row(dbc.Col(html.H1("MIPS Web Simulator", className="text-center my-4"))),
    
    dbc.Row([
        # Left Column: Code Editor and Controls
        dbc.Col(md=6, children=[
            dbc.Card([
                dbc.CardHeader("Assembly Code"),
                dbc.CardBody([
                    dcc.Textarea(
                        id='code-input',
                        value=initial_code,
                        style={'width': '100%', 'height': '400px', 'fontFamily': 'monospace'},
                        className="bg-dark text-light"
                    ),
                    dbc.Row([
                        dbc.Col(dbc.Button("Run", id="run-button", color="success", className="w-100")),
                        dbc.Col(dbc.Button("Step", id="step-button", color="primary", className="w-100", disabled=True)),
                        dbc.Col(dbc.Button("Reset", id="reset-button", color="warning", className="w-100")),
                    ], className="mt-3")
                ])
            ]),
            dbc.Card(className="mt-4", children=[
                dbc.CardHeader("Console Output"),
                dbc.CardBody(
                    html.Pre(id='console-output', style={'whiteSpace': 'pre-wrap', 'wordBreak': 'break-all', 'height': '150px', 'overflowY': 'auto'}),
                )
            ]),
        ]),
        
        # Right Column: Simulator State
        dbc.Col(md=6, children=[
            dbc.Card([
                dbc.CardHeader("CPU State"),
                dbc.CardBody([
                    html.H5("Program Counter (PC)"),
                    html.Pre(id='pc-output', children="0x00400000"),
                    html.Hr(),
                    html.H5("Registers"),
                    html.Div(id='register-output')
                ])
            ]),
            dbc.Card(className="mt-4", children=[
                dbc.CardHeader("Memory (.data segment)"),
                dbc.CardBody(
                    html.Pre(id='memory-output', style={'whiteSpace': 'pre-wrap', 'wordBreak': 'break-all', 'height': '150px', 'overflowY': 'auto'}),
                )
            ])
        ])
    ]),
    
    # Hidden storage for simulator state
    dcc.Store(id='simulator-state-storage'),
    dcc.Store(id='program-counter-storage', data=0)
])

# --- Callbacks ---
@app.callback(
    [Output('pc-output', 'children'),
     Output('register-output', 'children'),
     Output('console-output', 'children'),
     Output('memory-output', 'children'),
     Output('run-button', 'disabled'),
     Output('step-button', 'disabled')],
    [Input('run-button', 'n_clicks'),
     Input('step-button', 'n_clicks'),
     Input('reset-button', 'n_clicks')],
    [State('code-input', 'value'),
     State('program-counter-storage', 'data')]
)
def handle_execution(run_clicks, step_clicks, reset_clicks, code, current_pc):
    ctx = dash.callback_context
    if not ctx.triggered:
        # Initial load
        return "0x00000000", create_register_table(None), "", "", False, True

    button_id = ctx.triggered[0]['prop_id'].split('.')[0]
    
    # --- Reset Button ---
    if button_id == 'reset-button':
        return "0x00000000", create_register_table(None), "", "", False, True

    # --- Run Button ---
    if button_id == 'run-button':
        sim_state = run_simulator(code, "--run")
        if 'error' in sim_state:
            return no_update, no_update, sim_state['error'], no_update, False, True
        
        reg_table = create_register_table(sim_state.get('registers', {}))
        mem_hex = sim_state.get('memory', {}).get('data_segment_hex', '')
        # Format memory hex string
        mem_display = ' '.join(mem_hex[i:i+2] for i in range(0, len(mem_hex), 2))
        mem_display = '\n'.join(mem_display[i:i+48] for i in range(0, len(mem_display), 48))

        return (f"0x{sim_state.get('pc', 0):08x}",
                reg_table,
                sim_state.get('console_output', ''),
                mem_display,
                True, # Disable run
                True) # Disable step

    # --- Step Button (Simplified) ---
    # A true step-by-step requires passing state back and forth, which is complex.
    # This is a placeholder showing the concept. For now, we will just run the
    # whole thing and disable the button as it is not fully implemented.
    if button_id == 'step-button':
        # This functionality requires a more complex C++ backend that can accept
        # a state, execute one instruction, and return the new state.
        # The current implementation is a placeholder for this logic.
        return no_update, no_update, "Step-by-step execution is not yet fully implemented.", no_update, False, True

    return no_update

if __name__ == '__main__':
    # Check for executable
    if not os.path.exists(EXECUTABLE_PATH):
        print("="*50)
        print("ERROR: MIPS simulator executable not found!")
        print(f"Looked for: '{EXECUTABLE_PATH}'")
        print("Please compile the C++ project in the root directory first:")
        print("  mkdir build && cd build && cmake .. && make")
        print("="*50)
    else:
        app.run_server(debug=True)