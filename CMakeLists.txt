project(flatpak-platform-plugin)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.7.0")

################# set KDE specific information #################

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMPackageConfigHelpers)
include(ECMOptionalAddSubdirectory)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
)

if(Qt5Core_VERSION VERSION_LESS "5.8.0")
    find_package(Qt5PlatformSupport REQUIRED)
else()
    find_package(Qt5ThemeSupport REQUIRED)
endif()


if(Qt5Core_VERSION VERSION_LESS "5.8.0")
    add_definitions(-DQ_FONTCONFIGDATABASE)
    set(QT5PLATFORMSUPPORT_LIBS Qt5PlatformSupport::Qt5PlatformSupport)
else()
    set(QT5PLATFORMSUPPORT_LIBS
        Qt5ThemeSupport::Qt5ThemeSupport)
endif()

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY)

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
