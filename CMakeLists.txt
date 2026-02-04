cmake_minimum_required(VERSION 3.10)
project(kylin-distrobox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Quick Qml Widgets REQUIRED)
find_package(PkgConfig REQUIRED)

# Check for OpenKylin SDK (try multiple possible names)
pkg_check_modules(KYSDKQTWIDGETS_PKG kysdk-qtwidgets libkysdk-qtwidgets kysdk-qtwidgets-dev)

add_executable(kylin-distrobox
    src/main.cpp
    src/manager.cpp
    src/manager.h
    qml.qrc
)

target_link_libraries(kylin-distrobox 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets 
    Qt5::Quick 
    Qt5::Qml
)

if(KYSDKQTWIDGETS_PKG_FOUND)
    message(STATUS "Found kysdk-qtwidgets via pkg-config")
    target_include_directories(kylin-distrobox PRIVATE ${KYSDKQTWIDGETS_PKG_INCLUDE_DIRS})
    target_link_directories(kylin-distrobox PRIVATE ${KYSDKQTWIDGETS_PKG_LIBRARY_DIRS})
    target_link_libraries(kylin-distrobox ${KYSDKQTWIDGETS_PKG_LIBRARIES})
    add_definitions(-DUSE_KYLIN_SDK)
endif()

# UKUI Quick Detection Logic
# 1. Try find_package with common UKUI paths
find_package(ukui-quick QUIET PATHS /usr/share/cmake/ukui-quick /usr/lib/cmake/ukui-quick)

if(NOT ukui-quick_FOUND)
    # 2. Try pkg-config for the components
    pkg_check_modules(UKUI_QUICK_CORE ukui-quick-core)
    pkg_check_modules(UKUI_QUICK_PLATFORM ukui-quick-platform)
    
    if(UKUI_QUICK_CORE_FOUND OR UKUI_QUICK_PLATFORM_FOUND)
        set(ukui-quick_FOUND TRUE)
        message(STATUS "Found UKUI Quick components via pkg-config")
        if(UKUI_QUICK_CORE_FOUND)
            target_include_directories(kylin-distrobox PRIVATE ${UKUI_QUICK_CORE_INCLUDE_DIRS})
            target_link_libraries(kylin-distrobox ${UKUI_QUICK_CORE_LIBRARIES})
        endif()
        if(UKUI_QUICK_PLATFORM_FOUND)
            target_include_directories(kylin-distrobox PRIVATE ${UKUI_QUICK_PLATFORM_INCLUDE_DIRS})
            target_link_libraries(kylin-distrobox ${UKUI_QUICK_PLATFORM_LIBRARIES})
        endif()
    endif()
endif()

if(ukui-quick_FOUND)
    message(STATUS "Found system ukui-quick, skipping extra/")
else()
    message(STATUS "System ukui-quick not found, building from extra/")
    set(BUILD_SHARED_LIBS ON)
    
    # Also make KF5WindowSystem optional in the sub-project if it's missing
    # but that needs to be handled in the subdirectory's CMakeLists.txt
    
    # Add the subdirectory
    add_subdirectory(extra/ukui-quick)
    
    # Define local import paths
    set(LOCAL_IMPORT_DIR "${CMAKE_BINARY_DIR}/imports")
    set(ITEMS_PLUGIN_DIR "${LOCAL_IMPORT_DIR}/org/ukui/quick/items")
    set(PLATFORM_PLUGIN_DIR "${LOCAL_IMPORT_DIR}/org/ukui/quick/platform")
    
    # Create a custom target to handle deployment
    add_custom_target(deploy_ukui_plugins ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ITEMS_PLUGIN_DIR}/qml
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/extra/ukui-quick/items/qmldir ${ITEMS_PLUGIN_DIR}/
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/extra/ukui-quick/items/qml ${ITEMS_PLUGIN_DIR}/qml
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ukui-quick-items-plugin> ${ITEMS_PLUGIN_DIR}/
        
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLATFORM_PLUGIN_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/extra/ukui-quick/platform/qmldir ${PLATFORM_PLUGIN_DIR}/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ukui-quick-platform-plugin> ${PLATFORM_PLUGIN_DIR}/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/extra/ukui-quick/platform/ukui/DtThemeDefault.qml ${PLATFORM_PLUGIN_DIR}/
        
        COMMENT "Deploying UKUI plugins to local imports"
        DEPENDS ukui-quick-items-plugin ukui-quick-platform-plugin
    )
endif()