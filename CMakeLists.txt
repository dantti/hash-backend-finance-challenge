cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

project(hash_backend_finance_challenge
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Hash Backend Finance Challenge"
    HOMEPAGE_URL "https://github.com/dantti/hash-backend-finance-challenge"
)

if (POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

if(WIN32)
  if(MSVC)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  endif()
endif()

find_package(Qt5 COMPONENTS Core Network REQUIRED)
find_package(Cutelyst2Qt5 REQUIRED)
find_package(ASqlQt5 REQUIRED)

# Auto generate moc files
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE TEMPLATES_SRC root/*)

add_subdirectory(src)
add_subdirectory(tests)
