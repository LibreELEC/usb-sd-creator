cmake_minimum_required(VERSION 3.16)
project(LibreELEC.USB-SD.Creator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Network Widgets LinguistTools)

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

qt_add_translations(LibreELEC.USB-SD.Creator
    RESOURCE_PREFIX
        "/lang"
    TS_FILES
        lang/lang-ast_ES.ts
        lang/lang-bg_BG.ts
        lang/lang-cs_CZ.ts
        lang/lang-da_DK.ts
        lang/lang-de_DE.ts
        lang/lang-el_GR.ts
        lang/lang-en_GB.ts
        lang/lang-es_ES.ts
        lang/lang-fa_IR.ts
        lang/lang-fi_FI.ts
        lang/lang-fr_FR.ts
        lang/lang-he_IL.ts
        lang/lang-hr_HR.ts
        lang/lang-hu_HU.ts
        lang/lang-it_IT.ts
        lang/lang-lt_LT.ts
        lang/lang-nl_NL.ts
        lang/lang-no_NO.ts
        lang/lang-pl_PL.ts
        lang/lang-pt_BR.ts
        lang/lang-pt_PT.ts
        lang/lang-ru_RU.ts
        lang/lang-sk_SK.ts
        lang/lang-sl_SI.ts
        lang/lang-sr_RS.ts
        lang/lang-sv_SE.ts
        lang/lang-tr_TR.ts
        lang/lang-uk_UA.ts
        lang/lang-zh_CN.ts
)

# Resources:
set(resources_resource_files
    "device_eject.png"
    "device_load.png"
    "device_remove.png"
    "header.png"
    "le_icon_256.png"
    "no1.png"
    "no2.png"
    "no3.png"
    "no4.png"
    "opencollective.png"
    "refresh.png"
    "winapp_le_icons.ico"
)

qt6_add_resources(LibreELEC.USB-SD.Creator "resources"
    PREFIX
        "/icons"
    FILES
        ${resources_resource_files}
)
set(resources1_resource_files
    "stylesheet.qss"
    "stylesheet_osx.qss"
)

qt6_add_resources(LibreELEC.USB-SD.Creator "resources1"
    PREFIX
        "/qss"
    FILES
        ${resources1_resource_files}
)
set(resources2_resource_files
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

if(MACOS)
    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        privileges_unix.cpp privileges_unix.h
        diskwriter_unix.cpp diskwriter_unix.h
        deviceenumerator_unix.cpp deviceenumerator_unix.h
    )
endif()

if(LINUX)
    set_target_properties(LibreELEC.USB-SD.Creator PROPERTIES SUFFIX .Linux.bin)

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DBus)

    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        diskwriter_udisks2.cpp diskwriter_udisks2.h
        deviceenumerator_udisks2.cpp deviceenumerator_udisks2.h
        privileges.h
    )

    target_link_libraries(LibreELEC.USB-SD.Creator PUBLIC
        Qt::DBus
    )
endif()

install(TARGETS LibreELEC.USB-SD.Creator)
