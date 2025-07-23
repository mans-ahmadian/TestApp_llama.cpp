cmake_minimum_required(VERSION 4.0)
if(WIN32)
	File(TO_CMAKE_PATH $ENV{VCPKG_ROOT} VCPKG_ROOT)
	message(STATUS ${VCPKG_ROOT})
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
	set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
else(WIN32)
	if(NOT DEFINED ENV{VCPKG_ROOT})
		message(WARNING "Please set VCPKG_ROOT to the root of the vcpkg installation.")
	endif()
	file(TO_CMAKE_PATH $ENV{VCPKG_ROOT} VCPKG_ROOT)
	message(STATUS ${VCPKG_ROOT})
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
	set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
endif(WIN32)
project(MyLlamaApp)

set(CMAKE_CXX_STANDARD 20)

find_package(llama CONFIG REQUIRED)

add_executable(MyLlamaApp main.cpp)
target_link_libraries(MyLlamaApp PRIVATE llama)

# Define the source and destination paths for the data folder
set(SOURCE_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models")
set(DEST_DATA_DIR "$<TARGET_FILE_DIR:MyLlamaApp>/models")

# Add a custom command to copy the data folder if it doesn't exist
add_custom_command(
    TARGET MyLlamaApp
    POST_BUILD  # Execute this command after building MyLlamaApp
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    "${SOURCE_DATA_DIR}"
    "${DEST_DATA_DIR}"
    COMMENT "Copying data folder to output directory if needed"
)