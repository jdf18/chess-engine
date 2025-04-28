#ifndef MOVEMAPS_H
#define MOVEMAPS_H

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <string>
#include <optional>
#include <cmath>
#include <bitset>

#include "bitboard.h"

std::unordered_map<uint64_t, uint64_t> get_file_mask();
std::unordered_map<uint64_t, uint64_t> get_rank_mask();
std::unordered_map<uint64_t, uint64_t> get_diag_mask_ne();
std::unordered_map<uint64_t, uint64_t> get_diag_mask_nw();
std::unordered_map<uint16_t, uint8_t> get_rank_attacks();
std::unordered_map<std::bitset<128>, uint64_t> get_diag_attacks_ne();
std::unordered_map<std::bitset<128>, uint64_t> get_diag_attacks_nw();
std::unordered_map<uint16_t, uint64_t> get_diag_attacks_ne_better();
std::unordered_map<uint16_t, uint64_t> get_diag_attacks_nw_better();
std::unordered_map<uint64_t, int> get_square_to_index_map();


#endif //MOVEMAPS_H
