# calc1/grammar/CMakeLists.txt

project(calc1_grammar)

find_package(FLEX)
find_package(BISON)

# generated source filename should match .l filename
flex_target(flex_files calc1_lexer.flex.l
  ${CMAKE_CURRENT_BINARY_DIR}/calc1_lexer.flex.cpp
  COMPILE_FLAGS "-f --nounistd"
)

# generated source filename should match .y filename
bison_target(bison_files calc1_parser.bison.y
  ${CMAKE_CURRENT_BINARY_DIR}/calc1_parser.bison.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/calc1_parser.bison.h
  COMPILE_FLAGS "-Wall -Wdangling-alias -Werror --report lookaheads,cex,solved --report-file calc1_grammar_report.txt"
)

# makes bison_files depend on flex_files
add_flex_bison_dependency(flex_files bison_files)

add_library(flexbisonlib.calc1 STATIC ${FLEX_flex_files_OUTPUTS} ${BISON_bison_files_OUTPUT_SOURCE})

target_include_directories(flexbisonlib.calc1 PUBLIC ${CMAKE_SOURCE_DIR}/calc1 ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(flexbisonlib.calc1 PUBLIC
  $<$<PLATFORM_ID:Darwin>: ${FLEX_INCLUDE_DIRS}>
)


# copy generated files and bison grammar report to source tree
add_custom_command(TARGET flexbisonlib.calc1 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_grammar_report.txt ${CMAKE_CURRENT_SOURCE_DIR})

set(GEN_SRC_DIR ${CMAKE_SOURCE_DIR}/calc1/flexbison.gen)

add_custom_command(TARGET flexbisonlib.calc1 POST_BUILD
  COMMENT "Copy generated parser, lexer and bison grammar report to source tree"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_grammar_report.txt ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_parser.bison.h ${GEN_SRC_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_parser.bison.cpp ${GEN_SRC_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_locations.bison.h ${GEN_SRC_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_lexer.flex.cpp ${GEN_SRC_DIR}
)

# damn annoying mac
if(NOT APPLE)
  set(OPT_I -i)
else()
  set(OPT_I "-i \"\"")
endif()

add_custom_command(TARGET flexbisonlib.calc1 POST_BUILD
  COMMENT "Strip absolute local paths from generated bison, flex files"
  COMMAND sed -E ${OPT_I} 's!${CMAKE_CURRENT_BINARY_DIR}!${GEN_SRC_DIR}!' ${GEN_SRC_DIR}/calc1_parser.bison.h
  COMMAND sed -E ${OPT_I} 's!${CMAKE_SOURCE_DIR}!.!' ${GEN_SRC_DIR}/calc1_parser.bison.h
  COMMAND sed -E ${OPT_I} 's!${CMAKE_CURRENT_BINARY_DIR}!${GEN_SRC_DIR}!' ${GEN_SRC_DIR}/calc1_parser.bison.cpp
  COMMAND sed -E ${OPT_I} 's!${CMAKE_SOURCE_DIR}!.!' ${GEN_SRC_DIR}/calc1_parser.bison.cpp
  COMMAND sed -E ${OPT_I} 's!${CMAKE_CURRENT_BINARY_DIR}!${GEN_SRC_DIR}!' ${GEN_SRC_DIR}/calc1_locations.bison.h
  COMMAND sed -E ${OPT_I} 's!${CMAKE_SOURCE_DIR}!.!' ${GEN_SRC_DIR}/calc1_locations.bison.h
  COMMAND sed -E ${OPT_I} 's!${CMAKE_CURRENT_BINARY_DIR}!${GEN_SRC_DIR}!' ${GEN_SRC_DIR}/calc1_lexer.flex.cpp
  COMMAND sed -E ${OPT_I} 's!${CMAKE_SOURCE_DIR}!.!' ${GEN_SRC_DIR}/calc1_lexer.flex.cpp
)

add_custom_command(TARGET flexbisonlib.calc1 POST_BUILD
  COMMENT "Verbatim strip absolute local paths from generated bison, flex files"
  COMMAND sed -E ${OPT_I} [[s/(YY_YY).*(_CALC1_PARSER_BISON_H)/\1\2/]] ${GEN_SRC_DIR}/calc1_parser.bison.h
  COMMAND sed -E ${OPT_I} [[s/(YY_YY).*(_LOCATIONS_BISON_H)/\1\2/]] ${GEN_SRC_DIR}/calc1_locations.bison.h
  VERBATIM
)

# minimal parser without flex and without semantic actions

bison_target(minimal_bison_files calc1_parser.minimal.bison.y
  ${CMAKE_CURRENT_BINARY_DIR}/calc1_parser.minimal.bison.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/calc1_parser.minimal.bison.h
  COMPILE_FLAGS "-Wall -Wdangling-alias -Werror --report lookaheads,cex,solved --report-file calc1_grammar_report.minimal.txt"
)
add_library(minimalbisonlib.calc1 STATIC ${BISON_minimal_bison_files_OUTPUT_SOURCE})
target_include_directories(minimalbisonlib.calc1 PUBLIC ${CMAKE_SOURCE_DIR}/calc1 ${CMAKE_CURRENT_BINARY_DIR})


# copy generated files and bison grammar report to source tree
add_custom_command(TARGET minimalbisonlib.calc1 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_grammar_report.minimal.txt ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET minimalbisonlib.calc1 POST_BUILD
  COMMENT "Copy generated minimal parser, bison grammar report to source tree"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_grammar_report.minimal.txt ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_parser.minimal.bison.h ${GEN_SRC_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_parser.minimal.bison.cpp ${GEN_SRC_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/calc1_locations.minimal.bison.h ${GEN_SRC_DIR}
)

add_custom_command(TARGET minimalbisonlib.calc1 POST_BUILD
  COMMENT "Strip absolute local paths from generated minimal bison files"
  COMMAND sed -E ${OPT_I} 's!${CMAKE_CURRENT_BINARY_DIR}!${GEN_SRC_DIR}!' ${GEN_SRC_DIR}/calc1_parser.minimal.bison.h
  COMMAND sed -E ${OPT_I} 's!${CMAKE_SOURCE_DIR}!.!' ${GEN_SRC_DIR}/calc1_parser.minimal.bison.h
  COMMAND sed -E ${OPT_I} 's!${CMAKE_CURRENT_BINARY_DIR}!${GEN_SRC_DIR}!' ${GEN_SRC_DIR}/calc1_parser.minimal.bison.cpp
  COMMAND sed -E ${OPT_I} 's!${CMAKE_SOURCE_DIR}!.!' ${GEN_SRC_DIR}/calc1_parser.minimal.bison.cpp
  COMMAND sed -E ${OPT_I} 's!${CMAKE_CURRENT_BINARY_DIR}!${GEN_SRC_DIR}!' ${GEN_SRC_DIR}/calc1_locations.minimal.bison.h
  COMMAND sed -E ${OPT_I} 's!${CMAKE_SOURCE_DIR}!.!' ${GEN_SRC_DIR}/calc1_locations.minimal.bison.h
)

add_custom_command(TARGET minimalbisonlib.calc1 POST_BUILD
  COMMENT "Verbatim strip absolute local paths from generated minimal bison files"
  COMMAND sed -E ${OPT_I} [[s/(YY_YY).*(_CALC1_PARSER_MINIMAL_BISON_H)/\1\2/]] ${GEN_SRC_DIR}/calc1_parser.minimal.bison.h
  COMMAND sed -E ${OPT_I} [[s/(YY_YY).*(_LOCATIONS_MINIMAL_BISON_H)/\1\2/]] ${GEN_SRC_DIR}/calc1_locations.minimal.bison.h
  VERBATIM
)

