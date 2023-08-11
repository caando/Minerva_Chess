set(
        source_files
        nnue/nnue.h nnue/nnue.cpp
        nnue/misc.h nnue/misc.cpp
        types.h
        magic_constants.h
        attack.h attack.cpp
        perft.h perft.cpp
        movegen.h movegen.cpp
        eval.h eval.cpp
        search.h search.cpp
        transposition.h transposition.cpp
        uci.h uci.cpp
        util.h
        variables.h variables.cpp
        zobrist.h zobrist.cpp
        communication.h communication.cpp
        debug.h debug.cpp
)

include_directories(.)

add_executable(
        minerva_chess
        ${source_files}
        main.cpp
)

add_custom_command(
        TARGET minerva_chess  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/../nn-eba324f53044.nnue
        ${CMAKE_CURRENT_BINARY_DIR}/abc
)

add_library(
        source_library
        ${source_files}
)