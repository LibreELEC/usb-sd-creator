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

include(GNUInstallDirs)
# GNU dirs must be adjusted before finding Qt for qt_deploy_runtime_dependencies to use customzied vars
if(WIN32)
    set(CMAKE_INSTALL_BINDIR .)
endif()

# options
if(WIN32)
    option(DUMMY_WRITE "Write image to local file named 'dummy_image_file' instead to real device (for testing)")
    option(REQUIRE_ADMIN "Build exe that runs only as administrator" ON)
endif()

# dependencies
set(qtComponents Gui Network Widgets LinguistTools)
if(LINUX)
    list(APPEND qtComponents DBus)
endif()
find_package(QT NAMES Qt6 REQUIRED COMPONENTS ${qtComponents})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${qtComponents})

if(MSVC_TOOLSET_VERSION EQUAL 142)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64")
        list(APPEND CMAKE_PREFIX_PATH "windows/msvc2019-arm64/zlib")
    else()
        list(APPEND CMAKE_PREFIX_PATH "windows/msvc2019-x64/zlib")
    endif()
elseif(MSVC_TOOLSET_VERSION EQUAL 143)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64")
        list(APPEND CMAKE_PREFIX_PATH "windows/msvc2022-arm64/zlib")
    else()
        list(APPEND CMAKE_PREFIX_PATH "windows/msvc2022-x64/zlib")
    endif()
endif()
find_package(ZLIB REQUIRED)

# executable
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
set(copyrightYears "2016-${currentYear}")
set_target_properties(LibreELEC.USB-SD.Creator PROPERTIES
    # TODO: use Qt's template (<QTDIR>/lib/cmake/Qt6/macos/Info.plist.app.in) once we support dark mode properly
    MACOSX_BUNDLE_INFO_PLIST "macos/Info.plist.in"
    MACOSX_BUNDLE_BUNDLE_NAME "${projectDisplayName}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_COPYRIGHT "LibreELEC © ${copyrightYears}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "tv.libreelec.usb-sd-creator"
    MACOSX_BUNDLE_ICON_FILE "icon.icns"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

target_link_libraries(LibreELEC.USB-SD.Creator PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Widgets
    ZLIB::ZLIB
)

string(TIMESTAMP nowDateTime UTC)
target_compile_definitions(LibreELEC.USB-SD.Creator PRIVATE
    "BUILD_DATE=\"${nowDateTime}\""
    "BUILD_VERSION=\"${PROJECT_VERSION}\""
    "COPYRIGHT_YEARS=\"${copyrightYears}\""
    $<$<CONFIG:Debug>:ALWAYS_DEBUG_OUTPUT>
    $<$<BOOL:${DUMMY_WRITE}>:WINDOWS_DUMMY_WRITE>
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
    )

    if(REQUIRE_ADMIN)
        target_sources(LibreELEC.USB-SD.Creator PUBLIC
            windows/winapp.rc
        )
        if(MSVC)
            set_target_properties(LibreELEC.USB-SD.Creator PROPERTIES
                LINK_OPTIONS "/MANIFEST:NO" # disable default manifest
            )
        endif()
    endif()

    configure_file("windows/winapp.rc.in" "${CMAKE_SOURCE_DIR}/windows/winapp.rc" @ONLY)

    set(qtDeployExecutable "$<TARGET_FILE_NAME:LibreELEC.USB-SD.Creator>")
    set(qtDeployExtraCommands "
        # windeployqt also packages extra libs that aren't needed
        foreach(dll in Qt${QT_VERSION_MAJOR}Svg opengl32sw D3Dcompiler_47)
            file(REMOVE \"\\\${QT_DEPLOY_PREFIX}/\\\${QT_DEPLOY_BIN_DIR}/\\\${dll}.dll\")
        endforeach()
    ")
elseif(APPLE)
    set_target_properties(LibreELEC.USB-SD.Creator PROPERTIES
        OUTPUT_NAME "${projectDisplayName}"
    )

    target_sources(LibreELEC.USB-SD.Creator PUBLIC
        deviceenumerator_unix.cpp deviceenumerator_unix.h
        diskwriter_unix.cpp diskwriter_unix.h
        privileges_unix.cpp privileges_unix.h
        macos/askpass.cpp macos/askpass.h
    )

    install(FILES "macos/dmg_osx/icon.icns"
        DESTINATION "$<TARGET_BUNDLE_CONTENT_DIR:LibreELEC.USB-SD.Creator>/Resources"
    )

    set(qtDeployExecutable "$<TARGET_FILE_NAME:LibreELEC.USB-SD.Creator>.app")
    set(qtDeployExtraCommands "
        set(bundleContentsDir \"\\\${QT_DEPLOY_PREFIX}/${qtDeployExecutable}/Contents\")

        # macdeployqt also packages extra frameworks that aren't needed
        foreach(qtModule in Pdf Svg)
            file(REMOVE_RECURSE \"\\\${bundleContentsDir}/Frameworks/Qt\\\${qtModule}.framework\")
        endforeach()

        # codesign
        execute_process(COMMAND \"${CMAKE_SOURCE_DIR}/macos/codesign.sh\"
            WORKING_DIRECTORY \"\\\${bundleContentsDir}\"
        )
    ")
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

# install
install(TARGETS LibreELEC.USB-SD.Creator
    BUNDLE
        DESTINATION .
)
if(qtDeployExecutable)
    qt_generate_deploy_script(
        TARGET LibreELEC.USB-SD.Creator
        OUTPUT_SCRIPT qtDeployScript
        CONTENT "
            qt_deploy_runtime_dependencies(
                EXECUTABLE \"${qtDeployExecutable}\"
                GENERATE_QT_CONF
                NO_APP_STORE_COMPLIANCE
                NO_COMPILER_RUNTIME
                NO_TRANSLATIONS
                VERBOSE
            )
            ${qtDeployExtraCommands}
        "
    )
    install(SCRIPT ${qtDeployScript})
endif()

# cpack
set(CPACK_PACKAGE_DIRECTORY "cpack")
set(CPACK_PACKAGE_NAME "${projectDisplayName}")
set(CPACK_PACKAGE_VENDOR "LibreELEC")
if(WIN32)
    set(CPACK_PACKAGE_FILE_NAME "LibreELEC.USB-SD.Creator.x64")
    set(CPACK_PACKAGE_EXECUTABLES "LibreELEC.USB-SD.Creator;LibreELEC USB-SD Creator x64")
    set(CPACK_GENERATOR INNOSETUP)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${projectDisplayName}")
    set(CPACK_INNOSETUP_IGNORE_LICENSE_PAGE ON)
elseif(APPLE)
    set(CPACK_PACKAGE_FILE_NAME "LibreELEC.USB-SD.Creator.macOS")

    set(notarizeScript "${CMAKE_BINARY_DIR}/notarize.cmake")
    file(WRITE "${notarizeScript}"
        "execute_process(COMMAND sh -c \"${CMAKE_SOURCE_DIR}/macos/notarize.sh \${CPACK_PACKAGE_FILES}\")"
    )
    set(CPACK_POST_BUILD_SCRIPTS "${notarizeScript}")

    set(CPACK_GENERATOR DragNDrop)
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/macos/dmg_osx/background.png")
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "DS_Store_Setup.scpt")
    set(CPACK_DMG_VOLUME_NAME "${projectDisplayName}")

    configure_file("macos/dmg_osx/DS_Store_Setup.scpt.in" "${CPACK_DMG_DS_STORE_SETUP_SCRIPT}" @ONLY)
endif()
include(CPack)
