add_executable(
        minerva_chess
        nnue/nnue.h nnue/nnue.cpp
        nnue/misc.h nnue/misc.cpp
        types.h
        magic_constants.h
        attack.h attack.cpp
        perft.h perft.cpp
        magic.h magic.cpp
        movegen.h
        eval.h
        search.h search.cpp
        transposition.h transposition.cpp
        uci.h uci.cpp
        util.h
        variables.h variables.cpp
        zobrist.h zobrist.cpp
        communication.h communication.cpp
        debug.h debug.cpp
        main.cpp
)

add_custom_command(
        TARGET minerva_chess  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/../nn-eba324f53044.nnue
        ${CMAKE_CURRENT_BINARY_DIR}/abc)
