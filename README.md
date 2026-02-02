# Distrobox GUI Manager (Distrobox 环境管理器)

这是一个基于 Python 和 PyQt6 开发的现代图形界面工具，旨在简化 [Distrobox](https://github.com/89luca89/distrobox) 容器的管理。它提供了一个直观、美观的界面，让用户无需记忆复杂的命令行参数即可轻松创建、管理和使用容器化开发环境。

![Python](https://img.shields.io/badge/Python-3.8+-blue.svg)
![PyQt6](https://img.shields.io/badge/PyQt6-6.x-green.svg)
![License](https://img.shields.io/badge/License-MIT-orange.svg)

## ✨ 功能特性

*   **🖥️ 现代化界面**: 采用卡片式布局，配合 FontAwesome 矢量图标和现代深色主题（基于 Material Design）。
*   **📦 容器管理**:
    *   **一键查看**: 实时显示所有容器的状态（运行中/已停止）。
    *   **快速进入**: 自动识别系统终端（支持 GNOME Terminal, Konsole, Alacritty, Kitty 等）并一键进入容器 shell。
    *   **生命周期控制**: 支持停止和删除容器。
*   **🚀 智能创建**:
    *   支持从预设的流行发行版（Ubuntu, Fedora, Arch, Debian 等）快速选择。
    *   **高级选项**: 支持自定义 Home 目录映射、挂载卷以及 Root 权限模式。
    *   **镜像历史**: 自动保存手动输入的自定义镜像，方便下次快速使用。
*   **🛠️ 内置安装**: 如果系统中未检测到 Distrobox，提供一键式本地安装（无需 root 权限）。
*   **🧵 异步操作**: 耗时任务（如创建容器、安装脚本、拉取镜像）均在后台线程运行，确保 GUI 界面永不卡死。
*   **🌐 多语言支持**: 完整支持中文和英文界面。

## 📸 运行预览

*(此处可以添加你的运行截图)*

## 🚀 快速开始

### 1. 环境要求

*   **操作系统**: Linux (推荐支持 Podman 或 Docker)
*   **Python**: 3.10 或更高版本
*   **依赖**: `distrobox` (程序内提供一键安装)

### 2. 安装步骤

克隆项目到本地：

```bash
git clone <your-repo-url>
cd distrobox_gui
```

安装必要的 Python 依赖：

```bash
pip install -r requirements.txt
```

### 3. 运行程序

```bash
python main.py
```

## 🛠️ 技术架构

*   **GUI 框架**: [PyQt6](https://www.riverbankcomputing.com/software/pyqt/)
*   **样式库**: [qt-material](https://github.com/UN-GCPDS/qt-material)
*   **图标库**: [qtawesome](https://github.com/spyder-ide/qtawesome)
*   **核心逻辑**: 
    *   `core/manager.py`: 对 Distrobox CLI 的深度封装。
    *   `core/threads.py`: 基于 `QThread` 的多线程异步执行引擎。
    *   `core/i18n.py`: 动态国际化支持。
    *   `core/config.py`: 基于 JSON 的持久化配置管理。

## 📂 项目结构

```text
distrobox_gui/
├── core/               # 后端逻辑
│   ├── config.py       # 配置与历史记录
│   ├── i18n.py         # 多语言翻译
│   ├── manager.py      # Distrobox 命令包装器
│   └── threads.py      # 异步后台任务
├── ui/                 # 界面组件
│   ├── dialogs.py      # 创建容器对话框
│   ├── main_window.py  # 主窗口逻辑
│   ├── styles.py       # 自定义 QSS 样式
│   └── widgets.py      # 自定义 UI 小部件 (卡片)
├── main.py             # 程序入口
├── requirements.txt    # 依赖列表
└── README.md           # 项目文档
```

## 🤝 贡献建议

欢迎通过 Issue 或 Pull Request 提交改进建议，例如：
*   支持导出/导入容器。
*   支持更多的容器运行参数。
*   更好的发行版图标匹配逻辑。

---
*Powered by Gemini CLI & PyQt6*
