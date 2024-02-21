cmake_minimum_required(VERSION 3.19)
project(LibreELEC.USB-SD.Creator
    VERSION 1.5
    LANGUAGES CXX
    HOMEPAGE_URL "https://libreelec.tv/"
)

set(projectDisplayName "LibreELEC USB-SD Creator")

# LINUX is available only since CMake 3.25
if (NOT LINUX AND UNIX AND NOT APPLE)
    set(LINUX 1)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

set(qtComponents Gui Network Widgets LinguistTools)
if(LINUX)
    list(APPEND qtComponents DBus)
endif()
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${qtComponents})

qt_add_executable(LibreELEC.USB-SD.Creator WIN32 MACOSX_BUNDLE
    creator.cpp creator.h creator.ui
    deviceenumerator.h
    diskwriter.cpp diskwriter.h
    downloadmanager.cpp downloadmanager.h
    jsonparser.cpp jsonparser.h
    main.cpp
    movingaverage.cpp movingaverage.h
    resources.qrc
    translator.cpp translator.h
)

string(TIMESTAMP currentYear "%Y")
set_target_properties(LibreELEC.USB-SD.Creator PROPERTIES
    # TODO: use Qt's template (<QTDIR>/lib/cmake/Qt6/macos/Info.plist.app.in) once we support dark mode properly
    MACOSX_BUNDLE_INFO_PLIST "dmg_osx/Info.plist.in"
    MACOSX_BUNDLE_BUNDLE_NAME "${projectDisplayName}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_COPYRIGHT "LibreELEC © ${currentYear}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "tv.libreelec.usb-sd-creator"
    MACOSX_BUNDLE_ICON_FILE "icon.icns"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

target_link_libraries(LibreELEC.USB-SD.Creator PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Widgets
    z
)

target_compile_definitions(LibreELEC.USB-SD.Creator PRIVATE
    $<$<CONFIG:Debug>:ALWAYS_DEBUG_OUTPUT>
)

qt_add_translations(LibreELEC.USB-SD.Creator
    RESOURCE_PREFIX
        "/lang"
    TS_FILES
        lang/lang-ar_SA.ts
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
        lang/lang-ko_KR.ts
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

if(WIN32)
    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        deviceenumerator_windows.cpp deviceenumerator_windows.h
        diskwriter_windows.cpp diskwriter_windows.h
        privileges.h
        winapp.rc
    )
elseif(APPLE)
    set_target_properties(LibreELEC.USB-SD.Creator PROPERTIES
        OUTPUT_NAME "${projectDisplayName}"
    )

    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        deviceenumerator_unix.cpp deviceenumerator_unix.h
        diskwriter_unix.cpp diskwriter_unix.h
        privileges_unix.cpp privileges_unix.h
    )

    set(bundleContentsDir "$<TARGET_BUNDLE_CONTENT_DIR:LibreELEC.USB-SD.Creator>")
    install(FILES "dmg_osx/icon.icns"
        DESTINATION "${bundleContentsDir}/Resources"
    )

    qt_generate_deploy_script(
        TARGET LibreELEC.USB-SD.Creator
        OUTPUT_SCRIPT qtDeployScript
        CONTENT "
            qt_deploy_runtime_dependencies(
                EXECUTABLE \"$<TARGET_BUNDLE_DIR:LibreELEC.USB-SD.Creator>\"
                GENERATE_QT_CONF
                NO_APP_STORE_COMPLIANCE
                VERBOSE
            )
            # macdeployqt also packages extra frameworks that aren't needed
            foreach(qtModule in Pdf Svg)
                file(REMOVE_RECURSE \"${bundleContentsDir}/Frameworks/Qt\\\${qtModule}.framework\")
            endforeach()
        ")
    install(SCRIPT ${qtDeployScript})
elseif(LINUX)
    set_target_properties(LibreELEC.USB-SD.Creator PROPERTIES SUFFIX .Linux.bin)

    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        deviceenumerator_udisks2.cpp deviceenumerator_udisks2.h
        diskwriter_udisks2.cpp diskwriter_udisks2.h
        privileges.h
    )

    target_link_libraries(LibreELEC.USB-SD.Creator PUBLIC
        Qt${QT_VERSION_MAJOR}::DBus
    )
endif()

install(TARGETS LibreELEC.USB-SD.Creator
    BUNDLE
        DESTINATION .
)

# cpack
set(CPACK_PACKAGE_NAME "${projectDisplayName}")
set(CPACK_PACKAGE_VENDOR "LibreELEC")
if(APPLE)
    set(CPACK_PACKAGE_FILE_NAME "LibreELEC.USB-SD.Creator.macOS")

    set(CPACK_GENERATOR DragNDrop)
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/dmg_osx/background.png")
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "DS_Store_Setup.scpt")
    set(CPACK_DMG_VOLUME_NAME "${projectDisplayName}")

    configure_file("dmg_osx/DS_Store_Setup.scpt.in" "${CPACK_DMG_DS_STORE_SETUP_SCRIPT}" @ONLY)
endif()
include(CPack)
