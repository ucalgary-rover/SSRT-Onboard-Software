cmake_minimum_required(VERSION 3.10)
project(mqtt-server)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compilation database generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create symlink in source directory for clang-tidy
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(
        copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "Creating symlink to compile_commands.json"
    )
endif()

# Find required packages
find_library(PAHO_MQTTPP3 paho-mqttpp3 REQUIRED)
find_library(PAHO_MQTT3A paho-mqtt3a REQUIRED)

# build dependencies from source where possible
add_subdirectory(dependencies/cpp-dotenv)

# Create the mqtt-server executable
add_executable(mqtt-server mqtt-server.cpp)

# Link libraries
target_link_libraries(mqtt-server
  PRIVATE
    paho-mqttpp3
    paho-mqtt3a
    cpp_dotenv
)

target_include_directories(mqtt-server
  PRIVATE
    /usr/local/include
)
