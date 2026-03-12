import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/simulator_provider.dart';

class ControlPanel extends StatelessWidget {
  const ControlPanel({super.key});

  @override
  Widget build(BuildContext context) {
    final SimulatorProvider provider = context.watch<SimulatorProvider>();
    final ThemeData theme = Theme.of(context);

    return Card(
      elevation: 2,
      child: Padding(
        padding: const EdgeInsets.symmetric(vertical: 12.0, horizontal: 24.0),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            IconButton(
              icon: const Icon(Icons.first_page),
              onPressed: provider.canStepBackward
                  ? () => provider.jumpToBeginning()
                  : null,
              tooltip: "Jump to Beginning",
            ),
            IconButton(
              icon: const Icon(Icons.navigate_before),
              onPressed: provider.canStepBackward
                  ? () => provider.stepBackward()
                  : null,
              tooltip: "Step Backward",
            ),

            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 24.0),
              child: Text(
                "Cycle: ${provider.currentCycleNumber} / ${provider.totalCycles > 0 ? provider.totalCycles - 1 : 0}",
                style: theme.textTheme.titleMedium?.copyWith(
                  fontWeight: FontWeight.bold,
                ),
              ),
            ),

            IconButton(
              icon: const Icon(Icons.navigate_next),
              onPressed: provider.canStepForward
                  ? () => provider.stepForward()
                  : null,
              tooltip: "Step Forward",
            ),
            IconButton(
              icon: const Icon(Icons.last_page),
              onPressed: provider.canStepForward
                  ? () => provider.jumpToEnd()
                  : null,
              tooltip: "Jump to End",
            ),
          ],
        ),
      ),
    );
  }
}
