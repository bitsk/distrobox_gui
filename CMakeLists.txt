cmake_minimum_required(VERSION 3.10)
project(kylin-distrobox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(PkgConfig REQUIRED)

# Check for OpenKylin SDK
pkg_check_modules(KYSDKQTWIDGETS_PKG kysdk-qtwidgets)

add_executable(kylin-distrobox
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/manager.cpp
    src/manager.h
    src/containercard.cpp
    src/containercard.h
    src/createdialog.cpp
    src/createdialog.h
    src/workers.cpp
    src/workers.h
)

if(KYSDKQTWIDGETS_PKG_FOUND)
    target_include_directories(kylin-distrobox PRIVATE ${KYSDKQTWIDGETS_PKG_INCLUDE_DIRS})
    target_link_directories(kylin-distrobox PRIVATE ${KYSDKQTWIDGETS_PKG_LIBRARY_DIRS})
    target_link_libraries(kylin-distrobox Qt5::Widgets ${KYSDKQTWIDGETS_PKG_LIBRARIES})
    add_definitions(-DUSE_KYLIN_SDK)
else()
    message(WARNING "OpenKylin SDK not found. Falling back to standard Qt5 widgets.")
    target_link_libraries(kylin-distrobox Qt5::Widgets)
endif()
