cmake_minimum_required(VERSION 3.10)
project(FiveStagePipeline)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the main executable
add_executable(pipeline
    src/pipeline.cpp
    src/assembly_to_binary.cpp
)

# For Windows: Set output directory next to source
set_target_properties(pipeline PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)
