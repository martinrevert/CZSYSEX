# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **语言:** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · **简体中文** · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

基于 **C++20** 与硬件加速界面（**DirectX 11 + Dear ImGui**）开发的 **Casio CZ-3000** 合成器现代原生桌面应用程序（兼容 Casio CZ 全系列：CZ-101、CZ-1000、CZ-3000、CZ-5000 与 CZ-1）。

由 Python 项目 `kasploosh` (`czsysexy` / `czcommon`) 移植并全面扩展。

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 核心功能

1. **高度还原 Casio CZ-3000 硬件面板**:
   - 复古炭灰色机身搭配经典 Casio 配色条纹（青色、深蓝与红色）。
   - **16×2 HD44780 液晶显示屏**：5×7 点阵荧光绿屏幕、硬件闪烁光标以及真实编辑页面（`Play`、`Line`、`Vibrato`、`Detune`、`DCO`、`DCW`、`DCA`）。
   - **Casio 触感面板按键与状态 LED**：
     - *Programmer*：`COMPARE/RECALL`、`INIT`、`WRITE`。
     - *参数选择*：`PLAY`、`LINE`、`VIB`、`DETUNE`、`DCO`、`DCW`、`DCA`。
     - *声部线路*：Line 1 与 Line 2。
     - *数据输入*：`UP`、`DOWN`、`< CUR`、`CUR >`。
     - *音色库与音色*：Preset 与 Memory 音色库 `A`–`D` 及槽位 `1`–`8`（共 64 个音色）。
   - **6 个八度实时试听键盘**：交互式虚拟钢琴（**C1** 至 **C7**，73 键），用于实时 MIDI 试听。

2. **交互式 8 段包络线图表**:
   - 实时查看与编辑 **DCO**（音高，青色）、**DCW**（亮度/PD，琥珀色）和 **DCA**（音量，亮绿色）曲线，支持单独或叠加（`ALL`）显示。
   - 可视化 `SUS`（延音）与 `END`（结束）标记。
   - 全 8 段 `Rate`（0–99）与 `Level`（0–99）滑块。

3. **64 音色双音色库管理器（32 预设 + 32 用户记忆）**:
   - **PRESET BANK（32 固定）** 与 **MEMORY BANK（32 用户）** 标签页，采用硬件一致的 `*MEMORY* A-1` 命名格式并支持右键自定义重命名（通过 CZ-1 296 字节 `.syx` 导出/导入保留）。
   - **批量下载与预设缓存**：一键下载全部 32 个预设（本地缓存，无需每次重新读取）或全部 32 个用户记忆。
   - **Auto-Fetch SysEx**：点击预设或记忆槽位时自动从硬件请求 SysEx 数据。
   - **合成器音色随机生成器（`随机生成` / `Randomize`）**：在当前记忆槽位随机生成所有合成器参数（`Line Select`、`Octave`、`Detune`、`Vibrato`、波形、调制及 8 段 DCO/DCW/DCA 包络）并实时发送到编辑缓冲区（`0x60`）。
   - 发送到编辑缓冲区（`0x60`）、写入合成器内存、发送完整 32 音色库、槽位间复制/粘贴以及加载/保存 `.syx` 文件。

4. **多语言界面、PD 交互指南与内置官方手册（8 种语言）**:
   - 支持实时切换语言（**English**、**Español**、**Italiano**、**Deutsch**、**日本語**、**简体中文**、**Português (Brasil)**、**Français**），完整支持 CJK 字体。
   - **教程模式（`[?] 教程模式: ON/OFF`）**：悬停提示与交互式 **相位失真指南** 窗口，详细讲解 DCO、DCW、DCA、8 段包络、波形、Ring/Noise 调制及 SysEx。
   - **CZ-3000 官方操作手册视图**：内置 9 大章节参考手册（`视图 → CZ-3000 官方操作手册`），包含 32 预设表、`INITIALIZE` 默认值与 8 种语言的 MIDI 实现表。

5. **低延迟原生 MIDI 引擎与 VST3 插件 (DAW / Ableton Live)**:
   - 基于 Windows 原生多媒体 API（`winmm.dll`），支持热切换 MIDI In/Out 端口与官方 263/264 字节 Casio SysEx 传输。
   - **VST3 MIDI 插件 (`CZ3000SysEx.vst3`)**: 可安装至 `C:\Program Files\Common Files\VST3`，在宿主软件（Ableton Live、Cubase、Reaper 等）的 MIDI 轨道中即时访问 64 个音色、同步音色库、发送程序变更并保存工程状态。

---

## 🛠️ 编译与运行

### 环境要求
- Windows 10 或 11 (x64)
- Visual Studio 2022 / Build Tools（支持 C++20）
- CMake 3.20+
- 支持子模块的 Git（`git clone --recursive` 或 `git submodule update --init --recursive`）
- *(可选)* NSIS 3.x 用于打包 Windows 安装程序

### 快速编译与测试
```bat
build.bat
```
生成 `build\Release\CZ3000SysEx.exe`、`build\Release\CZ3000SysEx.vst3` 与 `build\Release\test_sysex.exe`，并运行所有核心单元测试。

### 生成安装程序
```bat
build_installer.bat
```

---

## 📁 项目结构

```
CZSYSEX/
├── docs/                  # 截图与文档资源
├── external/
│   └── imgui/             # Dear ImGui 子模块 + Win32/DX11 后端
├── installer/             # NSIS 安装脚本
├── src/
│   ├── core/              # 数据模型、数学运算、i18n、SysEx 编解码与音色库缓存
│   ├── midi/              # Windows Multimedia (WinMM) MIDI 驱动
│   ├── ui/                # LCD、教程、手册、包络图、音色库管理器与面板
│   ├── vst3/              # VST3 插件处理器、视图与入口
│   └── main.cpp           # Win32 窗口、DirectX 11 与主循环
├── tests/
│   └── test_sysex.cpp     # SysEx、音色库与缓存单元测试
├── AGENTS.md              # 协作者与 CI 验证规范
├── CMakeLists.txt
├── build.bat
└── build_installer.bat
```

---

## 📄 开源协议

基于 **GNU General Public License v3.0 (GPLv3)** 协议分发。详情请参阅 [LICENSE](LICENSE)。
