include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

# set(qflatpak_platform_plugin_SRCS
#     qflatpakglobal.cpp
#     qflatpakfiledialog.cpp
#     qflatpakintegration.cpp
#     qflatpaktheme.cpp
# )
#
# add_library(libqflatpak MODULE ${qflatpak_platform_plugin_SRCS})
#
# target_link_libraries(libqflatpak
#     PRIVATE
#         Qt5::Core
#         Qt5::DBus
#         Qt5::Gui
# )
#
# install(TARGETS libqflatpak DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins/platforms)

set(qflatpak_platform_theme_SRCS
    qflatpakhintssettings.cpp
    qflatpakfiledialog.cpp
    qflatpakplatformtheme.cpp
    platformplugin.cpp
)

add_library(libqflatpakplatform MODULE ${qflatpak_platform_theme_SRCS})

target_link_libraries(libqflatpakplatform
    PRIVATE
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        KF5::ConfigWidgets
        KF5::ConfigCore
        KF5::I18n
)

install(TARGETS libqflatpakplatform DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/platformthemes)
