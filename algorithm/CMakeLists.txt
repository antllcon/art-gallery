cmake_minimum_required(VERSION 3.10)

# Название проекта
project(MuseumGuards)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Установка пути к SFML
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

# Добавьте исходные файлы
add_executable(museum-guards
    main.cpp
    class-graph/graph.cpp
    class-app/app.cpp
    class-input/input.cpp
)

# Установка флагов компилятора
target_link_libraries(museum-guards PRIVATE sfml-graphics)

# Директории с заголовочными файлами
target_include_directories(museum-guards PRIVATE 
    ${CMAKE_SOURCE_DIR}/class-graph
    ${CMAKE_SOURCE_DIR}/class-app
    ${CMAKE_SOURCE_DIR}/class-input
)