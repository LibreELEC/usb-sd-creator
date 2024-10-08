# **LibreELEC USB/SD Creator**

This is a lightweight image writing app with a simple four-step GUI for creating LibreELEC USB/SD Card installation media on Linux, macOS and Windows. It automatically displays the currently available downloads (defaulting to the latest release) and detects OS locale to switch the GUI to a matching local language.

## Translation Changes

Changes to master language (en_GB) strings can be submitted via pull request to this GitHub repo. English strings are periodically synchronised to a Transifex project: [https://www.transifex.com/libreelec/libreelec-usb-sd-creator](https://www.transifex.com/libreelec/libreelec-usb-sd-creator) allowing contributors to translate them into other languages. Translated strings are periodically synchronised from Transifex back to this repo for inclusion in the next release.

Pull requests for all other languages will be rejected. You will be asked to submit changes via Transifex.

## Translation Languages

Requests for new languages should be made on Transifex. Once a new language has been approved it will be added to the translation project where you can conribute translated strings.

Please note: languages are translated by people (you) not magic!

## Issues and Support

Issues should be reported via the forum here: [https://forum.libreelec.tv/board/41-usb-sd-creator-support](https://forum.libreelec.tv/board/41-usb-sd-creator-support)

# **How to compile the USB/SD Creator**

Build instructions are supplied for Windows x64 (Installer and Portable), macOS (Intel and Apple Silicon) and Ubuntu Linux (x86_64). Instructions have been tested locally on Windows 11, macOS Sonoma and Ubuntu 24.04.

- [Windows](#Windows)
- [macOS](#macOS)
- [Linux](#Linux)

# Windows

Note: the offical release for Windows is built using Visual Studio

## Building for Windows using MSys2 and MinGW

### 1. Install msys2 and MinGW

Install the latest msys2 via the installer (not the base installer): [https://repo.msys2.org](https://repo.msys2.org/distrib/x86_64/)

After the install, from the resulting msys2 console install mingw:

```
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-ninja mingw-w64-x86_64-zlib mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-tools
```

Add msys2 to PATH: `C:\msys64\mingw64\bin`.

Note: you must use a standard commnd prompt when using mingw for the build.

### 2. Install Inno Setup (Compiler for Installer)

[https://jrsoftware.org](https://jrsoftware.org/isdl.php), download and install the latest stable version.

Add Inno Setup to PATH: `C:\Program Files (x86)\Inno Setup 6`.

### 3. Clone Git Repo

Clone the repository to `%UserProfile%/usb-sd-creator`
`git clone https://github.com/LibreELEC/usb-sd-creator.git`

### 4. Build USB-SD-Creator

Assuming the repo is in your home directory

```
cd %UserProfile%/usb-sd-creator
```

#### Debug build

```
cmake -S . -B build -G Ninja && cmake --build build
```

#### Release build
```
cmake --preset release-ninja && cmake --build --preset release
```

### 4. Build Installer

#### Release build

##### Create installer

```
cpack --preset release
```

##### Create Zip for portable installs (UWP)

```
cpack --preset release -G ZIP
```

#### Debug build

```
cd build
cpack -C Debug
```

### 5. Run USB-SD-Creator

Run the installer in `build/cpack`. Then run the app from Start Menu: `LibreELEC USB-SD Creator x64`.

## Building for Windows using Visual Studio (MSVC)

### 1. Install Qt 6.7.2 and Visual Studio Community 2022

#### Install required Qt packages

To see the available Qt versions run:

```
aqt list-qt windows desktop
```

To see the available compiler versions run:

```
aqt list-qt linux desktop --arch 6.7.2
```

Install the required packages:

```
aqt install-qt --outputdir %UserProfile%/Qt windows desktop 6.7.2 win64_msvc2019_64 --archives qtbase qttools opengl32sw d3dcompiler_47 --external 7z.exe
aqt install-qt --outputdir %UserProfile%/Qt windows desktop 6.7.2 win64_msvc2019_64 --modules debug_info --external 7z.exe
```

#### Install Visual Studio Community 2022

Install Visual Studio Community 2022 from: [https://visualstudio.microsoft.com](https://visualstudio.microsoft.com/vs/community/). Note that you only require the Desktop C++ package which you can select in the online installer.

Note that the command prompt is to be used thoughtout the MSVC build must be started from a standard command prompt for `x64` as follows: `"C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat" -arch=x64 -host_arch=x64`.

### 2. Clone Git Repo

Clone the repository to `%UserProfile%/usb-sd-creator`
`git clone https://github.com/LibreELEC/usb-sd-creator.git`

### 3. Build USB-SD-Creator

Assuming the repo is in your home directory

```
cd %UserProfile%/usb-sd-creator
```

#### Debug build

```
cmake -S . -B build -D CMAKE_PREFIX_PATH="%UserProfile%/Qt/6.7.2/msvc2019_64" && cmake --build build
```

#### Release build

```
cmake --preset release-msvc -D CMAKE_PREFIX_PATH="%UserProfile%/Qt/6.7.2/msvc2019_64" && cmake --build --preset release
```

### 4. Build Installer

#### Debug build

```
cd build
cpack -C Debug
```

#### Release build

##### Create installer

```
cpack --preset release
```

##### Create Zip for portable installs (UWP)

```
cpack --preset release -G ZIP
```

### 5. Run USB-SD-Creator

Run the installer in `build/cpack`. Then run the app from Start Menu: `LibreELEC USB-SD Creator x64`.

# macOS

## Building for macOS

### 1. Install XCode with Command-line tools

### 2. Setup Qt 6.7.2

#### Install pre-requisites

The build requires both `python3` and `cmake`. If you don't have them installed, run the following commands:

```
brew install python
brew install pipx
brew install cmake
```

Make sure that `/opt/homebrew/bin` is in your `PATH`.

Now install `aqt`, a command line package manager for `Qt`:

```
pipx install aqtinstall
pipx ensurepath
```

#### Install required Qt packages

To see the available Qt versions run:

```
aqt list-qt mac desktop
```

To see the available compiler versions run:

```
aqt list-qt linux desktop --arch 6.7.2
```

Install the required packages:

```
aqt install-qt --outputdir ~/Qt mac desktop 6.7.2 --archives qtbase qttools
```

### 3. Clone Git Repo
Clone the repository to `~/usb-sd-creator`
`git clone https://github.com/LibreELEC/usb-sd-creator.git`

### 4. Build USB-SD-Creator

Assuming the repo is in your home directory

```
cd ~/usb-sd-creator
```

#### Debug build

```
cmake -S . -B build -D CMAKE_PREFIX_PATH="/Users/$USER/Qt/6.7.2/macos" && cmake --build build
```

#### Release build

```
cmake --preset release -D CMAKE_PREFIX_PATH="/Users/$USER/Qt/6.7.2/macos" && cmake --build --preset release
```

### 5. Run USB-SD-Creator

#### Open the app

Simply double click the app from a finder window in the `build` folder in the repo: `build/LibreELEC USB-SD Creator`

#### Command line

Run the app from the command line, that will prompt for a password:
```
./build/LibreELEC\ USB-SD\ Creator.app/Contents/MacOS/LibreELEC\ USB-SD\ Creator
```

**Or:**

Run the app from the command line using sudo

```
sudo ./build/LibreELEC\ USB-SD\ Creator.app/Contents/MacOS/LibreELEC\ USB-SD\ Creator
```

### 6. Debugging USB-SD-Creator

#### Using Qt Creator

If you need to, install Qt Creator:

```
brew install --cask qt-creator
```

Then simply open CMakeLists.txt in Qt Creator

#### Using XCode

Build the xcode project, and open the project file in Xcode, located in the build folder (note you may need to clear any previous build files before genreating for XCode):

```
cmake -S . -B build -G Xcode -D CMAKE_PREFIX_PATH="/Users/$USER/Qt/6.7.2/macos" && cmake --build build
```
### 7. Build Install .dmg

If you wish to build the install `.dmg` (not required for development), you can create it using the following command. Note that code signing cannot be done locally (unless you have your own certs).

#### Debug build

```
cd build
cpack -C Debug
```

#### Release build

```
cpack --preset release
```


# Linux

### 1. Install build tools

```
sudo apt install build-essential
sudo apt install libgl-dev
sudo apt install cmake
sudo apt install git
```

### 2. Setup Qt 6.7.2

#### Install pre-requisites (Pip and Pipx)

```
sudo apt install python3-pip pipx
pipx ensurepath
```

Now install `aqt`, a command line package manager for `Qt`:

```
pipx install aqtinstall
```

#### Install required Qt packages

To see the available Qt versions run:

```
aqt list-qt linux desktop
```

To see the available compiler versions run:

```
aqt list-qt linux desktop --arch 6.7.2
```

Install the required packages:

```
aqt install-qt --outputdir ~/Qt linux desktop 6.7.2 linux_gcc_64 --archives qtbase qttools
aqt install-qt --outputdir ~/Qt linux desktop 6.7.2 linux_gcc_64 --modules debug_info
```

### 3. Clone Git Repo

Clone the repository to `~/usb-sd-creator`
`git clone https://github.com/LibreELEC/usb-sd-creator.git`

### 4. Build USB-SD-Creator

Assuming the repo is in your home directory

```
cd ~/usb-sd-creator
```

#### Debug build

```
cmake -S . -B build -D CMAKE_PREFIX_PATH="~/Qt/6.7.2/gcc_64" && cmake --build build
```

#### Release build

```
cmake --preset release -D CMAKE_PREFIX_PATH="~/Qt/6.7.2/gcc_64" && cmake --build --preset release
```

### 5. Run USB-SD-Creator

#### Command line

Run the app from the command line, that will prompt for a password:

```
./build/LibreELEC.USB-SD.Creator.Linux.bin
```

### 6. Build Install Package

If you wish to build the install package (not required for development), you can create tarballs using:

#### Debug build

```
cd build
cpack -C Debug
```

#### Release build

```
cpack --preset release
```
