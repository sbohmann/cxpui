cmake_minimum_required(VERSION 3.30)
project(cxpui LANGUAGES C Swift)

set(CMAKE_C_STANDARD 23)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

add_executable(swift_cxpui main.c)
add_library(swift_cxpui_backend AppDelegate.swift)
set_target_properties(swift_cxpui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist"
)
target_link_libraries(swift_cxpui PRIVATE swift_cxpui_backend)
target_link_libraries(swift_cxpui_backend "-framework AppKit")
set(CMAKE_Swift_FLAGS "-import-objc-header ${CMAKE_SOURCE_DIR}/cxpui.h")

add_executable(gtk_cxpui main.c)
add_library(gtk_cxpui_backend gtk.c)
target_include_directories(gtk_cxpui_backend PRIVATE ${GTK4_INCLUDE_DIRS})
target_compile_options(gtk_cxpui_backend PRIVATE ${GTK4_CFLAGS_OTHER})
target_link_directories(gtk_cxpui PRIVATE /opt/homebrew/lib)
target_link_libraries(gtk_cxpui PRIVATE gtk_cxpui_backend)
target_link_directories(gtk_cxpui_backend PRIVATE /opt/homebrew/lib)
target_link_libraries(gtk_cxpui_backend ${GTK4_LIBRARIES})

add_executable(gtk4_cxpui main.c)
add_library(gtk4_cxpui_backend gtk4.c)
target_include_directories(gtk4_cxpui_backend PRIVATE ${GTK4_INCLUDE_DIRS})
target_compile_options(gtk4_cxpui_backend PRIVATE ${GTK4_CFLAGS_OTHER})
target_link_directories(gtk4_cxpui PRIVATE /opt/homebrew/lib)
target_link_libraries(gtk4_cxpui PRIVATE gtk4_cxpui_backend)
target_link_directories(gtk4_cxpui_backend PRIVATE /opt/homebrew/lib)
target_link_libraries(gtk4_cxpui_backend ${GTK4_LIBRARIES})

message(STATUS "GTK4_INCLUDE_DIRS: ${GTK4_INCLUDE_DIRS}")
message(STATUS "GTK4_LIBRARIES: ${GTK4_LIBRARIES}")
