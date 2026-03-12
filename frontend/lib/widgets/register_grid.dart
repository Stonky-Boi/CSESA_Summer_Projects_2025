import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/simulator_provider.dart';
import '../models/cycle_state.dart';

class RegisterGrid extends StatelessWidget {
  const RegisterGrid({super.key});

  // MIPS ABI standard register names
  String _getRegisterName(int index) {
    if (index == 0) return "\$zero";
    if (index == 1) return "\$at";
    if (index >= 2 && index <= 3) return "\$v${index - 2}";
    if (index >= 4 && index <= 7) return "\$a${index - 4}";
    if (index >= 8 && index <= 15) return "\$t${index - 8}";
    if (index >= 16 && index <= 23) return "\$s${index - 16}";
    if (index >= 24 && index <= 25) return "\$t${index - 16}"; // t8, t9
    if (index >= 26 && index <= 27) return "\$k${index - 26}";
    if (index == 28) return "\$gp";
    if (index == 29) return "\$sp";
    if (index == 30) return "\$fp";
    if (index == 31) return "\$ra";
    return "R$index";
  }

  @override
  Widget build(BuildContext context) {
    final SimulatorProvider provider = context.watch<SimulatorProvider>();
    final CycleState? currentCycle = provider.currentCycleState;
    final ThemeData theme = Theme.of(context);

    if (currentCycle == null) {
      return const Center(child: Text("Awaiting execution..."));
    }

    final Map<int, int> registers = currentCycle.registerFile;

    return GridView.builder(
      shrinkWrap: true,
      gridDelegate: const SliverGridDelegateWithMaxCrossAxisExtent(
        maxCrossAxisExtent: 120,
        childAspectRatio: 2.0,
        crossAxisSpacing: 8,
        mainAxisSpacing: 8,
      ),
      itemCount: 32,
      itemBuilder: (BuildContext context, int index) {
        final int value = registers[index] ?? 0;
        // Highlight non-zero values slightly to make them pop out
        final bool isZero = value == 0;

        return Container(
          decoration: BoxDecoration(
            color: isZero
                ? theme.colorScheme.surfaceContainer
                : theme.colorScheme.secondaryContainer,
            border: Border.all(color: theme.colorScheme.outlineVariant),
            borderRadius: BorderRadius.circular(8),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                "R$index (${_getRegisterName(index)})",
                style: theme.textTheme.labelSmall?.copyWith(
                  color: isZero
                      ? theme.colorScheme.onSurfaceVariant
                      : theme.colorScheme.onSecondaryContainer,
                ),
              ),
              Text(
                value.toString(),
                style: theme.textTheme.titleMedium?.copyWith(
                  fontWeight: FontWeight.bold,
                  fontFamily: 'monospace',
                  color: isZero
                      ? theme.colorScheme.onSurface
                      : theme.colorScheme.onSecondaryContainer,
                ),
              ),
            ],
          ),
        );
      },
    );
  }
}
