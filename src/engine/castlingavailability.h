#ifndef CASTLINGAVAILABILITY_H
#define CASTLINGAVAILABILITY_H

#include <bitset>

typedef enum {
    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE
} CastleSide;

typedef struct  {
    Colour colour;
    CastleSide side;
} CastleType;


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
        white_kingside = copy.white_kingside;
        white_queenside = copy.white_queenside;
        black_kingside = copy.black_kingside;
        black_queenside = copy.black_queenside;
    }

    void remove_castle() {
        white_kingside = false;
        white_queenside = false;
        black_kingside = false;
        black_queenside = false;
    }

    bool castle_possible(const CastleType castle) const {
        switch (castle.colour) {
            case COL_WHITE:
                switch (castle.side) {
                    case CASTLE_KINGSIDE: return white_kingside;
                    case CASTLE_QUEENSIDE: return white_queenside;
                }
                break;
            case COL_BLACK:
                switch (castle.side) {
                    case CASTLE_KINGSIDE: return black_kingside;
                    case CASTLE_QUEENSIDE: return black_queenside;
                }
                break;
            default: return false;
        }
        return false;
    }
};


#endif //CASTLINGAVAILABILITY_H
