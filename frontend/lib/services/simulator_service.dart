import 'dart:io';
import 'dart:convert';
import 'package:flutter/services.dart' show rootBundle;
import 'package:flutter/foundation.dart';
import '../models/cycle_state.dart';

class SimulatorService {
  Stream<CycleState> runSimulation(
    String assemblyFilePath,
    String predictorType,
  ) async* {
    // 1. Extract the binary from the app bundle to the system temp folder
    final Directory tempDirectory = Directory.systemTemp;
    final File executableFile = File('${tempDirectory.path}/ember_executable');

    // Only unpack it if it hasn't been unpacked yet during this session
    if (!await executableFile.exists()) {
      final ByteData binaryData = await rootBundle.load('assets/ember');
      await executableFile.writeAsBytes(
        binaryData.buffer.asUint8List(
          binaryData.offsetInBytes,
          binaryData.lengthInBytes,
        ),
      );

      // Grant Unix execute permissions to the unpacked binary
      await Process.run('chmod', ['+x', executableFile.path]);
    }

    if (!File(assemblyFilePath).existsSync()) {
      throw FileSystemException("Assembly file not found.", assemblyFilePath);
    }

    // 2. Spawn the process using the newly unpacked executable
    final Process emberProcess = await Process.start(executableFile.path, [
      assemblyFilePath,
      '--mode=pipeline',
      '--trace',
      '--json',
      '--predictor=$predictorType',
    ]);

    // Listen for any fatal errors from the C++ backend
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
        if (decodedJson['type'] == 'cycle') {
          yield CycleState.fromJson(decodedJson);
        }
      } on FormatException {
        // Use debugPrint instead of print for production-safe logging
        debugPrint("Simulator Output: $consoleLine");
      }
    }

    final int exitCode = await emberProcess.exitCode;
    if (exitCode != 0) {
      throw ProcessException(
        executableFile.path,
        [],
        "Simulator exited abnormally with code $exitCode.",
      );
    }
  }
}
