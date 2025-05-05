cmake_minimum_required(VERSION 3.16)
project(ComputerClub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основной исполняемый файл
add_executable(task main.cpp)
target_compile_options(task PRIVATE -Wall -Wextra -pedantic)

# Подключение GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Добавляем каталог с тестами
add_subdirectory(tests)

