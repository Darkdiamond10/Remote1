# Remocolab Poltergeist Edition

This is a heavily modified, hyper-evasive version of the `remocolab.py` utility designed specifically for long-term persistence within cloud environments (e.g., Google Colab). It replaces traditional, easily detectable C2 and exfiltration modules with sophisticated simulation and steganography techniques to maintain an active session indefinitely while bypassing inactivity monitors.

## Core Architecture

The script operates through four specialized layers:

### Layer 1: Asymmetric Simulation Core (Spectrum Generator)
To maximize available compute (e.g., for cryptocurrency mining) while evading detection, this layer simulates a high-intensity machine learning workload without actually consuming resources.
*   **High-Fidelity Log Faking:** A low-priority thread continuously prints realistic, algorithmically generated training logs (epochs, loss, accuracy) to `stdout`.
*   **Static Memory Loading:** Uses `mmap` with a sparse file to reserve a massive block of virtual memory (4GB+), simulating a large dataset load without consuming physical RAM.
*   **Resource Management:** Employs `os.nice(19)` to ensure simulation threads yield CPU to the primary payload.

### Layer 3: Elastic Network & Protocol Steganography
Maintains network presence and masks C2 communication as legitimate background noise.
*   **"Slow-Loris" Alibi:** Initiates a heavily throttled (approx. 10 KB/s) download of a large dataset to justify baseline, continuous network activity.
*   **Synthetic Telemetry Tunnel (STT):** Encapsulates C2 commands and payload data within JSON payloads formatted to exactly mimic heartbeat telemetry from industry-standard tools like Weights & Biases (W&B) or MLflow over HTTPS.

### Layer 4: The Digital Poltergeist (Human Behavior Modeling)
The ultimate evasion tactic. Injects a JavaScript payload directly into the frontend to perfectly simulate human interaction, defeating client-side timeouts.
*   **Human Behavior Modeling (HBM):** Programmatically creates new Code cells and types realistic Python snippets (`import torch`, `model.summary()`) with variable, human-like keystroke delays.
*   **Error Simulation:** Intentionally introduces syntax errors during typing, executes the cell, waits to "read" the traceback, fixes the error, and re-executes.
*   **High-Fidelity Mimicry:** Utilizes Bezier curve algorithms for all simulated mouse movements and incorporates phantom scrolling behavior.

## Deployment

Simply import the modified `remocolab.py` script into your notebook environment and execute the `setupVNC` or `setupSSHD` functions as normal. The evasion layers deploy automatically and run silently in the background.

```python
import remocolab
remocolab.setupVNC(check_gpu_available=True)
```

## Disclaimer
This project is intended for educational purposes and internal environment testing. Use responsibly and in accordance with all applicable platform Terms of Service.