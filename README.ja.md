# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **言語:** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · **日本語** · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

**Casio CZ-3000** シンセサイザー向けのハードウェアアクセラレーション対応（**DirectX 11 + Dear ImGui**）ネイティブ **C++20** デスクトップアプリケーションです（Casio CZ シリーズ全機種：CZ-101、CZ-1000、CZ-3000、CZ-5000、CZ-1 に対応）。

Python 製プロジェクト `kasploosh` (`czsysexy` / `czcommon`) を移植・拡張しています。

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 主な機能

1. **Casio CZ-3000 ハードウェアの忠実なシミュレーション**:
   - 伝統的な Casio のアクセント（シアン、濃紺、レッド）を備えたチャコールグレーのレトロ筐体。
   - **16×2 HD44780 LCD ディスプレイ**: 5×7 ドットマトリクス蛍光グリーン表示、点滅カーソル、実機同様の編集ページ（`Play`、`Line`、`Vibrato`、`Detune`、`DCO`、`DCW`、`DCA`）。
   - **タクタイル Casio パネルボタン＆ステータス LED**:
     - *Programmer*: `COMPARE/RECALL`、`INIT`、`WRITE`。
     - *Parameter Select*: `PLAY`、`LINE`、`VIB`、`DETUNE`、`DCO`、`DCW`、`DCA`。
     - *Voice Lines*: Line 1 と Line 2。
     - *Data Entry*: `UP`、`DOWN`、`< CUR`、`CUR >`。
     - *Banks & Voices*: Preset / Memory バンク `A`〜`D`、スロット `1`〜`8`（計64音色）。
   - **6オクターブ・ライブ試聴キーボード**: リアルタイム MIDI 試聴が可能なインタラクティブ仮想ピアノ（**C1**〜**C7**、73鍵）。

2. **インタラクティブ 8ステップ・エンベロープグラフ**:
   - **DCO**（音程・シアン）、**DCW**（明るさ/PD・アンバー）、**DCA**（音量・ライムグリーン）のカーブを個別または重ね合わせ（`ALL`）でリアルタイム表示・編集。
   - `SUS`（サスティン）と `END`（エンド）の視覚マーカー。
   - 全8ステップの `Rate`（0〜99）と `Level`（0〜99）スライダー。

3. **64音色 デュアルバンク・ライブラリアン（32プリセット + 32ユーザーメモリー）**:
   - **PRESET BANK（32固定）** と **MEMORY BANK（32ユーザー）** タブ。実機準拠の `*MEMORY* A-1` 表記と右クリックによるカスタム名変更（CZ-1 296バイト `.syx` で保存・復元可能）。
   - **一括ダウンロード＆プリセットキャッシュ**: ワンクリックで32プリセット（ローカルキャッシュ対応）または32メモリーを一括取得。
   - **Auto-Fetch SysEx**: プリセットまたはメモリーをクリックした際に実機から自動で SysEx ダンプを要求。
   - **パッチランダマイザー（`ランダム` / `Randomize`）**: 現在のメモリー枠の全シンセパラメーター（`Line Select`、`Octave`、`Detune`、`Vibrato`、波形、変調、8段階 DCO/DCW/DCA エンベロープ）をランダム生成し、編集バッファ（`0x60`）へ即時送信。
   - 編集バッファ（`0x60`）への送信、実機メモリーへの書き込み、32音色バンク一括送信、スロット間コピー/ペースト、`.syx` ファイルの読み書きに対応。

4. **多言語 UI・インタラクティブ PD ガイド・公式取扱説明書（8言語対応）**:
   - 実行中の即時言語切り替え（**English**、**Español**、**Italiano**、**Deutsch**、**日本語**、**简体中文**、**Português (Brasil)**、**Français**）、CJK フォント対応。
   - **チュートリアルモード（`[?] チュートリアル: ON/OFF`）**: ホバーツールチップと、DCO・DCW・DCA・8ステップエンベロープ・波形・リング/ノイズ変調・SysEx を解説する **フェーズディストーション解説ガイド**。
   - **CZ-3000 公式取扱説明書ビュー**: 全9章のリファレンス（`表示 → CZ-3000 取扱説明書`）、32プリセット一覧表、`INITIALIZE` 初期値表、MIDI インプリメンテーションチャートを8言語で内蔵。

5. **低遅延ネイティブ MIDI エンジン＆ VST3 プラグイン (DAW / Ableton Live)**:
   - Windows Multimedia API（`winmm.dll`）を使用し、MIDI In/Out ポートのホットスワップおよび公式 263/264 バイト Casio SysEx フレーミングに対応。
   - **VST3 MIDI プラグイン (`CZ3000SysEx.vst3`)**: `C:\Program Files\Common Files\VST3` にインストール可能で、DAW（Ableton Live、Cubase、Reaper 等）の MIDI トラックから 64 音色すべての切り替え、バンク同期、DAW プロジェクト状態の保存に対応。

---

## 🛠️ ビルドと実行

### 必要環境
- Windows 10 または 11 (x64)
- Visual Studio 2022 / Build Tools（C++20 対応）
- CMake 3.20 以上
- サブモジュール対応の Git（`git clone --recursive` または `git submodule update --init --recursive`）
- *(任意)* Windows インストーラー作成用 NSIS 3.x

### クイックビルド＆テスト
```bat
build.bat
```
`build\Release\CZ3000SysEx.exe`、`build\Release\CZ3000SysEx.vst3` と `build\Release\test_sysex.exe` を生成し、すべてのコアユニットテストを実行します。

### インストーラー作成
```bat
build_installer.bat
```

---

## 📁 プロジェクト構成

```
CZSYSEX/
├── docs/                  # スクリーンショット・ドキュメント素材
├── external/
│   └── imgui/             # Dear ImGui サブモジュール + Win32/DX11 バックエンド
├── installer/             # NSIS インストーラースクリプト
├── src/
│   ├── core/              # データモデル、数値計算、i18n、SysEx コーデック、バンクキャッシュ
│   ├── midi/              # Windows Multimedia (WinMM) MIDI ドライバー
│   ├── ui/                # LCD、チュートリアル、取扱説明書、エンベロープ、ライブラリアン、パネル
│   ├── vst3/              # VST3 プラグイン プロセッサ、ビュー、エントリポイント
│   └── main.cpp           # Win32 ウィンドウ、DirectX 11、メインループ
├── tests/
│   └── test_sysex.cpp     # SysEx・バンク・キャッシュのユニットテスト
├── AGENTS.md              # 開発者・CI 検証ガイドライン
├── CMakeLists.txt
├── build.bat
└── build_installer.bat
```

---

## 📄 ライセンス

**GNU General Public License v3.0 (GPLv3)** の下で配布されています。詳細は [LICENSE](LICENSE) をご覧ください。
