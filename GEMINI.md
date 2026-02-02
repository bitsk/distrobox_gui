# Distrobox GUI Manager

## Project Overview
This project aims to create a graphical user interface (GUI) for managing development environments, acting as a wrapper and manager for `distrobox`. It allows users to easily create, list, enter, and manage containerized development environments via a modern desktop application.

**Core Objectives:**
1.  **Container Management:** Create, delete, stop, and inspect containers.
2.  **Environment Integration:** Seamlessly enter containers via terminal integration.
3.  **Visual Feedback:** Real-time status updates of container states.

## Architecture

The application is built using **Python** and **PyQt6**, utilizing modern styling libraries to ensure a polished look.

### Technology Stack
*   **Language:** Python 3.x
*   **GUI Framework:** `PyQt6` (Qt 6 bindings for Python).
*   **Theming:** `qt-material` (or custom QSS) for modern Material Design styling.
*   **Backend Interface:** `subprocess` module to wrap `distrobox` CLI commands.

### File Structure Plan
*   `main.py`: Application entry point. Sets up the `QApplication` and applies the theme.
*   `ui/`: Directory containing UI components.
    *   `main_window.py`: The primary application window (`QMainWindow`).
    *   `dialogs.py`: Popups for "Create Container", settings, etc.
    *   `widgets.py`: Custom reuseable widgets (e.g., a card representing a container).
*   `core/`: Directory for business logic.
    *   `manager.py`: Wrapper class for `distrobox` CLI commands.
    *   `models.py`: Data structures representing a Container.
*   `assets/`: Icons and static resources.

## Development Roadmap

1.  **Phase 1: Foundation**
    *   Set up project dependencies (`PyQt6`, theme library).
    *   Create the main window skeleton with the applied theme.
    *   Implement the basic `DistroboxManager` class to run `distrobox list`.

2.  **Phase 2: The Dashboard (Read Only)**
    *   Design a "Card" or "List" view to display active containers.
    *   Bind the backend data to the UI.

3.  **Phase 3: Interaction (Write Operations)**
    *   Implement "Enter" functionality (launching terminal).
    *   Implement "Stop" and "Delete" actions.

4.  **Phase 4: Creation Wizard**
    *   Build a form dialog to create new boxes (Name, Image, Flags).

## Building and Running

### Prerequisites
*   Python 3
*   `distrobox` installed on the host.

### Installation
```bash
pip install PyQt6 qt-material
```

### Run Command
```bash
python main.py
```

## Development Conventions
*   **Code Style:** PEP 8.
*   **Architecture:** MVC (Model-View-Controller) pattern where feasible. Use Qt Signals/Slots for communication between backend logic and UI.
*   **Type Hinting:** Strictly used for all method signatures.