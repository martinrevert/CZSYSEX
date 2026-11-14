# AGENTS.md — CZ-3000 SysEx Editor & Librarian

Guidelines for AI agents and contributors working in this repository to prevent build, test, and CI/CD deployment failures.

## 1. Mandatory Pre-Commit / Pre-Push Verification
Every push to `main` triggers `.github/workflows/release.yml` (`windows-latest` / MSVC x64), which builds **both** `CZ3000SysEx.exe` and `test_sysex.exe`, runs `test_sysex.exe`, and packages the NSIS installer.

**Always run a clean build + test after your final edit and before committing/pushing:**
```powershell
Move-Item build\Release\CZ3000SysEx.exe build\Release\CZ3000SysEx_prev.exe -Force -ErrorAction SilentlyContinue
.\build.bat
```
Or manually (matching CI):
```powershell
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
.\build\Release\test_sysex.exe
```

- **Do not rely on `test_sysex.exe` alone**: `test_sysex` only links `src/core/*` (`cz_math`, `cz_patch`, `cz_sysex`, `cz_bank`). UI/MIDI compilation errors in `src/ui/*` or `src/midi/*` will still break `CZ3000SysEx` in CI.
- **Locked `.exe` on Windows**: If `CZ3000SysEx.exe` is running during a local build, MSBuild fails with `LNK1104`. Move or close the running executable before running `.\build.bat`.
- **Clean staging check**: When using partial staging (`git add -p`), cherry-picking, or resolving merge conflicts, run `.\build.bat` against the exact staged/committed state so `.h` and `.cpp` files never drift out of sync.

## 2. Enum, Header & CMake Synchronization
- **Enum changes**: When editing an `enum class` (e.g. `LibrarianAction` in `src/ui/cz_librarian.h`, `Language` in `src/core/cz_i18n.h`), grep all usages across `src/` before committing. Never remove an enum member (such as `CancelBatch`) while `.cpp` files (`cz_librarian.cpp`, `cz_panel.cpp`) still reference it.
- **`CMakeLists.txt`**: Any new `.h`/`.cpp` file under `src/ui/` or `src/midi/` must be added to `APP_SOURCES`. Any new core file under `src/core/` must be added to both `APP_SOURCES` and `TEST_SOURCES`.

## 3. Encoding & Multi-Language Rules
- `CMakeLists.txt` enables `/utf-8` on MSVC. Keep all source files saved in **UTF-8** so strings in all 8 supported languages (`En`, `Es`, `It`, `De`, `Ja`, `Zh`, `PtBr`, `Fr`) compile cleanly without codepage warnings or corrupted glyphs.
- Whenever adding UI strings, add all 8 translations in `src/core/cz_i18n.cpp` (or per-language tables in `cz_tutorial.cpp` / `cz_manual.cpp`).

## 4. CI/CD & Versioning Notes
- **Workflows**:
  - `.github/workflows/ci.yml`: Runs on PRs to `main` (`cmake -B build`, `cmake --build build --config Release`, `.\build\Release\test_sysex.exe`).
  - `.github/workflows/release.yml`: Runs on pushes to `main` and `workflow_dispatch`. Automatically bumps SemVer from commits since the latest `v*` tag (`BREAKING CHANGE`/`major:` -> major, `feat:`/`feature:`/`minor:` -> minor, otherwise patch) and builds the NSIS installer.
- **Submodules**: Checkout uses `submodules: recursive` (`external/`). Do not commit untracked build artifacts (`build/`).
