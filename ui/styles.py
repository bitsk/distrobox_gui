# Custom QSS Styles

MAIN_STYLE = """
/* Global Font Fix (Optional) */
* {
    font-family: "Segoe UI", "Roboto", "Helvetica Neue", sans-serif;
}

/* Card Style */
QFrame#ContainerCard {
    background-color: #2c313c;
    border-radius: 12px;
    border: 1px solid #3e4451;
}

QFrame#ContainerCard:hover {
    border: 1px solid #61afef;
    background-color: #323842;
}

/* Action Buttons inside Card */
QPushButton#ActionBtn {
    border-radius: 6px;
    padding: 6px 12px;
    font-weight: bold;
    border: none;
    text-align: left;
}
QPushButton#ActionBtn:hover {
    background-color: rgba(255, 255, 255, 0.1);
}

/* Primary Create Button */
QPushButton#CreateBtn {
    background-color: #61afef;
    color: white;
    border-radius: 8px;
    padding: 8px 16px;
    font-weight: bold;
    font-size: 14px;
}
QPushButton#CreateBtn:hover {
    background-color: #528bca;
}

/* Header Label */
QLabel#HeaderTitle {
    color: white;
    font-size: 26px;
    font-weight: 800;
}
"""
