cmake_minimum_required(VERSION 3.12)

# Название проекта
project(MuseumGuards)

# Стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Установка пути к SFML и CGAL
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(CGAL REQUIRED COMPONENTS Core)

# Исходные файлы
add_executable(museum-guards
        main.cpp
        class-graph/polygon-with-holes.cpp
        class-app/app.cpp
        class-timer/timer.cpp
        class-input/input.cpp
        geom-utils/geom-utils.cpp
)

# Установка флагов компилятора
target_link_libraries(museum-guards PRIVATE sfml-graphics)
target_link_libraries(museum-guards PRIVATE CGAL::CGAL)

# Директории с заголовочными файлами
target_include_directories(museum-guards PRIVATE
        ${CMAKE_SOURCE_DIR}/class-graph
        ${CMAKE_SOURCE_DIR}/class-app
        ${CMAKE_SOURCE_DIR}/class-timer
        ${CMAKE_SOURCE_DIR}/class-input
        ${CMAKE_SOURCE_DIR}/class-set-cover
        ${CMAKE_SOURCE_DIR}/geom-utils
)

# Тестирование
enable_testing()
find_package(GTest REQUIRED)

add_executable(unit-tests
    ${CMAKE_SOURCE_DIR}/tests/main.cpp
    ${CMAKE_SOURCE_DIR}/tests/unit-tests.cpp
    ${CMAKE_SOURCE_DIR}/geom-utils/geom-utils.cpp
)

target_include_directories(unit-tests PRIVATE
    ${CMAKE_SOURCE_DIR}/geom-utils
)

target_link_libraries(unit-tests PRIVATE GTest::gtest GTest::gtest_main)

add_test(NAME AllTests COMMAND unit-tests)