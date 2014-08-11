#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
 * 棋盘: 6x6, 扩展为 8x8, 有效区域位于中间区域
 *
 * 0x00 0x01 ... 0x07
 * 0x08 0x09 ... 0x0f
 * .
 * .
 * .
 */

struct block_s {
	uint8_t topleft; /* 左上角坐标 */
	uint8_t x; /* 宽 */
	uint8_t y; /* 高 */

	void move_left() {
		this->topleft -= 1;
	}
	void move_right() {
		this->topleft += 1;
	}
	void move_up() {
		this->topleft -= 0x8;
	}
	void move_down() {
		this->topleft += 0x8;
	}
};

enum move_able {
	MOVE_N = 0x0, // 不能移动
	MOVE_L = 0x1,
	MOVE_R = 0x2,
	MOVE_U = 0x4,
	MOVE_D = 0x8,
};

class board_s {
private:
	struct block_s *block;
	int n;
	uint64_t board_bit; // 位棋盘 8x8
	struct board_hash_s {
		uint64_t board_bit;
		uint64_t board_hash;
		struct board_hash_s *next;
	} *board_hash_l;

	bool get_board_bit(uint8_t x, uint8_t y) {
		return this->board_bit & (0x1 << (y * 8 + x));
	}

	int can_move(int index) {
		int ret = MOVE_N;

		assert(index < n);
		assert(block[index].x > 0 && block[index].y > 0);
		assert(block[index].x != block[index].y);
		if (block[index].x > block[index].y) {
			/* 判断是否可向左移动 */
			if ((block[index].topleft & 0x8) > 1 &&
			    get_board_bit(block[index].topleft % 8 - 1,
					   block[index].topleft / 8)
			) {
				ret |= MOVE_L;
			}
			/* 判断是否可向右移动 */
			if ((block[index].topleft & 0x8) + block[index].x < 0x8 &&
			    get_board_bit(block[index].topleft % 8 + block[index].x,
					   block[index].topleft / 8)
			) {
				ret |= MOVE_R;
			}
		} else {
			/* 判断是否向可上移动 */
			if ((block[index].topleft / 8) > 1 &&
			    get_board_bit(block[index].topleft % 8,
				    block[index].topleft / 8 - 1)
			) {
				ret |= MOVE_U;
			}
			/* 判断是否向可下移动 */
			if ((block[index].topleft / 8 + block[index].y) < 0x8 &&
			    get_board_bit(block[index].topleft % 8,
				    block[index].topleft / 8 + block[index].y)
			) {
				ret |= MOVE_D;
			}
		}
		return ret;
	}

	bool is_solved() {
		if (block[0].topleft / 8 == 3 &&
		    block[0].topleft % 8 + block[0].x == 0x7
		) {
			return true;
		}
		return false;
	}
public:
	board_s() {
		this->n = 12;
		this->block = (struct block_s *)calloc(n, sizeof(struct block_s));
	}

	board_s(int n) {
		this->n = n;
		this->block = (struct block_s *)calloc(n, sizeof(struct block_s));
	}

	uint64_t get_hash() {
		uint64_t board_hash = 0;
		for (int i = 0; i < this->n; i++) {
			board_hash += block[i].topleft + i;
		}
		return board_hash;
	}

	uint64_t get_board_bit() {
		uint64_t board_bit = 0;
		for (int i = 0; i < this->n; i++) {
			for (int j = 0; j < block[i].x; j++) {
				for (int k = 0; k < block[i].y; k++) {
					board_bit |=
					 1 << ((block[i].topleft % 8) + j +
					       (block[i].topleft / 8 + k) * 8);
				}
			}
		}
		return board_bit;
	}

	void init_hase() {
		board_hash_l = (struct board_hash_s *)calloc(1, sizeof(*board_hash_l));
		assert(board_hash_l);
		board_hash_l->board_bit = get_board_bit();
		board_hash_l->board_hash = get_hash();
		board_hash_l->next = NULL;
	}

	bool is_in_hash_l(uint64_t bit, uint64_t hash) {
		struct board_hash_s *t;
		t = board_hash_l;
		while (t) {
			if (bit == t->board_bit && hash == t->board_hash) {
				return true;
			}
			t = t->next;
		}
		return false;
	}

	void add_hash(uint64_t bit, uint64_t hash) {
		struct board_hash_s *n = (struct board_hash_s *)calloc(1, sizeof(*n));
		assert(n);
		n->board_bit = bit;
		n->board_hash = hash;
		n->next = board_hash_l;
		board_hash_l = n;
	}

	bool search() {
		if (is_solved()) {
			return true;
		}
		for (int i = 0; i < n; i++) {
			int m = can_move(i);
			if (m & MOVE_L) {
				block[i].move_left();
				uint64_t ha = get_hash();
				uint64_t bit = get_board_bit();
				if (!is_in_hash_l(bit, ha)) { // 第一次出现局面
					this->board_bit = bit;
					add_hash(bit, ha);
					search();
				} else {
					block[i].move_right();
				}
			}
			if (m & MOVE_R) {
				block[i].move_right();
				uint64_t ha = get_hash();
				uint64_t bit = get_board_bit();
				if (!is_in_hash_l(bit, ha)) { // 第一次出现局面
					this->board_bit = bit;
					add_hash(bit, ha);
					search();
				} else {
					block[i].move_left();
				}
			}
			if (m & MOVE_U) {
				block[i].move_up();
				uint64_t ha = get_hash();
				uint64_t bit = get_board_bit();
				if (!is_in_hash_l(bit, ha)) { // 第一次出现局面
					this->board_bit = bit;
					add_hash(bit, ha);
					search();
				} else {
					block[i].move_down();
				}
			}
			if (m & MOVE_D) {
				block[i].move_down();
				uint64_t ha = get_hash();
				uint64_t bit = get_board_bit();
				if (!is_in_hash_l(bit, ha)) { // 第一次出现局面
					this->board_bit = bit;
					add_hash(bit, ha);
					search();
				} else {
					block[i].move_up();
				}
			}
		}
		return false;
	}
};

#endif
