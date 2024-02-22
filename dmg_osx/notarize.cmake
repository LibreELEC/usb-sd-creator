set(dmgPath "${CPACK_PACKAGE_FILES}")

if(NOT DEFINED ENV{CODE_SIGN_IDENTITY})
    message("skip dmg signing & notarization - no code sign identity provided in CODE_SIGN_IDENTITY")
    return()
endif()
message("sign dmg")
execute_process(COMMAND
    codesign --verbose=4 --force --sign "$ENV{CODE_SIGN_IDENTITY}" "${dmgPath}"
)

if(NOT DEFINED ENV{ASC_API_KEY} OR NOT DEFINED ENV{ASC_API_KEY_ID} OR NOT DEFINED ENV{ASC_API_KEY_ISSUER} OR NOT DEFINED ENV{ASC_TEAM_ID})
    message("skip dmg notarization - ASC_API_KEY / ASC_API_KEY_ID / ASC_API_KEY_ISSUER / ASC_TEAM_ID not provided")
    return()
endif()
message("notarize dmg")
execute_process(COMMAND
    xcrun notarytool submit
        --verbose
        --key "$ENV{ASC_API_KEY}"
        --key-id "$ENV{ASC_API_KEY_ID}"
        --issuer "$ENV{ASC_API_KEY_ISSUER}"
        --team-id "$ENV{ASC_TEAM_ID}"
        --wait
        --timeout 30m
        "${dmgPath}"
)
execute_process(COMMAND
    xcrun stapler staple "${dmgPath}"
)
