# calc1/CMakeLists.txt

project(calc1_top)

# don't run bison, flex for windows and webassembly builds
if(WIN32 OR EMSCRIPTEN)
  message("Use checked-in pre-generated calc1 bison, flex files from flexbison.gen directory")
  add_subdirectory(flexbison.gen)
else()
  message("Run bison, flex to Generate calc1 parser from grammar directory")
  add_subdirectory(grammar)
endif()

add_subdirectory(parser)
add_subdirectory(lexer)
add_subdirectory(main)

