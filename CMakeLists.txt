cmake_minimum_required(VERSION 3.30)
project(cxpui LANGUAGES C Swift)

set(CMAKE_C_STANDARD 23)

add_executable(cxpui main.c)

add_library(swift_cxpui AppDelegate.swift)

set_target_properties(cxpui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist"
)

target_link_libraries(swift_cxpui "-framework AppKit")

target_link_libraries(cxpui PRIVATE swift_cxpui)

set(CMAKE_Swift_FLAGS "-import-objc-header ${CMAKE_SOURCE_DIR}/cxpui.h")

