import 'dart:io';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/simulator_provider.dart';
import '../widgets/pipeline_view.dart';
import '../widgets/register_grid.dart';
import '../widgets/control_panel.dart';

class SimulatorScreen extends StatefulWidget {
  const SimulatorScreen({super.key});

  @override
  State<SimulatorScreen> createState() => _SimulatorScreenState();
}

class _SimulatorScreenState extends State<SimulatorScreen> {
  final TextEditingController _codeController = TextEditingController();
  String _selectedPredictor = "two_level";

  final List<String> _predictors = [
    "always_not_taken",
    "always_taken",
    "one_bit",
    "two_bit",
    "local_history",
    "global_history",
    "two_level",
  ];

  @override
  void initState() {
    super.initState();
    // Default test program
    _codeController.text = """.text
main:
    ADDI \$t0, \$zero, 5
    ADDI \$t1, \$zero, 0
    ADDI \$t2, \$zero, 1
loop:
    BEQ \$t0, \$zero, end
    ADD \$t1, \$t1, \$t0
    SUB \$t0, \$t0, \$t2
    J loop
end:
    ADDI \$v0, \$zero, 10
    SYSCALL""";
  }

  @override
  void dispose() {
    _codeController.dispose();
    super.dispose();
  }

  Future<void> _runSimulation() async {
    final SimulatorProvider provider = context.read<SimulatorProvider>();

    // Write the editor content to a temporary file
    final Directory tempDirectory = Directory.systemTemp;
    final File tempAssemblyFile = File('${tempDirectory.path}/ember_temp.s');
    await tempAssemblyFile.writeAsString(_codeController.text);

    // Command the backend to execute the file
    await provider.executeAssemblyProgram(
      tempAssemblyFile.path,
      _selectedPredictor,
    );
  }

  @override
  Widget build(BuildContext context) {
    final SimulatorProvider provider = context.watch<SimulatorProvider>();

    return Scaffold(
      appBar: AppBar(
        title: const Text("Ember - MIPS Pipeline Simulator"),
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
      ),
      body: Row(
        children: [
          // Left Panel: Code Editor
          Expanded(
            flex: 1,
            child: Padding(
              padding: const EdgeInsets.all(16.0),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text(
                        "Assembly Code",
                        style: Theme.of(context).textTheme.titleLarge,
                      ),
                      DropdownButton<String>(
                        value: _selectedPredictor,
                        items: _predictors.map((String predictor) {
                          return DropdownMenuItem<String>(
                            value: predictor,
                            child: Text(predictor),
                          );
                        }).toList(),
                        onChanged: (String? newValue) {
                          if (newValue != null) {
                            setState(() {
                              _selectedPredictor = newValue;
                            });
                          }
                        },
                      ),
                    ],
                  ),
                  const SizedBox(height: 8),
                  Expanded(
                    child: TextField(
                      controller: _codeController,
                      maxLines: null,
                      expands: true,
                      textAlignVertical: TextAlignVertical.top,
                      style: const TextStyle(fontFamily: 'monospace'),
                      decoration: const InputDecoration(
                        border: OutlineInputBorder(),
                        hintText: "Enter MIPS assembly here...",
                      ),
                    ),
                  ),
                  const SizedBox(height: 16),
                  ElevatedButton.icon(
                    onPressed: provider.isSimulationRunning
                        ? null
                        : _runSimulation,
                    icon: provider.isSimulationRunning
                        ? const SizedBox(
                            width: 16,
                            height: 16,
                            child: CircularProgressIndicator(strokeWidth: 2),
                          )
                        : const Icon(Icons.play_arrow),
                    label: const Text("Compile & Run"),
                    style: ElevatedButton.styleFrom(
                      padding: const EdgeInsets.all(16),
                    ),
                  ),
                  if (provider.fatalErrorMessage != null) ...[
                    const SizedBox(height: 16),
                    Text(
                      "Error: ${provider.fatalErrorMessage}",
                      style: TextStyle(
                        color: Theme.of(context).colorScheme.error,
                      ),
                    ),
                  ],
                ],
              ),
            ),
          ),

          const VerticalDivider(width: 1),

          // Right Panel: Hardware Visualization
          Expanded(
            flex: 2,
            child: Padding(
              padding: const EdgeInsets.all(16.0),
              child: Column(
                children: [
                  const PipelineView(),
                  const SizedBox(height: 16),
                  const ControlPanel(),
                  const SizedBox(height: 16),
                  const Expanded(child: RegisterGrid()),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}
