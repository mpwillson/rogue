/* storage.js - file listing, final flush and status messages for the page.
 *
 * Companion to pre.js: options() returns the object passed to
 * initEmscripten; pre.js then attaches storagePersistent and flushStorage
 * to it. Persistence is off when the options contain persist: false, in
 * which case this manager stays silent.
 *
 * Rogue-specific additions: hiddenFiles keeps internal files out of the
 * listing, and linkFor() turns each remaining file into a link that reloads
 * the page with ?restore=<name> so the game can load that saved game.
 */

export function createStorageManager({
  filesElement,
  statusElement,
  displayName,
  linkFor,
  hiddenFiles,
}) {
  let game = null;
  let before = {};
  const moduleOptions = {};
  const hidden = new Set(hiddenFiles || []);

  const enabled = () => moduleOptions.persist !== false;
  const root = () => moduleOptions.persistDir || "/data";

  // Display name -> { raw, key }, so a later snapshot can detect changes
  // and links still know the file's real name. displayName() may map a raw
  // file name for display, or return null to leave the file out.
  const snapshot = () => {
    const snap = {};
    if (!game) return snap;
    try {
      for (const f of game.FS.readdir(root())) {
        if (f === "." || f === ".." || hidden.has(f)) continue;
        const name = displayName ? displayName(f) : f;
        if (!name) continue;
        try {
          const st = game.FS.stat(root() + "/" + f);
          snap[name] = { raw: f, key: st.mtime.getTime() + ":" + st.size };
        } catch (e) {}
      }
    } catch (e) {}
    return snap;
  };

  const render = (snap) => {
    const names = Object.keys(snap).sort();
    filesElement.textContent = names.length ? "Saved games: " : "";
    const frag = document.createDocumentFragment();
    names.forEach((name, i) => {
      if (i) frag.appendChild(document.createTextNode(", "));
      const url = linkFor ? linkFor(snap[name].raw, name) : null;
      if (!url) {
        frag.appendChild(document.createTextNode(name));
        return;
      }
      const a = document.createElement("a");
      a.href = url;
      a.textContent = name;
      a.title = "Continue this saved game";
      frag.appendChild(a);
    });
    filesElement.appendChild(frag);
  };

  const report = (fresh) => {
    if (moduleOptions.storagePersistent === false) {
      statusElement.textContent =
        "Browser storage could not be written - files were not kept this session.";
      return;
    }
    statusElement.textContent =
      fresh.length > 0
        ? "Stored (" + fresh.join(", ") + ") — click a saved game above to continue."
        : "Reload the page to restart.";
  };

  const finish = () => {
    if (!enabled()) return;
    const after = snapshot();
    render(after);
    const fresh = Object.keys(after).filter(
      (n) => !before[n] || before[n].key !== after[n].key
    );
    if (moduleOptions.storagePersistent === false) return report(fresh);

    // A full flush also carries deletions. The program has stopped writing,
    // so it is safe here.
    let tries = 0;
    (function persist() {
      moduleOptions.flushStorage().then(handle, handle);
      function handle(err) {
        if (!err) return report(fresh);
        if (++tries < 20) return setTimeout(persist, 1500);
        console.error("utw: could not persist files:", err);
        moduleOptions.storagePersistent = false;
        report(fresh);
      }
    })();
  };

  return {
    // Pass this exact object to initEmscripten: pre.js attaches
    // flushStorage() and storagePersistent to it.
    options(extra) {
      const opts = Object.assign(moduleOptions, extra);
      if (enabled()) moduleOptions.onExit = finish;
      return opts;
    },

    attach(module) {
      game = module;
      if (!enabled()) return;
      before = snapshot();
      render(before);
      if (moduleOptions.storagePersistent === false)
        statusElement.textContent =
          "Warning: browser storage is unavailable - files will not survive a page reload.";
    },

    fail(error) {
      console.error(error);
      statusElement.textContent =
        "Failed to start the program (" + error + "). See the console for details.";
    },
  };
}
