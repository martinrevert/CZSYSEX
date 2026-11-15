# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Langue :** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · **Français**

Application bureautique native **C++20** avec interface accélérée matériellement (**DirectX 11 + Dear ImGui**) pour le synthétiseur **Casio CZ-3000** (compatible avec toute la famille Casio CZ : CZ-101, CZ-1000, CZ-3000, CZ-5000 et CZ-1).

Porté et enrichi à partir du projet Python `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Fonctionnalités Principales

1. **Simulation Fidèle du Matériel Casio CZ-3000** :
   - Châssis rétro gris anthracite avec touches classiques Casio.
   - **Écran LCD 16×2 HD44780** avec curseur clignotant et pages d'édition réelles (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Boutons Tactiles avec LED** et **Clavier Virtuel 6 Octaves (C1–C7)** pour l'écoute MIDI en temps réel.

2. **Graphique Interactif d'Enveloppes à 8 Étapes** :
   - Affichage et édition en direct des courbes **DCO** (hauteur), **DCW** (brillance/PD) et **DCA** (volume), séparément ou superposées (`ALL`), avec repères `SUS`/`END` et curseurs `Rate`/`Level` (0–99).

3. **Gestionnaire Double Banque 64 Voix (32 Presets + 32 Mémoires Utilisateur)** :
   - Onglets **PRESET BANK** et **MEMORY BANK** (`*MEMORY* A-1`, renommage personnalisé par clic droit conservé en `.syx` CZ-1 296 octets).
   - **Téléchargement par lot & Cache des Presets**, plus **Auto-Fetch SysEx** à la sélection.
   - **Générateur Aléatoire (`Aléatoire` / `Randomize`)** : Tire au sort tous les paramètres du synthétiseur dans l'emplacement Memory actif et l'envoie en direct vers le tampon d'édition (`0x60`).

4. **Interface Multilingue, Guide PD & Manuel Officiel (8 Langues)** :
   - Changement de langue instantané (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**).
   - **Mode Tutoriel**, **Guide de Distorsion de Phase** et **Manuel Officiel Casio CZ-3000** intégré (9 chapitres, tableau des 32 Presets, `INITIALIZE` et Tableau d'Implémentation MIDI).

5. **Moteur MIDI Natif Basse Latence (`winmm.dll`)** :
   - Sélection des ports MIDI In/Out et messages SysEx Casio officiels de 263/264 octets.

---

## 🛠️ Compilation et Exécution

- **Prérequis** : Windows 10/11 (x64), Visual Studio 2022 (C++20), CMake 3.20+, Git (`--recursive`), NSIS 3.x (optionnel).
- **Compilation et tests** : `build.bat` (génère `build\Release\CZ3000SysEx.exe` et exécute `test_sysex.exe`).
- **Installateur** : `build_installer.bat`.

---

## 📄 Licence

Distribué sous licence **GNU General Public License v3.0 (GPLv3)**. Voir [LICENSE](LICENSE).
