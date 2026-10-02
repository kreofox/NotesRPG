# NotesRPG

A lightweight desktop notes and file-browser application built with **Qt Widgets (C++)**.

## Features

- 📂 **File tree browser** — navigate folders with an expandable/collapsible tree (VS Code–style), including a smooth expand/collapse animation
- 📝 **Text editor** — open and edit `.txt`/`.md` files in a monospace editor
- 🖼️ **Image preview** — click on `.png`, `.jpg`, `.jpeg`, `.bmp`, `.webp`, or `.gif` files to preview them directly in the app (animated GIFs supported)
- 💾 **Save / New File** — create new notes and save your edits with `Ctrl+N` / `Ctrl+S`
- 📁 **Open Folder** — point the app at any folder on your system via `File → Open Folder...`
- ℹ️ **About dialog** — license and project info, accessible from the `About` menu

## Requirements

- Qt 6 (Widgets module)
- A C++17-compatible compiler
- Qt Creator (recommended) or any CMake/qmake-based build setup

> To enable `.webp` preview support, make sure the `imageformats` Qt module is included:
> ```
> QT += imageformats
> ```

## Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/kreofox/NotesRPG.git
   ```
2. Open the project in **Qt Creator**.
3. Build and run (`Ctrl+R`).

On first launch, the app creates a `notes/` folder next to the executable and opens it automatically. You can switch to any other folder via `File → Open Folder...`.

## Usage

| Action              | Shortcut   |
|---------------------|------------|
| New File            | `Ctrl+N`   |
| Save                | `Ctrl+S`   |
| Exit                | `Ctrl+Q`   |

- Click a **folder** in the tree to expand/collapse it.
- Click a **text file** to open it in the editor.
- Click an **image file** to preview it.