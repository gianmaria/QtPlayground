cmake_minimum_required(VERSION 3.5)

project(QtPlayground VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES QtPlayground_en_GB.ts)

set(PROJECT_SOURCES
        src/main.cpp
        src/main_window.cpp
        src/main_window.h
        src/main_window.ui
        ${TS_FILES}
)

# Set the icon for the executable
if(WIN32)
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.rc")
endif()

qt_add_executable(QtPlayground
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        res/resources.qrc
        ${app_icon_resource_windows}
)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(QtPlayground PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QtPlayground)
endif()


