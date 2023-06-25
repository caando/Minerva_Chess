add_executable(
        minerva_chess
        nnue/nnue.h nnue/nnue.cpp
        nnue/misc.h nnue/misc.cpp
        attack.h attack.cpp
        eval.h eval.cpp
        magic.h magic.cpp
        magic_constants.h
        movegen.h movegen.cpp
        search.h search.cpp
        transposition.h transposition.cpp
        types.h
        uci.h uci.cpp
        util.h util.cpp
        variables.h
        zobrist.h zobrist.cpp
        main.cpp
)