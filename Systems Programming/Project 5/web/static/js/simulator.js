class EnhancedMARSSimulator {
  constructor() {
    // State
    this.isAssembled=false; this.isRunning=false;
    this.pipelineOn=false; this.bpOn=false;
    this.registers=new Array(32).fill(0);
    this.pc=0x00400000; this.cycles=0; this.instrCount=0;
    // UI setup
    this.editor=CodeMirror.fromTextArea(document.getElementById('codeEditor'),{
      mode:'gas', theme:'eclipse', lineNumbers:true, indentUnit:4
    });
    this.setupListeners();
    this.updateStatus('Ready');
    this.updateCursor();
  }

  setupListeners() {
    // Toolbar
    document.getElementById('assembleBtn').addEventListener('click',()=>this.assemble());
    document.getElementById('runBtn').addEventListener('click',()=>this.run());
    document.getElementById('stepBtn').addEventListener('click',()=>this.step());
    document.getElementById('resetBtn').addEventListener('click',()=>this.reset());
    document.getElementById('pipelineToggle').addEventListener('click',()=>this.togglePipeline());
    document.getElementById('branchPredToggle').addEventListener('click',()=>this.toggleBP());
    // Editor events
    this.editor.on('change',()=>{this.isAssembled=false;this.updateStatus('Ready');});
    this.editor.on('cursorActivity',()=>this.updateCursor());
  }

  async assemble() {
    const code=this.editor.getValue();
    this.log(`Assembling program...`);
    const res=await fetch('/api/simulate',{
      method:'POST', headers:{'Content-Type':'application/json'},
      body:JSON.stringify({program:code,mode:'assemble',pipeline:this.pipelineOn,branch_prediction:this.bpOn})
    });
    const jr=await res.json();
    if(jr.success){ this.isAssembled=true; this.log('Assembly successful'); }
    else this.log('ERROR: '+jr.error);
    this.updateStatus('Assembled');
  }

  async run() {
    if(!this.isAssembled) await this.assemble();
    this.log('Running program...');
    const code=this.editor.getValue();
    const res=await fetch('/api/simulate',{
      method:'POST', headers:{'Content-Type':'application/json'},
      body:JSON.stringify({program:code,mode:'run',pipeline:this.pipelineOn,branch_prediction:this.bpOn})
    });
    const jr=await res.json();
    if(jr.success){
      this.log('Program output:\n'+jr.output);
      this.updateRegisters(jr.registers);
    } else this.log('ERROR: '+jr.error);
    this.updateStatus('Ready');
  }

  async step() {
    if(!this.isAssembled) await this.assemble();
    this.log('Executing one instruction...');
    const code=this.editor.getValue();
    const res=await fetch('/api/simulate',{
      method:'POST', headers:{'Content-Type':'application/json'},
      body:JSON.stringify({program:code,mode:'step',pipeline:this.pipelineOn,branch_prediction:this.bpOn,pc:this.pc})
    });
    const jr=await res.json();
    if(jr.success){
      this.updateRegisters(jr.registers); this.pc=jr.pc; this.log('Executed PC='+jr.pc.toString(16));
    } else this.log('ERROR: '+jr.error);
  }

  reset() {
    this.isAssembled=false; this.pc=0x00400000; this.cycles=0; this.instrCount=0;
    this.registers.fill(0); this.log('Simulator reset'); this.updateStatus('Ready');
  }

  togglePipeline() {
    this.pipelineOn=!this.pipelineOn;
    this.log(`Pipeline ${(this.pipelineOn?'enabled':'disabled')}`);
  }

  toggleBP() {
    this.bpOn=!this.bpOn;
    this.log(`Branch prediction ${(this.bpOn?'enabled':'disabled')}`);
  }

  updateRegisters(regs){
    this.registers=regs; /* update UI table accordingly */ }
  updateStatus(msg){ document.getElementById('statusMessage').textContent=msg; }
  updateCursor(){ const c=this.editor.getCursor(); document.getElementById('cursorPosition').textContent=`Line:${c.line+1},Col:${c.ch+1}`; }
  log(msg){ const cb=document.getElementById('marsMessages'); cb.textContent+=`\n${msg}`; cb.scrollTop=cb.scrollHeight; }
}

// Initialize
document.addEventListener('DOMContentLoaded',()=>window.simulator=new EnhancedMARSSimulator());
