# File Structure

```
SKYGUARDIS/
├── README.md
├── CMakeLists.txt              # C++ build configuration
├── skyguardis.gpr              # Ada GNAT project file
├── .gitignore
│
├── include/                    # Header files
│   ├── cpp/
│   │   ├── c2_controller/
│   │   │   ├── c2_controller.hpp
│   │   │   └── threat_evaluator.hpp
│   │   ├── radar_simulator/
│   │   │   └── radar_simulator.hpp
│   │   ├── message_gateway/
│   │   │   ├── message_gateway.hpp
│   │   │   └── protocol.hpp
│   │   └── logger/
│   │       ├── logger.hpp
│   │       └── visualizer.hpp
│   └── ada/
│       ├── gun_control/
│       │   └── engagement_state_machine.ads
│       ├── safety_kernel/
│       │   └── safety_interlocks.ads
│       └── ballistics/
│           └── ballistic_calculator.ads
│
├── src/                        # Source files
│   ├── cpp/
│   │   ├── c2_controller/
│   │   │   ├── c2_controller.cpp
│   │   │   └── threat_evaluator.cpp
│   │   ├── radar_simulator/
│   │   │   ├── radar_simulator.cpp
│   │   │   └── track_generator.cpp
│   │   ├── message_gateway/
│   │   │   ├── message_gateway.cpp
│   │   │   └── protocol.cpp
│   │   └── logger/
│   │       ├── logger.cpp
│   │       └── visualizer.cpp
│   └── ada/
│       ├── gun_control/
│       │   ├── gun_control.ads
│       │   └── engagement_state_machine.adb
│       ├── safety_kernel/
│       │   ├── safety_kernel.ads
│       │   └── safety_interlocks.adb
│       ├── ballistics/
│       │   ├── ballistics.ads
│       │   └── ballistic_calculator.adb
│       └── common/
│           ├── common.ads
│           └── types.ads
│
├── tests/                      # Test files
│   ├── cpp/
│   │   ├── CMakeLists.txt
│   │   └── test_threat_evaluator.cpp
│   └── ada/
│       └── test_ballistics.adb
│
├── docs/                       # Documentation
│   ├── architecture.md
│   ├── requirements.md
│   └── file_structure.md
│
├── scripts/                    # Build and utility scripts
│   └── build.sh
│
└── build/                      # Build output (generated)
```

## Component Organization

### C++ Components
- **c2_controller**: Command & Control logic, threat evaluation
- **radar_simulator**: Synthetic radar track generation
- **message_gateway**: Protocol definition and transport
- **logger**: Logging and visualization

### Ada Components
- **gun_control**: Engagement state machine
- **safety_kernel**: Safety interlocks and constraints
- **ballistics**: Lead angle and time-of-flight calculations
- **common**: Shared types and utilities

