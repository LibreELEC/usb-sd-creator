# Generated from creator.pro.

cmake_minimum_required(VERSION 3.16)
project(LibreELEC.USB-SD.Creator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Network Widgets)

qt_add_executable(LibreELEC.USB-SD.Creator WIN32 MACOSX_BUNDLE
    creator.cpp creator.h creator.ui
    deviceenumerator.h
    diskwriter.cpp diskwriter.h
    downloadmanager.cpp downloadmanager.h
    jsonparser.cpp jsonparser.h
    main.cpp
    movingaverage.cpp movingaverage.h
    translator.cpp translator.h
)
target_link_libraries(LibreELEC.USB-SD.Creator PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::Widgets
    z
)


# Resources:
set(resources_resource_files
    "device_eject.png"
    "device_load.png"
    "device_remove.png"
    "header.png"
    "lang/flag-ast_ES.png"
    "lang/flag-bg_BG.png"
    "lang/flag-cs_CZ.png"
    "lang/flag-da_DK.png"
    "lang/flag-de_DE.png"
    "lang/flag-el_GR.png"
    "lang/flag-empty.png"
    "lang/flag-en_GB.png"
    "lang/flag-es_ES.png"
    "lang/flag-fa_IR.png"
    "lang/flag-fi_FI.png"
    "lang/flag-fr_FR.png"
    "lang/flag-he_IL.png"
    "lang/flag-hr_HR.png"
    "lang/flag-hu_HU.png"
    "lang/flag-it_IT.png"
    "lang/flag-lt_LT.png"
    "lang/flag-nl_NL.png"
    "lang/flag-no_NO.png"
    "lang/flag-pl_PL.png"
    "lang/flag-pt_BR.png"
    "lang/flag-pt_PT.png"
    "lang/flag-ru_RU.png"
    "lang/flag-sk_SK.png"
    "lang/flag-sl_SI.png"
    "lang/flag-sr_RS.png"
    "lang/flag-sv_SE.png"
    "lang/flag-tr_TR.png"
    "lang/flag-uk_UA.png"
    "lang/flag-zh_CN.png"
    "lang/lang-ast_ES.qm"
    "lang/lang-bg_BG.qm"
    "lang/lang-cs_CZ.qm"
    "lang/lang-da_DK.qm"
    "lang/lang-de_DE.qm"
    "lang/lang-el_GR.qm"
    "lang/lang-en_GB.qm"
    "lang/lang-es_ES.qm"
    "lang/lang-fa_IR.qm"
    "lang/lang-fi_FI.qm"
    "lang/lang-fr_FR.qm"
    "lang/lang-he_IL.qm"
    "lang/lang-hr_HR.qm"
    "lang/lang-hu_HU.qm"
    "lang/lang-it_IT.qm"
    "lang/lang-lt_LT.qm"
    "lang/lang-nl_NL.qm"
    "lang/lang-no_NO.qm"
    "lang/lang-pl_PL.qm"
    "lang/lang-pt_BR.qm"
    "lang/lang-pt_PT.qm"
    "lang/lang-ru_RU.qm"
    "lang/lang-sk_SK.qm"
    "lang/lang-sl_SI.qm"
    "lang/lang-sr_RS.qm"
    "lang/lang-sv_SE.qm"
    "lang/lang-tr_TR.qm"
    "lang/lang-uk_UA.qm"
    "lang/lang-zh_CN.qm"
    "le_icon_256.png"
    "no1.png"
    "no2.png"
    "no3.png"
    "no4.png"
    "opencollective.png"
    "refresh.png"
    "stylesheet.qss"
    "stylesheet_osx.qss"
    "winapp_le_icons.ico"
)

qt6_add_resources(LibreELEC.USB-SD.Creator "resources"
    PREFIX
        "/icons"
    FILES
        ${resources_resource_files}
)
set(resources1_resource_files
    "device_eject.png"
    "device_load.png"
    "device_remove.png"
    "header.png"
    "lang/flag-ast_ES.png"
    "lang/flag-bg_BG.png"
    "lang/flag-cs_CZ.png"
    "lang/flag-da_DK.png"
    "lang/flag-de_DE.png"
    "lang/flag-el_GR.png"
    "lang/flag-empty.png"
    "lang/flag-en_GB.png"
    "lang/flag-es_ES.png"
    "lang/flag-fa_IR.png"
    "lang/flag-fi_FI.png"
    "lang/flag-fr_FR.png"
    "lang/flag-he_IL.png"
    "lang/flag-hr_HR.png"
    "lang/flag-hu_HU.png"
    "lang/flag-it_IT.png"
    "lang/flag-lt_LT.png"
    "lang/flag-nl_NL.png"
    "lang/flag-no_NO.png"
    "lang/flag-pl_PL.png"
    "lang/flag-pt_BR.png"
    "lang/flag-pt_PT.png"
    "lang/flag-ru_RU.png"
    "lang/flag-sk_SK.png"
    "lang/flag-sl_SI.png"
    "lang/flag-sr_RS.png"
    "lang/flag-sv_SE.png"
    "lang/flag-tr_TR.png"
    "lang/flag-uk_UA.png"
    "lang/flag-zh_CN.png"
    "lang/lang-ast_ES.qm"
    "lang/lang-bg_BG.qm"
    "lang/lang-cs_CZ.qm"
    "lang/lang-da_DK.qm"
    "lang/lang-de_DE.qm"
    "lang/lang-el_GR.qm"
    "lang/lang-en_GB.qm"
    "lang/lang-es_ES.qm"
    "lang/lang-fa_IR.qm"
    "lang/lang-fi_FI.qm"
    "lang/lang-fr_FR.qm"
    "lang/lang-he_IL.qm"
    "lang/lang-hr_HR.qm"
    "lang/lang-hu_HU.qm"
    "lang/lang-it_IT.qm"
    "lang/lang-lt_LT.qm"
    "lang/lang-nl_NL.qm"
    "lang/lang-no_NO.qm"
    "lang/lang-pl_PL.qm"
    "lang/lang-pt_BR.qm"
    "lang/lang-pt_PT.qm"
    "lang/lang-ru_RU.qm"
    "lang/lang-sk_SK.qm"
    "lang/lang-sl_SI.qm"
    "lang/lang-sr_RS.qm"
    "lang/lang-sv_SE.qm"
    "lang/lang-tr_TR.qm"
    "lang/lang-uk_UA.qm"
    "lang/lang-zh_CN.qm"
    "le_icon_256.png"
    "no1.png"
    "no2.png"
    "no3.png"
    "no4.png"
    "opencollective.png"
    "refresh.png"
    "stylesheet.qss"
    "stylesheet_osx.qss"
    "winapp_le_icons.ico"
)

qt6_add_resources(LibreELEC.USB-SD.Creator "resources1"
    PREFIX
        "/qss"
    FILES
        ${resources1_resource_files}
)
set(resources2_resource_files
    "device_eject.png"
    "device_load.png"
    "device_remove.png"
    "header.png"
    "lang/flag-ast_ES.png"
    "lang/flag-bg_BG.png"
    "lang/flag-cs_CZ.png"
    "lang/flag-da_DK.png"
    "lang/flag-de_DE.png"
    "lang/flag-el_GR.png"
    "lang/flag-empty.png"
    "lang/flag-en_GB.png"
    "lang/flag-es_ES.png"
    "lang/flag-fa_IR.png"
    "lang/flag-fi_FI.png"
    "lang/flag-fr_FR.png"
    "lang/flag-he_IL.png"
    "lang/flag-hr_HR.png"
    "lang/flag-hu_HU.png"
    "lang/flag-it_IT.png"
    "lang/flag-lt_LT.png"
    "lang/flag-nl_NL.png"
    "lang/flag-no_NO.png"
    "lang/flag-pl_PL.png"
    "lang/flag-pt_BR.png"
    "lang/flag-pt_PT.png"
    "lang/flag-ru_RU.png"
    "lang/flag-sk_SK.png"
    "lang/flag-sl_SI.png"
    "lang/flag-sr_RS.png"
    "lang/flag-sv_SE.png"
    "lang/flag-tr_TR.png"
    "lang/flag-uk_UA.png"
    "lang/flag-zh_CN.png"
    "lang/lang-ast_ES.qm"
    "lang/lang-bg_BG.qm"
    "lang/lang-cs_CZ.qm"
    "lang/lang-da_DK.qm"
    "lang/lang-de_DE.qm"
    "lang/lang-el_GR.qm"
    "lang/lang-en_GB.qm"
    "lang/lang-es_ES.qm"
    "lang/lang-fa_IR.qm"
    "lang/lang-fi_FI.qm"
    "lang/lang-fr_FR.qm"
    "lang/lang-he_IL.qm"
    "lang/lang-hr_HR.qm"
    "lang/lang-hu_HU.qm"
    "lang/lang-it_IT.qm"
    "lang/lang-lt_LT.qm"
    "lang/lang-nl_NL.qm"
    "lang/lang-no_NO.qm"
    "lang/lang-pl_PL.qm"
    "lang/lang-pt_BR.qm"
    "lang/lang-pt_PT.qm"
    "lang/lang-ru_RU.qm"
    "lang/lang-sk_SK.qm"
    "lang/lang-sl_SI.qm"
    "lang/lang-sr_RS.qm"
    "lang/lang-sv_SE.qm"
    "lang/lang-tr_TR.qm"
    "lang/lang-uk_UA.qm"
    "lang/lang-zh_CN.qm"
    "le_icon_256.png"
    "no1.png"
    "no2.png"
    "no3.png"
    "no4.png"
    "opencollective.png"
    "refresh.png"
    "stylesheet.qss"
    "stylesheet_osx.qss"
    "winapp_le_icons.ico"
)

qt6_add_resources(LibreELEC.USB-SD.Creator "resources2"
    PREFIX
        "/"
    FILES
        ${resources2_resource_files}
)

if(static)
    target_compile_definitions(LibreELEC.USB-SD.Creator PUBLIC
        STATIC
    )
endif()

if(WIN32)
    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        deviceenumerator_windows.cpp deviceenumerator_windows.h
        diskwriter_windows.cpp diskwriter_windows.h
        privileges.h
    )

    target_compile_options(LibreELEC.USB-SD.Creator
        -Ic:\Qt\Qt5.6.1\Tools\mingw492_32\i686-w64-mingw32\include
    )
endif()

if(UNIX)
    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        deviceenumerator_unix.cpp deviceenumerator_unix.h
        diskwriter_unix.cpp diskwriter_unix.h
    )

    target_compile_options(LibreELEC.USB-SD.Creator
        -fPIC
    )
endif()

if(MACOS)
    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        privileges_unix.cpp privileges_unix.h
    )
endif()

if(linux_x_)
    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        privileges_unix.cpp privileges_unix.h
    )

    target_link_libraries(LibreELEC.USB-SD.Creator PUBLIC
        blkid
    )

    target_compile_options(LibreELEC.USB-SD.Creator
        -std=c++11
    )
endif()

install(TARGETS LibreELEC.USB-SD.Creator
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
