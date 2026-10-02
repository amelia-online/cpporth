# ---------------------------------------------------------------------------
# 1. CMake Version & Project Metadata
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20...3.28)

project(cpporth 
    VERSION 2.0.0 
    LANGUAGES CXX
)

find_package(Boost REQUIRED COMPONENTS regex)

# Enforce out-of-source builds to keep your repository clean
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please build inside a 'build/' directory.")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# 2. Source Files Discovery
# ---------------------------------------------------------------------------
# Automatically find all source files in the src directory
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Define your executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# ---------------------------------------------------------------------------
# 3. Include Directories (Crucial for your layout)
# ---------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::regex)

# ---------------------------------------------------------------------------
# 4. Compiler Warnings (Optional but Recommended)
# ---------------------------------------------------------------------------
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
