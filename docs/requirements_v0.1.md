# SKYGUARDIS Requirements Document v1.0

## Document Information
- **Version**: 1.0
- **Date**: 2025-01-06
- **Status**: Final
- **Project**: SKYGUARDIS - Short-Range Air Defence Simulation

---

## 1. Executive Summary

SKYGUARDIS is a software-in-the-loop simulator that models a short-range air-defence system. The system emulates aerial objects (targets) with realistic motion dynamics, processes radar-like sensor data, evaluates threats, and executes engagement sequences with safety-critical constraints enforced by Ada components.

---

## 2. System Overview

### 2.1 Architecture
The system consists of:
- **Radar Simulator (C++)**: Generates synthetic radar tracks with realistic object motion
- **Command & Control Node (C++)**: Threat evaluation and weapon assignment
- **Gun Control Computer (Ada)**: Safety-critical engagement execution
- **Message Gateway (C++)**: Inter-process communication layer

### 2.2 Coordinate System
- **Origin**: Radar/defence system position (fixed reference point)
- **Azimuth (θ)**: Horizontal angle measured from North (0°) clockwise (0° to 360° or -180° to +180°)
- **Elevation (φ)**: Vertical angle measured from horizontal plane (-90° to +90°)
- **Range (r)**: Distance from radar to target in meters

---

## 3. Object Emulation Requirements

### 3.1 Object Definition
An **emulated object** (target) shall be defined by:

#### 3.1.1 Positional State
- **3D Position**: (x, y, z) in Cartesian coordinates relative to radar origin
- **Range (r)**: Distance from radar: `r = √(x² + y² + z²)`
- **Azimuth (θ)**: Horizontal bearing: `θ = atan2(y, x)` (radians)
- **Elevation (φ)**: Vertical angle: `φ = arcsin(z / r)` (radians)

#### 3.1.2 Kinematic State
- **Velocity Vector**: (vx, vy, vz) in m/s
- **Speed**: Magnitude of velocity vector: `|v| = √(vx² + vy² + vz²)`
- **Heading**: Direction of motion in horizontal plane (radians)
- **Climb Rate**: Vertical velocity component (m/s)

#### 3.1.3 Object Properties
- **Object ID**: Unique identifier (uint32_t)
- **Object Type**: Classification (e.g., aircraft, missile, drone, unknown)
- **Radar Cross Section (RCS)**: Signal strength modifier
- **Maneuverability**: Maximum turn rate (rad/s)

### 3.2 Object Motion Model

#### 3.2.1 Motion Dynamics
The system shall simulate object motion using:
- **Linear Motion**: Constant velocity segments
- **Maneuvering**: Random or scripted course changes
- **Physics Constraints**: Realistic acceleration limits
- **Altitude Changes**: Climb/descent with rate limits

#### 3.2.2 Update Model
- **Update Rate**: 10 Hz (100ms period) - configurable
- **State Propagation**: Position updated via `position(t+Δt) = position(t) + velocity(t) × Δt`
- **Velocity Updates**: May include acceleration for maneuvering objects

### 3.3 Radar Output Representation

#### 3.3.1 Radar Coordinate System
All radar outputs shall be expressed in **spherical coordinates** relative to the radar:
- **Azimuth (θ)**: Measured from North, clockwise positive
- **Elevation (φ)**: Measured from horizontal plane, positive upward
- **Range (r)**: Direct distance to target

#### 3.3.2 Coordinate Transformations
The system shall maintain bidirectional transformations:
- **Cartesian → Spherical**: For radar output generation
- **Spherical → Cartesian**: For motion calculations and visualization

#### 3.3.3 Radar Measurement Model
Radar outputs shall include:
- **Measured Range**: `r_measured = r_true + noise_range`
- **Measured Azimuth**: `θ_measured = θ_true + noise_azimuth`
- **Measured Elevation**: `φ_measured = φ_true + noise_elevation`
- **Measurement Noise**: Gaussian white noise with configurable variance
- **Update Timestamp**: Time of measurement

### 3.4 Realism and Randomness

#### 3.4.1 Motion Randomness
The system shall incorporate randomness for realism:
- **Initial Position**: Random within defined scenario bounds
- **Initial Velocity**: Random direction and magnitude within limits
- **Maneuver Timing**: Random intervals between course changes
- **Maneuver Magnitude**: Random turn angles within physical limits
- **Speed Variations**: Random speed changes within ±10% of nominal

#### 3.4.2 Sensor Noise Model
- **Range Noise**: Gaussian, σ = 5-50m (range-dependent)
- **Angle Noise**: Gaussian, σ = 0.01-0.1 rad (range-dependent)
- **Update Jitter**: ±5ms timing variation
- **Dropout Probability**: Configurable (0-5%) for sensor failures

#### 3.4.3 Scenario Variability
- **Multiple Objects**: Simultaneous tracking of 1-20 objects
- **Object Types**: Different motion characteristics per type
- **Entry/Exit**: Objects may enter or leave engagement zone
- **Clutter**: Optional false targets (configurable)

---

## 4. System Reaction Requirements

### 4.1 Relative Position Calculation

#### 4.1.1 Defence System Position
The defence system (radar/gun) shall have:
- **Fixed Position**: (x_def, y_def, z_def) in world coordinates
- **Orientation**: Azimuth offset (if radar not aligned with North)
- **Coordinate Frame**: Defined relative to world origin

#### 4.1.2 Relative Geometry
For each object, the system shall compute:
- **Relative Position**: `(Δx, Δy, Δz) = (x_obj - x_def, y_obj - y_def, z_obj - z_def)`
- **Relative Range**: `r_rel = √(Δx² + Δy² + Δz²)`
- **Relative Azimuth**: `θ_rel = atan2(Δy, Δx)`
- **Relative Elevation**: `φ_rel = arcsin(Δz / r_rel)`
- **Closing Velocity**: `v_closing = -d(r_rel)/dt`
- **Aspect Angle**: Angle between object heading and line-of-sight

### 4.2 Threat Evaluation

#### 4.2.1 Threat Scoring
The system shall evaluate threat based on:
- **Range Factor**: Closer = higher threat (inverse relationship)
- **Velocity Factor**: Faster = higher threat
- **Closing Rate**: Negative closing rate (approaching) = higher threat
- **Aspect Angle**: Head-on = higher threat than crossing
- **Object Type**: Known threat types weighted higher

#### 4.2.2 Prioritization
- **Threat Score**: Weighted combination of factors (0.0 - 1.0)
- **Priority Ranking**: Sort by threat score (descending)
- **Assignment Logic**: Assign highest priority target to gun control

### 4.3 Engagement Response

#### 4.3.1 Target Assignment
When a target is assigned:
- **Assignment Message**: Contains (target_id, range, azimuth, elevation, velocity, priority)
- **Transmission**: Sent via message gateway to gun control
- **Acknowledgment**: Wait for gun control acknowledgment

#### 4.3.2 Engagement Tracking
During engagement:
- **Track Updates**: Continue updating target position
- **Status Monitoring**: Receive engagement status from gun control
- **Re-evaluation**: May assign new target if current engagement completes

---

## 5. Safety Layer Requirements (Ada Implementation)

### 5.1 Safety-Critical Components

All safety-critical logic **MUST** be implemented in Ada to ensure:
- **Type Safety**: Strong typing prevents invalid values
- **Range Checking**: Runtime bounds checking
- **Determinism**: Predictable execution timing
- **Verifiability**: SPARK-compatible design for formal verification

### 5.2 Safety Interlocks

#### 5.2.1 No-Fire Zones
The Ada safety kernel shall enforce:
- **Ground Clearance**: Minimum elevation angle (e.g., -10° to prevent ground impact)
- **Friendly Zones**: Azimuth sectors where firing is prohibited
- **Range Limits**: Minimum range (e.g., 100m) and maximum range (e.g., 10km)
- **Angle Limits**: Mechanical limits of gun mount (azimuth: ±180°, elevation: -10° to +85°)

#### 5.2.2 Continuous Safety Monitoring
- **Pre-Fire Check**: Validate all constraints before `Firing` state transition
- **During-Fire Check**: Continuous monitoring during engagement
- **Abort Condition**: Immediate transition to `Idle` if safety violation detected
- **Safety Status**: Report safety status to C2 node

#### 5.2.3 Safety State Machine
```
Idle → [Safety Check Pass] → Acquiring
Acquiring → [Safety Check Pass] → Tracking
Tracking → [Safety Check Pass] → Firing
[Any State] → [Safety Violation] → Idle (Abort)
```

### 5.3 Ballistics Safety

#### 5.3.1 Trajectory Validation
- **Impact Point**: Calculate expected impact location
- **Clearance Check**: Verify no ground/friendly impact
- **Time-of-Flight**: Validate engagement window

#### 5.3.2 Physical Constraints
- **Gun Limits**: Respect mechanical constraints
- **Ammunition Limits**: Track remaining ammunition
- **Rate Limits**: Enforce maximum firing rate

### 5.4 Error Handling

#### 5.4.1 Input Validation
- **Range Validation**: 0 < range < max_range
- **Angle Validation**: Azimuth and elevation within limits
- **Velocity Validation**: Realistic velocity magnitudes
- **Type Validation**: Ensure numeric types are within valid ranges

#### 5.4.2 Failure Modes
- **Invalid Input**: Reject and report error, remain in safe state
- **Communication Failure**: Default to safe state (Idle)
- **Calculation Overflow**: Detect and abort engagement
- **State Inconsistency**: Reset to Idle state

---

## 6. Control Flow Requirements

### 6.1 Execution Control

#### 6.1.1 Graceful Shutdown
- **Signal Handling**: Respond to SIGINT (Ctrl+C) and SIGTERM
- **C++ Process**: Set running flag to false, exit main loop, close sockets
- **Ada Process**: Check for stop condition, exit main loop, close sockets
- **Cleanup**: Release all resources (sockets, files, memory)
- **Final Logging**: Write shutdown message to log

#### 6.1.2 Stop Mechanism
The system shall support multiple stop mechanisms:
- **Keyboard Interrupt**: Ctrl+C sends SIGINT to both processes
- **Stop File**: Optional file-based stop (`/tmp/skyguardis_stop`)
- **Timeout**: Optional maximum runtime limit
- **Error Condition**: Critical errors trigger graceful shutdown

#### 6.1.3 Process Coordination
- **C++ Process**: Main emulator loop, generates tracks, evaluates threats
- **Ada Process**: Real-time control loop, processes engagements
- **Synchronization**: Message-based coordination via UDP
- **Independent Shutdown**: Each process handles its own shutdown

### 6.2 Real-Time Requirements

#### 6.2.1 Timing Constraints
- **Radar Update Rate**: 10 Hz (100ms period) - hard requirement
- **C2 Processing**: < 50ms per cycle
- **Ada Control Loop**: 10 Hz (100ms period) - deterministic
- **Message Latency**: < 10ms for UDP communication

#### 6.2.2 Determinism
- **Ada Execution**: Deterministic timing (no dynamic allocation in hot path)
- **State Machine**: Predictable state transitions
- **Calculations**: Fixed-point or bounded floating-point operations

---

## 7. Data Flow Requirements

### 7.1 Object → Radar → C2 Flow

```
Object (Cartesian) 
  → Motion Model Update
  → Coordinate Transform (Cartesian → Spherical)
  → Add Sensor Noise
  → Radar Output (θ, φ, r)
  → C2 Track Processing
  → Threat Evaluation
  → Target Assignment Decision
```

### 7.2 C2 → Gun Control Flow

```
C2 Target Assignment
  → Message Serialization
  → UDP Transmission
  → Gun Control Reception
  → Message Deserialization
  → Safety Validation (Ada)
  → State Machine Update
  → Engagement Execution
```

### 7.3 Gun Control → C2 Flow

```
Engagement Status
  → Status Serialization
  → UDP Transmission
  → C2 Reception
  → Status Update
  → Logging/Visualization
```

---

## 8. Inter-Process Communication Requirements (CRITICAL)

### 8.1 Communication Protocol

#### 8.1.1 Transport Layer
- **Protocol**: UDP (User Datagram Protocol)
- **Address**: Localhost (127.0.0.1) for development
- **Ports**: 
  - C2 → Gun Control: Port 8888 (target assignments)
  - Gun Control → C2: Port 8889 (engagement status)
- **Reliability**: Application-level acknowledgment for critical messages

#### 8.1.2 Message Types
The system shall support the following message types:
- **TARGET_ASSIGNMENT** (C++ → Ada): New target assignment from C2
- **ENGAGEMENT_STATUS** (Ada → C++): Current engagement state and status
- **SAFETY_INTERLOCK** (Ada → C++): Safety violation notifications
- **HEARTBEAT** (Bidirectional): Keep-alive messages

#### 8.1.3 Message Format
Messages shall be serialized in a binary format for efficiency:

**TargetAssignment Message:**
```
[MessageType: 1 byte][TargetID: 4 bytes][Range: 8 bytes][Azimuth: 8 bytes]
[Elevation: 8 bytes][Velocity: 8 bytes][Priority: 1 byte][Checksum: 2 bytes]
Total: 40 bytes
```

**EngagementStatus Message:**
```
[MessageType: 1 byte][TargetID: 4 bytes][State: 1 byte][Firing: 1 byte]
[LeadAngle: 8 bytes][TimeToImpact: 8 bytes][Checksum: 2 bytes]
Total: 25 bytes
```

#### 8.1.4 Serialization Requirements
- **Endianness**: Network byte order (big-endian)
- **Floating Point**: IEEE 754 double precision
- **Integers**: Unsigned, network byte order
- **Checksum**: Simple 16-bit checksum for integrity verification
- **Versioning**: Message format version in header (for future compatibility)

### 8.2 C++ Message Gateway Implementation

#### 8.2.1 UDP Socket Management
- **Socket Creation**: Create UDP socket on initialization
- **Socket Binding**: Bind to localhost, ephemeral port for C2 side
- **Socket Options**: Set socket options for timeout and buffer sizes
- **Error Handling**: Handle socket errors gracefully, log failures

#### 8.2.2 Message Sending
- **Target Assignment Sending**:
  - Serialize `TargetAssignment` struct to binary format
  - Calculate checksum
  - Send to gun control (localhost:8888)
  - Handle send failures (log error, retry optional)
  - Return success/failure status

#### 8.2.3 Message Receiving
- **Engagement Status Receiving**:
  - Listen on port 8889 for status messages
  - Non-blocking receive with timeout (100ms)
  - Deserialize binary data to `EngagementStatus` struct
  - Validate checksum
  - Return status or empty status on timeout/error

#### 8.2.4 Connection Management
- **Initialization**: Establish sockets on startup
- **Heartbeat**: Optional periodic heartbeat messages
- **Cleanup**: Close sockets on shutdown
- **Reconnection**: Handle connection failures gracefully

### 8.3 Ada Message Handler Implementation

#### 8.3.1 UDP Socket Management
- **Socket Creation**: Use GNAT.Sockets package
- **Socket Binding**: Bind to localhost:8888 for receiving
- **Socket Options**: Configure for non-blocking operation
- **Error Handling**: Raise exceptions on critical errors, log warnings

#### 8.3.2 Message Receiving
- **Target Assignment Reception**:
  - Receive UDP datagram on port 8888
  - Deserialize binary data to Ada record type
  - Validate checksum
  - Validate message format (type, size)
  - Return received assignment or indicate error

#### 8.3.3 Message Sending
- **Engagement Status Sending**:
  - Serialize Ada record to binary format
  - Calculate checksum
  - Send to C2 node (localhost:8889)
  - Handle send failures (log, continue operation)

#### 8.3.4 Integration with State Machine
- **Message Reception Loop**: Periodic check for incoming messages (10 Hz)
- **State Machine Trigger**: Received target assignment triggers state transition
- **Status Updates**: State machine changes trigger status message sending
- **Thread Safety**: Use Ada protected objects for message queue

### 8.4 Message Protocol Details

#### 8.4.1 Target Assignment Protocol
1. C2 evaluates threats and selects highest priority target
2. C2 formats `TargetAssignment` message with target data
3. C2 sends message via UDP to gun control
4. Gun control receives and validates message
5. Gun control acknowledges (implicit via state change or explicit ACK)
6. Gun control begins engagement sequence

#### 8.4.2 Engagement Status Protocol
1. Gun control updates engagement state
2. Gun control formats `EngagementStatus` message
3. Gun control sends status to C2 (periodic or on state change)
4. C2 receives and processes status
5. C2 logs status and updates visualization
6. C2 may assign new target based on status

#### 8.4.3 Error Handling
- **Invalid Message**: Log error, discard message, remain in safe state
- **Checksum Mismatch**: Log error, discard message, request retransmission (optional)
- **Socket Error**: Log error, attempt reconnection, default to safe state
- **Timeout**: Handle gracefully, continue operation

---

## 9. Component Integration Requirements

### 9.1 C2 Controller to Message Gateway Integration

#### 9.1.1 Target Assignment Flow
- **Trigger**: After threat evaluation and prioritization
- **Condition**: Highest priority target exceeds threat threshold
- **Action**: 
  1. Format `TargetAssignment` from `Track` data
  2. Convert Cartesian coordinates to spherical (azimuth, elevation, range)
  3. Call `MessageGateway::sendTargetAssignment()`
  4. Log assignment (target ID, priority, coordinates)
  5. Handle send failure (log error, may retry or skip)

#### 9.1.2 Status Monitoring
- **Periodic Check**: Check for `EngagementStatus` messages (10 Hz)
- **Status Processing**: Update internal state based on gun control status
- **Re-assignment Logic**: May assign new target if current engagement completes
- **Logging**: Log all status updates

### 9.2 Gun Control Message Reception Integration

#### 9.2.1 Message Reception Loop
- **Frequency**: Check for messages every 100ms (10 Hz)
- **Non-Blocking**: Use non-blocking socket operations
- **Timeout**: 50ms timeout per receive attempt
- **Processing**: Process received messages immediately

#### 9.2.2 State Machine Integration
- **Target Assignment Reception**:
  - Validate assignment (range, angles within limits)
  - If in `Idle` state, transition to `Acquiring`
  - Store target data in engagement context
  - Begin tracking target
- **State Updates**:
  - On state transitions, send `EngagementStatus` message
  - Include current state, target ID, firing status
  - Include calculated lead angle and time-to-impact

#### 9.2.3 Safety Integration
- **Pre-Processing**: Validate target assignment against safety limits
- **Rejection**: If safety check fails, reject assignment, send status with error
- **Continuous Monitoring**: Check safety during all engagement states
- **Abort**: On safety violation, abort engagement, transition to `Idle`

### 9.3 Ballistics Integration in Engagement

#### 9.3.1 Lead Angle Calculation
- **Trigger**: When in `Tracking` or `Firing` state
- **Input**: Target range, velocity, heading, projectile velocity
- **Calculation**: Call `Ballistics.Ballistic_Calculator.Calculate_Lead_Angle()`
- **Update**: Recalculate lead angle as target moves (every cycle)
- **Usage**: Use lead angle for gun pointing direction

#### 9.3.2 Time-of-Flight Calculation
- **Trigger**: When preparing to fire
- **Input**: Target range, elevation, projectile velocity
- **Calculation**: Call `Ballistics.Ballistic_Calculator.Calculate_Time_Of_Flight()`
- **Usage**: 
  - Estimate time to impact
  - Validate engagement window
  - Include in status messages

### 9.4 Safety Interlock Integration

#### 9.4.1 Pre-Fire Safety Check
- **Trigger**: Before transitioning from `Tracking` to `Firing`
- **Checks**:
  1. Call `Safety_Kernel.Safety_Interlocks.Is_Safe_To_Fire()`
  2. Validate azimuth and elevation angles
  3. Check no-fire zones
  4. Verify range limits
- **Action**: 
  - If safe: Proceed to `Firing` state
  - If unsafe: Abort engagement, transition to `Idle`, send status with safety violation

#### 9.4.2 Continuous Safety Monitoring
- **Frequency**: Every control cycle (10 Hz)
- **Checks**: All safety conditions during engagement
- **Action**: If violation detected, immediately abort engagement

---

## 10. Enhanced Radar Simulation Requirements

### 10.1 Scenario Types

#### 10.1.1 Single Target Scenario
- **Description**: One object moving through engagement zone
- **Parameters**:
  - Initial position: Random within bounds
  - Initial velocity: Random direction, 50-300 m/s
  - Motion: Linear or simple maneuvering
  - Duration: Until object leaves zone or engaged

#### 10.1.2 Swarm Scenario
- **Description**: Multiple objects (3-10) simultaneously
- **Parameters**:
  - Object count: Configurable (3-10)
  - Entry timing: Staggered entry times
  - Motion: Independent motion for each object
  - Challenge: System must prioritize and engage multiple threats

#### 10.1.3 Saturation Scenario
- **Description**: Many objects (10-20) with high update rate
- **Parameters**:
  - Object count: 10-20
  - Update rate: High (may stress system)
  - Motion: Various patterns
  - Challenge: System performance under load

### 10.2 Track Persistence

#### 10.2.1 Track Management
- **Track Lifecycle**: 
  - Creation: When object enters detection zone
  - Update: Every cycle while in zone
  - Deletion: When object leaves zone or destroyed
- **Track ID**: Persistent ID for each object
- **Track History**: Maintain position history for prediction

#### 10.2.2 Track Association
- **Association Logic**: Match new detections to existing tracks
- **Track Update**: Update existing track with new measurement
- **New Track**: Create new track if no association found
- **Track Loss**: Handle track loss (sensor dropout)

### 10.3 Motion Models

#### 10.3.1 Linear Motion
- **Model**: Constant velocity
- **Update**: `position(t+Δt) = position(t) + velocity × Δt`
- **Use Case**: Straight-line targets

#### 10.3.2 Maneuvering Motion
- **Model**: Velocity changes with acceleration
- **Update**: `velocity(t+Δt) = velocity(t) + acceleration × Δt`
- **Acceleration**: Random or scripted course changes
- **Use Case**: Evasive targets

#### 10.3.3 Realistic Constraints
- **Maximum Speed**: 50-500 m/s (configurable)
- **Maximum Acceleration**: 50 m/s² (configurable)
- **Turn Rate**: Maximum angular velocity (rad/s)
- **Altitude Limits**: Minimum and maximum altitude

---

## 11. Testing Requirements (Enhanced)

### 11.1 Communication Testing

#### 11.1.1 Message Serialization Tests
- **Test**: Serialize and deserialize `TargetAssignment` message
- **Verify**: All fields correctly serialized/deserialized
- **Test**: Serialize and deserialize `EngagementStatus` message
- **Verify**: Checksum calculation and validation

#### 11.1.2 UDP Communication Tests
- **Test**: C++ sends message, Ada receives correctly
- **Test**: Ada sends message, C++ receives correctly
- **Test**: Handle message loss (simulate network issues)
- **Test**: Handle invalid messages (corrupted data)
- **Test**: Handle timeout scenarios

#### 11.1.3 Integration Tests
- **Test**: End-to-end communication (C2 → Gun Control → C2)
- **Test**: Multiple message exchanges
- **Test**: Concurrent message handling
- **Test**: Error recovery (socket errors, timeouts)

### 11.2 State Machine Testing

#### 11.2.1 State Transition Tests
- **Test**: Valid state transitions (Idle → Acquiring → Tracking → Firing)
- **Test**: Invalid state transitions (reject invalid transitions)
- **Test**: State transitions triggered by message reception
- **Test**: State transitions triggered by safety violations

#### 11.2.2 Engagement Flow Tests
- **Test**: Complete engagement sequence
- **Test**: Engagement abort on safety violation
- **Test**: Multiple engagements (sequential)
- **Test**: Engagement timeout

### 11.3 Safety Testing

#### 11.3.1 Safety Interlock Tests
- **Test**: All no-fire zone conditions
- **Test**: Angle limit enforcement
- **Test**: Range limit enforcement
- **Test**: Safety violation detection and abort

#### 11.3.2 Edge Case Tests
- **Test**: Boundary conditions (minimum/maximum ranges, angles)
- **Test**: Invalid input handling
- **Test**: Error condition handling

---

## 12. Implementation Status and Priorities

### 12.1 Completed (v0.1)
- ✅ Basic project structure
- ✅ Build system (Makefile with CMake fallback)
- ✅ C++ C2 node main loop
- ✅ Ada gun control main loop
- ✅ Threat evaluator (basic)
- ✅ Safety interlock checks (basic)
- ✅ Ballistics calculations (basic)
- ✅ Engagement state machine (skeleton)
- ✅ Basic logging
- ✅ Graceful shutdown (Ctrl+C) for C++ process

### 12.2 Critical Missing (v1.0 - HIGH PRIORITY)
- ❌ **Inter-Process Communication (UDP)**: C++ and Ada communication
- ❌ **Message Serialization**: Binary message format
- ❌ **C2 Controller Integration**: Send target assignments via gateway
- ❌ **Gun Control Message Reception**: Receive and process assignments
- ❌ **State Machine Integration**: Connect messages to state transitions
- ❌ **Safety Integration**: Use safety checks in engagement flow
- ❌ **Ballistics Integration**: Use ballistics in engagement calculations
- ❌ **Graceful Shutdown (Ada)**: Ctrl+C handling for Ada process

### 12.3 Important Missing (v1.0 - MEDIUM PRIORITY)
- ❌ **Enhanced Radar Simulation**: Scenarios (single, swarm, saturation)
- ❌ **Track Persistence**: Maintain tracks across cycles
- ❌ **Motion Models**: Linear and maneuvering motion
- ❌ **Comprehensive Testing**: Communication, state machine, safety tests

### 12.4 Nice-to-Have (Future Versions)
- ⏳ **Enhanced Logging**: Structured logging with levels and timestamps
- ⏳ **Visualization**: Real-time visualization of tracks and engagements
- ⏳ **Scenario Configuration**: Configurable scenario parameters
- ⏳ **Performance Optimization**: Latency optimization

---

## 13. Acceptance Criteria for v1.0

The system meets v1.0 requirements when:

1. ✅ Objects are emulated with realistic motion and radar output
2. ✅ Azimuth and elevation are correctly calculated relative to radar
3. ✅ System reacts to object position and motion
4. ✅ Safety layers are implemented in Ada and enforced
5. ✅ Ctrl+C gracefully shuts down both C++ and Ada processes
6. ✅ **C++ and Ada communicate via UDP messages**
7. ✅ **C2 controller sends target assignments to gun control**
8. ✅ **Gun control receives assignments and updates state machine**
9. ✅ **Safety checks are integrated into engagement flow**
10. ✅ **Ballistics calculations are used in engagement**
11. ✅ **End-to-end engagement sequence works (object → radar → C2 → gun control → engagement)**
12. ✅ **All safety interlocks prevent unsafe firing**

---

## 14. Document Control

- **Author**: System Requirements Team
- **Review Status**: Final
- **Approval Status**: Approved for Implementation
- **Next Review**: After v1.0 implementation complete

---

## 8. Implementation Constraints

### 8.1 Language Constraints
- **C++**: Command & Control, radar simulation, message gateway
- **Ada**: Safety-critical components (safety kernel, engagement state machine, ballistics)
- **Interface**: Clean message protocol between languages

### 8.2 Performance Constraints
- **Real-Time**: Ada components must meet timing deadlines
- **Throughput**: Support 1-20 simultaneous tracks
- **Latency**: End-to-end latency < 200ms (object detection to engagement decision)

### 8.3 Safety Constraints
- **Fail-Safe**: System defaults to safe state on any error
- **No Single Point of Failure**: Safety checks at multiple levels
- **Verifiability**: Ada code written in SPARK-compatible style

---

## 9. Testing Requirements

### 9.1 Unit Testing
- **Object Motion**: Verify coordinate transformations
- **Threat Evaluation**: Test scoring algorithm
- **Safety Interlocks**: Test all safety conditions
- **Ballistics**: Verify calculation correctness

### 9.2 Integration Testing
- **C++/Ada Communication**: Test message passing
- **End-to-End**: Full engagement sequence
- **Failure Modes**: Test error handling

### 9.3 Scenario Testing
- **Single Target**: Basic engagement
- **Multiple Targets**: Prioritization
- **Safety Violations**: Abort conditions
- **Edge Cases**: Boundary conditions

---

## 10. Non-Functional Requirements

### 10.1 Reliability
- **Uptime**: Continuous operation until explicit shutdown
- **Error Recovery**: Graceful degradation on non-critical errors
- **Data Integrity**: No data corruption during operation

### 10.2 Maintainability
- **Code Clarity**: Well-documented, readable code
- **Modularity**: Clear separation of concerns
- **Extensibility**: Easy to add new features

### 10.3 Observability
- **Logging**: Comprehensive logging of all operations
- **Traceability**: Track object IDs through system
- **Debugging**: Support for debugging and inspection

---

## 11. Future Extensions (Out of Scope for v0.1)

- Hardware-in-the-loop integration
- Advanced sensor fusion
- Multiple weapon systems
- Graphical user interface
- SPARK formal verification
- Performance optimization
- Extended scenario library

---

## 12. Acceptance Criteria

The system meets v0.1 requirements when:
1. ✅ Objects are emulated with realistic motion and radar output
2. ✅ Azimuth and elevation are correctly calculated relative to radar
3. ✅ System reacts to object position and motion
4. ✅ Safety layers are implemented in Ada and enforced
5. ✅ Ctrl+C gracefully shuts down both processes
6. ✅ End-to-end engagement sequence works
7. ✅ All safety interlocks prevent unsafe firing
8. ✅ System handles errors gracefully

---

## Document Control

- **Author**: System Requirements Team
- **Review Status**: Pending
- **Approval Status**: Draft
- **Next Review**: TBD

