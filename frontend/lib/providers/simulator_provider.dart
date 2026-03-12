import 'dart:async';
import 'package:flutter/foundation.dart';
import '../models/cycle_state.dart';
import '../services/simulator_service.dart';

class SimulatorProvider extends ChangeNotifier {
  final SimulatorService _simulatorService = SimulatorService();

  // The complete timeline of the CPU execution
  final List<CycleState> _cycleHistory = [];

  // The specific point in time the user is currently viewing
  int _currentCycleIndex = 0;

  // Status flags for the UI
  bool _isSimulationRunning = false;
  String? _fatalErrorMessage;

  // --- Getters for the UI ---

  bool get isSimulationRunning => _isSimulationRunning;
  bool get hasData => _cycleHistory.isNotEmpty;
  String? get fatalErrorMessage => _fatalErrorMessage;

  int get totalCycles => _cycleHistory.length;
  int get currentCycleNumber =>
      hasData ? _cycleHistory[_currentCycleIndex].clockCycle : 0;

  CycleState? get currentCycleState {
    if (_cycleHistory.isEmpty) {
      return null;
    }
    return _cycleHistory[_currentCycleIndex];
  }

  bool get canStepForward =>
      hasData && _currentCycleIndex < _cycleHistory.length - 1;
  bool get canStepBackward => hasData && _currentCycleIndex > 0;

  // --- Actions ---

  /// Clears the current state and runs the C++ backend
  Future<void> executeAssemblyProgram(
    String assemblyFilePath,
    String predictorType,
  ) async {
    _cycleHistory.clear();
    _currentCycleIndex = 0;
    _isSimulationRunning = true;
    _fatalErrorMessage = null;

    // Notify the UI to show a loading spinner
    notifyListeners();

    try {
      final Stream<CycleState> cycleStream = _simulatorService.runSimulation(
        assemblyFilePath,
        predictorType,
      );

      await for (final CycleState cycleSnapshot in cycleStream) {
        _cycleHistory.add(cycleSnapshot);
      }
    } catch (error) {
      _fatalErrorMessage = error.toString();
    } finally {
      _isSimulationRunning = false;
      // Notify the UI to remove the loading spinner and render the first cycle
      notifyListeners();
    }
  }

  /// Advances the CPU simulator by one clock cycle
  void stepForward() {
    if (canStepForward) {
      _currentCycleIndex++;
      notifyListeners();
    } else {
      throw StateError(
        "Cannot step forward: Already at the final clock cycle.",
      );
    }
  }

  /// Rewinds the CPU simulator by one clock cycle
  void stepBackward() {
    if (canStepBackward) {
      _currentCycleIndex--;
      notifyListeners();
    } else {
      throw StateError(
        "Cannot step backward: Already at the first clock cycle.",
      );
    }
  }

  /// Instantly jumps to the final clock cycle
  void jumpToEnd() {
    if (hasData) {
      _currentCycleIndex = _cycleHistory.length - 1;
      notifyListeners();
    }
  }

  /// Instantly jumps back to cycle 1
  void jumpToBeginning() {
    if (hasData) {
      _currentCycleIndex = 0;
      notifyListeners();
    }
  }

  /// Clears all data from memory
  void resetSimulation() {
    _cycleHistory.clear();
    _currentCycleIndex = 0;
    _isSimulationRunning = false;
    _fatalErrorMessage = null;
    notifyListeners();
  }
}
