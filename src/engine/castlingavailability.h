#ifndef CASTLINGAVAILABILITY_H
#define CASTLINGAVAILABILITY_H

#include <bitset>

typedef enum {
    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE
} CastleSide;

typedef enum {
    CASTLE_WHITE_KINGSIDE = 0,
    CASTLE_WHITE_QUEENSIDE = 1,
    CASTLE_BLACK_KINGSIDE = 2,
    CASTLE_BLACK_QUEENSIDE = 3
} CastleType;

const Move castle_moves[4] = { // indexes to match CastleType
    Move{SquarePosition{0,4}, SquarePosition{0, 1}},
    Move{SquarePosition{0,4}, SquarePosition{0, 6}},
    Move{SquarePosition{7,4}, SquarePosition{7, 1}},
    Move{SquarePosition{7,4}, SquarePosition{7, 6}},
};

struct CastlingAvailability {
    union {
        struct {
            bool white_kingside;
            bool white_queenside;
            bool black_kingside;
            bool black_queenside;
        };
        std::bitset<4> state;
    };

    CastlingAvailability() {
        white_kingside = true;
        white_queenside = true;
        black_kingside = true;
        black_queenside = true;
    }

    CastlingAvailability(const CastlingAvailability& copy) {
        state = copy.state;
    }

    void remove_castle() {
        white_kingside = false;
        white_queenside = false;
        black_kingside = false;
        black_queenside = false;
    }

    bool castle_possible(const CastleType castle) const {
        switch (castle) {
            case CASTLE_WHITE_KINGSIDE:
                return white_kingside;
            case CASTLE_WHITE_QUEENSIDE:
                return white_queenside;
            case CASTLE_BLACK_KINGSIDE:
                return black_kingside;
            case CASTLE_BLACK_QUEENSIDE:
                return black_queenside;
        }
        return false;
    }
};


#endif //CASTLINGAVAILABILITY_H
