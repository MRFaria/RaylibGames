# Define the helper-lib project
project(helper-lib CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files for the helper-lib
file(GLOB_RECURSE HELPER_LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

# Add the helper-lib as a static library
add_library(${PROJECT_NAME} STATIC ${HELPER_LIB_SOURCES})

# Define the include directory for the helper-lib
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/sources/")
# Add the helpers directory to the include path and expose it to dependent projects
target_include_directories(helper-lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
# Link the helper-lib to Raylib
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)