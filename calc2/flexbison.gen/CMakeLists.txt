# calc2/flexbison.gen/CMakeLists.txt

project(calc2_flexbison_gen)

add_library(flexbisonlib.calc2 STATIC calc2_lexer.flex.cpp calc2_parser.bison.cpp)
target_include_directories(flexbisonlib.calc2 PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/calc2
  $<$<CXX_COMPILER_ID:MSVC>: ${CMAKE_SOURCE_DIR}/thirdparty/flex>
  $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>: ${CMAKE_SOURCE_DIR}/thirdparty/flex>
)

# minimal parser without flex and without semantic actions

add_library(minimalbisonlib.calc2 STATIC calc2_parser.minimal.bison.cpp)
target_include_directories(minimalbisonlib.calc2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/calc2)

