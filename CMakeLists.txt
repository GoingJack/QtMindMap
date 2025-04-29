cmake_minimum_required(VERSION 3.5)

project(QtMindMap VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport LinguistTools)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/infinitecanvas.h
        src/infinitecanvas.cpp
        src/pch.h
)

# Setup translation support
set(TS_FILES
    resources/translations/qtmindmap_zh_CN.ts
    resources/translations/qtmindmap_en_US.ts
    # Add more languages as needed
)

# Enable precompiled headers
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QtMindMap
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    # Add precompiled header
    target_precompile_headers(QtMindMap PRIVATE src/pch.h)
else()
    add_executable(QtMindMap
        ${PROJECT_SOURCES}
    )
    # Add precompiled header
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
        target_precompile_headers(QtMindMap PRIVATE src/pch.h)
    endif()
endif()

target_link_libraries(QtMindMap PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QtMindMap)
endif()
set_target_properties(QtMindMap PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QtMindMap
    BUNDLE DESTINATION output/bin
    LIBRARY DESTINATION output/lib
    RUNTIME DESTINATION output/bin
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QtMindMap)
endif()

# Create translated QM files
qt_add_translation(QM_FILES ${TS_FILES})

# Create translations resource file
configure_file(resources/translations/translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Add the resource file to the project sources
target_sources(QtMindMap PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)
