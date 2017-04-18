include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
    ${Qt5ThemeSupport_INCLUDE_DIRS}
    ${Qt5PlatformSupport_INCLUDE_DIRS}
)

set(qflatpak_platform_plugin_SRCS
    qflatpakintegration.cpp
    qflatpakfiledialog.cpp
    qflatpakmain.cpp
    qflatpakplatformtheme.cpp
    qflatpakservices.cpp
)

add_library(libqflatpakplatform MODULE ${qflatpak_platform_plugin_SRCS})

target_link_libraries(libqflatpakplatform
    PRIVATE
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        ${QT5PLATFORMSUPPORT_LIBS}
)

install(TARGETS libqflatpakplatform DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/platforms)
