cmake_minimum_required(VERSION 3.21)
project(ImageFilterStudio VERSION 1.0 LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32 AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.ico")
    find_program(POWERSHELL_EXECUTABLE powershell REQUIRED)
    execute_process(
        COMMAND "${POWERSHELL_EXECUTABLE}" -NoProfile -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/generate_icon.ps1"
        RESULT_VARIABLE ICON_RESULT
    )
    if(NOT ICON_RESULT EQUAL 0)
        message(FATAL_ERROR "Could not generate the Windows application icon.")
    endif()
endif()

find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets Concurrent)

add_executable(ImageFilterStudio WIN32
    main.cpp
    mainwindow.h
    mainwindow.cpp
    previewwidget.h
    previewwidget.cpp
    filters.h
)
set_target_properties(ImageFilterStudio PROPERTIES OUTPUT_NAME PixelStudio)

if(WIN32)
    target_sources(ImageFilterStudio PRIVATE app_icon.rc)
    set_target_properties(ImageFilterStudio PROPERTIES WIN32_EXECUTABLE ON)
endif()

target_include_directories(ImageFilterStudio PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(ImageFilterStudio PRIVATE Qt6::Widgets Qt6::Concurrent)

if(MINGW)
    target_link_options(ImageFilterStudio PRIVATE -static-libgcc -static-libstdc++)
endif()