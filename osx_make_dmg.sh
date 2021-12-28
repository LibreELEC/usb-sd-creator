#!/bin/sh
################################################################################
#      This file is part of LibreELEC - http://www.libreelec.tv
#      Copyright (C) 2016 Team LibreELEC
#
#  LibreELEC is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  LibreELEC is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with LibreELEC.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

set -e

DEVICE=""

USER=$(whoami)
QBIN="/Users/$USER/Qt/6.2.2-static/bin"

function finish {
  echo "Cleaning up..."
  rm -f *.shadow
  rm -f hdiutil.log
  rm -f *.dmg

  if [ -n "$DEVICE" ]; then
    echo ""
    echo "Running hdiutil detach..."
    hdiutil detach $DEVICE
  fi
}

if diskutil list | grep "Windows_FAT_32 LIBREELEC" >/dev/null 2>&1; then
  echo "LIBREELEC disk already mounted"
  exit 1
fi

rm -f LibreELEC.USB-SD.Creator*.dmg

trap finish EXIT

DMG_BACKGROUND_IMG="dmg_osx/background.png"

# Check the background image DPI and convert it if it isn't 72x72
_BACKGROUND_IMAGE_DPI_H=`sips -g dpiHeight ${DMG_BACKGROUND_IMG} | grep -Eo '[0-9]+\.[0-9]+'`
_BACKGROUND_IMAGE_DPI_W=`sips -g dpiWidth ${DMG_BACKGROUND_IMG} | grep -Eo '[0-9]+\.[0-9]+'`

if [ $(echo " $_BACKGROUND_IMAGE_DPI_H != 72.0 " | bc) -eq 1 -o $(echo " $_BACKGROUND_IMAGE_DPI_W != 72.0 " | bc) -eq 1 ]; then
   echo "WARNING: The background image DPI is not 72.  This will result in distorted backgrounds on Mac OS X 10.7+."
   echo "Converting to 72 DPI."

   _DMG_BACKGROUND_TMP="${DMG_BACKGROUND_IMG%.*}"_dpifix."${DMG_BACKGROUND_IMG##*.}"

   sips -s dpiWidth 72 -s dpiHeight 72 ${DMG_BACKGROUND_IMG} --out ${_DMG_BACKGROUND_TMP}

   DMG_BACKGROUND_IMG="${_DMG_BACKGROUND_TMP}"
   exit 1
fi

USER=$(whoami)

chmod -R 755 dmg_osx

rm -f *.dmg
sync

echo ""
echo "Running macdeployqt..."
$QBIN/macdeployqt "LibreELEC USB-SD Creator.app" -dmg

echo ""
echo "Running hdiutil attach..."
hdiutil attach -owners on "LibreELEC USB-SD Creator.dmg" -shadow 2>&1 | tee hdiutil.log
DEVICE=$(cat hdiutil.log | egrep '^/dev/' | sed 1q | awk '{print $1}')

diskutil rename "LibreELEC USB-SD Creator" LibreELEC

mkdir /Volumes/LibreELEC/.background
cp dmg_osx/background.png /Volumes/LibreELEC/.background/

cp "/Volumes/LibreELEC/LibreELEC USB-SD Creator.app/Contents/Resources/applet.icns" /Volumes/LibreELEC/.VolumeIcon.icns
SetFile -c icnC /Volumes/LibreELEC/.VolumeIcon.icns
SetFile -a C /Volumes/LibreELEC
ln -s /Applications /Volumes/LibreELEC/Applications

# tell the Finder to resize the window, set the background,
#  change the icon size, place the icons in the right position, etc.
dmg_topleft_x=100
dmg_topleft_y=100
dmg_width=$(sips -g pixelWidth dmg_osx/background.png | tail -n1 | cut -d":" -f2)
dmg_height=$(sips -g pixelHeight dmg_osx/background.png | tail -n1 | cut -d":" -f2)
dmg_bottomright_x=$(expr $dmg_topleft_x + $dmg_width)
dmg_bottomright_y=$(expr $dmg_topleft_y + $dmg_height)

le_icon_x=130    # fix it
app_icon_x=360
both_icon_y=180

echo "     window: ${dmg_topleft_x} ${dmg_topleft_y} ${dmg_bottomright_x} ${dmg_bottomright_y}"
echo "window size: $dmg_width x $dmg_height"
echo "    le_icon: ${le_icon_x} ${both_icon_y}"
echo "   app_icon: ${app_icon_x} ${both_icon_y}"

echo ""
echo "Running window script..."
sleep 1
# https://asmaloney.com/2013/07/howto/packaging-a-mac-os-x-application-using-a-dmg/
# https://discussions.apple.com/thread/4848969?tstart=0
echo '
  (*
  activate application "Finder"
  tell application "System Events" to tell application process "Finder"
    if not (exists (first window whose subrole is "AXSystemFloatingWindow")) then
      keystroke "j" using {command down}
      repeat until exists (first window whose subrole is "AXSystemFloatingWindow")
        delay 0.1
      end repeat
    end if
    tell (first window whose subrole is "AXSystemFloatingWindow") to tell first group
      get value of slider 1 # To get the value in use in a window using default spacing
      set value of slider 1 to 10.0
    end tell
  end tell
  *)

  tell application "Finder"
    tell disk "LibreELEC"
      open
      set current view of container window to icon view
      set toolbar visible of container window to false
      set statusbar visible of container window to false
      set the bounds of container window to {'${dmg_topleft_x}', '${dmg_topleft_y}', '${dmg_bottomright_x}', '${dmg_bottomright_y}'}
      set viewOptions to the icon view options of container window
      set arrangement of viewOptions to not arranged
      set icon size of viewOptions to 72
      set background picture of viewOptions to file ".background:'background.png'"
      set position of item "LibreELEC USB-SD Creator.app" of container window to {'${le_icon_x}', '${both_icon_y}'}
      set position of item "Applications" of container window to {'${app_icon_x}', '${both_icon_y}'}
      close
      open
      update without registering applications
      delay 2
    end tell
  end tell
' | osascript
sync

echo ""
echo "Press Enter to continue..."
read a

echo ""
echo "Running hdiutil detach..."
hdiutil detach $DEVICE

echo ""
echo "Running hdiutil convert..."
hdiutil convert -imagekey zlib-level=9 -format UDZO -ov -o "LibreELEC USB-SD Creator_new.dmg" "LibreELEC USB-SD Creator.dmg" -shadow
mv "LibreELEC USB-SD Creator_new.dmg" "LibreELEC USB-SD Creator.dmg"
rm -f *.shadow

echo ""
echo "Running attach convert..."
hdiutil attach -owners on "LibreELEC USB-SD Creator.dmg" -shadow
ls -al /Volumes/LibreELEC

echo ""
echo "Running hdiutil detach..."
hdiutil detach $DEVICE

mv "LibreELEC USB-SD Creator.dmg" LibreELEC.USB-SD.Creator.macOS.dmg

echo ""
echo "Signing DMG..."
echo ""

codesign -v -fs "LibreELEC" "LibreELEC.USB-SD.Creator.macOS.dmg"
codesign -vvvv "LibreELEC.USB-SD.Creator.macOS.dmg"

rm -f *.shadow
rm -f hdiutil.log

chmod -R 644 dmg_osx

trap - EXIT
exit 0
