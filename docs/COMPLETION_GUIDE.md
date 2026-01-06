# SKYGUARDIS Completion Guide

This document outlines what needs to be implemented to complete the SKYGUARDIS project.

## Current Status

### ✅ Implemented
- Basic project structure
- Build system (Makefile with CMake fallback)
- C++ C2 node main loop
- Ada gun control main loop
- Threat evaluator (basic implementation)
- Safety interlock checks (basic)
- Ballistics calculations (basic)
- Engagement state machine (skeleton)
- Basic logging

### ❌ Missing / Incomplete

## 1. Inter-Process Communication (HIGH PRIORITY)

**Problem**: C++ C2 node and Ada gun control run independently with no communication.

**Solution**: Implement UDP-based message passing.

### C++ Side (Message Gateway)
- [ ] Implement UDP socket creation and binding
- [ ] Serialize `TargetAssignment` messages to binary/JSON
- [ ] Send messages to Ada process (e.g., localhost:8888)
- [ ] Receive and deserialize `EngagementStatus` messages
- [ ] Handle connection errors gracefully

### Ada Side (Message Receiver)
- [ ] Create UDP socket listener
- [ ] Receive `TargetAssignment` messages
- [ ] Deserialize messages
- [ ] Send `EngagementStatus` messages back
- [ ] Integrate with engagement state machine

**Files to modify:**
- `src/cpp/message_gateway/message_gateway.cpp`
- `src/cpp/message_gateway/protocol.cpp` (add serialization)
- `src/ada/main_gun_control.adb` (add message receiving)
- Create `src/ada/message_handler/message_handler.ads/adb`

## 2. C2 Controller Integration (MEDIUM PRIORITY)

**Problem**: C2 controller processes tracks but doesn't send assignments to gun control.

**Solution**: Connect threat evaluation to message gateway.

**Tasks:**
- [ ] In `c2_controller.cpp`, after prioritizing threats, send highest priority target via gateway
- [ ] Format `TargetAssignment` message from `Track` data
- [ ] Handle gateway send failures
- [ ] Log target assignments

**Files to modify:**
- `src/cpp/c2_controller/c2_controller.cpp`
- `src/cpp/main_c2_node.cpp` (enable gateway)

## 3. Engagement State Machine Integration (HIGH PRIORITY)

**Problem**: State machine exists but doesn't respond to target assignments.

**Solution**: Connect message reception to state machine.

**Tasks:**
- [ ] When target assignment received, transition from `Idle` to `Acquiring`
- [ ] Use ballistics calculator for lead angle calculations
- [ ] Check safety interlocks before transitioning to `Firing`
- [ ] Send engagement status updates back to C2 node
- [ ] Implement state transitions based on target tracking quality

**Files to modify:**
- `src/ada/main_gun_control.adb`
- `src/ada/gun_control/engagement_state_machine.adb`
- Integrate `Ballistics.Ballistic_Calculator`
- Integrate `Safety_Kernel.Safety_Interlocks`

## 4. Enhanced Radar Simulation (MEDIUM PRIORITY)

**Problem**: Radar generates very basic synthetic tracks.

**Solution**: Implement realistic track scenarios.

**Tasks:**
- [ ] Single target scenario (one track, varying range/velocity)
- [ ] Swarm scenario (multiple simultaneous tracks)
- [ ] Saturation scenario (many tracks, high update rate)
- [ ] Track persistence (tracks should persist across cycles)
- [ ] Track motion models (linear, maneuvering)
- [ ] Configurable scenario selection

**Files to modify:**
- `src/cpp/radar_simulator/radar_simulator.cpp`
- Create `src/cpp/radar_simulator/scenario_manager.cpp/hpp`

## 5. Safety Interlock Integration (HIGH PRIORITY)

**Problem**: Safety checks exist but aren't used in firing logic.

**Solution**: Integrate safety checks into engagement flow.

**Tasks:**
- [ ] Check safety before `Firing` state transition
- [ ] Continuously monitor safety during engagement
- [ ] Abort engagement if safety check fails
- [ ] Log safety violations

**Files to modify:**
- `src/ada/gun_control/engagement_state_machine.adb`
- `src/ada/main_gun_control.adb`

## 6. Ballistics Integration (MEDIUM PRIORITY)

**Problem**: Ballistics calculator exists but isn't used.

**Solution**: Use ballistics in engagement calculations.

**Tasks:**
- [ ] Calculate lead angle when tracking target
- [ ] Calculate time-of-flight for engagement planning
- [ ] Use ballistics data in firing solution
- [ ] Update calculations as target moves

**Files to modify:**
- `src/ada/gun_control/engagement_state_machine.adb`
- `src/ada/main_gun_control.adb`

## 7. Graceful Shutdown (LOW PRIORITY)

**Problem**: Ada main loop runs forever with no way to stop.

**Solution**: Add signal handling or file-based stop mechanism.

**Tasks:**
- [ ] Create stop file mechanism (check for `/tmp/skyguardis_stop`)
- [ ] Or use Ada tasking for signal handling
- [ ] Clean shutdown of sockets and resources

**Files to modify:**
- `src/ada/main_gun_control.adb`

## 8. Enhanced Logging (LOW PRIORITY)

**Problem**: Basic logging exists but could be more informative.

**Solution**: Add structured logging with timestamps and levels.

**Tasks:**
- [ ] Add log levels (INFO, WARN, ERROR)
- [ ] Add timestamps to all log entries
- [ ] Log state transitions
- [ ] Log safety violations
- [ ] Log target assignments and engagements

**Files to modify:**
- `src/cpp/logger/logger.cpp/hpp`
- `src/ada/main_gun_control.adb` (add Ada logging)

## 9. Visualization (LOW PRIORITY)

**Problem**: Visualizer is just a placeholder.

**Solution**: Implement basic visualization.

**Tasks:**
- [ ] Display current tracks
- [ ] Display engagement status
- [ ] Display safety status
- [ ] Simple ASCII or basic graphics output

**Files to modify:**
- `src/cpp/logger/visualizer.cpp/hpp`

## 10. Testing (MEDIUM PRIORITY)

**Problem**: Only basic threat evaluator tests exist.

**Solution**: Add comprehensive test coverage.

**Tasks:**
- [ ] Test message serialization/deserialization
- [ ] Test engagement state machine transitions
- [ ] Test safety interlock logic
- [ ] Test ballistics calculations
- [ ] Integration tests for C++/Ada communication

**Files to create/modify:**
- `tests/cpp/test_message_gateway.cpp`
- `tests/ada/test_engagement_state_machine.adb`
- `tests/ada/test_safety_interlocks.adb`
- `tests/ada/test_ballistics.adb` (enhance existing)

## Implementation Priority Order

1. **Inter-Process Communication** - Critical for system to work
2. **Engagement State Machine Integration** - Core functionality
3. **Safety Interlock Integration** - Safety-critical
4. **C2 Controller Integration** - Connects components
5. **Enhanced Radar Simulation** - Improves realism
6. **Ballistics Integration** - Completes engagement logic
7. **Testing** - Ensures correctness
8. **Enhanced Logging** - Improves observability
9. **Graceful Shutdown** - Quality of life
10. **Visualization** - Nice to have

## Quick Start: Minimal Working System

To get a minimal working system, focus on:

1. UDP communication between C++ and Ada
2. C2 controller sending target assignments
3. Gun control receiving assignments and updating state machine
4. Basic safety checks before firing

This will demonstrate the core architecture working end-to-end.

