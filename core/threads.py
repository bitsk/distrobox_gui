from PyQt6.QtCore import QThread, pyqtSignal, QObject

class CreateBoxWorker(QObject):
    finished = pyqtSignal(bool, str) # success, message

    def __init__(self, manager, name, image, **kwargs):
        super().__init__()
        self.manager = manager
        self.name = name
        self.image = image
        self.kwargs = kwargs

    def run(self):
        try:
            # We assume create_box returns True/False
            # But let's wrap it to catch any unexpected exceptions
            success = self.manager.create_box(self.name, self.image, **self.kwargs)
            if success:
                self.finished.emit(True, f"Container '{self.name}' created successfully!")
            else:
                self.finished.emit(False, "Failed to create container.\nCheck logs or image name.")
        except Exception as e:
            self.finished.emit(False, f"Error: {str(e)}")

class InstallWorker(QObject):
    finished = pyqtSignal(bool)

    def __init__(self, manager):
        super().__init__()
        self.manager = manager

    def run(self):
        success = self.manager.install_distrobox()
        self.finished.emit(success)

