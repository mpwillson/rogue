/* wasm/rogue-pre.js - Rogue-specific pre-js additions.
 *
 * Linked after the unix-tui-to-wasm pre.js (both are concatenated into the
 * module by --pre-js, in link order; see CMakeLists.txt). Rogue writes its
 * save file, score table and screen dump to the current directory, so run
 * it in the persistent IDBFS mount. The read-only preloaded data files
 * (rogue.termcap, rogue.instr) stay at the virtual filesystem root.
 *
 * NOTE: --pre-js content shares the module scope, so `Module`, `FS` and
 * `ENV` are already in scope here. Do not redeclare them.
 */
Module["preRun"] = Module["preRun"] || [];
Module["preRun"].push(function () {
  // Emscripten defaults USER/LOGNAME to "web_user"; use the page's name.
  if (typeof ENV !== "undefined") {
    ENV.USER = Module["playerName"] || "player";
    ENV.LOGNAME = ENV.USER;
    // There is no login environment in the browser.
    ENV.TERM = "xterm-256color";
    ENV.HOME = "/data";
    // Suggest a name in the save prompt (the "file name?" default is empty).
    ENV.ROGUEOPTS = "file=rogue.save";
    // Rogue's curses reads the termcap file named by $TERMCAP (max 40
    // characters) before falling back to /etc/termcap.
    ENV.TERMCAP = "/rogue.termcap";
  }

  // The template creates this directory before mounting IDBFS; create it
  // here too in case persistence is disabled.
  try {
    FS.mkdir("/data");
  } catch (e) {}
  try {
    FS.chdir("/data");
  } catch (e) {
    console.error("rogue: cannot use /data as the working directory:", e);
  }
});
