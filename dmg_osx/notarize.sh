#!/usr/bin/env bash

dmgPath="$1"

if [ -z "$CODE_SIGN_IDENTITY" ] ; then
  echo 'skip dmg signing & notarization - no code sign identity provided in CODE_SIGN_IDENTITY'
  exit 0
fi
echo 'sign dmg'
codesign --verbose=4 --force --sign "$CODE_SIGN_IDENTITY" "$dmgPath"

if [[ -z "$ASC_API_KEY_PATH" || -z "$ASC_API_KEY_ID" || -z "$ASC_API_KEY_ISSUER" || -z "$ASC_TEAM_ID" ]] ; then
  echo 'skip dmg notarization - ASC_API_KEY_PATH / ASC_API_KEY_ID / ASC_API_KEY_ISSUER / ASC_TEAM_ID not provided'
  exit 0
fi
echo 'notarize dmg'
xcrun notarytool submit \
  --verbose \
  --key "$ASC_API_KEY_PATH" \
  --key-id "$ASC_API_KEY_ID" \
  --issuer "$ASC_API_KEY_ISSUER" \
  --team-id "$ASC_TEAM_ID" \
  --wait \
  --timeout 30m \
  "$dmgPath"
xcrun stapler staple "$dmgPath"
