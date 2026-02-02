from PyQt6.QtWidgets import (
    QDialog, QVBoxLayout, QHBoxLayout, QLabel, QLineEdit, QComboBox, 
    QDialogButtonBox, QMessageBox, QPushButton, QFileDialog, QCheckBox, QWidget
)
from PyQt6.QtCore import Qt
from core.i18n import tr
import core.config as cfg
import qtawesome as qta

class CreateContainerDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle(tr("create_dialog_title"))
        self.setMinimumWidth(450)
        
        layout = QVBoxLayout(self)
        layout.setSpacing(15)

        # 1. Name Input
        self.add_field(layout, tr("label_name"), self.create_name_input())

        # 2. Image Selection/Input
        self.add_field(layout, tr("label_image"), self.create_image_input())

        # --- Advanced Options Separator ---
        sep_label = QLabel("高级选项")
        sep_label.setStyleSheet("font-weight: bold; color: #61afef; margin-top: 10px;")
        layout.addWidget(sep_label)

        # 3. Home Directory (with Browse button)
        home_layout = QHBoxLayout()
        self.home_input = QLineEdit()
        self.home_input.setPlaceholderText("/path/to/custom/home")
        home_layout.addWidget(self.home_input)
        
        btn_browse = QPushButton(tr("btn_browse"))
        btn_browse.clicked.connect(self.browse_home)
        home_layout.addWidget(btn_browse)
        
        self.add_field(layout, tr("label_home"), None, custom_layout=home_layout)

        # 4. Volume Mount
        self.volume_input = QLineEdit()
        self.volume_input.setPlaceholderText(tr("placeholder_volume"))
        self.add_field(layout, tr("label_volume"), self.volume_input)

        # 5. Root Checkbox
        self.root_check = QCheckBox(tr("check_root"))
        layout.addWidget(self.root_check)

        # Buttons (Create / Cancel)
        self.button_box = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        self.button_box.accepted.connect(self.validate_and_accept)
        self.button_box.rejected.connect(self.reject)
        layout.addWidget(self.button_box)

    def add_field(self, parent_layout, label_text, widget, custom_layout=None):
        """Helper to add a labeled field."""
        field_layout = QVBoxLayout()
        field_layout.setSpacing(5)
        label = QLabel(label_text)
        label.setStyleSheet("font-weight: bold;")
        field_layout.addWidget(label)
        
        if widget:
            field_layout.addWidget(widget)
        elif custom_layout:
            field_layout.addLayout(custom_layout)
            
        parent_layout.addLayout(field_layout)

    def create_name_input(self):
        self.name_input = QLineEdit()
        self.name_input.setPlaceholderText(tr("placeholder_name"))
        return self.name_input

    def create_image_input(self):
        self.image_combo = QComboBox()
        self.image_combo.setEditable(True) 
        # Load from config
        self.image_combo.addItems(cfg.get_image_list())
        return self.image_combo

    def browse_home(self):
        directory = QFileDialog.getExistingDirectory(self, "Select Home Directory")
        if directory:
            self.home_input.setText(directory)

    def validate_and_accept(self):
        name = self.name_input.text().strip()
        image = self.image_combo.currentText().strip()

        if not name:
            QMessageBox.warning(self, "Validation Error", tr("warn_name"))
            return
        
        if not image:
            QMessageBox.warning(self, "Validation Error", tr("warn_image"))
            return
            
        # Save custom image to history
        cfg.add_custom_image(image)

        self.accept()

    def get_data(self):
        return {
            "name": self.name_input.text().strip(),
            "image": self.image_combo.currentText().strip(),
            "home_path": self.home_input.text().strip() or None,
            "volume": self.volume_input.text().strip() or None,
            "root": self.root_check.isChecked()
        }
