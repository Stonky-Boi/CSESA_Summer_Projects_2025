import os
import subprocess         # Python subprocess module needed [4]
import argparse
import tempfile
import threading
import webbrowser
import time
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

class EnhancedMIPSSimulatorWrapper:
    def __init__(self):
        base = os.path.dirname(os.path.abspath(__file__))
        self.simulator_path = os.path.abspath(os.path.join(base, '..', 'build', 'mips_simulator'))
        self.temp_dir = tempfile.mkdtemp()

    def run_simulator(self, program, mode="step", pipeline=False, branch_prediction=False, pc=None):
        program_file = os.path.join(self.temp_dir, "program.txt")
        with open(program_file, 'w') as f:
            f.write(program)

        cmd = [self.simulator_path, program_file]
        if mode == "run":      cmd.append("--run")
        elif mode == "step":   cmd.append("--step")
        # assemble-only replaced by --step to validate without full run
        if pipeline:           cmd.append("--pipeline")
        if branch_prediction:  cmd.append("--branch-pred")

        if pc is not None:
            cmd += ["--pc", str(pc)]

        result = subprocess.run(cmd, capture_output=True, text=True)
        return {'success': result.returncode == 0,
                'output': result.stdout if result.returncode == 0 else None,
                'error': result.stderr if result.returncode != 0 else None}

simulator = EnhancedMIPSSimulatorWrapper()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/simulate', methods=['POST'])
def simulate():
    data = request.get_json()
    out = simulator.run_simulator(data['program'],
                                  mode=data.get('mode','step'),
                                  pipeline=data.get('pipeline',False),
                                  branch_prediction=data.get('branch_prediction',False),
                                  pc=data.get('pc', None))
    return jsonify(out)

@app.route('/api/examples')
def examples():
    # ... same as before ...
    return jsonify({})

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-p','--port', type=int, default=5000)
    parser.add_argument('--host', default='0.0.0.0')
    args = parser.parse_args()

    threading.Thread(target=lambda: (time.sleep(1), webbrowser.open(f'http://localhost:{args.port}'))).start()
    app.run(host=args.host, port=args.port, debug=True)
