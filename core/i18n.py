# Simple localization map

TRANSLATIONS = {
    "zh": {
        "app_title": "Distrobox 环境管理器",
        "header_title": "我的开发环境",
        "refresh": "刷新列表",
        "create_new": "新建环境",
        "install_distrobox": "安装 Distrobox",
        "install_desc": "Distrobox 尚未安装.\n它可以让您在终端中运行任何 Linux 发行版。",
        "install_btn": "安装 Distrobox (~/.local)",
        "status_up": "运行中",
        "status_exited": "已停止",
        "btn_enter": "进入终端",
        "btn_stop": "停止",
        "btn_delete": "删除",
        "confirm_stop": "确定要停止容器 '{0}' 吗？",
        "confirm_delete": "确定要删除容器 '{0}' 吗？\n此操作不可撤销！",
        "create_dialog_title": "新建 Distrobox 容器",
        "label_name": "名称 (Name):",
        "label_image": "镜像 (Image):",
        "label_home": "自定义 Home (可选):",
        "label_volume": "挂载卷 (可选):",
        "check_root": "使用 Root 权限 (Rootful)",
        "btn_browse": "浏览...",
        "placeholder_name": "例如: my-ubuntu-dev",
        "placeholder_volume": "宿主路径:容器路径",
        "warn_name": "请输入容器名称。",
        "warn_image": "请指定一个镜像 (例如: ubuntu:latest)。",
        "success_create": "容器 '{0}' 创建成功！",
        "error_create": "创建容器失败。\n请检查镜像名称是否正确。",
        "success_install": "Distrobox 安装成功！\n请重启应用或确保 ~/.local/bin 在您的 PATH 中。",
        "error_install": "安装失败，请检查网络连接。",
        "no_term_found": "未检测到支持的终端模拟器。",
        "empty_list": "暂无容器。点击“新建环境”开始吧！",
        "error_distrobox_missing": "未检测到 Distrobox。",
    },
    "en": {
        "app_title": "Distrobox Manager",
        "header_title": "My Environments",
        "refresh": "Refresh List",
        "create_new": "Create New Box",
        "install_distrobox": "Install Distrobox",
        "install_desc": "Distrobox is not installed.\nIt allows you to run any Linux distribution inside your terminal.",
        "install_btn": "Install Distrobox (~/.local)",
        "status_up": "Running",
        "status_exited": "Stopped",
        "btn_enter": "Enter",
        "btn_stop": "Stop",
        "btn_delete": "Delete",
        "confirm_stop": "Are you sure you want to stop '{0}'?",
        "confirm_delete": "Are you sure you want to DELETE '{0}'?\nThis action cannot be undone.",
        "create_dialog_title": "Create New Distrobox",
        "label_name": "Name:",
        "label_image": "Image:",
        "placeholder_name": "e.g., my-ubuntu-dev",
        "warn_name": "Please provide a name for the container.",
        "warn_image": "Please specify an image (e.g., ubuntu:latest).",
        "success_create": "Container '{0}' created successfully!",
        "error_create": "Failed to create container.\nCheck if the image name is correct.",
        "success_install": "Distrobox installed successfully!\nPlease restart app.",
        "error_install": "Failed to install Distrobox.",
        "no_term_found": "No supported terminal found.",
        "empty_list": "No containers found. Create one to get started!",
        "error_distrobox_missing": "Distrobox not found.",
    }
}

# Change this to 'en' if you want English
CURRENT_LANG = "zh"

def tr(key, *args):
    """Translate a key to the current language."""
    text = TRANSLATIONS[CURRENT_LANG].get(key, key)
    if args:
        return text.format(*args)
    return text
