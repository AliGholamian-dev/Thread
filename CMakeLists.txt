cmake_minimum_required(VERSION 3.23)
set(PROJECT_NAME ThreadTest)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(source_dir  ${PROJECT_SOURCE_DIR}/Src)
set(example_dir ${PROJECT_SOURCE_DIR}/Example)

find_package(Threads REQUIRED)

file(GLOB_RECURSE source_files ${source_dir}/*.cpp)
file(GLOB_RECURSE include_files ${source_dir}/*.h)
file(GLOB_RECURSE example_source_files ${example_dir}/*.cpp)
file(GLOB_RECURSE example_include_files ${example_dir}/*.h)

file(GLOB_RECURSE all_source_files_folders LIST_DIRECTORIES true ${source_dir}/*)
SET(include_dirs "")
FOREACH(file_or_folder ${all_source_files_folders})
    IF(IS_DIRECTORY ${file_or_folder})
        LIST(APPEND include_dirs ${file_or_folder})
    ENDIF()
ENDFOREACH()

file(GLOB_RECURSE all_example_files_folders LIST_DIRECTORIES true ${example_dir}/*)
FOREACH(file_or_folder ${all_example_files_folders})
    IF(IS_DIRECTORY ${file_or_folder})
        LIST(APPEND include_dirs ${file_or_folder})
    ENDIF()
ENDFOREACH()

include_directories(${PROJECT_NAME} ${source_dir} ${example_dir} ${include_dirs})

add_executable(${PROJECT_NAME}
        ${source_files}
        ${include_files}
        ${example_source_files}
        ${example_include_files})

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
