from flask import Flask, render_template, request, jsonify
import subprocess
import json
import os
import tempfile

app = Flask(__name__)

class MIPSSimulatorWrapper:
    def __init__(self):
        self.simulator_path = "../build/mips_simulator"
        self.temp_dir = tempfile.mkdtemp()
    
    def run_simulator(self, program, mode="step", pipeline=False, branch_prediction=False):
        """Run the MIPS simulator with given parameters"""
        try:
            # Create temporary program file
            program_file = os.path.join(self.temp_dir, "program.txt")
            with open(program_file, 'w') as f:
                f.write(program)
            
            # Build command
            cmd = [self.simulator_path, program_file]
            if mode == "run":
                cmd.append("--run")
            if pipeline:
                cmd.append("--pipeline")
            if branch_prediction:
                cmd.append("--branch-prediction")
            
            # Execute simulator
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                return {
                    'success': True,
                    'output': result.stdout,
                    'error': None
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

simulator = MIPSSimulatorWrapper()

@app.route('/')
def index():
    """Main simulator interface"""
    return render_template('index.html')

@app.route('/api/simulate', methods=['POST'])
def simulate():
    """API endpoint for running simulation"""
    data = request.get_json()
    
    program = data.get('program', '')
    mode = data.get('mode', 'step')
    pipeline = data.get('pipeline', False)
    branch_prediction = data.get('branch_prediction', False)
    
    if not program:
        return jsonify({
            'success': False,
            'error': 'No program provided'
        })
    
    result = simulator.run_simulator(program, mode, pipeline, branch_prediction)
    return jsonify(result)

@app.route('/api/examples')
def examples():
    """Get example MIPS programs"""
    examples = {
        'simple_add': {
            'name': 'Simple Addition',
            'description': 'Basic addition operation',
            'code': '''# Simple addition example
20020005  # addi $v0, $zero, 5
20030003  # addi $v1, $zero, 3
00622020  # add $a0, $v1, $v0
'''
        },
        'loop': {
            'name': 'Simple Loop',
            'description': 'Loop with branch instruction',
            'code': '''# Simple loop example
20020000  # addi $v0, $zero, 0
2003000A  # addi $v1, $zero, 10
20420001  # addi $v0, $v0, 1
1443FFFD  # bne $v0, $v1, -3
'''
        },
        'memory': {
            'name': 'Memory Operations',
            'description': 'Load and store operations',
            'code': '''# Memory operations example
20020064  # addi $v0, $zero, 100
AC020000  # sw $v0, 0($zero)
8C030000  # lw $v1, 0($zero)
'''
        }
    }
    return jsonify(examples)

@app.route('/documentation')
def documentation():
    """MIPS instruction documentation"""
    return render_template('documentation.html')

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5500)
