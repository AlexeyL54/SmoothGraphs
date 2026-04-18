cmake_minimum_required(VERSION 3.10)
project(SmoothGraphs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(SOURCES
    main.cpp
    src/front/MainWindow.cpp
    src/front/GraphView.cpp
    src/front/Figures.cpp
    src/front/MenuBar.cpp
    src/back/Graph.cpp
    src/back/Logger.cpp
)

set(HEADERS
    main.cpp
    src/front/MainWindow.cpp
    src/front/GraphView.cpp
    src/front/Figures.cpp
    src/front/MenuBar.cpp
    src/back/Graph.cpp
    src/back/Logger.cpp)

  add_executable(SmoothGraphs ${SOURCES} ${HEADERS})

  target_link_libraries(SmoothGraphs Qt6::Core Qt6::Widgets)
