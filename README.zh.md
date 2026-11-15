# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **语言:** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · **简体中文** · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

基于 **C++20** 与硬件加速界面（**DirectX 11 + Dear ImGui**）开发的 **Casio CZ-3000** 合成器现代原生桌面编辑器与音色库管理器（兼容 CZ-101、CZ-1000、CZ-3000、CZ-5000 与 CZ-1）。

由 Python 项目 `kasploosh` (`czsysexy` / `czcommon`) 移植并全面扩展。

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 核心功能

1. **高度还原 Casio CZ-3000 硬件面板**:
   - 经典炭灰机身与 Casio 配色条纹。
   - **16×2 HD44780 液晶屏**：5×7 点阵荧光绿显示、硬件闪烁光标与完整编辑页面（`Play`、`Line`、`Vibrato`、`Detune`、`DCO`、`DCW`、`DCA`）。
   - **实体感按键与状态 LED**，以及 **6 个八度（C1–C7）实时 MIDI 试听虚拟键盘**。

2. **交互式 8 段包络线图表**:
   - 实时查看和编辑 **DCO**（音高）、**DCW**（亮度/PD）与 **DCA**（音量）包络曲线（支持单层或 `ALL` 叠加显示），带 `SUS`/`END` 标记和 `Rate`/`Level`（0–99）滑块。

3. **64 音色双音色库管理器（32 预设 + 32 用户记忆）**:
   - **PRESET BANK** 与 **MEMORY BANK**（硬件风格 `*MEMORY* A-1`，支持右键自定义重命名并通过 CZ-1 296 字节 `.syx` 保留）。
   - **32 音色批量下载与预设本地缓存**，支持选中时 **Auto-Fetch SysEx** 自动读取。
   - **随机音色生成器（`随机生成` / `Randomize`）**：随机生成当前记忆槽位的所有合成器参数并实时发送到编辑缓冲区（`0x60`）试听。

4. **8 国语言界面、PD 交互指南与内置官方手册**:
   - 支持即时切换 **English**、**Español**、**Italiano**、**Deutsch**、**日本語**、**简体中文**、**Português (Brasil)**、**Français**（完整支持 CJK 字体）。
   - **教程模式**悬停提示、**相位失真交互指南**，以及内置 **Casio CZ-3000 官方操作手册**（9 大章节 + 32 预设表 + `INITIALIZE` 默认表 + MIDI 实现表）。

5. **低延迟原生 MIDI 引擎（`winmm.dll`）**:
   - 支持 MIDI In/Out 热切换及官方 263/264 字节 Casio SysEx 收发。

---

## 🛠️ 编译与运行

- **环境要求**：Windows 10/11 (x64)、Visual Studio 2022 (C++20)、CMake 3.20+、Git (`--recursive`)、NSIS 3.x（可选）。
- **快速编译与测试**：运行 `build.bat`（生成 `build\Release\CZ3000SysEx.exe` 并运行 `test_sysex.exe`）。
- **打包安装程序**：运行 `build_installer.bat`。

---

## 📄 开源协议

基于 **GNU General Public License v3.0 (GPLv3)** 协议分发。详情请参阅 [LICENSE](LICENSE)。
