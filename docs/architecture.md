# Architecture Documentation

## System Architecture

```
Radar Simulator (C++)
        ↓ tracks
C2 Controller (C++)
        ↓ assignments
Message Gateway (C++)
        ↕ UDP / IPC
Gun Control Computer (Ada)
        ↓ status & results
Logger / Visualizer (C++)
```

## Component Responsibilities

### C++ Layer
- **Radar Simulator**: Generates synthetic radar tracks
- **C2 Controller**: Threat evaluation and weapon assignment
- **Message Gateway**: Protocol translation and transport
- **Logger**: Execution trace and visualization

### Ada Layer
- **Gun Control**: Engagement state machine
- **Safety Kernel**: Interlocks and constraints
- **Ballistics**: Lead angle and time-of-flight calculations

