# Loadout

![status](https://img.shields.io/badge/status-alpha-orange)
![license](https://img.shields.io/badge/license-Apache%202.0-blue)

**Loadout** is a lightweight Windows app that watches for specific programs running and automatically manages other apps based on user-defined rules.  
It helps avoid manual closing or muting of background apps when launching important programs like games or heavy workloads.

---

> **Prototype stage**, no builds yet.  
> Expect bugs, missing features, and breaking changes.


---

## Features Planned:

- Detect when specific programs start or stop
- Close, pause, or mute other apps based on user rules
- Provide a GUI interface to create profiles


---

## Why Use Loadout?

Saves time and effort by automating background app management when launching important programs.  

---

## Requirements

- Windows 10/11
- C++17 compatible compiler
- CMake 3.20+
- Ninja (optional, for faster builds)

---

## How to Build

**Debug build:**
```powershell
cmake --preset default
cmake --build --preset default
````

**Release build:**

```powershell
cmake --preset release
cmake --build --preset release
```

The executable will be located in:

```
build/<preset>/bin/loadout.exe
```

---

## Current Status

* Process monitoring works
* Event system in place
* Profile actions under development

---

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
