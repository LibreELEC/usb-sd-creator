#!/usr/bin/env bash
set -e

dmgPath="$1"

if [ -z "$CODE_SIGN_IDENTITY" ] ; then
  echo 'skip dmg signing & notarization - no code sign identity provided in CODE_SIGN_IDENTITY'
  exit 0
fi
echo 'sign dmg'
codesign --verbose=4 --force --timestamp --sign "$CODE_SIGN_IDENTITY" "$dmgPath"

if [[ -z "$ASC_API_KEY_PATH" || -z "$ASC_API_KEY_ID" || -z "$ASC_API_KEY_ISSUER" || -z "$ASC_TEAM_ID" ]] ; then
  echo 'skip dmg notarization - ASC_API_KEY_PATH / ASC_API_KEY_ID / ASC_API_KEY_ISSUER / ASC_TEAM_ID not provided'
  exit 0
fi
ascAuth=(
  --key "$ASC_API_KEY_PATH"
  --key-id "$ASC_API_KEY_ID"
  --issuer "$ASC_API_KEY_ISSUER"
  --team-id "$ASC_TEAM_ID"
)

echo 'notarize dmg'
result=$(xcrun notarytool submit "${ascAuth[@]}" --wait --timeout 30m --output-format json "$dmgPath")
echo "$result"
submissionId=$(plutil -extract id raw -o - - <<< "$result")
status=$(plutil -extract status raw -o - - <<< "$result")

if [ "$status" != 'Accepted' ] ; then
  echo "notarization failed with status '$status', log:"
  xcrun notarytool log "${ascAuth[@]}" "$submissionId" || true
  exit 1
fi

xcrun stapler staple "$dmgPath"
xcrun stapler validate "$dmgPath"
