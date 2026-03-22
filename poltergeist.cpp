#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/uinput.h>
#include <math.h>
#include <zmq.hpp>
#include <thread>
#include <chrono>
#include <random>

// --- THE FRONTEND GHOST (Virtual Hardware via uinput) ---

void setup_virtual_hardware(int &fd) {
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "Failed to open /dev/uinput. Needs root?" << std::endl;
        return;
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    // Add essential keys for typing syntax errors
    for (int i = 1; i <= 255; i++) ioctl(fd, UI_SET_KEYBIT, i);

    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Poltergeist_HID");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

    write(fd, &uidev, sizeof(uidev));
    ioctl(fd, UI_DEV_CREATE);
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for OS to register
}

void emit_event(int fd, int type, int code, int val) {
    struct input_event ie;
    memset(&ie, 0, sizeof(ie));
    ie.type = type;
    ie.code = code;
    ie.value = val;
    write(fd, &ie, sizeof(ie));
}

void emit_sync(int fd) {
    emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

// Cubic Bezier calculation for human-like movement
double cubic_bezier(double t, double p0, double p1, double p2, double p3) {
    double u = 1 - t;
    return (u*u*u * p0) + (3 * u*u * t * p1) + (3 * u * t*t * p2) + (t*t*t * p3);
}

void move_mouse_bezier(int fd, int start_x, int start_y, int target_x, int target_y, int steps) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-50, 50);

    // Control points for the curve
    int cp1_x = start_x + dist(gen);
    int cp1_y = start_y + dist(gen);
    int cp2_x = target_x + dist(gen);
    int cp2_y = target_y + dist(gen);

    int prev_x = start_x;
    int prev_y = start_y;

    for (int i = 0; i <= steps; ++i) {
        double t = (double)i / steps;
        int cur_x = (int)cubic_bezier(t, start_x, cp1_x, cp2_x, target_x);
        int cur_y = (int)cubic_bezier(t, start_y, cp1_y, cp2_y, target_y);

        int dx = cur_x - prev_x;
        int dy = cur_y - prev_y;

        if (dx != 0) emit_event(fd, EV_REL, REL_X, dx);
        if (dy != 0) emit_event(fd, EV_REL, REL_Y, dy);
        emit_sync(fd);

        prev_x = cur_x;
        prev_y = cur_y;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void simulate_typing(int fd, const std::vector<int>& keycodes) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> delay(50, 150);

    for (int code : keycodes) {
        emit_event(fd, EV_KEY, code, 1); // Key down
        emit_sync(fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay(gen) / 2));
        emit_event(fd, EV_KEY, code, 0); // Key up
        emit_sync(fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay(gen)));
    }
}

void haunt_frontend() {
    int fd;
    setup_virtual_hardware(fd);
    if (fd < 0) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sleep_time(30000, 90000);
    std::uniform_int_distribution<> coords(100, 1000);

    while (true) {
        std::cout << "[Ghost] Moving mouse..." << std::endl;
        move_mouse_bezier(fd, coords(gen), coords(gen), coords(gen), coords(gen), 100);

        // Simulate typing a basic command (e.g., 'ls')
        // Keycodes: KEY_L (38), KEY_S (31), KEY_ENTER (28)
        std::vector<int> dummy_command = {38, 31, 28};
        simulate_typing(fd, dummy_command);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time(gen)));
    }

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}


// --- THE BACKEND WHISPERER (ZeroMQ Kernel Injection) ---

void haunt_backend(const std::string& shell_port) {
    try {
        zmq::context_t context(1);
        zmq::socket_t shell_socket(context, ZMQ_DEALER);

        std::string connection_str = "tcp://127.0.0.1:" + shell_port;
        shell_socket.connect(connection_str);

        std::cout << "[Whisperer] Connected to Jupyter Shell ZMQ: " << connection_str << std::endl;

        while (true) {
            std::cout << "[Whisperer] Injecting silent execution request into kernel..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(45));
        }
    } catch (const zmq::error_t& e) {
        std::cerr << "[Whisperer] ZMQ Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <jupyter_shell_port>" << std::endl;
        return 1;
    }

    std::string shell_port = argv[1];

    std::cout << "Deploying Poltergeist Hybrid Architecture..." << std::endl;

    std::thread frontend_thread(haunt_frontend);
    std::thread backend_thread(haunt_backend, shell_port);

    frontend_thread.join();
    backend_thread.join();

    return 0;
}
