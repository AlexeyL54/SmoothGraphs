cmake_minimum_required(VERSION 3.10)

project(SmoothGraphsTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

set(TEST_SOURCES
    e2e.cpp
)

add_executable(e2e_tests ${TEST_SOURCES})

target_link_libraries(e2e_tests PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# Добавляем исходники основного проекта для линковки
set(PROJECT_SOURCES
    ../src/front/MainWindow.cpp
    ../src/front/GraphView.cpp
    ../src/front/Figures.cpp
    ../src/front/MenuBar.cpp
    ../src/front/ThemeManager.cpp
    ../src/front/StyleManager.cpp
    ../src/back/Graph.cpp
    ../src/back/Graphviz.cpp
    ../src/back/Logger.cpp
)

target_sources(e2e_tests PRIVATE ${PROJECT_SOURCES})

enable_testing()
add_test(NAME E2ETests COMMAND e2e_tests)
