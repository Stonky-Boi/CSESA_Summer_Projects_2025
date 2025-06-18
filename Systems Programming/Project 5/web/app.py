from flask import Flask, render_template, request, jsonify
import subprocess
import os
import tempfile
import argparse
import threading
import webbrowser
import time

app = Flask(__name__)

class EnhancedMIPSSimulatorWrapper:
    def __init__(self):
        script_dir = os.path.dirname(os.path.abspath(__file__))
        self.simulator_path = os.path.abspath(os.path.join(script_dir, '..', 'build', 'mips_simulator'))
        self.temp_dir = tempfile.mkdtemp()
        print(f"Using simulator at: {self.simulator_path}")
    
    def run_simulator(self, program, mode="step", pipeline=False, branch_prediction=False, pc=None):
        """Run the MIPS simulator with enhanced parameters"""
        try:
            # Create temporary program file
            program_file = os.path.join(self.temp_dir, "program.txt")
            with open(program_file, 'w') as f:
                f.write(program)
            
            # Build command with enhanced options
            cmd = [self.simulator_path, program_file]
            if mode == "run":
                cmd.append("--run")
            elif mode == "step":
                cmd.append("--step")
            elif mode == "assemble":
                cmd.append("--assemble-only")
            
            if pipeline:
                cmd.append("--pipeline")
            if branch_prediction:
                cmd.append("--branch-pred")
                cmd.append("--pred-type")
                cmd.append("2bit")
            
            if pc is not None:
                cmd.append("--pc")
                cmd.append(str(pc))
            
            # Execute simulator
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                # Parse enhanced output
                output_data = self.parse_simulator_output(result.stdout)
                return {
                    'success': True,
                    'output': result.stdout,
                    'error': None,
                    **output_data
                }
            else:
                return {
                    'success': False,
                    'output': None,
                    'error': result.stderr
                }
                
        except subprocess.TimeoutExpired:
            return {
                'success': False,
                'output': None,
                'error': "Simulation timed out"
            }
        except Exception as e:
            return {
                'success': False,
                'output': None,
                'error': str(e)
            }
    
    def parse_simulator_output(self, output):
        """Parse enhanced simulator output to extract structured data"""
        data = {
            'registers': [0] * 32,
            'pc': 0x00400000,
            'cycles': 0,
            'instructions': [],
            'pipeline': None,
            'branchStats': None,
            'hazards': [],
            'stalls': 0
        }
        
        lines = output.split('\n')
        current_section = None
        
        for line in lines:
            line = line.strip()
            if not line:
                continue
                
            # Parse different sections of output
            if 'Register File:' in line:
                current_section = 'registers'
            elif 'Pipeline State:' in line:
                current_section = 'pipeline'
            elif 'Branch Prediction Statistics:' in line:
                current_section = 'branch_stats'
            elif 'PC:' in line:
                try:
                    pc_value = line.split(':')[1].strip()
                    data['pc'] = int(pc_value, 16) if pc_value.startswith('0x') else int(pc_value)
                except:
                    pass
            elif current_section == 'registers' and '$' in line and ':' in line:
                try:
                    # Parse register line: $t0: 0x12345678
                    parts = line.split(':')
                    if len(parts) == 2:
                        reg_name = parts[0].strip()
                        reg_value = parts[1].strip()
                        if reg_value.startswith('0x'):
                            value = int(reg_value, 16)
                        else:
                            value = int(reg_value)
                        
                        # Map register name to index
                        reg_index = self.get_register_index(reg_name)
                        if reg_index is not None:
                            data['registers'][reg_index] = value
                except:
                    pass
        
        return data
    
    def get_register_index(self, reg_name):
        """Map register name to index"""
        reg_map = {
            '$zero': 0, '$at': 1, '$v0': 2, '$v1': 3,
            '$a0': 4, '$a1': 5, '$a2': 6, '$a3': 7,
            '$t0': 8, '$t1': 9, '$t2': 10, '$t3': 11,
            '$t4': 12, '$t5': 13, '$t6': 14, '$t7': 15,
            '$s0': 16, '$s1': 17, '$s2': 18, '$s3': 19,
            '$s4': 20, '$s5': 21, '$s6': 22, '$s7': 23,
            '$t8': 24, '$t9': 25, '$k0': 26, '$k1': 27,
            '$gp': 28, '$sp': 29, '$fp': 30, '$ra': 31
        }
        return reg_map.get(reg_name)

simulator = EnhancedMIPSSimulatorWrapper()

@app.route('/')
def index():
    """Enhanced main simulator interface"""
    return render_template('index.html')

@app.route('/api/simulate', methods=['POST'])
def simulate():
    """Enhanced API endpoint for running simulation"""
    data = request.get_json()
    
    program = data.get('program', '')
    mode = data.get('mode', 'step')
    pipeline = data.get('pipeline', False)
    branch_prediction = data.get('branch_prediction', False)
    pc = data.get('pc', None)
    
    if not program:
        return jsonify({
            'success': False,
            'error': 'No program provided'
        })
    
    result = simulator.run_simulator(program, mode, pipeline, branch_prediction, pc)
    return jsonify(result)

@app.route('/api/examples')
def examples():
    """Enhanced example MIPS programs with pipeline and branch prediction features"""
    examples = {
        'simple_add': {
            'name': 'Simple Addition',
            'description': 'Basic arithmetic operations demonstrating register usage',
            'code': '''# Simple addition program
# Demonstrates basic arithmetic and register manipulation
addi $t0, $zero, 15    # Load 15 into $t0
addi $t1, $zero, 25    # Load 25 into $t1
add  $t2, $t0, $t1     # Add $t0 and $t1, store in $t2
sub  $t3, $t2, $t0     # Subtract $t0 from $t2, store in $t3
'''
        },
        'pipeline_hazards': {
            'name': 'Pipeline Hazards Demo',
            'description': 'Demonstrates data hazards and pipeline stalls',
            'code': '''# Pipeline hazards demonstration
# Shows RAW (Read After Write) hazards
addi $t0, $zero, 10    # Load 10 into $t0
add  $t1, $t0, $t0     # RAW hazard: depends on previous instruction
addi $t2, $t1, 5       # RAW hazard: depends on previous instruction
sub  $t3, $t2, $t0     # Multiple dependencies
'''
        },
        'branch_prediction': {
            'name': 'Branch Prediction Test',
            'description': 'Loop demonstrating branch prediction behavior',
            'code': '''# Branch prediction demonstration
# Simple counting loop to test predictor accuracy
addi $t0, $zero, 0     # Initialize counter
addi $t1, $zero, 10    # Set loop limit
loop:
    addi $t0, $t0, 1   # Increment counter
    bne  $t0, $t1, loop # Branch back if not equal
    # Loop exit
    add  $t2, $t0, $t1 # Final computation
'''
        },
        'memory_operations': {
            'name': 'Memory Operations',
            'description': 'Load and store operations with memory visualization',
            'code': '''# Memory operations demonstration
# Shows interaction between registers and memory
addi $t0, $zero, 100   # Load immediate value
sw   $t0, 0($zero)     # Store word to memory address 0
lw   $t1, 0($zero)     # Load word from memory address 0
addi $t2, $t1, 50      # Add to loaded value
sw   $t2, 4($zero)     # Store to next memory location
'''
        },
        'fibonacci': {
            'name': 'Fibonacci Sequence',
            'description': 'Advanced program showing loops and calculations',
            'code': '''# Fibonacci sequence generator
# Demonstrates complex control flow and arithmetic
addi $t0, $zero, 0     # F(0) = 0
addi $t1, $zero, 1     # F(1) = 1
addi $t2, $zero, 8     # Calculate 8 Fibonacci numbers
addi $t3, $zero, 2     # Loop counter starts at 2
fib_loop:
    add  $t4, $t0, $t1 # F(n) = F(n-1) + F(n-2)
    add  $t0, $t1, $zero # F(n-2) = F(n-1)
    add  $t1, $t4, $zero # F(n-1) = F(n)
    addi $t3, $t3, 1   # Increment counter
    bne  $t3, $t2, fib_loop # Continue if not done
    # Result in $t1
'''
        },
        'cache_test': {
            'name': 'Cache Performance Test',
            'description': 'Memory access patterns for cache analysis',
            'code': '''# Cache performance demonstration
# Different memory access patterns
addi $t0, $zero, 0     # Base address
addi $t1, $zero, 16    # Loop counter
sequential_loop:
    sw   $t1, 0($t0)   # Sequential access
    addi $t0, $t0, 4   # Next word
    addi $t1, $t1, -1  # Decrement counter
    bne  $t1, $zero, sequential_loop
    
    # Strided access pattern
    addi $t0, $zero, 0 # Reset base
    addi $t1, $zero, 8 # New counter
stride_loop:
    lw   $t2, 0($t0)   # Load with stride
    addi $t0, $t0, 16  # Skip cache lines
    addi $t1, $t1, -1  # Decrement
    bne  $t1, $zero, stride_loop
'''
        }
    }
    return jsonify(examples)

@app.route('/api/performance')
def performance_metrics():
    """API endpoint for detailed performance analysis"""
    # This would integrate with the simulator to provide real-time metrics
    metrics = {
        'cpi': 1.2,
        'ipc': 0.83,
        'branch_accuracy': 89.5,
        'cache_hit_rate': 95.2,
        'pipeline_stalls': 15,
        'hazard_breakdown': {
            'data_hazards': 8,
            'control_hazards': 5,
            'structural_hazards': 2
        }
    }
    return jsonify(metrics)

@app.route('/documentation')
def documentation():
    """Enhanced MIPS instruction documentation"""
    return render_template('documentation.html')

def open_browser(port):
    """Open browser after a short delay"""
    time.sleep(1.5)
    webbrowser.open(f'http://localhost:{port}')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Enhanced MARS MIPS Simulator Flask Server")
    parser.add_argument('-p', '--port', type=int, default=5000, help='Port to run the server on')
    parser.add_argument('--host', type=str, default='0.0.0.0', help='Host to run the server on')
    parser.add_argument('--debug', action='store_true', help='Enable debug mode')
    args = parser.parse_args()

    print(f"Starting Enhanced MARS Simulator on http://localhost:{args.port}")
    print("Features enabled: 5-stage pipeline, branch prediction, hazard detection")
    
    app.run(debug=args.debug, host=args.host, port=args.port)
