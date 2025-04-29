#include "bitboard.h"
#include <cstdint>

uint8_t BitBoard::count_set_bits() {
	// Taken from here https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
	// 'bits' can be uint8_t as there can be a maximum of 64 set bits in the board.
	uint64_t copy = board;
	uint8_t bits;
	for (bits = 0; copy; bits++) {
		copy &= copy - 1;
	}
	return bits;
}

void BitBoard::out() {
	BitBoard to_out = reverse();
	std::cout << std::bitset<8>((to_out.board & FIRST_RANK)).to_string() << std::endl;
	std::cout << std::bitset<8>((to_out.board & SECOND_RANK) >> 8).to_string() << std::endl;
	std::cout << std::bitset<8>((to_out.board & THIRD_RANK) >> 16).to_string() << std::endl;
	std::cout << std::bitset<8>((to_out.board & FOURTH_RANK) >> 24).to_string() << std::endl;
	std::cout << std::bitset<8>((to_out.board & FIFTH_RANK) >> 32).to_string() << std::endl;
	std::cout << std::bitset<8>((to_out.board & SIXTH_RANK) >> 40).to_string() << std::endl;
	std::cout << std::bitset<8>((to_out.board & SEVENTH_RANK) >> 48).to_string() << std::endl;
	std::cout << std::bitset<8>((to_out.board & EIGHTH_RANK) >> 56).to_string() << std::endl;
}

BitBoard BitBoard::reverse() {
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