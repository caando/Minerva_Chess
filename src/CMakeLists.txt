add_executable(
        minerva_chess
        nnue/nnue.h nnue/nnue.cpp
        nnue/misc.h nnue/misc.cpp
        types.h
        magic_constants.h
        attack.h attack.cpp
        perft.h perft.cpp
        magic.h magic.cpp
        movegen.h movegen.cpp
        eval.h eval.cpp
        search.h search.cpp
        transposition.h transposition.cpp
        uci.h uci.cpp
        util.h util.cpp
        variables.h
        zobrist.h zobrist.cpp
        communication.h communication.cpp
        main.cpp
)