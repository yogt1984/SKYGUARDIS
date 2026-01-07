# EtherCAT Frame Design for SKYGUARDIS

## Document Information
- **Version**: 1.0
- **Date**: 2025-01-06
- **Status**: Design Specification
- **Project**: SKYGUARDIS - Short-Range Air Defence Simulation

---

## 1. Overview

This document specifies the EtherCAT frame design used in SKYGUARDIS. The design is based on educated assumptions about EtherCAT protocol requirements, tailored for real-time air defence system communication.

**Important Note:** This is a **simplified, educated guess** at EtherCAT frame structure suitable for the SKYGUARDIS use case. Actual EtherCAT implementations may vary, but this design provides a foundation for deterministic real-time communication.

---

## 2. EtherCAT Protocol Rationale

### 2.1 Why EtherCAT?

EtherCAT (Ethernet for Control Automation Technology) is chosen for SKYGUARDIS because:

- **Microsecond-level cycle times**: Required for deterministic real-time control
- **Nanosecond-level synchronization jitter**: Critical for safety-critical engagement timing
- **Deterministic timing (hard-real-time)**: TCP/IP cannot guarantee this due to:
  - Buffering delays
  - Retry mechanisms
  - Congestion control
  - Variable routing paths
  - Operating system scheduling delays

### 2.2 How EtherCAT Works

- Uses **Ethernet physical layer (IEEE 802.3)**
- **Bypasses TCP/IP completely**
- Embeds **lean real-time datagrams directly in Ethernet frames**
- Enables **deterministic, hard-real-time communication**

---

## 3. EtherCAT Frame Structure

### 3.1 Complete Frame Layout

```
┌─────────────────────────────────────────────────────────────┐
│ Ethernet Header (14 bytes)                                 │
├─────────────────────────────────────────────────────────────┤
│ Destination MAC Address      │ 6 bytes │ 0x01:0x02:0x03... │
│ Source MAC Address           │ 6 bytes │ 0xAA:0xBB:0xCC... │
│ EtherType                     │ 2 bytes │ 0x88A4 (EtherCAT) │
├─────────────────────────────────────────────────────────────┤
│ EtherCAT Header (2 bytes)                                  │
├─────────────────────────────────────────────────────────────┤
│ Length (11 bits)             │ 11 bits │ Datagram length   │
│ Type (1 bit)                 │ 1 bit   │ 0 = datagram       │
│ Reserved                      │ 4 bits  │ 0x0               │
├─────────────────────────────────────────────────────────────┤
│ SKYGUARDIS Datagram Payload (variable length)              │
├─────────────────────────────────────────────────────────────┤
│ [Message Header: 6 bytes]                                  │
│   MessageType                │ 1 byte  │ 0x01-0x04         │
│   Version                     │ 1 byte  │ 0x01              │
│   Length                      │ 2 bytes │ Payload length    │
│   Checksum                    │ 2 bytes │ 16-bit checksum   │
│ [Message Payload: variable]                                 │
│   ... application data ...                                   │
├─────────────────────────────────────────────────────────────┤
│ Ethernet FCS (4 bytes)                                      │
│ Frame Check Sequence         │ 4 bytes │ CRC-32            │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Frame Size Calculation

**Minimum Frame Size:**
- Ethernet minimum: 64 bytes
- Ethernet header: 14 bytes
- EtherCAT header: 2 bytes
- Minimum payload: 46 bytes (to reach 64 bytes total)
- FCS: 4 bytes

**SKYGUARDIS Frame Sizes:**
- **TargetAssignment Frame**: 60 bytes (14 + 2 + 40 + 4 = 60, padded to 64)
- **EngagementStatus Frame**: 45 bytes (14 + 2 + 25 + 4 = 45, padded to 64)
- **SafetyInterlock Frame**: 32 bytes (14 + 2 + 12 + 4 = 32, padded to 64)
- **Heartbeat Frame**: 34 bytes (14 + 2 + 14 + 4 = 34, padded to 64)

---

## 4. EtherCAT Header Specification

### 4.1 Header Format

```
Bit:  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     ┌───────────────────────────────────────────────┐
     │ L │ L │ L │ L │ L │ L │ L │ L │ L │ L │ L │ T │ R │ R │ R │ R │
     └───────────────────────────────────────────────┘
      Length (11 bits)    Type Reserved (4 bits)
```

**Field Definitions:**
- **Length (bits 0-10)**: Datagram length in bytes (0-2047 bytes)
- **Type (bit 11)**: 
  - 0 = Datagram (used for SKYGUARDIS messages)
  - 1 = Mailbox (not used in SKYGUARDIS)
- **Reserved (bits 12-15)**: Reserved for future use, set to 0

### 4.2 Header Encoding Example

For a TargetAssignment datagram (40 bytes):
```
Length = 40 (0x0028 in 11 bits)
Type = 0 (datagram)
Reserved = 0x0

Header = 0x0028 (little-endian in frame)
```

---

## 5. SKYGUARDIS Datagram Formats

### 5.1 Common Datagram Header

All SKYGUARDIS datagrams start with a common header:

```
Offset  Size    Field           Description
0       1       MessageType     Message type identifier
1       1       Version         Protocol version (currently 1)
2       2       Length          Payload length in bytes (network byte order)
4       2       Checksum        16-bit checksum (network byte order)
```

**Total Header Size:** 6 bytes

### 5.2 TargetAssignment Datagram

**Message Type:** 0x01

**Complete Structure:**
```
[Header: 6 bytes]
  [MessageType: 0x01][Version: 0x01][Length: 0x0022][Checksum: 2 bytes]
[Payload: 34 bytes]
  [TargetID: 4 bytes]
  [Range: 8 bytes]
  [Azimuth: 8 bytes]
  [Elevation: 8 bytes]
  [Velocity: 8 bytes]
  [Priority: 1 byte]
  [Reserved: 1 byte] (padding)
```

**Total Datagram Size:** 40 bytes

### 5.3 EngagementStatus Datagram

**Message Type:** 0x02

**Complete Structure:**
```
[Header: 6 bytes]
  [MessageType: 0x02][Version: 0x01][Length: 0x0010][Checksum: 2 bytes]
[Payload: 16 bytes]
  [TargetID: 4 bytes]
  [State: 1 byte]
  [Firing: 1 byte]
  [LeadAngle: 8 bytes]
  [TimeToImpact: 8 bytes]
```

**Total Datagram Size:** 22 bytes

### 5.4 SafetyInterlock Datagram

**Message Type:** 0x03

**Complete Structure:**
```
[Header: 6 bytes]
  [MessageType: 0x03][Version: 0x01][Length: 0x0006][Checksum: 2 bytes]
[Payload: 6 bytes]
  [TargetID: 4 bytes]
  [ViolationType: 1 byte]
  [Reserved: 1 byte]
```

**Total Datagram Size:** 12 bytes

### 5.5 Heartbeat Datagram

**Message Type:** 0x04

**Complete Structure:**
```
[Header: 6 bytes]
  [MessageType: 0x04][Version: 0x01][Length: 0x0008][Checksum: 2 bytes]
[Payload: 8 bytes]
  [Timestamp: 8 bytes] (Unix timestamp in milliseconds)
```

**Total Datagram Size:** 14 bytes

---

## 6. Frame Construction Algorithm

### 6.1 Frame Building Process

1. **Create Ethernet Header**
   - Set destination MAC (gun control MAC address)
   - Set source MAC (C2 node MAC address)
   - Set EtherType = 0x88A4 (EtherCAT)

2. **Create EtherCAT Header**
   - Calculate datagram length
   - Set Type = 0 (datagram)
   - Set Reserved = 0x0
   - Encode header (11-bit length + 1-bit type + 4-bit reserved)

3. **Create SKYGUARDIS Datagram**
   - Serialize message to binary format
   - Calculate checksum
   - Add header (MessageType, Version, Length, Checksum)

4. **Assemble Frame**
   - Ethernet header (14 bytes)
   - EtherCAT header (2 bytes)
   - SKYGUARDIS datagram (variable)
   - Pad to minimum 64 bytes if needed

5. **Calculate FCS**
   - Calculate CRC-32 over entire frame (excluding FCS)
   - Append FCS (4 bytes)

### 6.2 Frame Parsing Process

1. **Receive Frame**
   - Read Ethernet frame from network

2. **Validate Ethernet Header**
   - Check EtherType = 0x88A4 (EtherCAT)
   - Validate MAC addresses

3. **Parse EtherCAT Header**
   - Extract length (11 bits)
   - Extract type (1 bit, must be 0 for datagram)
   - Validate reserved bits

4. **Extract Datagram**
   - Read datagram based on length from EtherCAT header
   - Validate datagram size matches EtherCAT length

5. **Parse SKYGUARDIS Datagram**
   - Extract message header
   - Validate message type and version
   - Validate checksum
   - Deserialize payload

6. **Validate FCS**
   - Calculate CRC-32 over frame
   - Compare with received FCS
   - Discard frame if mismatch

---

## 7. Timing Characteristics

### 7.1 Performance Requirements

- **Cycle Time**: < 1 millisecond (microsecond-level)
- **Frame Latency**: < 10 microseconds end-to-end
- **Synchronization Jitter**: < 100 nanoseconds (nanosecond-level)
- **Determinism**: Hard real-time guarantee (no buffering delays)

### 7.2 Current Implementation (UDP Emulation)

**Note:** The current UDP-based implementation achieves:
- **Latency**: ~1-10 milliseconds (acceptable for development)
- **Jitter**: ~1-5 milliseconds (acceptable for development)
- **Determinism**: Soft real-time (OS-dependent)

**Future Migration:**
When migrating to actual EtherCAT hardware, the application interface remains the same, but timing characteristics will improve to meet hard real-time requirements.

---

## 8. Implementation Notes

### 8.1 Current Emulation Approach

The current implementation uses UDP sockets to **emulate** EtherCAT frames:

- **UDP Datagram** simulates complete EtherCAT frame
- **Frame structure** is maintained in application code
- **EtherCAT headers** are conceptually present but not transmitted over UDP
- **Application interface** is identical to future EtherCAT hardware interface

### 8.2 Migration Path

When migrating to actual EtherCAT hardware:

1. Replace UDP socket calls with EtherCAT driver API
2. Implement actual EtherCAT frame construction/parsing
3. Add hardware-level frame transmission/reception
4. No changes required to application-level message format
5. No changes required to datagram serialization/deserialization

### 8.3 Frame Validation

**Current (UDP Emulation):**
- Validate datagram checksum
- Validate message format
- Validate message type and version

**Future (EtherCAT Hardware):**
- Validate Ethernet FCS
- Validate EtherCAT header
- Validate datagram checksum
- Validate message format
- Hardware-level error detection

---

## 9. Frame Examples

### 9.1 TargetAssignment Frame (Hex Dump)

```
Ethernet Header:
00:01:02:03:04:05  AA:BB:CC:DD:EE:FF  88 A4

EtherCAT Header:
00 28  (Length=40, Type=0, Reserved=0)

SKYGUARDIS Datagram:
01 01 00 22 [checksum] [target_id] [range] [azimuth] [elevation] [velocity] [priority]
```

### 9.2 EngagementStatus Frame (Hex Dump)

```
Ethernet Header:
AA:BB:CC:DD:EE:FF  00:01:02:03:04:05  88 A4

EtherCAT Header:
00 16  (Length=22, Type=0, Reserved=0)

SKYGUARDIS Datagram:
02 01 00 10 [checksum] [target_id] [state] [firing] [lead_angle] [time_to_impact]
```

---

## 10. Error Handling

### 10.1 Frame-Level Errors

- **Invalid EtherType**: Discard frame (not EtherCAT)
- **Invalid EtherCAT Header**: Discard frame
- **FCS Mismatch**: Discard frame (corruption)
- **Frame Too Short**: Discard frame (malformed)
- **Frame Too Long**: Discard frame (malformed)

### 10.2 Datagram-Level Errors

- **Invalid Message Type**: Discard datagram
- **Invalid Version**: Discard datagram
- **Checksum Mismatch**: Discard datagram
- **Length Mismatch**: Discard datagram
- **Invalid Format**: Discard datagram

### 10.3 Recovery Strategy

- **Transient Errors**: Log and continue
- **Persistent Errors**: Enter safe state
- **Network Errors**: Attempt recovery, default to safe state
- **Critical Errors**: Abort engagement, return to Idle state

---

## 11. Assumptions and Limitations

### 11.1 Design Assumptions

1. **EtherCAT Frame Structure**: Based on standard EtherCAT specification (IEC 61158)
2. **Datagram Format**: Custom design optimized for SKYGUARDIS messages
3. **Timing Requirements**: Assumed based on typical EtherCAT performance
4. **Error Handling**: Simplified for initial implementation

### 11.2 Limitations

1. **Current Implementation**: UDP emulation does not achieve true hard real-time
2. **Frame Structure**: Simplified compared to full EtherCAT specification
3. **Hardware Abstraction**: Future EtherCAT hardware may require adjustments
4. **Network Topology**: Assumes point-to-point or simple ring topology

### 11.3 Future Enhancements

1. **Actual EtherCAT Hardware**: Migrate to real EtherCAT interface
2. **Advanced Error Recovery**: Implement sophisticated error handling
3. **Network Topology Support**: Add support for complex EtherCAT topologies
4. **Performance Optimization**: Optimize for actual hardware characteristics

---

## Document Control

- **Author**: Design Team
- **Review Status**: Draft
- **Approval Status**: Pending Review
- **Version**: 1.0
- **Last Updated**: 2025-01-06

---

**End of EtherCAT Frame Design Document**

