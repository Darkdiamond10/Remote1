# remocolab: Distributed Worker Engine
remocolab is a Python module to allow remote access and distributed resource management on [Google Colaboratory](https://colab.research.google.com/), Kaggle, and Azure instances. It supports SSH, [TurboVNC](https://www.turbovnc.org/), and an automated background Worker Orchestration mode.

- [FAQ](https://github.com/demotomohiro/remocolab/wiki/Frequently-Asked-Questions)

## Features
- **Distributed Worker Mode:** Automated background connectivity for managed resource tasks.
- **Stealth Connectivity:** Uses [Argo Tunnel (Cloudflare)](https://blog.cloudflare.com/a-free-argo-tunnel-for-your-next-project/) for resilient, firewall-friendly access without third-party tokens.
- **Resource Persistence:** Built-in "keep-alive" heartbeat to prevent environment timeouts during long-running tasks.
- **Graphics Support:** Installs [VirtualGL](https://www.virtualgl.org/) and TurboVNC for remote desktop interaction with GPU acceleration.

## Requirements
- Google, Kaggle, or Azure account.
- SSH client for interactive access.
- (Optional) [TurboVNC Viewer](https://sourceforge.net/projects/turbovnc/files/) for graphical interaction.

## Setup Instructions

### 1. Distributed Worker Mode (Recommended)
This mode initializes the environment as a headless managed node that connects back to a central orchestrator.

```python3
!pip install git+https://github.com/demotomohiro/remocolab.git
import remocolab
# master_node: Your control server IP or hostname
# port: Your listener port
remocolab.setupWorker(master_node="YOUR_IP", port=PORT)
```

### 2. Interactive SSH Mode
Initializes a secure SSH server accessible via Cloudflare Argo Tunnel.

```python3
!pip install git+https://github.com/demotomohiro/remocolab.git
import remocolab
remocolab.setupSSHD()
```

- SSH and TurboVNC:
```python3
!pip install git+https://github.com/demotomohiro/remocolab.git
import remocolab
remocolab.setupVNC()
```
4. (Optional) If you want to run OpenGL applications or any programs that use GPU,
Click "Runtime" -> "Change runtime type" in top menu and change Hardware accelerator to GPU. 
5. Run that cell
6. remocolab will set up the Argo Tunnel and SSH server (and desktop environment if requested).
7. root and colab user passwords and the SSH connection command will be displayed.
8. Copy and paste the SSH command to your local terminal to connect.

* If you use TurboVNC:
9. Run TurboVNC viewer on your local machine, set server address to ``localhost:1`` and connect.
10. Use the VNC password displayed in the output.

When you got error and want to rerun `remocolab.setupVNC()` or `remocolab.setupSSHD()`, you need to do `factory reset runtime` before rerun the command.
As you can run only 1 ngrok process with free ngrok account, running `remocolab.setupVNC/setupSSHD` will fail if there is another instace that already ran remocolab.
In that case, terminate another instance from `Manage sessions` screen.

## How to run OpenGL applications
Put the command to run the OpenGL application after ``vglrun``.
For example, ``vglrun firefox`` runs firefox and you can watch web sites using WebGL with hardware acceleration.

## How to mount Google Drive
remocolab can allow colab user reading/writing files on Google Drive.
If you just mount drive on Google Colaboratory, only root can access it.

Click the folder icon on left side of Google Colaboratory and click mount Drive icon.
If you got new code cell that contains python code "from google.colab import ...", create a new notebook, copy your code to it and mount drive same way.
On new notebook, you can mount drive without getting such code cell.
  - You can still mount google drive by clicking the given code cell, but it requres getting authorization code and copy&pasting it everytime you run your notebook.
  - If you mount google drive on new notebook, it automatically mount when your notebook connect to the instance.

Add `mount_gdrive_to` argument with directory name to `remocolab.setupSSHD()` or `remocolab.setupVNC()`.
For example:
```python
  remocolab.setupSSHD(mount_gdrive_to = "drive")
```
Then, you can access the content of your drive in `/home/colab/drive`.
You can also mount specific directory on your drive under colab user's home directory.
For example:
```python
  remocolab.setupSSHD(mount_gdrive_to = "drive", mount_gdrive_from = "My Drive/somedir")
```

## Arguments of `remocolab.setupSSHD()` and `remocolab.setupVNC()`
- `check_gpu_available`
  When it is `True`, it checks whether GPU is available and shows a warning if not.
- `mount_gdrive_to`
  Specify a directory under colab user's home directory which is used to mount Google Drive.
  If it was not specified, Google Drive is not mount under colab user's home directory.
  Specifying it without mounting Google Drive on Google Colaboratory is error.
- `mount_gdrive_from`
  Specify a path of existing directory on your Google Drive which is mounted on the directory specified by `mount_gdrive_to`.
  This argument is ignored when `mount_gdrive_to` was not specified.
- `public_key`
  Specify ssh public key if you want to use public key authentication.
## How to setup public key authentication for root login
If you want to login as root, use following code:
```python3
!mkdir /root/.ssh
with open("/root/.ssh/authorized_keys", 'w') as f:
  f.write("my public key")
!chmod 700 /root/.ssh
!chmod 600 /root/.ssh/authorized_keys
```
And replace user name colab in ssh command to root.

## Experimental kaggle support
remocolab in kaggle branch works on [Kaggle](https://www.kaggle.com/).
1. Create a new Notebook with Python language.
2. Set settings to:
   - Internet on
   - Docker to Latest Available
   - GPU on if you use TurboVNC
3. Add a code cell and copy & paste one of following codes to the cell

- SSH only:
```python3
!pip install git+https://github.com/demotomohiro/remocolab.git@kaggle
import remocolab
remocolab.setupSSHD()
```

- SSH and TurboVNC:
```python3
!pip install git+https://github.com/demotomohiro/remocolab.git@kaggle
import remocolab
remocolab.setupVNC()
```

4. Follow instructions from step 4 in above "How to use".
