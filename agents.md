# AGENTS.md

Guidance and operating rules for AI agents and automated tools working on the **LibreELEC USB-SD Creator** codebase.

---

## 1. Core Operating Rules & Constraints

When modifying this repository, strictly adhere to the following rules:

### One Commit per Meaningful Change
* Every atomic, meaningful change must be encapsulated in its own standalone git commit.
* Use concise Conventional Commits syntax (`feat:`, `fix:`, `chore:`, `content:`, `docs:`, `refactor:`).
* Never bundle unrelated changes (e.g., dependency updates mixed with content edits or styling changes) into a single commit.

### Do Not Create Pull Requests
* Commit directly to the repository or target branch as instructed. Do not initiate pull requests (`gh pr create`, etc.) unless explicitly instructed by the user.

### Minimal Scope
* Make the smallest change that completely solves the issue.
* **No speculative hardening, unsolicited refactoring, or formatting churn.**
* If the scope of a task must expand to resolve unexpected blockers, stop immediately and report why to the user before proceeding.

### Comments & Explanations
* Keep code comments concise. Explain *why* something is done, not the obvious *what*.
* Never include rejected attempts, conversational scratchpad text, or historical trial-and-error reasoning in code or commit messages.
* **Never remove or alter existing human comments** unless they are proven obsolete by the change.

### Pre-submission Checklist
Before concluding any task or making a commit:
1. **Clean Diff**: Run `git diff` to verify only the intended lines have been touched.
2. **No AI Residue**: Avoid AI-specific phrasing markers, marketing buzzwords (e.g., "game-changer", "delve", "testament", "silently purges"), and em-dashes (`—`). Use standard punctuation (hyphens, commas, or parentheses).
3. **Build Verification**: Run `npm run build` and ensure it exits with code 0 (zero errors). *(Note: for this C++/CMake project, verify builds using `cmake --build --preset release` or `cmake --build build`)*.
4. **No Output Pollution**: Do not leave temporary scripts, scratch files, or unwanted logs in the workspace.

---

## 2. Project Overview

The **LibreELEC USB-SD Creator** is a cross-platform desktop utility developed in C++ and Qt 6. It provides a simple 4-step wizard interface allowing users to download official LibreELEC operating system images (or choose custom local images) and write them directly to removable USB flash drives and SD cards.

### Core Architecture & Flow
1. **Step 1 (Select Image / Hardware)**: Fetches and parses remote JSON feeds of available LibreELEC releases and hardware project variants.
2. **Step 2 (Download / Local File)**: Downloads target `.img.gz` images with SHA256 checksum validation via `DownloadManager`, or allows selecting a pre-downloaded image.
3. **Step 3 (Target Selection)**: Enumerates removable storage devices safely while filtering out fixed system disks.
4. **Step 4 (Write & Verify)**: Unpacks and writes the disk image to the target device with real-time speed calculation and progress indication.

---

## 3. Technology Stack & Key Dependencies

- **Language**: C++17 (`set(CMAKE_CXX_STANDARD 17)`).
- **Framework**: Qt 6 (minimum 6.11 recommended in CI; components: `Gui`, `Network`, `Widgets`, `LinguistTools`, and `DBus` on Linux).
- **Build System**: CMake (minimum 3.25) configured via `CMakePresets.json`.
- **Packaging Tools**:
  - **Linux**: `linuxdeploy` + `linuxdeploy-plugin-qt` + `linuxdeploy-plugin-appimage` (produces standalone `.AppImage`).
  - **macOS**: CPack `DragNDrop` generator (produces `.dmg`) with Apple Developer ID code signing and notarization scripts.
  - **Windows**: CPack `INNOSETUP` generator (uses Inno Setup 6 to compile `LibreELEC.USB-SD.Creator.x64.exe`).

---

## 4. Codebase Layout

```text
├── CMakeLists.txt              # Root CMake configuration, versioning, deployment, and packaging
├── CMakePresets.json           # Presets: release, release-ninja, release-macos, release-msvc
├── creator.cpp, .h, .ui        # Main UI application window, wizard flow, and event logic
├── diskwriter.cpp, .h          # Abstract disk writer interface
├── diskwriter_udisks2.*        # Linux disk writer via UDisks2 DBus
├── diskwriter_unix.*           # Unix / macOS raw block disk writer
├── diskwriter_windows.*        # Windows Win32 disk writer (device handle locking & writing)
├── deviceenumerator.h          # Abstract storage device enumerator
├── deviceenumerator_udisks2.*  # Linux device enumeration via UDisks2
├── deviceenumerator_unix.*     # macOS / Unix device enumeration
├── deviceenumerator_windows.*  # Windows device enumeration via SetupAPI
├── downloadmanager.cpp, .h     # Asynchronous file download and verification
├── jsonparser.cpp, .h          # LibreELEC image mirror and version JSON parser
├── translator.cpp, .h          # Runtime Qt language loader and UI switcher
├── lang/                       # Translations (.ts files) and 16px Gosquared country flags
├── ci/                         # Linux and macOS CI setup scripts
├── linux/                      # Desktop entry file and icons
├── macos/                      # macOS bundle Info.plist, DMG background, codesign & notarization
├── windows/                    # Windows manifest, resource files (.rc), and bundled static zlib
└── .github/workflows/build.yaml# Unified CI build and release workflow
```

---

## 5. Development & Build Commands

### CMake Presets
The repository defines standard presets in `CMakePresets.json`:
- `release`: Unix Makefiles (default on Linux)
- `release-ninja`: Ninja generator (used for MinGW and macOS)
- `release-macos`: macOS Universal (x86_64;arm64) targeting macOS 13.0+
- `release-msvc`: MSVC NMake Makefiles (used for official 64-bit Windows releases)

### Build Commands
```bash
# Configure (using preset)
cmake --preset release

# Build
cmake --build --preset release

# Package
cpack --preset release
```

### Safe Testing Option (`DUMMY_WRITE`)
When modifying or debugging disk writing logic on Windows, enable the dummy write CMake option to prevent accidental overwrites of actual physical drives:
```bash
cmake -DDUMMY_WRITE=ON ...
```
This writes the image stream to a local file named `dummy_image_file` rather than touching a physical disk.

---

## 6. Versioning & Release Workflow

- **Version Authority**: `CMakeLists.txt` lines 8-12:
  ```cmake
  project(LibreELEC.USB-SD.Creator
      VERSION 1.6
      LANGUAGES CXX
      HOMEPAGE_URL "https://libreelec.tv/"
  )
  ```
  `BUILD_VERSION` and bundle metadata are derived automatically from `PROJECT_VERSION`.
- **CI Version Injection**:
  In `.github/workflows/build.yaml`, the workflow inspects the git reference:
  - If a tag is pushed (e.g. `1.6` or `v1.6`), `CMakeLists.txt` is updated to that version prior to compilation.
  - If no tag is present (PRs or master commits), the version defaults to `0.0`.
- **Release Assets**:
  When a release is triggered:
  - **Windows**: ONLY `LibreELEC.USB-SD.Creator.x64.exe` (from the `windows-msvc` build) is published. MinGW and portable `.zip` archives are omitted.
  - **macOS**: `LibreELEC.USB-SD.Creator.macOS.dmg`
  - **Linux**: `LibreELEC.USB-SD.Creator.Linux.x86_64.AppImage`

---

## 7. Translation & i18n Rules

- **English Strings**: `en_GB` strings in UI files (`creator.ui`) and C++ sources are master strings.
- **Other Languages**: Translations (`lang-*.ts`) are synchronized via Transifex. Do not submit direct manual edits to non-English translation files unless requested by repository maintainers.
