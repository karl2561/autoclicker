# Autoclicker

A terminal-based autoclicker for Linux, written in C.

## Description

This program allows you to automate mouse clicks. It provides multiple modes for clicking, including a feature to record and play back a mouse movement pattern. The application runs in the background and is controlled by global hotkeys.

**Disclaimer:** This project is designed for Linux and interacts directly with low-level input devices. It requires running with sufficient permissions (e.g., as root or by adding the user to the `input` group) to read keyboard events and create a virtual mouse device.

## Features

*   **Multiple Clicking Modes:**
    *   Normal: Click indefinitely at a set interval.
    *   Timer: Click for a specified duration.
    *   Amount: Click a specific number of times.
    *   Pattern: Replay a recorded sequence of mouse movements and clicks.
*   **Mouse Pattern Recording:** Record a sequence of movements and clicks to be replayed later.
*   **Runtime Configuration:** Change settings like click interval and mode on the fly using hotkeys.
*   **Persistent Configuration:** Your settings are saved to a `.config` file in the same directory as the executable and loaded on startup.

## WARNING: Hardcoded Device Paths

This application uses hardcoded paths to read keyboard input (e.g., `/dev/input/event3`). This path is **highly likely to be different** on your system, which will cause the program to fail to capture hotkeys.

I wanted to make the paths dynamic, but couldn't be bothered to do so currently.
Additionally whilst record- and play-pattern technically works, I cannot fully verify and operate it, due to limitations with wayland not allowing me to directly letting me set inputs.
It is important to note that the pattern always starts at 0,0 and that the screensize also is hardcoded. I did not bother to fix this, since I am not able to correctly make it work anyways.

Before running, you must identify the correct event device for your keyboard and update the `PATH_KEYEV` constant in `src/utils/constants.h`.

You can find your keyboard's device file by running:
```sh
ls -l /dev/input/by-id/
```
Look for the device that ends with `-event-kbd`. The full path will be something like `/dev/input/eventX`. You will need to recompile the program after changing the path.

## Building from Source

This project uses CMake.

1.  **Clone the repository:**
    ```sh
    git clone <repository-url>
    cd autoclicker
    ```

2.  **Create a build directory:**
    ```sh
    mkdir build && cd build
    ```

3.  **Run CMake and build the project:**
    ```sh
    cmake ..
    make
    ```
    An executable named `autoclicker` will be created in the `build` directory.

## Usage

Run the executable with superuser privileges:
```sh
sudo ./autoclicker
```

The application runs in the terminal and displays the current configuration.

### Default Hotkeys

*   **F5:** Start/Stop Autoclicker
*   **F1:** Start/Stop recording a mouse pattern.
    *   **Left-click** to record a position.
    *   **Right-click** to finish recording.
*   **Arrow Keys:** Navigate and modify settings in the terminal UI.
*   **ESC:** Exit the program.

These keybindings are configurable at runtime and are saved in the `.config` file.

## Documentation

The project is documented using Doxygen. To generate the documentation:

1.  **Install Doxygen:**
    ```sh
    sudo apt-get install doxygen graphviz # Example for Debian/Ubuntu
    ```
2.  **Generate the documentation:**
    ```sh
    doxygen Doxyfile
    ```
    The output will be in the `build/docs/` directory.

## License

This project is not licensed. Please add a license file.
