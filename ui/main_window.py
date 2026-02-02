from PyQt6.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton, 
    QScrollArea, QMessageBox, QApplication, QProgressDialog
)
from PyQt6.QtCore import Qt, QThread, QTimer
import subprocess
import qtawesome as qta

from core.manager import DistroboxManager
from core.i18n import tr
from core.threads import CreateBoxWorker, InstallWorker
from ui.widgets import ContainerCard
from ui.dialogs import CreateContainerDialog
from ui.styles import MAIN_STYLE

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle(tr("app_title"))
        self.setMinimumSize(950, 650)
        
        # Apply Global Custom Styles
        self.setStyleSheet(MAIN_STYLE)
        
        self.manager = DistroboxManager()

        # Main Layout Structure
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        self.main_layout = QVBoxLayout(central_widget)
        self.main_layout.setContentsMargins(30, 30, 30, 30)
        self.main_layout.setSpacing(20)

        # 1. Header Area
        self.setup_header()

        # 2. Container List Area (Scrollable)
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setFrameShape(QScrollArea.Shape.NoFrame)
        self.scroll_area.setStyleSheet("background-color: transparent;") # Transparent background
        
        # This widget holds all the cards
        self.cards_container = QWidget()
        self.cards_container.setStyleSheet("background-color: transparent;")
        self.cards_layout = QVBoxLayout(self.cards_container)
        self.cards_layout.setAlignment(Qt.AlignmentFlag.AlignTop)
        self.cards_layout.setSpacing(15)
        
        self.scroll_area.setWidget(self.cards_container)
        self.main_layout.addWidget(self.scroll_area)

        # Initial Load
        self.refresh_list()

    def setup_header(self):
        header_layout = QHBoxLayout()
        
        # Icon + Title
        title_box = QHBoxLayout()
        icon_label = QLabel()
        icon_label.setPixmap(qta.icon('fa5s.cubes', color='white').pixmap(32, 32))
        title_box.addWidget(icon_label)
        
        title_label = QLabel(tr("header_title"))
        title_label.setObjectName("HeaderTitle")
        title_box.addWidget(title_label)
        
        header_layout.addLayout(title_box)
        header_layout.addStretch() # Spacer
        
        # Actions
        actions_box = QHBoxLayout()
        actions_box.setSpacing(10)
        
        btn_height = 40
        
        self.refresh_btn = QPushButton(tr("refresh"))
        self.refresh_btn.setIcon(qta.icon('fa5s.sync-alt', color='white'))
        self.refresh_btn.setFixedWidth(130)
        self.refresh_btn.setFixedHeight(btn_height)
        self.refresh_btn.setCursor(Qt.CursorShape.PointingHandCursor)
        self.refresh_btn.clicked.connect(self.refresh_list)
        actions_box.addWidget(self.refresh_btn)

        self.create_btn = QPushButton(tr("create_new"))
        self.create_btn.setObjectName("CreateBtn")
        self.create_btn.setIcon(qta.icon('fa5s.plus', color='white'))
        self.create_btn.setFixedHeight(btn_height)
        self.create_btn.setPadding = 10
        self.create_btn.setCursor(Qt.CursorShape.PointingHandCursor)
        self.create_btn.clicked.connect(self.create_box_dialog)
        actions_box.addWidget(self.create_btn)
        
        header_layout.addLayout(actions_box)
        self.main_layout.addLayout(header_layout)

    def refresh_list(self):
        # Clear existing items
        while self.cards_layout.count():
            item = self.cards_layout.takeAt(0)
            widget = item.widget()
            if widget:
                widget.deleteLater()

        if not self.manager.is_distrobox_installed():
            self.show_install_ui()
            return

        containers = self.manager.get_containers()
        
        if not containers:
            empty_box = QVBoxLayout()
            empty_icon = QLabel()
            empty_icon.setPixmap(qta.icon('fa5s.box-open', color='#5c6370').pixmap(64, 64))
            empty_icon.setAlignment(Qt.AlignmentFlag.AlignCenter)
            
            empty_label = QLabel(tr("empty_list"))
            empty_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            empty_label.setStyleSheet("color: #abb2bf; font-size: 16px; margin-top: 10px;")
            
            empty_container = QWidget()
            empty_layout = QVBoxLayout(empty_container)
            empty_layout.addStretch()
            empty_layout.addWidget(empty_icon)
            empty_layout.addWidget(empty_label)
            empty_layout.addStretch()
            
            self.cards_layout.addWidget(empty_container)
            return

        for container in containers:
            image = container.get('image', 'Unknown')
            icon = self.manager.get_icon_name(image)
            
            card = ContainerCard(container, icon)
            card.enter_clicked.connect(self.on_enter_box)
            card.stop_clicked.connect(self.on_stop_box)
            card.delete_clicked.connect(self.on_delete_box)
            
            self.cards_layout.addWidget(card)

    def show_install_ui(self):
        container = QWidget()
        layout = QVBoxLayout(container)
        layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.setSpacing(20)
        
        icon = QLabel()
        icon.setPixmap(qta.icon('fa5s.download', color='#e06c75').pixmap(64, 64))
        icon.setAlignment(Qt.AlignmentFlag.AlignCenter)
        
        label = QLabel(tr("error_distrobox_missing"))
        label.setStyleSheet("font-size: 20px; font-weight: bold; color: #e06c75;")
        label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(label)
        
        desc = QLabel(tr("install_desc"))
        desc.setStyleSheet("color: #abb2bf; font-size: 14px;")
        desc.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(desc)
        
        btn = QPushButton(tr("install_btn"))
        btn.setObjectName("CreateBtn") # Greenish via style override or reuse blue
        btn.setStyleSheet("background-color: #98c379; border: none;") # Override to green
        btn.setCursor(Qt.CursorShape.PointingHandCursor)
        btn.setFixedWidth(250)
        btn.clicked.connect(self.on_install_distrobox)
        layout.addWidget(btn)
        
        self.cards_layout.addWidget(container)

    def on_install_distrobox(self):
        self.install_progress = QProgressDialog("Installing Distrobox...", None, 0, 0, self)
        self.install_progress.setWindowTitle("Please Wait")
        self.install_progress.setWindowModality(Qt.WindowModality.WindowModal)
        self.install_progress.show()

        self.install_thread = QThread()
        self.install_worker = InstallWorker(self.manager)
        self.install_worker.moveToThread(self.install_thread)
        
        self.install_thread.started.connect(self.install_worker.run)
        self.install_worker.finished.connect(self.install_thread.quit)
        self.install_worker.finished.connect(self.install_worker.deleteLater)
        self.install_thread.finished.connect(self.install_thread.deleteLater)
        
        self.install_worker.finished.connect(self.on_install_finished)
        
        self.install_thread.start()

    def on_install_finished(self, success):
        self.install_progress.close()
        if success:
            QMessageBox.information(self, "Success", tr("success_install"))
            self.refresh_list()
        else:
            QMessageBox.critical(self, "Error", tr("error_install"))

    def on_enter_box(self, name: str):
        # Map of terminal executables to their 'execute command' flag
        terminal_map = {
            "gnome-terminal": "--",
            "mate-terminal": "--",
            "xfce4-terminal": "--",
            "tilix": "--",
            "konsole": "-e",
            "alacritty": "-e",
            "xterm": "-e",
            "kitty": "", # Kitty usually doesn't need a flag, just 'kitty command'
            "foot": "" 
        }
        
        found_term = None
        flag = ""
        
        # Try to find an installed terminal from our list
        for term, f in terminal_map.items():
            if subprocess.run(["which", term], capture_output=True).returncode == 0:
                found_term = term
                flag = f
                break
        
        if found_term:
            # We wrap the command in bash to keep the window open if it fails
            # Command becomes: terminal_flag bash -c "distrobox enter name || read -p 'Error. Press Enter...'"
            
            base_cmd = self.manager.enter_box_command(name) # ['distrobox', 'enter', 'name']
            flat_cmd = " ".join(base_cmd)
            
            # Construct the shell wrapper
            shell_cmd = f"{flat_cmd} || {{ echo 'Command failed.'; read -p 'Press Enter to close...' var; }}"
            
            final_cmd = [found_term]
            if flag:
                final_cmd.append(flag)
            
            # For most terminals, we now execute bash -c "..."
            final_cmd.extend(["bash", "-c", shell_cmd])
            
            print(f"Launching terminal command: {final_cmd}") # Debug output
            subprocess.Popen(final_cmd)
            
            # Refresh list after 3 seconds to reflect status change
            QTimer.singleShot(3000, self.refresh_list)
        else:
            QMessageBox.warning(self, "Error", tr("no_term_found"))

    def on_stop_box(self, name: str):
        reply = QMessageBox.question(self, "Confirm", tr("confirm_stop", name),
                                     QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
        if reply == QMessageBox.StandardButton.Yes:
            self.manager.stop_box(name)
            self.refresh_list() # Simply refresh

    def on_delete_box(self, name: str):
        reply = QMessageBox.question(self, "Confirm", tr("confirm_delete", name),
                                     QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
        if reply == QMessageBox.StandardButton.Yes:
            self.manager.delete_box(name)
            self.refresh_list()

    def create_box_dialog(self):
        dialog = CreateContainerDialog(self)
        if dialog.exec():
            data = dialog.get_data()
            
            # Show Indeterminate Progress Dialog
            self.progress = QProgressDialog(f"Creating container '{data['name']}'...\nThis may take a while to pull images.", None, 0, 0, self)
            self.progress.setWindowTitle("Processing")
            self.progress.setWindowModality(Qt.WindowModality.WindowModal)
            self.progress.setMinimumDuration(0) # Show immediately
            self.progress.show()
            
            # Setup Thread
            self.thread = QThread()
            self.worker = CreateBoxWorker(
                self.manager, 
                name=data['name'], 
                image=data['image'],
                home_path=data['home_path'],
                volume=data['volume'],
                root=data['root']
            )
            self.worker.moveToThread(self.thread)
            
            # Connect signals
            self.thread.started.connect(self.worker.run)
            self.worker.finished.connect(self.thread.quit)
            self.worker.finished.connect(self.worker.deleteLater)
            self.thread.finished.connect(self.thread.deleteLater)
            
            self.worker.finished.connect(self.on_create_finished)
            
            # Start
            self.thread.start()

    def on_create_finished(self, success, message):
        self.progress.close()
        if success:
            QMessageBox.information(self, "Success", message)
            self.refresh_list()
        else:
            QMessageBox.critical(self, "Error", message)