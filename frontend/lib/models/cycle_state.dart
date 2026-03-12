class PipelineTrace {
  final String instructionFetch;
  final String instructionDecode;
  final String execute;
  final String memoryAccess;

  PipelineTrace({
    required this.instructionFetch,
    required this.instructionDecode,
    required this.execute,
    required this.memoryAccess,
  });

  factory PipelineTrace.fromJson(Map<String, dynamic> jsonMap) {
    if (!jsonMap.containsKey('IF') ||
        !jsonMap.containsKey('ID') ||
        !jsonMap.containsKey('EX') ||
        !jsonMap.containsKey('MEM')) {
      throw FormatException(
        "Malformed pipeline JSON: Missing required stage keys.",
      );
    }

    return PipelineTrace(
      instructionFetch: jsonMap['IF'] as String,
      instructionDecode: jsonMap['ID'] as String,
      execute: jsonMap['EX'] as String,
      memoryAccess: jsonMap['MEM'] as String,
    );
  }
}

class CycleState {
  final int clockCycle;
  final PipelineTrace pipelineLatches;
  final Map<int, int> registerFile;

  CycleState({
    required this.clockCycle,
    required this.pipelineLatches,
    required this.registerFile,
  });

  factory CycleState.fromJson(Map<String, dynamic> jsonMap) {
    if (!jsonMap.containsKey('cycle') ||
        !jsonMap.containsKey('pipeline') ||
        !jsonMap.containsKey('registers')) {
      throw FormatException("Malformed cycle JSON: Missing top-level keys.");
    }

    // Safely parse the 32 registers from string keys ("0", "1") to integer keys
    final Map<String, dynamic> rawRegisters =
        jsonMap['registers'] as Map<String, dynamic>;
    final Map<int, int> parsedRegisters = {};

    rawRegisters.forEach((key, value) {
      final int? registerIndex = int.tryParse(key);
      if (registerIndex == null) {
        throw FormatException("Invalid register index encountered: $key");
      }
      parsedRegisters[registerIndex] = value as int;
    });

    return CycleState(
      clockCycle: jsonMap['cycle'] as int,
      pipelineLatches: PipelineTrace.fromJson(
        jsonMap['pipeline'] as Map<String, dynamic>,
      ),
      registerFile: parsedRegisters,
    );
  }
}
