# **LibreELEC USB/SD Creator** Test

This is a lightweight image writing app with a simple four-step GUI for creating LibreELEC USB/SD Card installation media on Linux, macOS and Windows. It automatically displays the currently available downloads (defaulting to the latest release) and detects OS locale to switch the GUI to a matching local language.

## Translation Changes

Changes to master language (en_GB) strings can be submitted via pull request to this GitHub repo. English strings are periodically synchronised to a Transifex project: https://www.transifex.com/libreelec/libreelec-usb-sd-creator allowing contributors to translate them into other languages. Translated strings are periodically synchronised from Transifex back to this repo for inclusion in the next release.

Pull requests for all other languages will be rejected. You will be asked to submit changes via Transifex.

## Translation Languages

Requests for new languages should be made on Transifex. Once a new language has been approved it will be added to the translation project where you can conribute translated strings.

Please note: languages are translated by people (you) not magic!

## Issues and Support

Issues should be reported via the forum here: https://forum.libreelec.tv/board/41-usb-sd-creator-support

# **How to compile the USB/SD Creator**

# Windows

### 1. install "Qt Online Installer"
select: Custom install -> select QT 6.6.2 -> disable all besides MinGW
Install to `C:\QT`

### 2. Install 7zip
https://www.7-zip.org/download.html

### 3. Install Git
https://git-scm.com/download/win

### 4. Install Ruby
https://rubyinstaller.org/downloads/ select "WITHOUT DEVKIT" for download

### 5. Install Perl
https://www.activestate.com/products/perl/ (needs registration) or

https://www.softpedia.com/get/Programming/Coding-languages-Compilers/ActivePerl.shtml

### 6. Install Python
https://www.python.org/downloads/windows/ select Windows installer (64-bit) for download
at install select "Add Python to PATH"

### 7. Add programs to PATH variable

`C:\Qt\Tools\Ninja`
`C:\Qt\Tools\CMake_64\bin`

reboot Windows

### 8. Clone Git Repo
Clone the repository to `C:\usb-sd-creator`
`git clone https://github.com/LibreELEC/usb-sd-creator.git`

### 9. Enable powershell script execution
Open cmd with admin rights
`powershell Set-ExecutionPolicy RemoteSigned`

### 10. Build static Qt files

Compiling needs around **13GB RAM**.

Open powershell at C:\usb-sd-creator
`.\windows_qt_build_static.ps1`

After the compiling is finished the files are located at `C:\Qt\static\$version` .

### 11. Build USB-SD-Creator
Open a cmd at `C:\usb-sd-creator` and run `windows_build.bat`.
The finished executable is located at `C:\usb-sd-creator\releases`.

# MacOS

### Building for MacOS

### 1. Install XCode with Command-line tools

### 2. Setup QT 6.6.2 (static build or shared)

#### Static build

Open a command prompt and type the following in the console:

```
mkdir -p ~/Downloads ~/Qt
cd ~/Downloads

wget https://download.qt.io/official_releases/qt/6.6/6.6.2/single/qt-everywhere-src-6.6.2.tar.xz
tar xf qt-everywhere-src-6.6.2.tar.xz
cd qt-everywhere-src-6.6.2
./configure -static -no-shared -release -opensource -confirm-license -silent -prefix ~/Qt/6.6.2-static -nomake examples -nomake tests -no-strip -no-cups -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-harfbuzz -qt-freetype
ninja qtbase/all qttools/all
ninja install
```

#### Shared library

TODO

### 4. Build USB-SD-Creator

Assuming the repo is in your home directory

```
cd ~/usb-sd-creator

./osx_build.sh
```

If building again cleanup using:
```
./osx_clean.sh
```

### 5. Run USB-SD-Creator

#### Open the app

Simply double click the app from a finder window in the root of the repo: `LibreELEC USB-SD Creator`

#### Command line

Run the app from the command line, that will prompt for a password:
```
./LibreELEC\ USB-SD\ Creator.app/Contents/MacOS/LibreELEC\ USB-SD\ Creator
```

**Or:**

Run the app from the command line using sudo

```
sudo ./LibreELEC\ USB-SD\ Creator.app/Contents/MacOS/LibreELEC\ USB-SD\ Creator
```
