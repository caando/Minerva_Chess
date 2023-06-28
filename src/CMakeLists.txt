add_executable(
        minerva_chess
        nnue/nnue.h nnue/nnue.cpp
        nnue/misc.h nnue/misc.cpp
        types.h types.cpp
        magic_constants.h magic_constant.cpp
        attack.h attack.cpp
        perft.h perft.cpp
        magic.h magic.cpp
        movegen.h movegen.cpp
        eval.h eval.cpp
        search.h search.cpp
        transposition.h transposition.cpp
        uci.h uci.cpp
        util.h util.cpp
        variables.h variables.cpp
        zobrist.h zobrist.cpp
        communication.h communication.cpp
        debug.h debug.cpp
        main.cpp
)