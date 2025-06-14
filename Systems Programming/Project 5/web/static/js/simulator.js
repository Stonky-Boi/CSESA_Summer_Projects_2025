class MIPSSimulatorUI {
    constructor() {
        this.currentProgram = '';
        this.isRunning = false;
        this.cycleCount = 0;
        this.initializeEventListeners();
        this.initializeRegisterTable();
    }

    initializeEventListeners() {
        document.getElementById('stepBtn').addEventListener('click', () => this.step());
        document.getElementById('runBtn').addEventListener('click', () => this.run());
        document.getElementById('resetBtn').addEventListener('click', () => this.reset());
        document.getElementById('loadExampleBtn').addEventListener('click', () => this.loadExamples());
        
        document.getElementById('pipelineCheck').addEventListener('change', (e) => {
            document.getElementById('pipelineSection').style.display = 
                e.target.checked ? 'block' : 'none';
        });
        
        document.getElementById('branchPredictionCheck').addEventListener('change', (e) => {
            document.getElementById('branchStatsSection').style.display = 
                e.target.checked ? 'block' : 'none';
        });
    }

    initializeRegisterTable() {
        const registerNames = [
            '$zero', '$at', '$v0', '$v1', '$a0', '$a1', '$a2', '$a3',
            '$t0', '$t1', '$t2', '$t3', '$t4', '$t5', '$t6', '$t7',
            '$s0', '$s1', '$s2', '$s3', '$s4', '$s5', '$s6', '$s7',
            '$t8', '$t9', '$k0', '$k1', '$gp', '$sp', '$fp', '$ra'
        ];

        let tableHTML = '<table class="table table-sm register-table"><tbody>';
        for (let i = 0; i < 32; i += 4) {
            tableHTML += '<tr>';
            for (let j = 0; j < 4; j++) {
                const regNum = i + j;
                tableHTML += `<td><strong>${registerNames[regNum]}:</strong></td>`;
                tableHTML += `<td><span id="reg${regNum}">0x00000000</span></td>`;
            }
            tableHTML += '</tr>';
        }
        tableHTML += '</tbody></table>';
        document.getElementById('registerTable').innerHTML = tableHTML;
    }

    async step() {
        if (this.isRunning) return;
        
        const program = document.getElementById('codeEditor').value;
        const pipeline = document.getElementById('pipelineCheck').checked;
        const branchPrediction = document.getElementById('branchPredictionCheck').checked;
        
        if (!program.trim()) {
            this.showError('Please enter a MIPS program');
            return;
        }
        
        this.setLoading(true);
        
        try {
            const response = await fetch('/api/simulate', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    program: program,
                    mode: 'step',
                    pipeline: pipeline,
                    branch_prediction: branchPrediction
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.updateSimulationOutput(result.output);
                this.cycleCount++;
                document.getElementById('cycleCount').textContent = this.cycleCount;
            } else {
                this.showError(result.error);
            }
        } catch (error) {
            this.showError('Network error: ' + error.message);
        } finally {
            this.setLoading(false);
        }
    }

    async run() {
        if (this.isRunning) return;
        
        const program = document.getElementById('codeEditor').value;
        const pipeline = document.getElementById('pipelineCheck').checked;
        const branchPrediction = document.getElementById('branchPredictionCheck').checked;
        
        if (!program.trim()) {
            this.showError('Please enter a MIPS program');
            return;
        }
        
        this.isRunning = true;
        this.setLoading(true);
        document.getElementById('runBtn').innerHTML = '<i class="fas fa-spinner fa-spin"></i> Running...';
        
        try {
            const response = await fetch('/api/simulate', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    program: program,
                    mode: 'run',
                    pipeline: pipeline,
                    branch_prediction: branchPrediction
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.updateSimulationOutput(result.output);
                document.getElementById('statusValue').textContent = 'Completed';
            } else {
                this.showError(result.error);
            }
        } catch (error) {
            this.showError('Network error: ' + error.message);
        } finally {
            this.isRunning = false;
            this.setLoading(false);
            document.getElementById('runBtn').innerHTML = '<i class="fas fa-play"></i> Run';
        }
    }

    reset() {
        this.cycleCount = 0;
        document.getElementById('cycleCount').textContent = '0';
        document.getElementById('pcValue').textContent = '0x00000000';
        document.getElementById('statusValue').textContent = 'Ready';
        document.getElementById('outputPanel').innerHTML = 'Simulator reset. Ready for new program.';
        
        // Reset all registers to zero
        for (let i = 0; i < 32; i++) {
            const regElement = document.getElementById(`reg${i}`);
            if (regElement) {
                regElement.textContent = '0x00000000';
            }
        }
    }

    async loadExamples() {
        try {
            const response = await fetch('/api/examples');
            const examples = await response.json();
            
            let examplesHTML = '';
            for (const [key, example] of Object.entries(examples)) {
                examplesHTML += `
                    <div class="card mb-3">
                        <div class="card-header">
                            <h6>${example.name}</h6>
                            <small class="text-muted">${example.description}</small>
                        </div>
                        <div class="card-body">
                            <pre class="code-preview">${example.code}</pre>
                            <button class="btn btn-primary btn-sm" onclick="simulator.loadExample('${key}')">
                                Load Example
                            </button>
                        </div>
                    </div>
                `;
            }
            
            document.getElementById('examplesList').innerHTML = examplesHTML;
            new bootstrap.Modal(document.getElementById('examplesModal')).show();
        } catch (error) {
            this.showError('Failed to load examples: ' + error.message);
        }
    }

    async loadExample(key) {
        try {
            const response = await fetch('/api/examples');
            const examples = await response.json();
            
            if (examples[key]) {
                document.getElementById('codeEditor').value = examples[key].code;
                bootstrap.Modal.getInstance(document.getElementById('examplesModal')).hide();
                this.reset();
            }
        } catch (error) {
            this.showError('Failed to load example: ' + error.message);
        }
    }

    updateSimulationOutput(output) {
        document.getElementById('outputPanel').innerHTML = this.formatOutput(output);
        
        // Parse output to update UI elements
        const lines = output.split('\n');
        for (const line of lines) {
            if (line.startsWith('PC:')) {
                const pc = line.split(':')[1].trim();
                document.getElementById('pcValue').textContent = pc;
            }
            
            // Update register values if present in output
            const regMatch = line.match(/\$(\d+):\s*(0x[0-9a-fA-F]+)/);
            if (regMatch) {
                const regNum = parseInt(regMatch[1]);
                const value = regMatch[2];
                const regElement = document.getElementById(`reg${regNum}`);
                if (regElement) {
                    regElement.textContent = value;
                }
            }
        }
    }

    formatOutput(output) {
        return output.replace(/\n/g, '<br>').replace(/\s/g, '&nbsp;');
    }

    showError(message) {
        document.getElementById('outputPanel').innerHTML = 
            `<span style="color: #ff6b6b;"><i class="fas fa-exclamation-triangle"></i> Error: ${message}</span>`;
    }

    setLoading(loading) {
        const buttons = ['stepBtn', 'runBtn', 'resetBtn'];
        buttons.forEach(btnId => {
            document.getElementById(btnId).disabled = loading;
        });
    }
}

// Initialize simulator when page loads
const simulator = new MIPSSimulatorUI();
