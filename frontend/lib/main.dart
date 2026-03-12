import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'providers/simulator_provider.dart';
import 'screens/simulator_screen.dart';

void main() {
  runApp(
    ChangeNotifierProvider(
      create: (BuildContext context) => SimulatorProvider(),
      child: const EmberApp(),
    ),
  );
}

class EmberApp extends StatelessWidget {
  const EmberApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Ember Simulator',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(
          seedColor: Colors.deepOrange,
          brightness:
              Brightness.dark, // Dark mode fits IDEs and hardware tools well!
        ),
        useMaterial3: true,
      ),
      home: const SimulatorScreen(),
    );
  }
}
