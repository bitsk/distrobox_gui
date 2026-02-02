import sys
from PyQt6.QtWidgets import QApplication
from qt_material import apply_stylesheet

from ui.main_window import MainWindow

def main():
    app = QApplication(sys.argv)
    
    # Setup styling - using a dark theme with blue secondary color
    # You can try other themes like 'dark_teal.xml', 'light_blue.xml'
    apply_stylesheet(app, theme='dark_blue.xml')

    window = MainWindow()
    window.show()
    
    sys.exit(app.exec())

if __name__ == "__main__":
    main()