#!/bin/sh
# wasm/build-wasm.sh - build and stage the servable browser bundle.
#
# Builds the CMake project at ROOT (default: this script's parent
# directory) and stages a self-contained bundle into WEB_DIR. Set ROOT
# when invoking the script from a wrapper outside the project tree.
#
# Needs: the external/unix-tui-to-wasm submodule, emsdk (emcc + emcmake on
# PATH) in the pinned version from the submodule's emsdk-version file, cmake,
# a host C compiler (cc), curl and network access for the pinned browser
# assets below.
set -eu

ROOT="${ROOT:-$(cd "$(dirname "$0")/.." && pwd)}"
WEB_DIR="${WEB_DIR:-$ROOT/build/web}"        # the servable bundle
OBJ_DIR="${OBJ_DIR:-$ROOT/build/emscripten}" # CMake working dir
UTW_DIR="$ROOT/external/unix-tui-to-wasm"
if [ ! -f "$UTW_DIR/wasm/utw.cmake" ]; then
  echo "error: unix-tui-to-wasm submodule is missing." >&2
  echo "  run: git submodule update --init --recursive" >&2
  exit 1
fi
EMSDK_VERSION="$(cat "$UTW_DIR/wasm/emsdk-version")"

if ! command -v emcc >/dev/null 2>&1; then
  echo "error: emcc not found. Install the pinned Emscripten SDK ($EMSDK_VERSION) first:" >&2
  echo "  git clone https://github.com/emscripten-core/emsdk.git" >&2
  echo "  cd emsdk && ./emsdk install $EMSDK_VERSION && ./emsdk activate $EMSDK_VERSION" >&2
  echo "  source ./emsdk_env.sh" >&2
  exit 1
fi

EMCC_VERSION="$(emcc --version | head -n 1 |
  grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n 1)"
if [ "$EMCC_VERSION" != "$EMSDK_VERSION" ]; then
  if [ "${ALLOW_UNTESTED_EMSDK:-0}" != "1" ]; then
    echo "error: emcc $EMCC_VERSION is active, but the build is pinned to $EMSDK_VERSION." >&2
    echo "  Install/activate $EMSDK_VERSION (see README.md), or set" >&2
    echo "  ALLOW_UNTESTED_EMSDK=1 to build with an untested version." >&2
    exit 1
  fi
  echo "warning: using untested Emscripten $EMCC_VERSION (pinned: $EMSDK_VERSION)" >&2
else
  echo "Using Emscripten $EMCC_VERSION"
fi

# Pinned browser assets (@xterm/xterm + xterm-pty, both MIT): fetched once,
# checksum-verified, skipped if a good copy is already present. The bundle
# references only these local files, so the page works offline at runtime.
fetch() { # URL DEST SHA256
  if [ -f "$2" ] && [ "$(sha256sum "$2" | cut -d' ' -f1)" = "$3" ]; then
    return 0
  fi
  if ! curl -fsSL "$1" -o "$2"; then
    echo "error: failed to fetch $1" >&2
    exit 1
  fi
  if [ "$(sha256sum "$2" | cut -d' ' -f1)" != "$3" ]; then
    echo "error: checksum mismatch for $2" >&2
    exit 1
  fi
}

mkdir -p "$WEB_DIR" "$OBJ_DIR"
XTERM=https://unpkg.com/@xterm/xterm@5.5.0
PTY=https://unpkg.com/xterm-pty@0.12.0
fetch "$XTERM/lib/xterm.js"    "$WEB_DIR/xterm.js"       1f991ac3b4b283ebf96e60ae23a00a52765dd3a2e46fa6fdda9f1aab032f7495
fetch "$XTERM/css/xterm.css"   "$WEB_DIR/xterm.css"      ba8e6985669488981ccf40c0cefe3aba80722cb6c92de7ad628b0bd717faf2b6
fetch "$PTY/index.mjs"         "$WEB_DIR/xterm-pty.js"   9d1ab8c5f3af23ddc835f9cf61f3db7097ad338e4655ae20a7dc263adee552e2
# The submodule's utw.cmake references this gitignored glue file.
fetch "$PTY/emscripten-pty.js" "$UTW_DIR/wasm/emscripten-pty.js" 93d474cb4141461145c6a01aa76bbe6f75313b562051519c8bcd21d367125315

# The default build includes the <target>_web CMake target, which stages
# the runtime files into WEB_DIR; the fetched assets are already there.
emcmake cmake -S "$ROOT" -B "$OBJ_DIR" -DCMAKE_BUILD_TYPE=Release \
  -DUTW_WEB_DIR="$WEB_DIR"
cmake --build "$OBJ_DIR"

echo
echo "Built: $WEB_DIR (self-contained; serve it and open /):"
echo "  cd $WEB_DIR && python3 -m http.server 8000"
echo "  -> http://localhost:8000/"
