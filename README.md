# Loadout

![status](https://img.shields.io/badge/status-alpha-orange)
![license](https://img.shields.io/badge/license-Apache%202.0-blue)

**Loadout** is a lightweight desktop session orchestrator for Windows. It watches for processes and runs user-defined actions like closing browsers when games launch or opening communication apps automatically.

> Prototype stage, expect bugs and breaking changes.

---

## Demo

**Current prototype in action:**

[![Loadout Demo](https://img.youtube.com/vi/r2Zaor9yPTA/0.jpg)](https://youtu.be/r2Zaor9yPTA)

> *When Valorant starts → automatically closes browsers + launches Discord & OBS*

---

**Terminal Output Screenshot:**

![Terminal demo of Loadout prototype](demo/loadout-demo.png)

> *Early prototype verifying Valorant launch, then executing automation commands.*




## Features

- Process monitoring and lifecycle detection
- Automatic app management based on triggers
- Command system for launching/closing applications *(for now)*
- Event-driven architecture with logging

---

## Build

Requires Windows 10/11, CMake 3.20+, and C++17 compiler.

```bash
cmake --preset default && cmake --build --preset default
```

or 


```bash
cmake --preset release && cmake --build --preset release
```

---

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
