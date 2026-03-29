cmake_minimum_required(VERSION 3.16)
project(calculator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(calculator
    main.cpp
    calculator.cpp
    calculator.h
    settings_dialog.cpp
    settings_dialog.h
    app_settings.h
)

target_link_libraries(calculator Qt6::Widgets)
