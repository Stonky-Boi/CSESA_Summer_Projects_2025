from flask import Flask, render_template, request, jsonify, session
import subprocess
import json
import os
import tempfile
from werkzeug.utils import secure_filename

app = Flask(__name__)
app.secret_key = 'mips_simulator_secret_key'

class MIPSSimulatorInterface:
    def __init__(self):
        self.simulator_path = './mips_simulator'
        self.temp_dir = tempfile.mkdtemp()
        
    def compile_simulator(self):
        """Compile the C++ simulator using CMake"""
        try:
            subprocess.run(['cmake', '.'], check=True, capture_output=True, text=True)
            subprocess.run(['make'], check=True, capture_output=True, text=True)
            return True
        except subprocess.CalledProcessError as e:
            print(f"Compilation error: {e}")
            return False
            
    def run_simulator(self, assembly_code, config):
        """Run the MIPS simulator with given assembly code and configuration"""
        
        # Create temporary assembly file
        asm_file = os.path.join(self.temp_dir, 'program.asm')
        with open(asm_file, 'w') as f:
            f.write(assembly_code)
        
        # Create configuration file
        config_file = os.path.join(self.temp_dir, 'config.json')
        with open(config_file, 'w') as f:
            json.dump(config, f)
        
        try:
            # Run simulator
            result = subprocess.run([
                self.simulator_path,
                '--input', asm_file,
                '--config', config_file,
                '--output-format', 'json'
            ], capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                return json.loads(result.stdout)
            else:
                return {'error': result.stderr}
                
        except subprocess.TimeoutExpired:
            return {'error': 'Simulation timeout'}
        except Exception as e:
            return {'error': str(e)}

simulator = MIPSSimulatorInterface()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/simulator')
def simulator_page():
    return render_template('simulator.html')

@app.route('/api/simulate', methods=['POST'])
def simulate():
    data = request.json
    
    assembly_code = data.get('assembly', '')
    config = {
        'pipeline_enabled': data.get('pipeline_enabled', True),
        'step_mode': data.get('step_mode', False),
        'branch_predictor': data.get('branch_predictor', 'static_not_taken'),
        'predictor_params': data.get('predictor_params', {}),
        'show_pipeline': data.get('show_pipeline', True),
        'show_registers': data.get('show_registers', True),
        'show_memory': data.get('show_memory', True),
        'memory_start': data.get('memory_start', '0x400000'),
        'memory_length': data.get('memory_length', 64)
    }
    
    result = simulator.run_simulator(assembly_code, config)
    return jsonify(result)

@app.route('/api/examples')
def get_examples():
    examples = {
        'basic_arithmetic': {
            'name': 'Basic Arithmetic',
            'description': 'Simple arithmetic operations',
            'code': '''# Basic arithmetic example
addi $t0, $zero, 10
addi $t1, $zero, 20
add $t2, $t0, $t1
sub $t3, $t1, $t0
'''
        },
        'loop_example': {
            'name': 'Loop Example',
            'description': 'Simple loop with branch',
            'code': '''# Loop example
addi $t0, $zero, 0    # counter
addi $t1, $zero, 10   # limit

loop:
    addi $t0, $t0, 1  # increment counter
    bne $t0, $t1, loop # branch if not equal
    
# end of program
'''
        }
    }
    return jsonify(examples)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)