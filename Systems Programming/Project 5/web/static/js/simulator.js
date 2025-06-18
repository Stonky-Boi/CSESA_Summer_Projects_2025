class MARSSimulator {
    constructor() {
        this.currentProgram = '';
        this.isRunning = false;
        this.isAssembled = false;
        this.cycleCount = 0;
        this.registers = Array(32).fill(0);
        this.registerNames = [
            '$zero', '$at', '$v0', '$v1', '$a0', '$a1', '$a2', '$a3',
            '$t0', '$t1', '$t2', '$t3', '$t4', '$t5', '$t6', '$t7',
            '$s0', '$s1', '$s2', '$s3', '$s4', '$s5', '$s6', '$s7',
            '$t8', '$t9', '$k0', '$k1', '$gp', '$sp', '$fp', '$ra'
        ];
        this.pc = 0;
        this.memory = {};
        this.textSegment = [];
        this.dataSegment = [];
        this.stackSegment = [];
        this.kernelSegment = [];
        this.breakpoints = new Set();
        this.runSpeed = 20; // instructions per second
        this.initializeUI();
        this.initializeEventListeners();
        this.populateRegisterTable();
    }

    initializeUI() {
        // Initialize console tabs
        document.querySelectorAll('.mars-console-tab').forEach(tab => {
            tab.addEventListener('click', () => {
                document.querySelectorAll('.mars-console-tab').forEach(t => t.classList.remove('active'));
                tab.classList.add('active');
                document.querySelectorAll('.mars-console').forEach(c => c.style.display = 'none');
                document.getElementById(tab.dataset.target).style.display = 'block';
            });
        });

        // Initialize speed slider
        const speedSlider = document.getElementById('speedSlider');
        const speedValue = document.getElementById('speedValue');
        speedSlider.addEventListener('input', () => {
            this.runSpeed = parseInt(speedSlider.value);
            speedValue.textContent = `${this.runSpeed} inst/sec`;
        });

        // Initialize memory view
        document.getElementById('closeMemoryView').addEventListener('click', () => {
            document.getElementById('memoryView').style.display = 'none';
            document.getElementById('overlay').style.display = 'none';
        });
    }

    initializeEventListeners() {
        // File menu
        document.getElementById('newFile').addEventListener('click', () => this.newFile());
        document.getElementById('openFile').addEventListener('click', () => this.openFile());
        document.getElementById('saveFile').addEventListener('click', () => this.saveFile());
        document.getElementById('saveAsFile').addEventListener('click', () => this.saveAsFile());
        document.getElementById('exit').addEventListener('click', () => this.exit());
        
        // Toolbar buttons
        document.getElementById('newBtn').addEventListener('click', () => this.newFile());
        document.getElementById('openBtn').addEventListener('click', () => this.openFile());
        document.getElementById('saveBtn').addEventListener('click', () => this.saveFile());
        document.getElementById('assembleBtn').addEventListener('click', () => this.assemble());
        document.getElementById('runBtn').addEventListener('click', () => this.run());
        document.getElementById('stepBtn').addEventListener('click', () => this.step());
        document.getElementById('backstepBtn').addEventListener('click', () => this.backstep());
        document.getElementById('resetBtn').addEventListener('click', () => this.reset());
        document.getElementById('stopBtn').addEventListener('click', () => this.stop());
        
        // Run menu
        document.getElementById('assemble').addEventListener('click', () => this.assemble());
        document.getElementById('go').addEventListener('click', () => this.run());
        document.getElementById('step').addEventListener('click', () => this.step());
        document.getElementById('backstep').addEventListener('click', () => this.backstep());
        document.getElementById('reset').addEventListener('click', () => this.reset());
        document.getElementById('clearBreakpoints').addEventListener('click', () => this.clearBreakpoints());
        
        // Help menu
        document.getElementById('about').addEventListener('click', () => this.showAbout());
        
        // Editor events
        const codeEditor = document.getElementById('codeEditor');
        codeEditor.addEventListener('input', () => {
            this.currentProgram = codeEditor.value;
            this.isAssembled = false;
            this.updateStatusBar();
        });
        codeEditor.addEventListener('keydown', (e) => {
            if (e.key === 'Tab') {
                e.preventDefault();
                const start = codeEditor.selectionStart;
                const end = codeEditor.selectionEnd;
                codeEditor.value = codeEditor.value.substring(0, start) + '    ' + codeEditor.value.substring(end);
                codeEditor.selectionStart = codeEditor.selectionEnd = start + 4;
            }
        });
        
        // Load examples button (custom addition)
        document.getElementById('loadExampleBtn').addEventListener('click', () => this.loadExamples());
    }

    populateRegisterTable() {
        const registerTable = document.getElementById('registerTable');
        registerTable.innerHTML = '';
        
        for (let i = 0; i < 32; i++) {
            const row = document.createElement('tr');
            
            const nameCell = document.createElement('td');
            nameCell.textContent = this.registerNames[i];
            row.appendChild(nameCell);
            
            const numCell = document.createElement('td');
            numCell.textContent = i;
            row.appendChild(numCell);
            
            const valueCell = document.createElement('td');
            valueCell.className = 'register-value';
            valueCell.textContent = '0x' + this.registers[i].toString(16).padStart(8, '0');
            valueCell.contentEditable = true;
            valueCell.addEventListener('blur', () => {
                try {
                    const value = parseInt(valueCell.textContent.replace(/^0x/, ''), 16);
                    if (!isNaN(value)) {
                        this.registers[i] = value;
                        valueCell.textContent = '0x' + value.toString(16).padStart(8, '0');
                    } else {
                        valueCell.textContent = '0x' + this.registers[i].toString(16).padStart(8, '0');
                    }
                } catch (e) {
                    valueCell.textContent = '0x' + this.registers[i].toString(16).padStart(8, '0');
                }
            });
            row.appendChild(valueCell);
            
            registerTable.appendChild(row);
        }
    }

    updateRegisterTable() {
        const registerTable = document.getElementById('registerTable');
        const rows = registerTable.getElementsByTagName('tr');
        
        for (let i = 0; i < rows.length; i++) {
            const valueCell = rows[i].getElementsByClassName('register-value')[0];
            const oldValue = valueCell.textContent;
            const newValue = '0x' + this.registers[i].toString(16).padStart(8, '0');
            
            if (oldValue !== newValue) {
                valueCell.textContent = newValue;
                valueCell.classList.add('changed');
                setTimeout(() => {
                    valueCell.classList.remove('changed');
                }, 1000);
            }
        }
    }

    updateStatusBar() {
        const statusBar = document.querySelector('.mars-status-bar');
        const statusText = statusBar.firstElementChild;
        const cursorPos = statusBar.lastElementChild;
        
        if (this.isAssembled) {
            statusText.textContent = 'Assembled';
        } else {
            statusText.textContent = 'Ready';
        }
        
        const codeEditor = document.getElementById('codeEditor');
        const text = codeEditor.value;
        const cursorPosition = codeEditor.selectionStart;
        
        let line = 1;
        let column = 1;
        
        for (let i = 0; i < cursorPosition; i++) {
            if (text[i] === '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        }
        
        cursorPos.textContent = `Line: ${line}, Column: ${column}`;
    }

    newFile() {
        if (this.currentProgram && !confirm('Discard changes to current file?')) {
            return;
        }
        
        document.getElementById('codeEditor').value = '';
        this.currentProgram = '';
        this.isAssembled = false;
        this.updateStatusBar();
        
        // Update tab name
        const tab = document.querySelector('.mars-editor-tab');
        tab.textContent = 'untitled.asm';
    }

    openFile() {
        if (this.currentProgram && !confirm('Discard changes to current file?')) {
            return;
        }
        
        const input = document.createElement('input');
        input.type = 'file';
        input.accept = '.asm';
        
        input.onchange = e => {
            const file = e.target.files[0];
            if (!file) return;
            
            const reader = new FileReader();
            reader.onload = readerEvent => {
                const content = readerEvent.target.result;
                document.getElementById('codeEditor').value = content;
                this.currentProgram = content;
                this.isAssembled = false;
                this.updateStatusBar();
                
                // Update tab name
                const tab = document.querySelector('.mars-editor-tab');
                tab.textContent = file.name;
            };
            reader.readAsText(file);
        };
        
        input.click();
    }

    saveFile() {
        const content = document.getElementById('codeEditor').value;
        const blob = new Blob([content], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        
        const a = document.createElement('a');
        a.href = url;
        a.download = document.querySelector('.mars-editor-tab').textContent;
        a.click();
        
        URL.revokeObjectURL(url);
    }

    saveAsFile() {
        const content = document.getElementById('codeEditor').value;
        const blob = new Blob([content], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        
        const filename = prompt('Enter filename:', 'untitled.asm');
        if (!filename) return;
        
        const a = document.createElement('a');
        a.href = url;
        a.download = filename;
        a.click();
        
        URL.revokeObjectURL(url);
        
        // Update tab name
        const tab = document.querySelector('.mars-editor-tab');
        tab.textContent = filename;
    }

    exit() {
        if (this.currentProgram && !confirm('Discard changes to current file?')) {
            return;
        }
        
        window.close();
    }

    assemble() {
        const program = document.getElementById('codeEditor').value;
        if (!program) {
            this.showMessage('No program to assemble.');
            return;
        }
        
        this.showMessage('Assembling program...');
        
        fetch('/api/simulate', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                program: program,
                mode: 'assemble'
            })
        })
        .then(response => response.json())
        .then(result => {
            if (result.success) {
                this.isAssembled = true;
                this.showMessage('Program assembled successfully.');
                this.updateStatusBar();
                
                // Switch to execution view
                this.showExecutionView(result.output);
            } else {
                this.showMessage('Assembly failed: ' + result.error);
            }
        })
        .catch(error => {
            this.showMessage('Error: ' + error.message);
        });
    }

    showExecutionView(output) {
        // Parse the output to populate text and data segments
        // This is a simplified version - in a real implementation, 
        // you would parse the actual output from the simulator
        
        // Clear previous data
        this.textSegment = [];
        this.dataSegment = [];
        
        // Create some sample data for demonstration
        for (let i = 0; i < 10; i++) {
            this.textSegment.push({
                address: 0x00400000 + (i * 4),
                code: '0x' + Math.floor(Math.random() * 0xFFFFFFFF).toString(16).padStart(8, '0'),
                basic: 'add $t0, $t1, $t2',
                source: 'add $t0, $t1, $t2 # Sample instruction'
            });
            
            this.dataSegment.push({
                address: 0x10010000 + (i * 16),
                values: [
                    Math.floor(Math.random() * 0xFFFFFFFF),
                    Math.floor(Math.random() * 0xFFFFFFFF),
                    Math.floor(Math.random() * 0xFFFFFFFF),
                    Math.floor(Math.random() * 0xFFFFFFFF)
                ],
                ascii: 'Sample'
            });
        }
        
        // Show memory view
        document.getElementById('memoryView').style.display = 'flex';
        document.getElementById('overlay').style.display = 'block';
        
        // Populate text segment table
        const textSegmentTable = document.getElementById('textSegmentTable');
        textSegmentTable.innerHTML = '';
        
        this.textSegment.forEach(item => {
            const row = document.createElement('tr');
            
            const addressCell = document.createElement('td');
            addressCell.textContent = '0x' + item.address.toString(16).padStart(8, '0');
            row.appendChild(addressCell);
            
            const codeCell = document.createElement('td');
            codeCell.textContent = item.code;
            row.appendChild(codeCell);
            
            const basicCell = document.createElement('td');
            basicCell.textContent = item.basic;
            row.appendChild(basicCell);
            
            const sourceCell = document.createElement('td');
            sourceCell.textContent = item.source;
            row.appendChild(sourceCell);
            
            // Add breakpoint checkbox
            const checkbox = document.createElement('input');
            checkbox.type = 'checkbox';
            checkbox.dataset.address = item.address;
            checkbox.addEventListener('change', () => {
                if (checkbox.checked) {
                    this.breakpoints.add(item.address);
                } else {
                    this.breakpoints.delete(item.address);
                }
            });
            
            addressCell.prepend(checkbox);
            
            textSegmentTable.appendChild(row);
        });
        
        // Populate data segment table
        const dataSegmentTable = document.getElementById('dataSegmentTable');
        dataSegmentTable.innerHTML = '';
        
        this.dataSegment.forEach(item => {
            const row = document.createElement('tr');
            
            const addressCell = document.createElement('td');
            addressCell.textContent = '0x' + item.address.toString(16).padStart(8, '0');
            row.appendChild(addressCell);
            
            for (let i = 0; i < 4; i++) {
                const valueCell = document.createElement('td');
                valueCell.textContent = '0x' + item.values[i].toString(16).padStart(8, '0');
                valueCell.contentEditable = true;
                valueCell.addEventListener('blur', () => {
                    try {
                        const value = parseInt(valueCell.textContent.replace(/^0x/, ''), 16);
                        if (!isNaN(value)) {
                            item.values[i] = value;
                            valueCell.textContent = '0x' + value.toString(16).padStart(8, '0');
                        } else {
                            valueCell.textContent = '0x' + item.values[i].toString(16).padStart(8, '0');
                        }
                    } catch (e) {
                        valueCell.textContent = '0x' + item.values[i].toString(16).padStart(8, '0');
                    }
                });
                row.appendChild(valueCell);
            }
            
            const asciiCell = document.createElement('td');
            asciiCell.textContent = item.ascii;
            row.appendChild(asciiCell);
            
            dataSegmentTable.appendChild(row);
        });
    }

    run() {
        if (!this.isAssembled) {
            this.assemble();
            return;
        }
        
        this.isRunning = true;
        this.showMessage('Running program...');
        
        const program = document.getElementById('codeEditor').value;
        
        fetch('/api/simulate', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                program: program,
                mode: 'run',
                pipeline: document.getElementById('pipelineCheck')?.checked || false,
                branch_prediction: document.getElementById('branchPredictionCheck')?.checked || false
            })
        })
        .then(response => response.json())
        .then(result => {
            this.isRunning = false;
            
            if (result.success) {
                this.showMessage('Program execution completed.');
                this.showRunOutput(result.output);
                this.updateRegisters(result.output);
            } else {
                this.showMessage('Execution failed: ' + result.error);
            }
        })
        .catch(error => {
            this.isRunning = false;
            this.showMessage('Error: ' + error.message);
        });
    }

    step() {
        if (!this.isAssembled) {
            this.assemble();
            return;
        }
        
        this.showMessage('Executing one instruction...');
        
        const program = document.getElementById('codeEditor').value;
        
        fetch('/api/simulate', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                program: program,
                mode: 'step',
                pipeline: document.getElementById('pipelineCheck')?.checked || false,
                branch_prediction: document.getElementById('branchPredictionCheck')?.checked || false
            })
        })
        .then(response => response.json())
        .then(result => {
            if (result.success) {
                this.showMessage('Instruction executed.');
                this.updateRegisters(result.output);
                this.highlightCurrentInstruction(result.output);
            } else {
                this.showMessage('Execution failed: ' + result.error);
            }
        })
        .catch(error => {
            this.showMessage('Error: ' + error.message);
        });
    }

    backstep() {
        this.showMessage('Backstep not implemented in this version.');
    }

    reset() {
        this.showMessage('Resetting simulator...');
        this.registers = Array(32).fill(0);
        this.pc = 0;
        this.updateRegisterTable();
        this.showMessage('Simulator reset.');
    }

    stop() {
        if (this.isRunning) {
            this.isRunning = false;
            this.showMessage('Execution stopped by user.');
        }
    }

    clearBreakpoints() {
        this.breakpoints.clear();
        
        // Uncheck all breakpoint checkboxes
        document.querySelectorAll('#textSegmentTable input[type="checkbox"]').forEach(checkbox => {
            checkbox.checked = false;
        });
        
        this.showMessage('All breakpoints cleared.');
    }

    showAbout() {
        const aboutModal = new bootstrap.Modal(document.getElementById('aboutModal'));
        aboutModal.show();
    }

    showMessage(message) {
        const messagesConsole = document.getElementById('mars-messages');
        messagesConsole.innerHTML += '\n' + message;
        messagesConsole.scrollTop = messagesConsole.scrollHeight;
    }

    showRunOutput(output) {
        const runIOConsole = document.getElementById('run-io');
        runIOConsole.textContent = output;
        
        // Switch to Run I/O tab
        document.querySelectorAll('.mars-console-tab').forEach(tab => {
            tab.classList.remove('active');
            if (tab.dataset.target === 'run-io') {
                tab.classList.add('active');
            }
        });
        
        document.querySelectorAll('.mars-console').forEach(c => c.style.display = 'none');
        runIOConsole.style.display = 'block';
    }

    updateRegisters(output) {
        // Parse register values from output
        // This is a simplified version - in a real implementation, 
        // you would parse the actual output from the simulator
        
        // Simulate some register changes
        for (let i = 0; i < 5; i++) {
            const regIndex = Math.floor(Math.random() * 32);
            this.registers[regIndex] = Math.floor(Math.random() * 0xFFFFFFFF);
        }
        
        this.updateRegisterTable();
    }

    highlightCurrentInstruction(output) {
        // Parse PC from output and highlight the corresponding instruction
        // This is a simplified version - in a real implementation, 
        // you would parse the actual output from the simulator
        
        // Simulate PC update
        this.pc = 0x00400000 + (Math.floor(Math.random() * 10) * 4);
        
        // Highlight the instruction in the text segment table
        const rows = document.querySelectorAll('#textSegmentTable tr');
        rows.forEach(row => {
            row.classList.remove('table-primary');
            
            const addressCell = row.querySelector('td:first-child');
            if (addressCell) {
                const address = parseInt(addressCell.textContent.replace(/^0x/, ''), 16);
                if (address === this.pc) {
                    row.classList.add('table-primary');
                }
            }
        });
    }

    loadExamples() {
        fetch('/api/examples')
            .then(response => response.json())
            .then(examples => {
                const examplesList = document.getElementById('examplesList');
                examplesList.innerHTML = '';
                
                for (const [key, example] of Object.entries(examples)) {
                    const card = document.createElement('div');
                    card.className = 'card mb-3';
                    
                    const cardHeader = document.createElement('div');
                    cardHeader.className = 'card-header';
                    cardHeader.innerHTML = `<h6>${example.name}</h6><small class="text-muted">${example.description}</small>`;
                    
                    const cardBody = document.createElement('div');
                    cardBody.className = 'card-body';
                    
                    const pre = document.createElement('pre');
                    pre.className = 'code-preview';
                    pre.textContent = example.code;
                    
                    const loadButton = document.createElement('button');
                    loadButton.className = 'btn btn-primary btn-sm';
                    loadButton.textContent = 'Load Example';
                    loadButton.addEventListener('click', () => {
                        document.getElementById('codeEditor').value = example.code;
                        this.currentProgram = example.code;
                        this.isAssembled = false;
                        this.updateStatusBar();
                        
                        // Update tab name
                        const tab = document.querySelector('.mars-editor-tab');
                        tab.textContent = `${example.name.toLowerCase().replace(/\s+/g, '_')}.asm`;
                        
                        // Close modal
                        bootstrap.Modal.getInstance(document.getElementById('examplesModal')).hide();
                    });
                    
                    cardBody.appendChild(pre);
                    cardBody.appendChild(loadButton);
                    
                    card.appendChild(cardHeader);
                    card.appendChild(cardBody);
                    
                    examplesList.appendChild(card);
                }
                
                // Show modal
                const examplesModal = new bootstrap.Modal(document.getElementById('examplesModal'));
                examplesModal.show();
            })
            .catch(error => {
                this.showMessage('Error loading examples: ' + error.message);
            });
    }
}

// Initialize simulator when page loads
document.addEventListener('DOMContentLoaded', () => {
    window.simulator = new MARSSimulator();
});