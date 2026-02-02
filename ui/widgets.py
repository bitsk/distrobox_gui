from PyQt6.QtWidgets import (
    QFrame, QVBoxLayout, QHBoxLayout, QLabel, QPushButton, QWidget, QSizePolicy, QGraphicsDropShadowEffect
)
from PyQt6.QtCore import Qt, pyqtSignal
from PyQt6.QtGui import QColor
import qtawesome as qta
from core.i18n import tr

class ContainerCard(QFrame):
    # Signals for actions
    enter_clicked = pyqtSignal(str) 
    stop_clicked = pyqtSignal(str)
    delete_clicked = pyqtSignal(str)

    def __init__(self, container_data: dict, icon_name: str, parent=None):
        super().__init__(parent)
        self.setObjectName("ContainerCard") # For QSS styling
        self.container_data = container_data
        self.name = container_data.get("name", "Unknown")
        self.status = container_data.get("status", "Unknown")
        self.image = container_data.get("image", "Unknown")
        
        # Determine status color/icon
        is_running = "up" in self.status.lower() or "running" in self.status.lower()
        status_color = "#98c379" if is_running else "#e06c75" # Green or Red
        status_text = tr("status_up") if is_running else tr("status_exited")
        status_icon = qta.icon('fa5s.play-circle', color=status_color) if is_running else qta.icon('fa5s.stop-circle', color=status_color)

        # Layout
        layout = QHBoxLayout(self)
        layout.setContentsMargins(15, 15, 15, 15)
        layout.setSpacing(15)

        # 1. Distro Icon (Large)
        # We try to map distro names to fontawesome brands or generic linux
        distro_icon_map = {
            "ubuntu": "fa5b.ubuntu",
            "fedora": "fa5b.fedora",
            "archlinux": "fa5b.linux", # Arch icon not always standard in FA free
            "debian": "fa5b.linux",    # Debian icon not always standard
            "suse": "fa5b.suse",
            "kali": "fa5s.skull",
            "alpine": "fa5s.mountain",
            "linux-generic": "fa5b.linux"
        }
        
        # Basic mapping logic based on the 'icon_name' passed from manager
        fa_icon_name = "fa5b.linux"
        for key in distro_icon_map:
            if key in icon_name:
                fa_icon_name = distro_icon_map[key]
                break
                
        self.icon_label = QLabel()
        self.icon_label.setPixmap(qta.icon(fa_icon_name, color="white").pixmap(48, 48))
        layout.addWidget(self.icon_label)

        # 2. Info Section
        info_layout = QVBoxLayout()
        info_layout.setSpacing(5)
        
        # Name
        self.name_label = QLabel(self.name)
        self.name_label.setStyleSheet("font-size: 18px; font-weight: bold; color: white;")
        info_layout.addWidget(self.name_label)
        
        # Image & Status Row
        details_layout = QHBoxLayout()
        details_layout.setSpacing(10)
        
        # Status Badge (Icon + Text)
        self.status_badge = QLabel(status_text)
        self.status_badge.setStyleSheet(f"color: {status_color}; font-weight: bold;")
        details_layout.addWidget(self.status_badge)
        
        # Separator
        sep = QLabel("|")
        sep.setStyleSheet("color: #5c6370;")
        details_layout.addWidget(sep)

        # Image Name
        self.image_label = QLabel(self.image)
        self.image_label.setStyleSheet("font-size: 13px; color: #abb2bf;")
        details_layout.addWidget(self.image_label)
        
        details_layout.addStretch()
        info_layout.addLayout(details_layout)
        
        layout.addLayout(info_layout)
        layout.addStretch()

        # 3. Actions Section (Right Side)
        actions_layout = QHBoxLayout()
        actions_layout.setSpacing(8)

        # Common button style properties
        btn_height = 38
        
        # Enter Button (Primary Action)
        self.btn_enter = QPushButton(tr("btn_enter"))
        self.btn_enter.setIcon(qta.icon('fa5s.terminal', color='white'))
        self.btn_enter.setObjectName("CreateBtn") # Reuse primary button style
        self.btn_enter.setFixedHeight(btn_height)
        self.btn_enter.setCursor(Qt.CursorShape.PointingHandCursor)
        self.btn_enter.clicked.connect(lambda: self.enter_clicked.emit(self.name))
        actions_layout.addWidget(self.btn_enter)

        # Menu/More Actions (Stop/Delete)
        # Using simple icon buttons for cleaner look
        
        self.btn_stop = QPushButton()
        self.btn_stop.setIcon(qta.icon('fa5s.stop', color='#e06c75')) # Red
        self.btn_stop.setToolTip(tr("btn_stop"))
        self.btn_stop.setFixedSize(btn_height, btn_height) # Square
        self.btn_stop.setStyleSheet("border-radius: 6px; background-color: rgba(255,255,255,0.05); border: 1px solid rgba(255,255,255,0.1);")
        self.btn_stop.setCursor(Qt.CursorShape.PointingHandCursor)
        self.btn_stop.clicked.connect(lambda: self.stop_clicked.emit(self.name))
        actions_layout.addWidget(self.btn_stop)

        self.btn_delete = QPushButton()
        self.btn_delete.setIcon(qta.icon('fa5s.trash', color='#e06c75'))
        self.btn_delete.setToolTip(tr("btn_delete"))
        self.btn_delete.setFixedSize(btn_height, btn_height) # Square
        self.btn_delete.setStyleSheet("border-radius: 6px; background-color: rgba(255,255,255,0.05); border: 1px solid rgba(255,255,255,0.1);")
        self.btn_delete.setCursor(Qt.CursorShape.PointingHandCursor)
        self.btn_delete.clicked.connect(lambda: self.delete_clicked.emit(self.name))
        actions_layout.addWidget(self.btn_delete)

        layout.addLayout(actions_layout)