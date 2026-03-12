import 'dart:io';
import 'dart:convert';
import '../models/cycle_state.dart';

class SimulatorService {
  /// Executes the Ember C++ binary and yields a stream of CycleState objects.
  /// Note: The relative path assumes the Flutter app is run from the frontend/ directory.
  Stream<CycleState> runSimulation(
    String assemblyFilePath,
    String predictorType,
  ) async* {
    final String executablePath = '../backend/build/ember';

    if (!File(executablePath).existsSync()) {
      throw ProcessException(
        executablePath,
        [],
        "Ember binary not found. Please compile the C++ backend first.",
      );
    }

    if (!File(assemblyFilePath).existsSync()) {
      throw FileSystemException("Assembly file not found.", assemblyFilePath);
    }

    final Process emberProcess = await Process.start(executablePath, [
      assemblyFilePath,
      '--mode=pipeline',
      '--trace',
      '--json',
      '--predictor=$predictorType',
    ]);

    // Listen for any fatal errors from the C++ backend and throw them in Dart
    emberProcess.stderr.transform(utf8.decoder).listen((String errorMessage) {
      if (errorMessage.trim().isNotEmpty) {
        throw Exception("Ember Backend Error: ${errorMessage.trim()}");
      }
    });

    // Process the standard output line-by-line
    final Stream<String> lineStream = emberProcess.stdout
        .transform(utf8.decoder)
        .transform(const LineSplitter());

    await for (final String consoleLine in lineStream) {
      try {
        final Map<String, dynamic> decodedJson = jsonDecode(consoleLine);

        // We only yield states that are explicitly marked as cycle traces.
        // This naturally ignores the execution report at the end of the program.
        if (decodedJson['type'] == 'cycle') {
          yield CycleState.fromJson(decodedJson);
        }
      } on FormatException {
        // If it is not valid JSON, it might be a SYSCALL print statement.
        // For now, we print it to the Flutter debug console.
        print("Simulator Output: $consoleLine");
      }
    }

    final int exitCode = await emberProcess.exitCode;
    if (exitCode != 0) {
      throw ProcessException(
        executablePath,
        [],
        "Simulator exited abnormally with code $exitCode.",
      );
    }
  }
}
