cmake_minimum_required(VERSION 3.5)

project(tictactoe VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

qt_add_executable(tictactoe
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc
)

qt_add_resources(tictactoe resources.qrc)

target_link_libraries(tictactoe PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)
