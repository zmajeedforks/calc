# calc1/flexbison.gen/CMakeLists.txt

project(calc1_flexbison_gen)

add_library(flexbisonlib.calc1 STATIC calc1_lexer.flex.cpp calc1_parser.bison.cpp)
target_include_directories(flexbisonlib.calc1 PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/calc1
  $<$<CXX_COMPILER_ID:MSVC>: ${CMAKE_SOURCE_DIR}/thirdparty/flex>
  $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>: ${CMAKE_SOURCE_DIR}/thirdparty/flex>
)

# minimal parser without flex and without semantic actions

add_library(minimalbisonlib.calc1 STATIC calc1_parser.minimal.bison.cpp)
target_include_directories(minimalbisonlib.calc1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/calc1)

