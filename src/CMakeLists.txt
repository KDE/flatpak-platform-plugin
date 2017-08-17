include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
    ${Qt5ThemeSupport_INCLUDE_DIRS}
    ${Qt5PlatformSupport_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
    # needed for qdbustrayicon_p.h
    ${Qt5ThemeSupport_INCLUDE_DIRS}/QtThemeSupport
    ${Qt5PlatformSupport_INCLUDE_DIRS}/QtPlatformSupport
)

set(qflatpak_platform_theme_SRCS
    qflatpakfiledialog.cpp
    qflatpakplatformtheme.cpp
    qflatpakplatformthemeplugin.cpp
)

add_library(qflatpakplatformtheme MODULE ${qflatpak_platform_theme_SRCS})

target_link_libraries(qflatpakplatformtheme
    PRIVATE
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::Widgets
        ${QT5PLATFORMSUPPORT_LIBS}
)

install(TARGETS qflatpakplatformtheme DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/platformthemes)

set(qflatpak_platform_services_SRCS
    qflatpakplatformservices.cpp
    qflatpakplatformservicesplugin.cpp
)

add_library(qflatpakplatservices MODULE ${qflatpak_platform_services_SRCS})

target_link_libraries(qflatpakplatservices
    PRIVATE
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::Widgets
        ${QT5PLATFORMSUPPORT_LIBS}
)

install(TARGETS qflatpakplatservices DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/platformservices)
