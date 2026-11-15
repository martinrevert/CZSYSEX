# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **言語:** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · **日本語** · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

**Casio CZ-3000** シンセサイザー向けのハードウェアアクセラレーション対応（**DirectX 11 + Dear ImGui**）ネイティブ **C++20** デスクトップアプリケーションです（CZ-101 / CZ-1000 / CZ-3000 / CZ-5000 / CZ-1 対応）。

Python 製プロジェクト `kasploosh` (`czsysexy` / `czcommon`) をベースに移植・拡張しています。

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 主な機能

1. **Casio CZ-3000 ハードウェアの忠実な再現**:
   - チャコールグレーの筐体と Casio 伝統のカラーアクセント。
   - **16×2 HD44780 LCD ディスプレイ**：点滅カーソルと実機同様の編集ページ（`Play` / `Line` / `Vibrato` / `Detune` / `DCO` / `DCW` / `DCA`）。
   - **LED 付きパネルボタン**と **6オクターブ・バーチャルキーボード（C1〜C7）**によるリアルタイム MIDI 試聴。

2. **インタラクティブ 8ステップ・エンベロープグラフ**:
   - **DCO**（音程）・**DCW**（明るさ/PD）・**DCA**（音量）を個別または重ね合わせ（`ALL`）で表示・編集。`SUS` / `END` マーカーと `Rate`/`Level`（0〜99）スライダー搭載。

3. **64音色 デュアルバンク・ライブラリアン（32プリセット + 32ユーザーメモリー）**:
   - **PRESET BANK** と **MEMORY BANK**（`*MEMORY* A-1` 表記、右クリックでカスタム名変更・CZ-1 296バイト `.syx` 保存対応）。
   - **32音色一括ダウンロード & プリセットキャッシュ**、および選択時の **Auto-Fetch SysEx**。
   - **ランダマイザー（`ランダム` / `Randomize`）**：現在のメモリー枠の全パラメーターをランダム生成し、編集バッファ（`0x60`）へ即時送信します。

4. **8言語対応 UI・PD 解説ガイド・公式取扱説明書ビュー**:
   - **English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français** を即時切り替え（CJK フォント対応）。
   - **チュートリアルモード**、**フェイズディストーション解説ガイド**、および全9章＋32プリセット表・`INITIALIZE` 表・MIDI インプリメンテーション表を備えた **CZ-3000 公式取扱説明書**を内蔵。

5. **低遅延ネイティブ MIDI エンジン（`winmm.dll`）**:
   - MIDI In/Out の切り替えと公式 263/264 バイト Casio SysEx 送受信に対応。

---

## 🛠️ ビルドと実行

- **動作環境**: Windows 10/11 (x64), Visual Studio 2022 (C++20), CMake 3.20+, Git (`--recursive`), NSIS 3.x (任意)
- **ビルド & テスト**: `build.bat`（`build\Release\CZ3000SysEx.exe` と `test_sysex.exe` を生成・実行）
- **インストーラー作成**: `build_installer.bat`

---

## 📄 ライセンス

**GNU General Public License v3.0 (GPLv3)** の下で配布されています。詳細は [LICENSE](LICENSE) をご覧ください。
