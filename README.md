# cca vibe jam

A [raylib](https://www.raylib.com/) game skeleton in C. The same source builds as a native desktop app and as a WebAssembly page that gets published to GitHub Pages.

## Layout

```
src/main.c                 game code (desktop + web)
assets/                    images, sounds, fonts... loaded as "assets/<file>"
web/shell.html             HTML page the web build is wrapped in
CMakeLists.txt             fetches raylib 6.2 and builds the game
.github/workflows/web.yml  CI: web build on every push, deploy to Pages from the default branch
```

## Desktop build (fastest dev loop)

Requires CMake 3.20+ and a C compiler. On Linux you also need the X11/GL headers:
`sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`.

```bash
cmake -S . -B build
cmake --build build
./build/game
```

## Web build

Install the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) once. CI uses 5.0.3:

```bash
git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
~/emsdk/emsdk install 5.0.3 && ~/emsdk/emsdk activate 5.0.3
source ~/emsdk/emsdk_env.sh
```

Then build and serve it (opening `index.html` straight from disk won't work, browsers block loading the `.wasm` over `file://`):

```bash
emcmake cmake -S . -B build-web
cmake --build build-web
python3 -m http.server -d build-web 8080
```

Open http://localhost:8080.

## Publishing to GitHub Pages

One-time setup: in the repo go to **Settings → Pages → Build and deployment → Source** and pick **GitHub Actions**.

After that, every push to the default branch builds the web version and deploys it to `https://<user>.github.io/<repo>/`. Pushes to other branches and PRs only build, so a broken web build shows up before it is merged. The built site is also attached to each workflow run as the `github-pages` artifact.

## Web gotchas

- **Main loop:** the browser owns the loop. `main.c` hands `UpdateDrawFrame` to `emscripten_set_main_loop` on the web and uses a normal `while (!WindowShouldClose())` loop on desktop. Keep all per-frame work inside `UpdateDrawFrame`.
- **Assets:** everything in `assets/` is packed into `index.data` and mounted at `/assets`, so `LoadTexture("assets/foo.png")` works on both platforms. Writing files at runtime only goes to an in-memory filesystem on the web.
- **Audio:** browsers keep audio muted until the user clicks or presses a key, so start music after the first input.
- **Graphics:** the web build uses WebGL (OpenGL ES 2.0). Custom shaders need `#version 100` GLSL ES variants.
