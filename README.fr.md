# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Langue :** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · **Français**

Application bureautique native moderne en **C++20** avec interface accélérée matériellement (**DirectX 11 + Dear ImGui**) pour le synthétiseur **Casio CZ-3000** (compatible avec toute la famille Casio CZ : CZ-101, CZ-1000, CZ-3000, CZ-5000 et CZ-1).

Porté et enrichi à partir du projet Python `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Fonctionnalités Principales

1. **Simulation Authentique du Matériel Casio CZ-3000** :
   - Châssis rétro gris anthracite avec accents classiques Casio (cyan, bleu foncé et rouge).
   - **Écran LCD 16×2 HD44780** : Matrice 5×7 vert phosphore avec curseur clignotant et pages d'édition réelles (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Boutons Tactiles Casio & LEDs d'État** :
     - *Programmer* : `COMPARE/RECALL`, `INIT`, `WRITE`.
     - *Sélection de Paramètre* : `PLAY`, `LINE`, `VIB`, `DETUNE`, `DCO`, `DCW`, `DCA`.
     - *Lignes de Voix* : Ligne 1 et Ligne 2.
     - *Data Entry* : `UP`, `DOWN`, `< CUR`, `CUR >`.
     - *Banques & Voix* : Banques Preset et Memory `A`–`D` et Emplacements `1`–`8` (64 voix au total).
   - **Clavier d'Écoute en Direct 6 Octaves** : Piano virtuel interactif (**C1** à **C7**, 73 touches) pour l'écoute MIDI en temps réel.

2. **Graphique Interactif d'Enveloppes à 8 Étapes** :
   - Visualisation et édition en temps réel des courbes **DCO** (hauteur, cyan), **DCW** (brillance/PD, ambre) et **DCA** (volume, vert lime), séparément ou superposées (`ALL`).
   - Repères visuels `SUS` (Sustain) et `END` (Fin).
   - Curseurs `Rate` (0–99) et `Level` (0–99) pour les 8 étapes.

3. **Gestionnaire Double Banque 64 Voix (32 Presets + 32 Mémoires Utilisateur)** :
   - Onglets **PRESET BANK (32 Fixes)** et **MEMORY BANK (32 Utilisateur)** avec nommage fidèle `*MEMORY* A-1` et renommage personnalisé par clic droit (conservé via export/import `.syx` CZ-1 de 296 octets).
   - **Téléchargement par Lot & Cache des Presets** : Téléchargez les 32 Presets (mis en cache localement) ou les 32 Mémoires en un clic.
   - **Auto-Fetch SysEx** : Demande automatiquement le contenu de l'emplacement sélectionné au synthétiseur lors d'un clic sur un Preset ou une Mémoire.
   - **Générateur Aléatoire de Patch (`Aléatoire` / `Randomize`)** : Tire au sort tous les paramètres du synthétiseur (`Line Select`, `Octave`, `Detune`, `Vibrato`, formes d'onde, modulation et enveloppes à 8 étapes DCO/DCW/DCA) dans la mémoire active et l'envoie en direct vers le tampon d'édition (`0x60`).
   - Envoi vers le Tampon d'Édition (`0x60`), Écriture en Mémoire Synthé, Envoi Banque Complète de 32 patches, Copier/Coller entre emplacements et ouverture/sauvegarde de fichiers `.syx`.

4. **Interface Multilingue, Guide PD Interactif & Manuel Officiel (8 Langues)** :
   - Changement de langue à la volée (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**) avec support des polices CJK.
   - **Mode Tutoriel (`[?] MODE TUTORIEL : ON/OFF`)** : Info-bulles au survol et fenêtre interactive **Guide de Distorsion de Phase** expliquant DCO, DCW, DCA, enveloppes à 8 étapes, ondes, modulation Ring/Noise et SysEx.
   - **Vue Manuel Officiel CZ-3000** : Manuel de référence intégré en 9 chapitres (`Vues → Manuel officiel CZ-3000`) avec tableau des 32 Presets, valeurs `INITIALIZE` et Tableau d'Implémentation MIDI dans les 8 langues.

5. **Moteur MIDI Natif Basse Latence** :
   - Basé sur l'API multimédia native de Windows (`winmm.dll`) avec sélection à chaud des ports MIDI In/Out et trames SysEx officielles Casio de 263/264 octets.

---

## 🛠️ Compilation et Exécution

### Prérequis
- Windows 10 ou 11 (x64)
- Visual Studio 2022 / Build Tools avec support C++20
- CMake 3.20+
- Git avec sous-modules (`git clone --recursive` ou `git submodule update --init --recursive`)
- *(Optionnel)* NSIS 3.x pour générer l'installateur Windows

### Compilation rapide et Tests
```bat
build.bat
```
Génère `build\Release\CZ3000SysEx.exe` et `build\Release\test_sysex.exe`, puis exécute tous les tests unitaires.

### Générer l'Installateur
```bat
build_installer.bat
```

---

## 📁 Structure du Projet

```
CZSYSEX/
├── docs/                  # Captures d'écran et ressources documentaires
├── external/
│   └── imgui/             # Sous-module Dear ImGui + backends Win32/DX11
├── installer/             # Script d'installation NSIS
├── src/
│   ├── core/              # Modèle de données, maths, i18n, codec SysEx et cache banque
│   ├── midi/              # Pilote MIDI Windows Multimedia (WinMM)
│   ├── ui/                # LCD, Tutoriel, Manuel, Graphique d'Enveloppes, Librarian et Panneau
│   └── main.cpp           # Fenêtre Win32, DirectX 11 et boucle principale
├── tests/
│   └── test_sysex.cpp     # Tests unitaires SysEx, banque et cache
├── AGENTS.md              # Recommandations contributeurs et vérification CI
├── CMakeLists.txt
├── build.bat
└── build_installer.bat
```

---

## 📄 Licence

Distribué sous licence **GNU General Public License v3.0 (GPLv3)**. Voir [LICENSE](LICENSE) pour plus de détails.
