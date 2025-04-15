#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <iostream>
#include <bitset>

#define BOARD_ROW 8
#define BOARD_COL 1

#define BOARD_WHITE_SQUARE_MASK 0x55AA55AA55AA55AA
#define BOARD_BLACK_SQUARE_MASK ~BOARD_WHITE_SQUARE_MASK

#define FIRST_RANK   0x00000000000000FF
#define SECOND_RANK  0x000000000000FF00
#define THIRD_RANK   0x0000000000FF0000
#define FOURTH_RANK  0x00000000FF000000
#define FIFTH_RANK   0x000000FF00000000
#define SIXTH_RANK   0x0000FF0000000000
#define SEVENTH_RANK 0x00FF000000000000
#define EIGHTH_RANK  0xFF00000000000000

#define FIRST_FILE   0x0101010101010101
#define SECOND_FILE  0x0202020202020202
#define THIRD_FILE   0x0404040404040404
#define FOURTH_FILE  0x0808080808080808
#define FIFTH_FILE   0x1010101010101010
#define SIXTH_FILE   0x2020202020202020
#define SEVENTH_FILE 0x4040404040404040
#define EIGHTH_FILE  0x8080808080808080


struct BitBoard {
    uint64_t board;

    BitBoard(const uint64_t b) {
        board = b;
    }
    
    BitBoard() {
        board = 0;
    }

    inline BitBoard reverse() {
        uint64_t rev = 0;
        uint64_t board_copy = board;
        for (int i = 0; i < 64; i++) {
            rev <<= 1;

            if ((board_copy & 1) == 1) {
                rev ^= 1;
            }

            board_copy >>= 1;
        }
        return BitBoard(rev);
    }

    inline void out() {
        BitBoard to_out = reverse();
        std::cout << std::bitset<8>((to_out.board & FIRST_RANK  )      ).to_string() << std::endl;
        std::cout << std::bitset<8>((to_out.board & SECOND_RANK ) >> 8 ).to_string() << std::endl;
        std::cout << std::bitset<8>((to_out.board & THIRD_RANK  ) >> 16).to_string() << std::endl;
        std::cout << std::bitset<8>((to_out.board & FOURTH_RANK ) >> 24).to_string() << std::endl;
        std::cout << std::bitset<8>((to_out.board & FIFTH_RANK  ) >> 32).to_string() << std::endl;
        std::cout << std::bitset<8>((to_out.board & SIXTH_RANK  ) >> 40).to_string() << std::endl;
        std::cout << std::bitset<8>((to_out.board & SEVENTH_RANK) >> 48).to_string() << std::endl;
        std::cout << std::bitset<8>((to_out.board & EIGHTH_RANK ) >> 56).to_string() << std::endl;


    }
};

inline BitBoard operator &(BitBoard lhs, BitBoard rhs) {
    return BitBoard(lhs.board & rhs.board);
}

inline BitBoard operator |(BitBoard lhs, BitBoard rhs) {
    return BitBoard(lhs.board | rhs.board);
}

inline BitBoard operator ^(BitBoard lhs, BitBoard rhs) {
    return BitBoard(lhs.board ^ rhs.board);
}

inline void operator &=(BitBoard& lhs, BitBoard rhs) {
    lhs.board &= rhs.board;
}

inline void operator |=(BitBoard& lhs, BitBoard rhs) {
    lhs.board |= rhs.board;
}

inline void operator ^=(BitBoard& lhs, BitBoard rhs) {
    lhs.board ^= rhs.board;
}

inline BitBoard operator ~(BitBoard b) {
    return BitBoard(~b.board);
}

inline BitBoard operator <<(BitBoard lhs, int8_t rhs) {
    return BitBoard(lhs.board << rhs);
}

inline BitBoard operator >>(BitBoard lhs, uint8_t rhs) {
    return BitBoard(lhs.board >> rhs);
}

inline void operator <<=(BitBoard& lhs, int8_t rhs) {
    lhs.board <<= rhs;
}

inline void operator >>=(BitBoard& lhs, int8_t rhs) {
    lhs.board >>= rhs;
}

inline bool operator ==(BitBoard lhs, BitBoard rhs) {
    return lhs.board == rhs.board;
}

inline bool operator !=(BitBoard lhs, BitBoard rhs) {
    return lhs.board != rhs.board;
}

inline bool operator >(BitBoard lhs, uint64_t rhs) {
    return lhs.board > rhs;
}

inline bool operator <(BitBoard lhs, uint64_t rhs) {
    return lhs.board < rhs;
}

#endif //BITBOARD_H