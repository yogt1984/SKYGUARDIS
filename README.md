# SKYGUARDIS
SKYDOME is a mixed C++/Ada software-in-the-loop simulator of a short-range air-defence unit. C++ implements the command-and-control layer with radar track handling and threat evaluation, while Ada runs the real-time, safety-critical gun-control and engagement logic, demonstrating deterministic control and high-integrity design.


Absolutely — here’s a **polished, employer-ready `README.md`** you can drop straight into your repo.
It’s written to signal **systems thinking, safety-critical mindset, Ada competence, C++ architecture maturity, and defence-domain awareness** — without implying access to real Rheinmetall IP.

---

# **SKYDOME — Short-Range Air Defence Simulation (C++ + Ada)**

**SKYDOME** is a **software-in-the-loop simulation** of a short-range air-defence fire unit, inspired by modern networked systems such as Rheinmetall’s Skynex and Skyranger platforms.

The project demonstrates **mixed-language safety-critical software design**:

✅ **C++** — Command & Control (C2), radar track handling, threat prioritization
✅ **Ada** — Real-time gun-control logic, safety interlocks, deterministic engagement state-machine

The goal is to model how a **battle-management node assigns aerial threats to an autonomous effector**, while the **Ada subsystem executes engagements predictably and safely** — as expected in **mission-critical defence environments**.

This project is designed as a **portfolio-level demonstration of architecture thinking, correctness, clarity, and discipline** — not as a weapon system.

---

## 🚀 **Motivation**

Modern air-defence software must be:

* **reliable**
* **predictable**
* **testable**
* **maintainable over decades**

This is why safety-critical modules are often written in **Ada/SPARK**, while **C++** powers scalable control and integration layers.

**SKYDOME reproduces this split intentionally.**

---

## 🧠 **System Overview**

### **C++ — Command-and-Control Layer**

The C++ C2 node simulates:

* Radar-like track generation
* Target classification & threat scoring
* Weapon assignment logic
* Scenario orchestration & logging

It communicates with the gun controller via a **clean message protocol** (UDP / IPC depending on build).

---

### **Ada — Gun-Control Computer**

The Ada subsystem models:

* A deterministic **engagement state machine**
* **Lead-angle & time-of-flight calculations** (simplified ballistics)
* **Safety-zone enforcement** (no-fire arcs, angle limits)
* **Real-time periodic control tasks**
* Strong type-safety using range-restricted physical units

This demonstrates **Ada strengths in high-integrity domains**:

* Runtime range checks
* Explicit concurrency
* Contracts & defensive design
* SPARK-friendly style where possible

---

## 🏗 **Architecture**

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

**Clear separation of responsibility:**

| Layer             | Language  | Responsibility                       |
| ----------------- | --------- | ------------------------------------ |
| C2 Node           | C++       | Sensor tracks, threat logic, tasking |
| Protocol          | C++       | Message definition & transport       |
| Fire-Control Core | Ada       | Real-time deterministic engagement   |
| Safety Kernel     | Ada       | Interlocks & constraints             |
| Tools & Tests     | C++ / Ada | Simulation & verification            |

---

## 📋 **Key Features**

✔ Mixed-language safety-critical architecture
✔ Real-time control loop (Ada tasks)
✔ Strong typing for physical quantities
✔ Deterministic engagement behaviour
✔ Scenarios: single target / swarm / saturation
✔ Logging & inspectable execution trace

---

## 🧪 **Testing Approach**

This repo emphasizes **structured verification**:

* Unit tests for:

  * threat-evaluation logic (C++)
  * ballistic & kinematic helpers (Ada)
* Deterministic control loop assertions
* Fault-injection (sensor dropouts, invalid ranges)
* Reproducible simulation scenarios

---

## 🔐 **Ethics & Scope**

This project is:

* **non-operational**
* **non-export-controlled**
* **mathematically simplified**
* **for educational & professional-portfolio use only**

It does **NOT** model classified, proprietary, or tactical behaviour.

---

## ⚙️ **Build & Run**

### Requirements

* GCC / Clang
* GNAT Ada compiler
* CMake ≥ 3.16
* Linux recommended

```bash
mkdir build && cd build
cmake ..
make
```

Run C2 node & gun controller in separate terminals.

---

## 🧭 **Why This Project Matters**

This project demonstrates capability in:

🛡 **Safety-critical system design**
🧩 **Mixed-language embedded architecture**
⏱ **Real-time software concepts**
📐 **Deterministic control logic**
📜 **Readable, certifiable-style code discipline**
🤝 **Cross-team collaboration mindset**

All qualities expected in **aerospace & defence software engineering roles**.

---

## 📘 **Future Extensions**

* SPARK subset proof annotations
* Hardware-in-the-loop extension
* More advanced TEWA logic
* Graphical operator console
* Sensor fusion improvements

---

## 👤 **Author**

**Yigit Onat** — Embedded & systems engineer with focus on
high-integrity software, trading infrastructure, and real-time platforms.

---

If you want, I can now:

🔹 tailor the README tone to match **German-speaking engineering culture**
🔹 add **badges, diagrams, and CI config**
🔹 write **`requirements.md` and `architecture.md`**
🔹 help scaffold the repo structure & code skeletons

This is already an *excellent* employer-ready showcase — we can now sharpen it to perfection 💼
