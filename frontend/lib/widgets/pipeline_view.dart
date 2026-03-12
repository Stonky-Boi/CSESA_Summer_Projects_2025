import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/simulator_provider.dart';
import '../models/cycle_state.dart';

class PipelineView extends StatelessWidget {
  const PipelineView({super.key});

  Widget _buildStageCard(
    String stageName,
    String instruction,
    ThemeData theme,
  ) {
    // Highlight active instructions; dim NOPs/bubbles
    final bool isNop = instruction == "NOP";
    final Color cardColor = isNop
        ? theme.colorScheme.surfaceContainerHighest
        : theme.colorScheme.primaryContainer;
    final Color textColor = isNop
        ? theme.colorScheme.onSurfaceVariant
        : theme.colorScheme.onPrimaryContainer;

    return Expanded(
      child: Card(
        color: cardColor,
        elevation: isNop ? 0 : 4,
        child: Padding(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              Text(
                stageName,
                style: theme.textTheme.titleMedium?.copyWith(
                  fontWeight: FontWeight.bold,
                  color: textColor,
                ),
              ),
              const SizedBox(height: 8),
              Text(
                instruction,
                textAlign: TextAlign.center,
                style: theme.textTheme.bodyMedium?.copyWith(
                  fontFamily: 'monospace', // Monospace for assembly code
                  color: textColor,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final SimulatorProvider provider = context.watch<SimulatorProvider>();
    final CycleState? currentCycle = provider.currentCycleState;
    final ThemeData theme = Theme.of(context);

    if (currentCycle == null) {
      return const Center(child: Text("Run a program to view the pipeline."));
    }

    final PipelineTrace trace = currentCycle.pipelineLatches;

    return Row(
      children: [
        _buildStageCard("FETCH", trace.instructionFetch, theme),
        const Icon(Icons.arrow_forward),
        _buildStageCard("DECODE", trace.instructionDecode, theme),
        const Icon(Icons.arrow_forward),
        _buildStageCard("EXECUTE", trace.execute, theme),
        const Icon(Icons.arrow_forward),
        _buildStageCard("MEMORY", trace.memoryAccess, theme),
      ],
    );
  }
}
