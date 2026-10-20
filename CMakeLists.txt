cmake_minimum_required(VERSION 3.16)
project(rogue C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(ROGUE_SOURCES
    curses.c hit.c init.c instruct.c inventory.c level.c machdep.c
    main.c message.c monster.c move.c object.c pack.c play.c random.c
    ring.c room.c save.c score.c spec_hit.c throw.c trap.c use.c zap.c)

add_executable(rogue ${ROGUE_SOURCES})
target_include_directories(rogue PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Rogue ships its own termcap-based curses emulation. Using it everywhere
# keeps the native and browser builds on the same code path.
target_compile_definitions(rogue PRIVATE RCURSES)

if(EMSCRIPTEN)
  # --- browser terminal infrastructure ---------------------------------
  # The shared runtime is the external/unix-tui-to-wasm submodule. See
  # https://github.com/neuhaus/unix-tui-to-wasm.
  set(UTW_ROOT
      "${CMAKE_CURRENT_SOURCE_DIR}/external/unix-tui-to-wasm/wasm")
  if(NOT EXISTS "${UTW_ROOT}/utw.cmake")
    message(FATAL_ERROR
      "unix-tui-to-wasm submodule is missing; run "
      "'git submodule update --init --recursive'")
  endif()
  set(UTW_TITLE "Rogue 9.8.6")
  set(UTW_COLS 80)
  set(UTW_ROWS 24)
  set(UTW_PERSIST ON)
  set(UTW_PERSIST_DIR "/data")
  # rogue.termcap and rogue.instr are read-only and stay at the FS root;
  # scores and saved games are written to the persistent /data mount.
  # The preload directory is generated so rogue.instr is not duplicated:
  # it is taken from the original and converted from CRLF to LF.
  set(ROGUE_PRELOAD "${CMAKE_BINARY_DIR}/preload")
  file(MAKE_DIRECTORY "${ROGUE_PRELOAD}")
  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/rogue.instr" _instr)
  string(REPLACE "\r\n" "\n" _instr "${_instr}")
  file(WRITE "${ROGUE_PRELOAD}/rogue.instr" "${_instr}")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/wasm/rogue.termcap"
                 "${ROGUE_PRELOAD}/rogue.termcap" COPYONLY)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
               "${CMAKE_CURRENT_SOURCE_DIR}/rogue.instr")
  set(UTW_PRELOAD_DIR "${ROGUE_PRELOAD}")
  # Rogue customizes the page, storage manager and pre-js. UTW_WEB_FILES is
  # staged after the runtime's storage.js, replacing it in the bundle.
  set(UTW_PAGE "${CMAKE_CURRENT_SOURCE_DIR}/wasm/index.html.in")
  set(UTW_WEB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/wasm/storage.js")
  set(UTW_PRE_JS_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/wasm/rogue-pre.js")

  include("${UTW_ROOT}/utw.cmake")
  utw_enable(rogue)
  # Relink (and rebuild rogue.data) when the generated help file changes.
  set_property(TARGET rogue APPEND PROPERTY LINK_DEPENDS
               "${ROGUE_PRELOAD}/rogue.instr")
endif()
