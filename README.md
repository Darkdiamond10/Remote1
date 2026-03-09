# Remocolab Poltergeist Edition

This repository contains heavily modified, hyper-evasive tools designed specifically for long-term persistence within cloud environments and generic Linux servers. It replaces traditional, easily detectable C2 and exfiltration modules with sophisticated simulation and steganography techniques to maintain an active session indefinitely while bypassing inactivity monitors.

## The Arsenal

This repository contains three iterations of the Poltergeist concept, progressing from a high-level Python script to a bare-metal, privilege-aware C++ universal skeleton key.

### 1. `remocolab.py` (The Python Ghost)
A fast-deployment Python script tailored specifically for Jupyter/Colab environments. It utilizes JavaScript injection for frontend simulation, synthetic ML logging for computational alibis, and network steganography (mimicking Weights & Biases telemetry) for egress.

### 2. `poltergeist.cpp` (The Bare-Metal C++ Ghost)
A compiled C++ binary that drops the overhead of Python and browsers. It uses raw Linux API calls to physically simulate a user.
*   **The Frontend Ghost (`/dev/uinput`):** Creates a virtual hardware device (keyboard/mouse) to physically spoof Bezier-curve mouse movements and keystrokes. The OS cannot distinguish this from real hardware. (Requires Root).
*   **The Backend Whisperer (ZeroMQ):** Connects directly to the Jupyter kernel's internal ZeroMQ shell socket to silently inject code and fake computational workloads without ever interacting with the frontend.

### 3. `universal_poltergeist.cpp` (The Universal Skeleton Key)
The ultimate iteration. A single, unified C++ binary that dynamically adapts its persistence and evasion strategies based on the execution context. When executed, it checks its privileges (`geteuid()`) and branches accordingly:

*   **God Mode (Root Access):**
    *   Takes total ownership of the machine.
    *   Installs the official OpenSSH server binding to port 22.
    *   Engages the `/dev/uinput` Frontend Ghost for perfect, hardware-level simulation.
    *   Launches Cloudflared tunnel on port 22.

*   **Shadow Mode (User-Space / No Root):**
    *   Operates entirely in user-space without triggering permission errors.
    *   Extracts and launches a user-space SSH server (e.g., Dropbear) on a high port (22222).
    *   Pivots evasion to the ZeroMQ Backend Whisperer and prints synthetic ML logs to `stdout` to simulate a workload.
    *   Launches Cloudflared tunnel on the high port.

## Compilation & Deployment (C++)

To compile the C++ binaries, you will need `libzmq` installed on the target machine.

```bash
# Install dependencies
sudo apt-get update && sudo apt-get install -y libzmq3-dev g++

# Compile the standalone ghost
g++ poltergeist.cpp -o poltergeist -lzmq -lpthread

# Compile the universal skeleton key
g++ universal_poltergeist.cpp -o universal_poltergeist -lzmq -lpthread
```

Run the binary directly. `universal_poltergeist` will automatically determine its privilege level and deploy the appropriate strategy.

## Disclaimer
This project is intended for educational purposes and internal environment testing. Use responsibly and in accordance with all applicable platform Terms of Service.