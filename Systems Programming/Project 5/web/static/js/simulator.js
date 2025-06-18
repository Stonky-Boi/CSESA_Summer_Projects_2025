class EnhancedMARSSimulator {
    constructor() {
        this.currentProgram = '';
        this.isRunning = false;
        this.isAssembled = false;
        this.isPipelineEnabled = false;
        this.isBranchPredictionEnabled = false;
        this.cycleCount = 0;
        this.instructionCount = 0;
        this.stallCount = 0;
        this.currentPC = 0x00400000;
        this.breakpoints = new Set();
        this.runSpeed = 10;
        
        // Register state
        this.registers = Array(32).fill(0);
        this.registerNames = [
            '$zero', '$at', '$v0', '$v1', '$a0', '$a1', '$a2', '$a3',
            '$t0', '$t1', '$t2', '$t3', '$t4', '$t5', '$t6', '$t7',
            '$s0', '$s1', '$s2', '$s3', '$s4', '$s5', '$s6', '$s7',
            '$t8', '$t9', '$k0', '$k1', '$gp', '$sp', '$fp', '$ra'
        ];
        
        // Coprocessor registers
        this.coprocessor1 = Array(32).fill(0);
        this.coprocessor0 = Array(32).fill(0);
        
        // Pipeline state
        this.pipelineStages = {
            IF: { instruction: null, pc: 0, valid: false },
            ID: { instruction: null, pc: 0, valid: false },
            EX: { instruction: null, pc: 0, valid: false },
            MEM: { instruction: null, pc: 0, valid: false },
            WB: { instruction: null, pc: 0, valid: false }
        };
        
        // Branch prediction state
        this.branchStats = {
            total: 0,
            correct: 0,
            incorrect: 0,
            accuracy: 0
        };
        
        // Memory segments
        this.textSegment = [];
        this.dataSegment = [];
        this.stackSegment = [];
        this.kernelSegment = [];
        
        // Hazard detection
        this.hazards = [];
        
        this.initializeUI();
        this.initializeEventListeners();
        this.populateRegisterTables();
        this.updateStatusBar();
    }

    initializeUI() {
        // Initialize CodeMirror editor
        this.editor = CodeMirror.fromTextArea(document.getElementById('codeEditor'), {
            mode: 'gas',
            theme: 'eclipse',
            lineNumbers: true,
            lineWrapping: true,
            indentUnit: 4,
            tabSize: 4,
            autoCloseBrackets: true,
            matchBrackets: true,
            showCursorWhenSelecting: true,
            styleActiveLine: true
        });

        this.editor.on('change', () => {
            this.currentProgram = this.editor.getValue();
            this.isAssembled = false;
            this.updateStatusBar();
        });

        this.editor.on('cursorActivity', () => {
            this.updateStatusBar();
        });

        // Initialize console tabs
        this.initializeConsoleTabs();
        
        // Initialize register tabs
        this.initializeRegisterTabs();
        
        // Initialize memory tabs
        this.initializeMemoryTabs();
        
        // Initialize speed slider
        this.initializeSpeedSlider();
    }

    initializeEventListeners() {
        // File operations
        document.getElementById('newFile').addEventListener('click', () => this.newFile());
        document.getElementById('openFile').addEventListener('click', () => this.openFile());
        document.getElementById('saveFile').addEventListener('click', () => this.saveFile());
        document.getElementById('saveAsFile').addEventListener('click', () => this.saveAsFile());
        document.getElementById('exitApp').addEventListener('click', () => this.exitApp());

        // Edit operations
        document.getElementById('undoAction').addEventListener('click', () => this.editor.undo());
        document.getElementById('redoAction').addEventListener('click', () => this.editor.redo());
        document.getElementById('cutText').addEventListener('click', () => this.editor.execCommand('cut'));
        document.getElementById('copyText').addEventListener('click', () => this.editor.execCommand('copy'));
        document.getElementById('pasteText').addEventListener('click', () => this.editor.execCommand('paste'));
        document.getElementById('selectAllText').addEventListener('click', () => this.editor.execCommand('selectAll'));

        // Toolbar buttons
        document.getElementById('newBtn').addEventListener('click', () => this.newFile());
        document.getElementById('openBtn').addEventListener('click', () => this.openFile());
        document.getElementById('saveBtn').addEventListener('click', () => this.saveFile());
        document.getElementById('assembleBtn').addEventListener('click', () => this.assemble());
        document.getElementById('runBtn').addEventListener('click', () => this.run());
        document.getElementById('stepBtn').addEventListener('click', () => this.step());
        document.getElementById('backstepBtn').addEventListener('click', () => this.backstep());
        document.getElementById('resetBtn').addEventListener('click', () => this.reset());
        document.getElementById('pauseBtn').addEventListener('click', () => this.pause());
        document.getElementById('stopBtn').addEventListener('click', () => this.stop());

        // Run menu
        document.getElementById('assembleProgram').addEventListener('click', () => this.assemble());
        document.getElementById('runProgram').addEventListener('click', () => this.run());
        document.getElementById('stepProgram').addEventListener('click', () => this.step());
        document.getElementById('backstepProgram').addEventListener('click', () => this.backstep());
        document.getElementById('resetProgram').addEventListener('click', () => this.reset());
        document.getElementById('clearBreakpoints').addEventListener('click', () => this.clearBreakpoints());

        // Pipeline and branch prediction toggles
        document.getElementById('pipelineToggle').addEventListener('click', () => this.togglePipeline());
        document.getElementById('branchPredToggle').addEventListener('click', () => this.toggleBranchPrediction());

        // Tools menu
        document.getElementById('memoryViewer').addEventListener('click', () => this.showMemoryViewer());
        
        // Help menu
        document.getElementById('aboutMars').addEventListener('click', () => this.showAbout());

        // Memory viewer controls
        document.getElementById('closeMemoryBtn').addEventListener('click', () => this.hideMemoryViewer());

        // Keyboard shortcuts
        document.addEventListener('keydown', (e) => {
            if (e.ctrlKey) {
                switch (e.key) {
                    case 'n': e.preventDefault(); this.newFile(); break;
                    case 'o': e.preventDefault(); this.openFile(); break;
                    case 's': e.preventDefault(); this.saveFile(); break;
                }
            } else {
                switch (e.key) {
                    case 'F3': e.preventDefault(); this.assemble(); break;
                    case 'F5': e.preventDefault(); this.run(); break;
                    case 'F7': e.preventDefault(); this.step(); break;
                    case 'F8': e.preventDefault(); this.backstep(); break;
                    case 'F12': e.preventDefault(); this.reset(); break;
                }
            }
        });
    }

    initializeConsoleTabs() {
        document.querySelectorAll('.mars-console-tab').forEach(tab => {
            tab.addEventListener('click', () => {
                document.querySelectorAll('.mars-console-tab').forEach(t => t.classList.remove('active'));
                document.querySelectorAll('.mars-console').forEach(c => c.classList.remove('active'));
                
                tab.classList.add('active');
                document.getElementById(tab.dataset.target).classList.add('active');
            });
        });
    }

    initializeRegisterTabs() {
        document.querySelectorAll('.mars-registers-tab').forEach(tab => {
            tab.addEventListener('click', () => {
                document.querySelectorAll('.mars-registers-tab').forEach(t => t.classList.remove('active'));
                document.querySelectorAll('.registers-panel').forEach(p => {
                    p.classList.remove('active');
                    p.style.display = 'none';
                });
                
                tab.classList.add('active');
                const panel = document.getElementById(tab.dataset.target);
                panel.classList.add('active');
                panel.style.display = 'block';
            });
        });
    }

    initializeMemoryTabs() {
        document.querySelectorAll('.mars-memory-tab').forEach(tab => {
            tab.addEventListener('click', () => {
                document.querySelectorAll('.mars-memory-tab').forEach(t => t.classList.remove('active'));
                document.querySelectorAll('.mars-memory-table-container').forEach(c => {
                    c.classList.remove('active');
                    c.style.display = 'none';
                });
                
                tab.classList.add('active');
                const container = document.getElementById(tab.dataset.target);
                container.classList.add('active');
                container.style.display = 'block';
            });
        });
    }

    initializeSpeedSlider() {
        const speedSlider = document.getElementById('speedSlider');
        const speedValue = document.getElementById('speedValue');
        
        speedSlider.addEventListener('input', () => {
            this.runSpeed = parseInt(speedSlider.value);
            speedValue.textContent = `${this.runSpeed} inst/sec`;
        });
    }

    populateRegisterTables() {
        this.populateMainRegisters();
        this.populateCoprocessor1();
        this.populateCoprocessor0();
    }

    populateMainRegisters() {
        const tbody = document.getElementById('registerTableBody');
        tbody.innerHTML = '';
        
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
            valueCell.textContent = this.formatHex(this.registers[i]);
            valueCell.contentEditable = i !== 0; // $zero is not editable
            valueCell.dataset.register = i;
            
            if (i !== 0) {
                valueCell.addEventListener('blur', (e) => this.updateRegisterValue(e));
                valueCell.addEventListener('keydown', (e) => {
                    if (e.key === 'Enter') {
                        e.preventDefault();
                        e.target.blur();
                    }
                });
            }
            
            row.appendChild(valueCell);
            tbody.appendChild(row);
        }
    }

    populateCoprocessor1() {
        const tbody = document.getElementById('coproc1TableBody');
        tbody.innerHTML = '';
        
        for (let i = 0; i < 32; i++) {
            const row = document.createElement('tr');
            
            const nameCell = document.createElement('td');
            nameCell.textContent = `$f${i}`;
            row.appendChild(nameCell);
            
            const numCell = document.createElement('td');
            numCell.textContent = i;
            row.appendChild(numCell);
            
            const valueCell = document.createElement('td');
            valueCell.className = 'register-value';
            valueCell.textContent = this.formatFloat(this.coprocessor1[i]);
            valueCell.contentEditable = true;
            valueCell.dataset.register = i;
            valueCell.addEventListener('blur', (e) => this.updateCoprocessor1Value(e));
            
            row.appendChild(valueCell);
            tbody.appendChild(row);
        }
    }

    populateCoprocessor0() {
        const tbody = document.getElementById('coproc0TableBody');
        tbody.innerHTML = '';
        
        const cop0Names = [
            'Index', 'Random', 'EntryLo0', 'EntryLo1', 'Context', 'PageMask', 'Wired', 'Reserved',
            'BadVAddr', 'Count', 'EntryHi', 'Compare', 'Status', 'Cause', 'EPC', 'PRId',
            'Config', 'LLAddr', 'WatchLo', 'WatchHi', 'XContext', 'Reserved', 'Reserved', 'Debug',
            'DEPC', 'PerfCnt', 'ErrCtl', 'CacheErr', 'TagLo', 'TagHi', 'ErrorEPC', 'DESAVE'
        ];
        
        for (let i = 0; i < 32; i++) {
            const row = document.createElement('tr');
            
            const nameCell = document.createElement('td');
            nameCell.textContent = cop0Names[i];
            row.appendChild(nameCell);
            
            const numCell = document.createElement('td');
            numCell.textContent = i;
            row.appendChild(numCell);
            
            const valueCell = document.createElement('td');
            valueCell.className = 'register-value';
            valueCell.textContent = this.formatHex(this.coprocessor0[i]);
            valueCell.contentEditable = true;
            valueCell.dataset.register = i;
            valueCell.addEventListener('blur', (e) => this.updateCoprocessor0Value(e));
            
            row.appendChild(valueCell);
            tbody.appendChild(row);
        }
    }

    updateRegisterValue(event) {
        const regIndex = parseInt(event.target.dataset.register);
        const value = this.parseValue(event.target.textContent);
        
        if (value !== null && regIndex !== 0) {
            this.registers[regIndex] = value;
            event.target.textContent = this.formatHex(value);
            this.highlightRegisterChange(regIndex);
        } else {
            event.target.textContent = this.formatHex(this.registers[regIndex]);
        }
    }

    updateCoprocessor1Value(event) {
        const regIndex = parseInt(event.target.dataset.register);
        const value = parseFloat(event.target.textContent);
        
        if (!isNaN(value)) {
            this.coprocessor1[regIndex] = value;
            event.target.textContent = this.formatFloat(value);
        } else {
            event.target.textContent = this.formatFloat(this.coprocessor1[regIndex]);
        }
    }

    updateCoprocessor0Value(event) {
        const regIndex = parseInt(event.target.dataset.register);
        const value = this.parseValue(event.target.textContent);
        
        if (value !== null) {
            this.coprocessor0[regIndex] = value;
            event.target.textContent = this.formatHex(value);
        } else {
            event.target.textContent = this.formatHex(this.coprocessor0[regIndex]);
        }
    }

    highlightRegisterChange(regIndex) {
        const cell = document.querySelector(`[data-register="${regIndex}"]`);
        if (cell) {
            cell.classList.add('changed');
            setTimeout(() => cell.classList.remove('changed'), 2000);
        }
    }

    parseValue(text) {
        try {
            if (text.startsWith('0x') || text.startsWith('0X')) {
                return parseInt(text, 16);
            } else if (text.startsWith('0b') || text.startsWith('0B')) {
                return parseInt(text.slice(2), 2);
            } else if (text.startsWith('0o') || text.startsWith('0O')) {
                return parseInt(text.slice(2), 8);
            } else {
                return parseInt(text, 10);
            }
        } catch (e) {
            return null;
        }
    }

    formatHex(value) {
        return '0x' + (value >>> 0).toString(16).padStart(8, '0').toUpperCase();
    }

    formatFloat(value) {
        return value.toFixed(6);
    }

    updateStatusBar() {
        const cursor = this.editor.getCursor();
        document.getElementById('cursorPosition').textContent = 
            `Line: ${cursor.line + 1}, Column: ${cursor.ch + 1}`;
        
        if (this.isAssembled) {
            document.getElementById('statusMessage').textContent = 'Assembled';
        } else {
            document.getElementById('statusMessage').textContent = 'Ready';
        }
    }

    updateCPUState() {
        document.getElementById('pcValue').textContent = this.formatHex(this.currentPC);
        document.getElementById('cycleCount').textContent = this.cycleCount;
        document.getElementById('instrCount').textContent = this.instructionCount;
        
        if (this.isRunning) {
            document.getElementById('statusValue').textContent = 'Running';
        } else if (this.isAssembled) {
            document.getElementById('statusValue').textContent = 'Assembled';
        } else {
            document.getElementById('statusValue').textContent = 'Ready';
        }
    }

    updatePerformanceMetrics() {
        const cpi = this.instructionCount > 0 ? this.cycleCount / this.instructionCount : 1.0;
        const ipc = cpi > 0 ? 1.0 / cpi : 1.0;
        const efficiency = this.cycleCount > 0 ? (this.instructionCount / this.cycleCount) * 100 : 100;
        
        document.getElementById('cpiValue').textContent = cpi.toFixed(2);
        document.getElementById('ipcValue').textContent = ipc.toFixed(2);
        document.getElementById('stallCycles').textContent = this.stallCount;
        document.getElementById('efficiency').textContent = efficiency.toFixed(1) + '%';
    }

    updateBranchPredictionStats() {
        if (this.branchStats.total > 0) {
            this.branchStats.accuracy = (this.branchStats.correct / this.branchStats.total) * 100;
        }
        
        document.getElementById('totalBranches').textContent = this.branchStats.total;
        document.getElementById('correctPreds').textContent = this.branchStats.correct;
        document.getElementById('incorrectPreds').textContent = this.branchStats.incorrect;
        document.getElementById('predAccuracy').textContent = this.branchStats.accuracy.toFixed(1) + '%';
    }

    updatePipelineView() {
        const stages = ['IF', 'ID', 'EX', 'MEM', 'WB'];
        
        stages.forEach(stage => {
            const element = document.getElementById(`stage${stage}`);
            const instrElement = document.getElementById(`${stage.toLowerCase()}Instr`);
            
            if (this.pipelineStages[stage].valid) {
                element.classList.add('active');
                instrElement.textContent = this.pipelineStages[stage].instruction || 'NOP';
            } else {
                element.classList.remove('active');
                instrElement.textContent = 'NOP';
            }
            
            // Remove other classes
            element.classList.remove('stalled', 'flushed');
        });
    }

    updateHazardDetection() {
        const hazardList = document.getElementById('hazardList');
        hazardList.innerHTML = '';
        
        if (this.hazards.length === 0) {
            const item = document.createElement('div');
            item.className = 'hazard-item';
            item.textContent = 'No hazards detected';
            hazardList.appendChild(item);
        } else {
            this.hazards.forEach(hazard => {
                const item = document.createElement('div');
                item.className = `hazard-item ${hazard.severity}`;
                item.textContent = hazard.description;
                hazardList.appendChild(item);
            });
        }
    }

    togglePipeline() {
        this.isPipelineEnabled = !this.isPipelineEnabled;
        const container = document.getElementById('pipelineContainer');
        const button = document.getElementById('pipelineToggle');
        
        if (this.isPipelineEnabled) {
            container.style.display = 'flex';
            button.style.background = 'linear-gradient(to bottom, #90EE90, #7CFC00)';
            this.logMessage('Pipeline simulation enabled');
        } else {
            container.style.display = 'none';
            button.style.background = '';
            this.logMessage('Pipeline simulation disabled');
        }
    }

    toggleBranchPrediction() {
        this.isBranchPredictionEnabled = !this.isBranchPredictionEnabled;
        const container = document.getElementById('branchStatsContainer');
        const button = document.getElementById('branchPredToggle');
        
        if (this.isBranchPredictionEnabled) {
            container.style.display = 'block';
            button.style.background = 'linear-gradient(to bottom, #90EE90, #7CFC00)';
            this.logMessage('Branch prediction enabled (2-bit adaptive)');
        } else {
            container.style.display = 'none';
            button.style.background = '';
            this.logMessage('Branch prediction disabled');
        }
    }

    newFile() {
        if (this.currentProgram && !confirm('Discard changes to current file?')) {
            return;
        }
        
        this.editor.setValue('');
        this.currentProgram = '';
        this.isAssembled = false;
        document.getElementById('editorTab').textContent = 'untitled.asm';
        this.logMessage('New file created');
        this.updateStatusBar();
    }

    openFile() {
        if (this.currentProgram && !confirm('Discard changes to current file?')) {
            return;
        }
        
        const input = document.createElement('input');
        input.type = 'file';
        input.accept = '.asm,.s,.txt';
        
        input.onchange = (e) => {
            const file = e.target.files[0];
            if (!file) return;
            
            const reader = new FileReader();
            reader.onload = (event) => {
                const content = event.target.result;
                this.editor.setValue(content);
                this.currentProgram = content;
                this.isAssembled = false;
                document.getElementById('editorTab').textContent = file.name;
                this.logMessage(`File opened: ${file.name}`);
                this.updateStatusBar();
            };
            reader.readAsText(file);
        };
        
        input.click();
    }

    saveFile() {
        const content = this.editor.getValue();
        const filename = document.getElementById('editorTab').textContent;
        this.downloadFile(content, filename);
        this.logMessage(`File saved: ${filename}`);
    }

    saveAsFile() {
        const content = this.editor.getValue();
        const filename = prompt('Enter filename:', 'program.asm');
        if (filename) {
            this.downloadFile(content, filename);
            document.getElementById('editorTab').textContent = filename;
            this.logMessage(`File saved as: ${filename}`);
        }
    }

    downloadFile(content, filename) {
        const blob = new Blob([content], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = filename;
        a.click();
        URL.revokeObjectURL(url);
    }

    exitApp() {
        if (this.currentProgram && !confirm('Discard changes to current file?')) {
            return;
        }
        window.close();
    }

    async assemble() {
        const program = this.editor.getValue();
        if (!program.trim()) {
            this.logMessage('Error: No program to assemble');
            return;
        }
        
        this.logMessage('Assembling program...');
        
        try {
            const response = await fetch('/api/simulate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    program: program,
                    mode: 'assemble',
                    pipeline: this.isPipelineEnabled,
                    branch_prediction: this.isBranchPredictionEnabled
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.isAssembled = true;
                this.logMessage('Assembly successful');
                this.populateTextSegment(result.instructions || []);
                this.updateStatusBar();
            } else {
                this.logMessage('Assembly failed: ' + result.error, 'error');
            }
        } catch (error) {
            this.logMessage('Error: ' + error.message, 'error');
        }
    }

    async run() {
        if (!this.isAssembled) {
            await this.assemble();
            if (!this.isAssembled) return;
        }
        
        this.isRunning = true;
        this.logMessage('Running program...');
        this.updateCPUState();
        
        try {
            const response = await fetch('/api/simulate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    program: this.editor.getValue(),
                    mode: 'run',
                    pipeline: this.isPipelineEnabled,
                    branch_prediction: this.isBranchPredictionEnabled
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.logMessage('Program execution completed');
                this.parseSimulationResults(result);
            } else {
                this.logMessage('Execution failed: ' + result.error, 'error');
            }
        } catch (error) {
            this.logMessage('Error: ' + error.message, 'error');
        } finally {
            this.isRunning = false;
            this.updateCPUState();
        }
    }

    async step() {
        if (!this.isAssembled) {
            await this.assemble();
            if (!this.isAssembled) return;
        }
        
        this.logMessage('Executing one instruction...');
        this.cycleCount++;
        this.instructionCount++;
        
        try {
            const response = await fetch('/api/simulate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    program: this.editor.getValue(),
                    mode: 'step',
                    pipeline: this.isPipelineEnabled,
                    branch_prediction: this.isBranchPredictionEnabled,
                    pc: this.currentPC
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.parseSimulationResults(result);
                this.logMessage('Instruction executed');
            } else {
                this.logMessage('Step failed: ' + result.error, 'error');
            }
        } catch (error) {
            this.logMessage('Error: ' + error.message, 'error');
        }
        
        this.updateCPUState();
        this.updatePerformanceMetrics();
        
        if (this.isPipelineEnabled) {
            this.updatePipelineView();
            this.updateHazardDetection();
        }
        
        if (this.isBranchPredictionEnabled) {
            this.updateBranchPredictionStats();
        }
    }

    backstep() {
        this.logMessage('Backstep not implemented in this version');
    }

    reset() {
        this.isRunning = false;
        this.isAssembled = false;
        this.cycleCount = 0;
        this.instructionCount = 0;
        this.stallCount = 0;
        this.currentPC = 0x00400000;
        this.breakpoints.clear();
        
        // Reset registers
        this.registers.fill(0);
        this.coprocessor1.fill(0);
        this.coprocessor0.fill(0);
        
        // Reset pipeline
        Object.keys(this.pipelineStages).forEach(stage => {
            this.pipelineStages[stage] = { instruction: null, pc: 0, valid: false };
        });
        
        // Reset branch prediction
        this.branchStats = { total: 0, correct: 0, incorrect: 0, accuracy: 0 };
        
        // Reset hazards
        this.hazards = [];
        
        this.populateRegisterTables();
        this.updateCPUState();
        this.updatePerformanceMetrics();
        
        if (this.isPipelineEnabled) {
            this.updatePipelineView();
            this.updateHazardDetection();
        }
        
        if (this.isBranchPredictionEnabled) {
            this.updateBranchPredictionStats();
        }
        
        this.logMessage('Simulator reset');
    }

    pause() {
        if (this.isRunning) {
            this.isRunning = false;
            this.logMessage('Execution paused');
            this.updateCPUState();
        }
    }

    stop() {
        this.isRunning = false;
        this.logMessage('Execution stopped');
        this.updateCPUState();
    }

    clearBreakpoints() {
        this.breakpoints.clear();
        document.querySelectorAll('.breakpoint-checkbox').forEach(cb => cb.checked = false);
        this.logMessage('All breakpoints cleared');
    }

    parseSimulationResults(result) {
        // Parse registers from result
        if (result.registers) {
            result.registers.forEach((value, index) => {
                if (this.registers[index] !== value) {
                    this.registers[index] = value;
                    this.highlightRegisterChange(index);
                }
            });
            this.populateRegisterTables();
        }
        
        // Parse PC
        if (result.pc !== undefined) {
            this.currentPC = result.pc;
        }
        
        // Parse pipeline state
        if (result.pipeline && this.isPipelineEnabled) {
            this.pipelineStages = result.pipeline;
        }
        
        // Parse branch prediction stats
        if (result.branchStats && this.isBranchPredictionEnabled) {
            this.branchStats = result.branchStats;
        }
        
        // Parse hazards
        if (result.hazards) {
            this.hazards = result.hazards;
        }
        
        // Parse performance metrics
        if (result.cycles !== undefined) {
            this.cycleCount = result.cycles;
        }
        
        if (result.stalls !== undefined) {
            this.stallCount = result.stalls;
        }
    }

    populateTextSegment(instructions) {
        const tbody = document.getElementById('textSegmentTable');
        tbody.innerHTML = '';
        
        this.textSegment = instructions;
        
        instructions.forEach((instr, index) => {
            const row = document.createElement('tr');
            
            // Breakpoint checkbox
            const bkptCell = document.createElement('td');
            const checkbox = document.createElement('input');
            checkbox.type = 'checkbox';
            checkbox.className = 'breakpoint-checkbox';
            checkbox.addEventListener('change', (e) => {
                if (e.target.checked) {
                    this.breakpoints.add(instr.address);
                } else {
                    this.breakpoints.delete(instr.address);
                }
            });
            bkptCell.appendChild(checkbox);
            row.appendChild(bkptCell);
            
            // Address
            const addrCell = document.createElement('td');
            addrCell.textContent = this.formatHex(instr.address);
            row.appendChild(addrCell);
            
            // Machine code
            const codeCell = document.createElement('td');
            codeCell.textContent = this.formatHex(instr.machineCode);
            row.appendChild(codeCell);
            
            // Basic instruction
            const basicCell = document.createElement('td');
            basicCell.textContent = instr.basic || instr.source;
            row.appendChild(basicCell);
            
            // Source
            const sourceCell = document.createElement('td');
            sourceCell.textContent = instr.source;
            row.appendChild(sourceCell);
            
            tbody.appendChild(row);
        });
    }

    showMemoryViewer() {
        document.getElementById('memoryOverlay').style.display = 'block';
        this.populateMemoryTables();
    }

    hideMemoryViewer() {
        document.getElementById('memoryOverlay').style.display = 'none';
    }

    populateMemoryTables() {
        this.populateDataSegmentTable();
        this.populateStackTable();
        this.populateKernelTable();
    }

    populateDataSegmentTable() {
        const tbody = document.getElementById('dataSegmentTable');
        tbody.innerHTML = '';
        
        // Generate sample data segment entries
        for (let i = 0; i < 16; i++) {
            const row = document.createElement('tr');
            const baseAddr = 0x10010000 + (i * 16);
            
            // Address
            const addrCell = document.createElement('td');
            addrCell.textContent = this.formatHex(baseAddr);
            row.appendChild(addrCell);
            
            // Four word values
            for (let j = 0; j < 4; j++) {
                const valueCell = document.createElement('td');
                valueCell.textContent = this.formatHex(0);
                valueCell.contentEditable = true;
                row.appendChild(valueCell);
            }
            
            // ASCII representation
            const asciiCell = document.createElement('td');
            asciiCell.textContent = '....';
            row.appendChild(asciiCell);
            
            tbody.appendChild(row);
        }
    }

    populateStackTable() {
        const tbody = document.getElementById('stackTable');
        tbody.innerHTML = '';
        
        // Generate sample stack entries
        for (let i = 0; i < 16; i++) {
            const row = document.createElement('tr');
            const baseAddr = 0x7FFFEFFC - (i * 16);
            
            // Address
            const addrCell = document.createElement('td');
            addrCell.textContent = this.formatHex(baseAddr);
            row.appendChild(addrCell);
            
            // Four word values
            for (let j = 0; j < 4; j++) {
                const valueCell = document.createElement('td');
                valueCell.textContent = this.formatHex(0);
                valueCell.contentEditable = true;
                row.appendChild(valueCell);
            }
            
            // ASCII representation
            const asciiCell = document.createElement('td');
            asciiCell.textContent = '....';
            row.appendChild(asciiCell);
            
            tbody.appendChild(row);
        }
    }

    populateKernelTable() {
        const tbody = document.getElementById('kernelTable');
        tbody.innerHTML = '';
        
        // Generate sample kernel entries
        for (let i = 0; i < 16; i++) {
            const row = document.createElement('tr');
            const baseAddr = 0x80000000 + (i * 16);
            
            // Address
            const addrCell = document.createElement('td');
            addrCell.textContent = this.formatHex(baseAddr);
            row.appendChild(addrCell);
            
            // Four word values
            for (let j = 0; j < 4; j++) {
                const valueCell = document.createElement('td');
                valueCell.textContent = this.formatHex(0);
                valueCell.contentEditable = true;
                row.appendChild(valueCell);
            }
            
            // ASCII representation
            const asciiCell = document.createElement('td');
            asciiCell.textContent = '....';
            row.appendChild(asciiCell);
            
            tbody.appendChild(row);
        }
    }

    showAbout() {
        const aboutModal = new bootstrap.Modal(document.getElementById('aboutModal'));
        aboutModal.show();
    }

    logMessage(message, type = 'info') {
        const console = document.getElementById('marsMessages');
        const timestamp = new Date().toLocaleTimeString();
        const prefix = type === 'error' ? 'ERROR: ' : '';
        console.textContent += `\n[${timestamp}] ${prefix}${message}`;
        console.scrollTop = console.scrollHeight;
        
        // Switch to MARS Messages tab if error
        if (type === 'error') {
            document.querySelectorAll('.mars-console-tab').forEach(tab => {
                tab.classList.remove('active');
                if (tab.dataset.target === 'marsMessages') {
                    tab.classList.add('active');
                }
            });
            document.querySelectorAll('.mars-console').forEach(c => c.classList.remove('active'));
            document.getElementById('marsMessages').classList.add('active');
        }
    }
}

// Initialize the simulator when the page loads
document.addEventListener('DOMContentLoaded', () => {
    window.simulator = new EnhancedMARSSimulator();
});