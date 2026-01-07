# SKYGUARDIS Design Specification v1.0

## Document Information
- **Version**: 1.0
- **Date**: 2025-01-06
- **Status**: Draft
- **Project**: SKYGUARDIS - Short-Range Air Defence Simulation
- **Based On**: Requirements Document v1.0

---

## 1. Executive Summary

This document provides the detailed design specification for SKYGUARDIS v1.0. It bridges the requirements document to implementation, specifying data structures, algorithms, interfaces, and protocols needed to build the system.

---

## 2. Design Principles

### 2.1 Safety-First Design
- All safety-critical logic in Ada
- Fail-safe defaults (safe state on error)
- Multiple layers of validation
- Deterministic execution where possible

### 2.2 Separation of Concerns
- C++ for command & control and simulation
- Ada for safety-critical engagement logic
- Clear message-based interface between layers

### 2.3 Real-Time Constraints
- Deterministic timing in Ada components
- Bounded execution times
- No dynamic allocation in hot paths
- Predictable state transitions

### 2.4 Verifiability
- SPARK-compatible Ada code style
- Clear contracts and preconditions
- Testable interfaces
- Documented assumptions

---

## 3. Message Protocol Design

### 3.1 Protocol Overview

The communication between C++ C2 node and Ada gun control uses UDP datagrams with a binary message format.

**Transport:**
- Protocol: UDP (User Datagram Protocol)
- Address: 127.0.0.1 (localhost)
- Ports:
  - C2 → Gun Control: 8888 (target assignments)
  - Gun Control → C2: 8889 (engagement status)

**Message Characteristics:**
- Binary format (network byte order)
- Fixed-size messages for efficiency
- Checksum for integrity verification
- Version field for future compatibility

### 3.2 Message Format Specification

#### 3.2.1 Common Message Header

All messages start with a common header:

```
Offset  Size    Field           Description
0       1       MessageType     Message type identifier
1       1       Version         Protocol version (currently 1)
2       2       Length          Message length in bytes (excluding header)
4       2       Checksum        16-bit checksum
```

**Total Header Size:** 6 bytes

#### 3.2.2 TargetAssignment Message (C++ → Ada)

**Message Type:** 0x01

**Structure:**
```
Offset  Size    Type        Field           Description
0       1       uint8_t     MessageType     0x01
1       1       uint8_t     Version         0x01
2       2       uint16_t    Length          40 (network byte order)
4       2       uint16_t    Checksum        16-bit checksum
6       4       uint32_t    TargetID        Unique target identifier
10      8       double      RangeM          Range in meters (IEEE 754)
18      8       double      AzimuthRad      Azimuth in radians (IEEE 754)
26      8       double      ElevationRad    Elevation in radians (IEEE 754)
34      8       double      VelocityMs      Velocity in m/s (IEEE 754)
42      1       uint8_t     Priority        Threat priority (0-255)
```

**Total Message Size:** 43 bytes

**C++ Structure:**
```cpp
struct TargetAssignment {
    uint8_t message_type = 0x01;
    uint8_t version = 0x01;
    uint16_t length = 40;
    uint16_t checksum;
    uint32_t target_id;
    double range_m;
    double azimuth_rad;
    double elevation_rad;
    double velocity_ms;
    uint8_t priority;
};
```

**Ada Record:**
```ada
type Target_Assignment_Message is record
    Message_Type : Unsigned_8 := 16#01#;
    Version      : Unsigned_8 := 16#01#;
    Length       : Unsigned_16 := 40;
    Checksum     : Unsigned_16;
    Target_ID    : Unsigned_32;
    Range_M      : Float;
    Azimuth_Rad  : Float;
    Elevation_Rad : Float;
    Velocity_Ms  : Float;
    Priority     : Unsigned_8;
end record;
```

#### 3.2.3 EngagementStatus Message (Ada → C++)

**Message Type:** 0x02

**Structure:**
```
Offset  Size    Type        Field           Description
0       1       uint8_t     MessageType     0x02
1       1       uint8_t     Version         0x01
2       2       uint16_t    Length          25 (network byte order)
4       2       uint16_t    Checksum        16-bit checksum
6       4       uint32_t    TargetID        Target identifier
10      1       uint8_t     State           Engagement state (0-5)
11      1       uint8_t     Firing          0 = not firing, 1 = firing
12      8       double      LeadAngleRad    Lead angle in radians
20      8       double      TimeToImpactS   Time to impact in seconds
```

**Total Message Size:** 28 bytes

**State Values:**
- 0: Idle
- 1: Acquiring
- 2: Tracking
- 3: Firing
- 4: Verifying
- 5: Complete

#### 3.2.4 SafetyInterlock Message (Ada → C++)

**Message Type:** 0x03

**Structure:**
```
Offset  Size    Type        Field           Description
0       1       uint8_t     MessageType     0x03
1       1       uint8_t     Version         0x01
2       2       uint16_t    Length          10 (network byte order)
4       2       uint16_t    Checksum        16-bit checksum
6       4       uint32_t    TargetID        Target identifier
10      1       uint8_t     ViolationType   Type of safety violation
11      1       uint8_t     Reserved        Reserved for future use
```

**Total Message Size:** 12 bytes

#### 3.2.5 Heartbeat Message (Bidirectional)

**Message Type:** 0x04

**Structure:**
```
Offset  Size    Type        Field           Description
0       1       uint8_t     MessageType     0x04
1       1       uint8_t     Version         0x01
2       2       uint16_t    Length          8 (network byte order)
4       2       uint16_t    Checksum        16-bit checksum
6       8       uint64_t    Timestamp       Unix timestamp (milliseconds)
```

**Total Message Size:** 14 bytes

### 3.3 Checksum Algorithm

**16-bit Checksum Calculation:**
1. Initialize checksum to 0
2. For each byte in message (excluding checksum field):
   - Add byte value to checksum
   - If checksum > 65535, subtract 65536
3. Store checksum in network byte order

**Validation:**
- Calculate checksum on received message
- Compare with received checksum value
- Reject message if mismatch

### 3.4 Serialization/Deserialization

#### 3.4.1 C++ Serialization

**Function Signature:**
```cpp
bool serializeTargetAssignment(
    const TargetAssignment& msg,
    uint8_t* buffer,
    size_t buffer_size
);
```

**Algorithm:**
1. Check buffer size >= message size
2. Write header fields (network byte order)
3. Write payload fields (convert doubles to network byte order)
4. Calculate checksum
5. Write checksum
6. Return true on success

**Deserialization:**
```cpp
bool deserializeEngagementStatus(
    const uint8_t* buffer,
    size_t buffer_size,
    EngagementStatus& msg
);
```

#### 3.4.2 Ada Serialization

**Package:** `Message_Handler.Serialization`

**Procedure:**
```ada
procedure Serialize_Target_Assignment (
    Message : Target_Assignment_Message;
    Buffer  : out Byte_Array;
    Success : out Boolean
);
```

**Algorithm:**
1. Convert Ada record to byte array
2. Convert all multi-byte fields to network byte order
3. Calculate checksum
4. Write checksum to buffer
5. Set Success flag

**Deserialization:**
```ada
procedure Deserialize_Target_Assignment (
    Buffer  : Byte_Array;
    Message : out Target_Assignment_Message;
    Success : out Boolean
);
```

---

## 4. Data Structure Design

### 4.1 Object/Track Data Structures

#### 4.1.1 C++ Track Structure

```cpp
namespace skyguardis {
namespace c2 {

struct Track {
    uint32_t id;              // Unique track identifier
    double range_m;            // Range in meters
    double azimuth_rad;        // Azimuth in radians
    double elevation_rad;       // Elevation in radians
    double velocity_ms;         // Velocity magnitude in m/s
    double heading_rad;        // Heading direction in radians
    double x, y, z;            // Cartesian position (m)
    double vx, vy, vz;         // Velocity vector (m/s)
    uint64_t timestamp_us;     // Timestamp in microseconds
    uint8_t object_type;       // Object classification
};

} // namespace c2
} // namespace skyguardis
```

#### 4.1.2 Ada Target Data Structure

```ada
package Common.Types is

   type Target_ID_Type is mod 2**32;
   
   type Range_Meters is new Float range 0.0 .. 50_000.0;
   type Angle_Radians is new Float range -3.14159 .. 3.14159;
   type Velocity_Ms is new Float range 0.0 .. 2_000.0;
   
   type Target_Data is record
      ID          : Target_ID_Type;
      Range_M     : Range_Meters;
      Azimuth_Rad : Angle_Radians;
      Elevation_Rad : Angle_Radians;
      Velocity_Ms : Velocity_Ms;
      Heading_Rad : Angle_Radians;
   end record;

end Common.Types;
```

### 4.2 Engagement Context

#### 4.2.1 Ada Engagement Context

```ada
package Gun_Control.Engagement_State_Machine is

   type Engagement_State is (
      Idle,        -- 0
      Acquiring,    -- 1
      Tracking,     -- 2
      Firing,       -- 3
      Verifying,    -- 4
      Complete      -- 5
   );

   type Engagement_Context is record
      State            : Engagement_State := Idle;
      Target_ID        : Common.Types.Target_ID_Type := 0;
      Target_Data      : Common.Types.Target_Data;
      Lead_Angle_Rad   : Float := 0.0;
      Time_To_Impact_S : Float := 0.0;
      Last_Update_Time : Ada.Real_Time.Time;
      Safety_Status    : Safety_Kernel.Safety_Interlocks.Safety_Status;
   end record;

end Gun_Control.Engagement_State_Machine;
```

### 4.3 Threat Evaluation Data

#### 4.3.1 C++ Threat Score

```cpp
struct ThreatScore {
    uint32_t track_id;
    double score;           // 0.0 to 1.0
    uint8_t priority;       // 0 to 255
    double range_factor;
    double velocity_factor;
    double closing_rate_factor;
};
```

---

## 5. Algorithm Design

### 5.1 Coordinate Transformations

#### 5.1.1 Cartesian to Spherical

**Input:** (x, y, z) in meters
**Output:** (range, azimuth, elevation) in meters and radians

**Algorithm:**
```cpp
void cartesianToSpherical(
    double x, double y, double z,
    double& range, double& azimuth, double& elevation
) {
    range = sqrt(x*x + y*y + z*z);
    azimuth = atan2(y, x);
    elevation = asin(z / range);
}
```

**Ada Implementation:**
```ada
procedure Cartesian_To_Spherical (
    X, Y, Z : Float;
    Range_M : out Range_Meters;
    Azimuth_Rad : out Angle_Radians;
    Elevation_Rad : out Angle_Radians
) is
begin
    Range_M := Range_Meters (Sqrt (X * X + Y * Y + Z * Z));
    Azimuth_Rad := Angle_Radians (Arctan (Y, X));
    Elevation_Rad := Angle_Radians (Arcsin (Z / Float (Range_M)));
end Cartesian_To_Spherical;
```

#### 5.1.2 Spherical to Cartesian

**Input:** (range, azimuth, elevation)
**Output:** (x, y, z)

**Algorithm:**
```cpp
void sphericalToCartesian(
    double range, double azimuth, double elevation,
    double& x, double& y, double& z
) {
    x = range * cos(elevation) * cos(azimuth);
    y = range * cos(elevation) * sin(azimuth);
    z = range * sin(elevation);
}
```

### 5.2 Threat Evaluation Algorithm

#### 5.2.1 Threat Scoring

**Input:** Track data
**Output:** Threat score (0.0 to 1.0)

**Algorithm:**
```cpp
double ThreatEvaluator::computeThreatScore(const Track& track) const {
    // Range factor: closer = higher threat (inverse relationship)
    double range_factor = 1.0 / (1.0 + track.range_m / 1000.0);
    
    // Velocity factor: faster = higher threat
    double velocity_factor = std::min(1.0, track.velocity_ms / 300.0);
    
    // Closing rate: negative (approaching) = higher threat
    // Simplified: assume heading toward radar if azimuth matches heading
    double closing_factor = 1.0; // Simplified for v1.0
    
    // Combined score
    double score = (range_factor * 0.5) + 
                   (velocity_factor * 0.3) + 
                   (closing_factor * 0.2);
    
    return std::min(1.0, std::max(0.0, score));
}
```

### 5.3 Ballistics Calculations

#### 5.3.1 Lead Angle Calculation

**Input:** Target range, velocity, heading; Projectile velocity
**Output:** Lead angle in radians

**Algorithm:**
```ada
function Calculate_Lead_Angle (
    Target_Range_M : Range_M;
    Target_Velocity_Ms : Velocity_Ms;
    Target_Heading_Rad : Angle_Rad;
    Projectile_Velocity_Ms : Velocity_Ms
) return Angle_Rad is
    Time_To_Impact : Time_S;
    Lead_Distance : Float;
begin
    -- Simplified: assume level flight for now
    Time_To_Impact := Calculate_Time_Of_Flight (
        Target_Range_M,
        Projectile_Velocity_Ms,
        0.0  -- Elevation = 0 (level)
    );
    
    -- Calculate how far target moves in that time
    Lead_Distance := Float (Target_Velocity_Ms) * Float (Time_To_Impact);
    
    -- Lead angle = arcsin(lead_distance / range)
    return Angle_Rad (Arcsin (Lead_Distance / Float (Target_Range_M)));
end Calculate_Lead_Angle;
```

#### 5.3.2 Time of Flight

**Input:** Range, projectile velocity, elevation
**Output:** Time of flight in seconds

**Algorithm:**
```ada
function Calculate_Time_Of_Flight (
    Range_M : Range_M;
    Projectile_Velocity_Ms : Velocity_Ms;
    Elevation_Rad : Angle_Rad
) return Time_S is
    -- Simplified: ignore gravity and air resistance for v1.0
    -- TOF = range / (velocity * cos(elevation))
begin
    if Projectile_Velocity_Ms > 0.0 then
        return Time_S (Float (Range_M) / 
                      (Float (Projectile_Velocity_Ms) * Cos (Float (Elevation_Rad))));
    else
        return 0.0;
    end if;
end Calculate_Time_Of_Flight;
```

### 5.4 Safety Interlock Algorithm

#### 5.4.1 Safety Check

**Input:** Azimuth, elevation, range
**Output:** Safety status (Safe, Unsafe, Blocked)

**Algorithm:**
```ada
function Is_Safe_To_Fire (
    Azimuth_Rad : Angle_Rad;
    Elevation_Rad : Elevation_Rad;
    Range_M : Range_M
) return Safety_Status is
begin
    -- Check angle limits
    if not Validate_Angle_Limits (Azimuth_Rad, Elevation_Rad) then
        return Unsafe;
    end if;
    
    -- Check no-fire zones
    if not Check_No_Fire_Zones (Azimuth_Rad, Elevation_Rad) then
        return Blocked;
    end if;
    
    -- Check range limits
    if Range_M < 100.0 or Range_M > 10_000.0 then
        return Unsafe;
    end if;
    
    return Safe;
end Is_Safe_To_Fire;
```

---

## 6. Component Interface Design

### 6.1 C++ Message Gateway Interface

#### 6.1.1 Class Definition

```cpp
namespace skyguardis {
namespace gateway {

class MessageGateway {
public:
    MessageGateway();
    ~MessageGateway();
    
    // Initialize UDP sockets
    bool initialize(uint16_t send_port, uint16_t receive_port);
    
    // Send target assignment
    bool sendTargetAssignment(const protocol::TargetAssignment& assignment);
    
    // Receive engagement status (non-blocking)
    bool receiveEngagementStatus(protocol::EngagementStatus& status);
    
    // Cleanup
    void shutdown();

private:
    int send_socket_;
    int receive_socket_;
    struct sockaddr_in gun_control_addr_;
    bool initialized_;
};

} // namespace gateway
} // namespace skyguardis
```

#### 6.1.2 Implementation Details

**Socket Initialization:**
```cpp
bool MessageGateway::initialize(uint16_t send_port, uint16_t receive_port) {
    // Create UDP socket
    send_socket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (send_socket_ < 0) {
        return false;
    }
    
    // Setup gun control address
    memset(&gun_control_addr_, 0, sizeof(gun_control_addr_));
    gun_control_addr_.sin_family = AF_INET;
    gun_control_addr_.sin_addr.s_addr = inet_addr("127.0.0.1");
    gun_control_addr_.sin_port = htons(send_port);
    
    // Setup receive socket
    receive_socket_ = socket(AF_INET, SOCK_DGRAM, 0);
    // ... bind to receive_port
    
    initialized_ = true;
    return true;
}
```

### 6.2 Ada Message Handler Interface

#### 6.2.1 Package Specification

```ada
package Message_Handler is

   type Message_Handler_Type is limited private;
   
   procedure Initialize (
      Handler : in out Message_Handler_Type;
      Port    : Port_Type
   );
   
   function Receive_Target_Assignment (
      Handler : in out Message_Handler_Type;
      Message : out Target_Assignment_Message;
      Success : out Boolean
   ) return Boolean;
   
   procedure Send_Engagement_Status (
      Handler : in out Message_Handler_Type;
      Status  : Engagement_Status_Message;
      Success : out Boolean
   );
   
   procedure Shutdown (Handler : in out Message_Handler_Type);

private
   type Message_Handler_Type is limited record
      Socket      : GNAT.Sockets.Socket_Type;
      Initialized : Boolean := False;
   end record;

end Message_Handler;
```

### 6.3 C2 Controller Interface

#### 6.3.1 Integration with Message Gateway

```cpp
void C2Controller::processTracks(const std::vector<Track>& tracks) {
    if (tracks.empty()) {
        return;
    }
    
    // Evaluate and prioritize threats
    auto prioritized = evaluator_.prioritize(tracks);
    
    // Send highest priority target
    if (!prioritized.empty() && prioritized[0].score > 0.5) {
        // Find track
        for (const auto& track : tracks) {
            if (track.id == prioritized[0].track_id) {
                assignTarget(track);
                break;
            }
        }
    }
}

void C2Controller::assignTarget(const Track& track) {
    // Convert to target assignment message
    protocol::TargetAssignment assignment;
    assignment.target_id = track.id;
    assignment.range_m = track.range_m;
    assignment.azimuth_rad = track.azimuth_rad;
    assignment.elevation_rad = track.elevation_rad;
    assignment.velocity_ms = track.velocity_ms;
    assignment.priority = evaluator_.evaluate(track).priority;
    
    // Send via gateway
    if (gateway_) {
        gateway_->sendTargetAssignment(assignment);
    }
}
```

### 6.4 Gun Control Integration

#### 6.4.1 Message Reception and State Machine

```ada
-- In main_gun_control.adb main loop
loop
   -- Check for incoming messages
   declare
      Assignment : Target_Assignment_Message;
      Received   : Boolean;
   begin
      if Message_Handler.Receive_Target_Assignment (
         Handler, Assignment, Received) and then Received
      then
         -- Validate assignment
         if Validate_Assignment (Assignment) then
            -- Update engagement context
            Context.Target_ID := Assignment.Target_ID;
            Context.Target_Data.Range_M := Assignment.Range_M;
            Context.Target_Data.Azimuth_Rad := Assignment.Azimuth_Rad;
            Context.Target_Data.Elevation_Rad := Assignment.Elevation_Rad;
            Context.Target_Data.Velocity_Ms := Assignment.Velocity_Ms;
            
            -- Trigger state machine
            if Context.State = Idle then
               Process_Command (Context, Start_Engagement, ...);
            end if;
         end if;
      end if;
   end;
   
   -- Process state machine
   Process_State_Machine (Context);
   
   -- Send status updates
   Send_Status_Update (Context);
   
   delay until Next_Time;
   Next_Time := Next_Time + Period;
end loop;
```

---

## 7. State Machine Design

### 7.1 Engagement State Machine

#### 7.1.1 State Transition Diagram

```
[Idle]
  | Start_Engagement (if safety check passes)
  v
[Acquiring]
  | Continue_Tracking (if target acquired)
  v
[Tracking]
  | Fire (if safety check passes)
  v
[Firing]
  | (automatic after firing duration)
  v
[Verifying]
  | (automatic after verification)
  v
[Complete]
  | (automatic)
  v
[Idle]

Any State
  | Abort (or safety violation)
  v
[Idle]
```

#### 7.1.2 State Transition Logic

```ada
procedure Process_Command (
    Context : in out Engagement_Context;
    Command : Engagement_Command;
    Target_Range_M : Float;
    Target_Azimuth_Rad : Float;
    Target_Elevation_Rad : Float
) is
    Safety_Check : Safety_Status;
begin
    case Context.State is
        when Idle =>
            if Command = Start_Engagement then
                -- Pre-acquisition safety check
                Safety_Check := Is_Safe_To_Fire (
                    Target_Azimuth_Rad,
                    Target_Elevation_Rad,
                    Target_Range_M
                );
                if Safety_Check = Safe then
                    Context.State := Acquiring;
                    Context.Target_ID := ...;
                end if;
            end if;
            
        when Acquiring =>
            if Command = Continue_Tracking then
                Context.State := Tracking;
            elsif Command = Abort then
                Context.State := Idle;
            end if;
            
        when Tracking =>
            if Command = Fire then
                -- Pre-fire safety check
                Safety_Check := Is_Safe_To_Fire (...);
                if Safety_Check = Safe then
                    Context.State := Firing;
                else
                    Context.State := Idle;  -- Abort on safety violation
                end if;
            elsif Command = Abort then
                Context.State := Idle;
            end if;
            
        when Firing =>
            -- Automatic transition after firing duration
            Context.State := Verifying;
            
        when Verifying =>
            -- Automatic transition after verification
            Context.State := Complete;
            
        when Complete =>
            -- Automatic transition
            Context.State := Idle;
    end case;
end Process_Command;
```

---

## 8. Error Handling Design

### 8.1 Error Types

#### 8.1.1 Communication Errors
- Socket creation failure
- Send/receive failures
- Timeout errors
- Checksum mismatches
- Invalid message format

#### 8.1.2 Validation Errors
- Invalid range values
- Invalid angle values
- Invalid state transitions
- Safety violations

#### 8.1.3 Calculation Errors
- Division by zero
- Overflow conditions
- Invalid input ranges

### 8.2 Error Handling Strategy

#### 8.2.1 C++ Error Handling
- Return boolean for success/failure
- Log errors with context
- Default to safe state on critical errors
- Continue operation on non-critical errors

#### 8.2.2 Ada Error Handling
- Use exceptions for critical errors
- Use return status for validation
- Always return to Idle state on errors
- Log all errors

---

## 9. Performance Design

### 9.1 Timing Requirements

- **Radar Update:** 100ms period (10 Hz)
- **C2 Processing:** < 50ms per cycle
- **Ada Control Loop:** 100ms period (10 Hz)
- **Message Latency:** < 10ms
- **State Transition:** < 5ms

### 9.2 Resource Constraints

- **Memory:** Bounded allocation, no leaks
- **CPU:** Deterministic execution paths
- **Network:** Minimal bandwidth (UDP localhost)

---

## 10. Testing Design

### 10.1 Unit Test Design

- Message serialization/deserialization
- Coordinate transformations
- Threat evaluation algorithm
- Ballistics calculations
- Safety interlock logic

### 10.2 Integration Test Design

- End-to-end message flow
- State machine transitions
- Error recovery
- Concurrent operation

---

## 11. Implementation Notes

### 11.1 Development Order

1. Message protocol implementation (C++ and Ada)
2. C2 controller integration
3. Gun control message reception
4. State machine integration
5. Safety interlock integration
6. Ballistics integration

### 11.2 Dependencies

- **C++:** Standard library, POSIX sockets
- **Ada:** GNAT runtime, GNAT.Sockets
- **Build:** CMake (optional), GNAT project file

### 11.3 Assumptions

- Localhost communication only (v1.0)
- Single target engagement at a time
- Simplified ballistics (no gravity/air resistance)
- Fixed update rates (10 Hz)

---

## 12. Document Control

- **Author**: Design Team
- **Review Status**: Draft
- **Approval Status**: Pending Review
- **Version**: 1.0
- **Last Updated**: 2025-01-06
- **Next Review**: Before implementation start

---

## Appendix A: Message Format Diagrams

### A.1 TargetAssignment Message Layout

```
[Header: 6 bytes]
  [Type: 1] [Version: 1] [Length: 2] [Checksum: 2]
[Payload: 37 bytes]
  [TargetID: 4] [Range: 8] [Azimuth: 8] [Elevation: 8]
  [Velocity: 8] [Priority: 1]
```

### A.2 EngagementStatus Message Layout

```
[Header: 6 bytes]
  [Type: 1] [Version: 1] [Length: 2] [Checksum: 2]
[Payload: 22 bytes]
  [TargetID: 4] [State: 1] [Firing: 1] [LeadAngle: 8] [TimeToImpact: 8]
```

---

## Appendix B: Code Examples

### B.1 C++ Message Sending Example

```cpp
TargetAssignment assignment;
assignment.target_id = 123;
assignment.range_m = 5000.0;
assignment.azimuth_rad = 0.785;  // 45 degrees
assignment.elevation_rad = 0.174; // 10 degrees
assignment.velocity_ms = 200.0;
assignment.priority = 128;

if (gateway.sendTargetAssignment(assignment)) {
    logger.log("Target assignment sent: ID=" + std::to_string(assignment.target_id));
} else {
    logger.log("Failed to send target assignment");
}
```

### B.2 Ada Message Receiving Example

```ada
declare
    Assignment : Target_Assignment_Message;
    Received   : Boolean;
begin
    if Message_Handler.Receive_Target_Assignment (
       Handler, Assignment, Received) and then Received
    then
        Ada.Text_IO.Put_Line ("Received target assignment: ID=" & 
                             Target_ID_Type'Image (Assignment.Target_ID));
        -- Process assignment...
    end if;
end;
```

---

**End of Design Specification v1.0**

