# 3D Editor Lab

Small C++ OpenGL/GLU/GLUT 3D editor for the lab assignment.

## Build

```bash
make
```

On Debian/Ubuntu, the expected packages are:

```bash
sudo apt install build-essential freeglut3-dev libglu1-mesa-dev
```

## Run

```bash
./editor scene
# or
./editor scene.f3d
```

Scene files are normalized to the `.f3d` extension. `./editor scene` and `./editor scene.f3d` both load and save `scene.f3d`. If another extension is provided, it is replaced with `.f3d`; for example, `./editor scene.txt` uses `scene.f3d`.

If the normalized scene file exists, the scene is loaded. If it does not exist, an initial scene is created and saved.
The terminal always shows the controls menu. After each logged operation, the terminal is cleared, the menu is printed again, and only the latest operation log is shown below it.

## Controls

- `1`: create cube
- `2`: create sphere
- `3`: create torus
- `4`: create teapot
- `Tab`: select next object
- `Shift + Tab`: select previous object
- `t`: translation mode
- `r`: rotation mode
- `s`: scaling mode
- `x` / `y` / `z`: choose the active transform axis for translation and rotation
- Left mouse drag: transform selected object in the active mode; translation and rotation affect only the active axis
- Right mouse drag: rotate camera trackball
- Mouse wheel or `+` / `-`: zoom
- `w`: toggle wireframe mode
- `d`: duplicate selected object
- `Delete` or `Backspace`: delete selected object
- `Ctrl + s`: save scene
- `f`: focus camera on selected object
- Arrow keys: move camera laterally/frontally
- `Page Up` / `Page Down`: move camera vertically
- `[` / `]`: decrease/increase FOV
- `n` / `m`: decrease/increase near plane
- `,` / `.`: decrease/increase far plane
- `Esc`: exit

## Autosave and Logs

Object changes are saved automatically after the operation completes. This includes creating, duplicating, deleting, translating, rotating, and scaling objects. Mouse transform drags autosave once when the drag ends, not on every mouse movement.

Camera, projection, selection, mode, axis, and wireframe changes are logged when applicable but do not autosave. Manual save remains available through `Ctrl + s` and `F1`; `Esc` saves before exiting.

Selected-object information and operation logs are printed in the terminal with id, type, position, rotation, scale, and color. Transform result logs also include the current transform mode and active axis.
