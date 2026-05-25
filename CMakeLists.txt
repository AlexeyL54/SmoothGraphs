cmake_minimum_required(VERSION 3.16)
project(SmoothGraphs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Настройки для MSVC
if(MSVC)
    add_compile_options(/EHsc /MP)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Выводим информацию о Qt для отладки
message(STATUS "Qt6 found at: ${Qt6_DIR}")
message(STATUS "Qt6 version: ${Qt6_VERSION}")

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

# Создаем исполняемый файл
if(WIN32)
    set(APP_ICON_RESOURCE resources/app_icon.rc)
    add_executable(SmoothGraphs WIN32 ${SOURCES} ${HEADERS} ${APP_ICON_RESOURCE})
else()
    add_executable(SmoothGraphs ${SOURCES} ${HEADERS})
endif()

target_link_libraries(SmoothGraphs Qt6::Core Qt6::Widgets)

# Установка
install(TARGETS SmoothGraphs
    RUNTIME DESTINATION bin
)

# Копируем иконку если есть
install(FILES resources/graph.ico
    DESTINATION bin
    OPTIONAL
)
