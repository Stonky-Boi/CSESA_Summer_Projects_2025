import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/simulator_provider.dart';
import '../models/cycle_state.dart';

class MemoryViewer extends StatelessWidget {
  const MemoryViewer({super.key});

  @override
  Widget build(BuildContext context) {
    final SimulatorProvider provider = context.watch<SimulatorProvider>();
    final CycleState? currentCycle = provider.currentCycleState;
    final ThemeData theme = Theme.of(context);

    if (currentCycle == null) {
      return const Center(child: Text("Memory uninitialized."));
    }

    final Map<int, int> memory = currentCycle.dataMemory;

    if (memory.isEmpty) {
      return Card(
        elevation: 2,
        child: Center(
          child: Padding(
            padding: const EdgeInsets.all(16.0),
            child: Text(
              "No data in memory.",
              style: theme.textTheme.bodyMedium,
            ),
          ),
        ),
      );
    }

    // Sort the memory addresses so they appear in sequential order
    final List<int> sortedAddresses = memory.keys.toList()..sort();

    return Card(
      elevation: 2,
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Container(
            padding: const EdgeInsets.all(12.0),
            color: theme.colorScheme.surfaceContainerHighest,
            child: Text(
              "Data Memory",
              style: theme.textTheme.titleMedium?.copyWith(
                fontWeight: FontWeight.bold,
              ),
              textAlign: TextAlign.center,
            ),
          ),
          Expanded(
            child: ListView.separated(
              itemCount: sortedAddresses.length,
              separatorBuilder: (context, index) => const Divider(height: 1),
              itemBuilder: (context, index) {
                final int address = sortedAddresses[index];
                final int value = memory[address]!;

                // Format the address as an 8-character hex string (e.g., 0x10010000)
                final String hexAddress =
                    "0x${address.toRadixString(16).padLeft(8, '0').toUpperCase()}";

                return ListTile(
                  dense: true,
                  leading: const Icon(Icons.memory, size: 20),
                  title: Text(
                    hexAddress,
                    style: const TextStyle(fontFamily: 'monospace'),
                  ),
                  trailing: Text(
                    value.toString(),
                    style: const TextStyle(
                      fontFamily: 'monospace',
                      fontWeight: FontWeight.bold,
                      fontSize: 16,
                    ),
                  ),
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}
