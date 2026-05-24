cmake_minimum_required(VERSION 3.10)
project(SmoothGraphs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(SOURCES
    main.cpp
    src/front/MainWindow.cpp
    src/front/GraphView.cpp
    src/front/Figures.cpp
    src/front/MenuBar.cpp
    src/front/ThemeManager.cpp
    src/back/Graph.cpp
    src/back/Graphviz.cpp
    src/back/Logger.cpp
)

set(HEADERS
    src/front/MainWindow.hpp
    src/front/GraphView.hpp
    src/front/Figures.hpp
    src/front/MenuBar.hpp
    src/front/ThemeManager.hpp
    src/back/Graph.hpp
    src/back/Graphviz.hpp
    src/back/Logger.hpp
)

if(WIN32)
    set(APP_ICON_RESOURCE resources/app_icon.rc)
endif()

add_executable(SmoothGraphs ${SOURCES} ${HEADERS} ${APP_ICON_RESOURCE})

target_link_libraries(SmoothGraphs Qt6::Core Qt6::Widgets)
