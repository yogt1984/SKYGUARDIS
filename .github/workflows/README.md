# GitHub Actions CI/CD

This directory contains GitHub Actions workflows for continuous integration and testing.

## Workflows

### `ci.yml` - Main CI Pipeline

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop` branches
- Manual trigger via `workflow_dispatch`

**What it does:**
1. Sets up C++ build environment (g++, cmake, build-essential)
2. Optionally sets up Ada build environment (gnat)
3. Builds all C++ components
4. Optionally builds Ada components (if gnat is available)
5. Runs all C++ test executables
6. Reports test results with summary
7. Uploads test logs on failure

**Test Execution:**
- Automatically discovers all `test_*` executables in `build/`
- Runs each test individually
- Captures output and exit codes
- Provides detailed pass/fail summary
- Fails the workflow if any test fails

### `test-only.yml` - Quick Test Run

**Triggers:**
- Manual trigger only (`workflow_dispatch`)

**What it does:**
- Quick test run without full build
- Useful for debugging test issues

## Test Coverage

The CI runs all 9 C++ test suites:

1. `test_ballistics_comprehensive` - Ballistics calculations
2. `test_comprehensive_integration` - End-to-end integration
3. `test_logging` - Enhanced logging system
4. `test_message_gateway` - Message serialization & UDP
5. `test_radar_simulation` - Radar simulation (17 tests)
6. `test_safety_comprehensive` - Safety & threat evaluation
7. `test_state_machine_integration` - State machine integration
8. `test_threat_evaluator` - Threat evaluation algorithms
9. `test_visualization` - Visualization system (13 tests)

**Total: 50+ individual test cases**

## Status Badge

Add this to your README.md to show CI status:

```markdown
![CI Status](https://github.com/YOUR_USERNAME/SKYGUARDIS/workflows/CI%20-%20Build%20and%20Test/badge.svg)
```

## Local Testing

To test the CI workflow locally, you can use [act](https://github.com/nektos/act):

```bash
# Install act
curl https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash

# Run the CI workflow locally
act -j build-and-test
```

## Troubleshooting

### Tests fail in CI but pass locally

1. Check that all dependencies are installed
2. Verify test executables are built correctly
3. Check for environment-specific issues (paths, permissions)

### Ada tests not running

- Ada/GNAT is optional in CI
- C++ tests will still run if Ada is unavailable
- To enable Ada tests, ensure `gnatmake` is available in the CI environment

### Build failures

- Check that all source files are committed
- Verify CMake configuration is correct
- Ensure all required headers are present

