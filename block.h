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
	uint8_t w; /* 宽 */
	uint8_t h; /* 高 */

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
	struct move_s {
		int i;
		enum move_able m;
		struct move_s *next;
	} *move_save = NULL;

	void move_save_push(int i, enum move_able m) {
		struct move_s *n = (struct move_s *)calloc(1, sizeof(*n));
		assert(n);
		n->i = i;
		n->m = m;
		n->next = move_save;
		move_save = n;
	}

	void move_save_pop() {
		assert(move_save);
		struct move_s *t = move_save;
		move_save = move_save->next;
		free(t);
	}

	struct move_s *move_save_pop(struct move_s *ms) {
		assert(move_save && ms);
		struct move_s *t = move_save;
		ms->i = move_save->i;
		ms->m = move_save->m;
		move_save = move_save->next;
		free(t);
		return ms;
	}

	int can_move(int index) {
		int ret = MOVE_N;

		assert(index < n);
		assert(block[index].w > 0 && block[index].h > 0);
		assert(block[index].w != block[index].h);
		if (block[index].w > block[index].h) {
			/* 判断是否可向左移动 */
			if ((block[index].topleft & 0x7) > 1 &&
			    !get_board_bit(block[index].topleft % 8 - 1,
					   block[index].topleft / 8)
			) {
				ret |= MOVE_L;
			}
			/* 判断是否可向右移动 */
			if ((block[index].topleft & 0x7) + block[index].w < 0x7 &&
			    !get_board_bit(block[index].topleft % 8 + block[index].w,
					   block[index].topleft / 8)
			) {
				ret |= MOVE_R;
			}
		} else {
			/* 判断是否向可上移动 */
			if ((block[index].topleft / 8) > 1 &&
			    !get_board_bit(block[index].topleft % 8,
				    block[index].topleft / 8 - 1)
			) {
				ret |= MOVE_U;
			}
			/* 判断是否向可下移动 */
			if ((block[index].topleft / 8 + block[index].h) < 0x7 &&
			    !get_board_bit(block[index].topleft % 8,
				    block[index].topleft / 8 + block[index].h)
			) {
				ret |= MOVE_D;
			}
		}
		return ret;
	}

	bool is_solved() {
		if (block[0].topleft / 8 == 3 &&
		    block[0].topleft % 8 + block[0].w == 0x7
		) {
			return true;
		}
		return false;
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
			for (int j = 0; j < block[i].w; j++) {
				for (int k = 0; k < block[i].h; k++) {
					board_bit |=
					 (uint64_t)1 << ((block[i].topleft % 8) + j +
					       (block[i].topleft / 8 + k) * 8);
				}
			}
		}
		return board_bit;
	}

public:
	bool get_board_bit(uint8_t x, uint8_t y) {
		return this->board_bit & ((uint64_t)0x1 << (y * 8 + x));
	}

	board_s() {
		this->n = 12;
		this->block = (struct block_s *)calloc(n, sizeof(struct block_s));
	}

	board_s(int n) {
		this->n = n;
		this->block = (struct block_s *)calloc(n, sizeof(struct block_s));
	}

	board_s(struct block_s blockt[]) {
		this->n = 0;
		struct block_s *t = &blockt[0];
		while (t->topleft != 0) {
			t = &blockt[++this->n];
		}
		this->block = (struct block_s *)calloc(this->n,
				sizeof(struct block_s));
		for (int i = 0; i < this->n; i++) {
			this->block[i].topleft = blockt[i].topleft;
			this->block[i].w = blockt[i].w;
			this->block[i].h = blockt[i].h;
		}
		this->board_bit = get_board_bit();
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

	void search() {
		if (is_solved()) {
			struct move_s *move_tmp = move_save;
			int step = 0;
			printf("=========================\n");
			printf("solved!\n");

			while (move_tmp) {
				printf("move: i: %d, %s\n", move_tmp->i,
					move_tmp->m == MOVE_L ? "L" :
					(move_tmp->m == MOVE_R ? "R" :
					 (move_tmp->m == MOVE_U ? "U" :
					  (move_tmp->m == MOVE_D ? "D" : "error"))));
				move_tmp = move_tmp->next;
				step++;
			}
			printf("==end: %d=======================\n", step);
			// return;
			exit(true);
		}
		uint64_t ha = get_hash();
		uint64_t bit = get_board_bit();
		if (!is_in_hash_l(bit, ha)) { // 第一次出现局面
			this->board_bit = bit;
			add_hash(bit, ha);
		} else {
			return;
		}
		for (int i = 0; i < n; i++) {
			int m = can_move(i);
			// printf("%d: i: %d, moveable: %d\n", __LINE__, i, m);
			if (m & MOVE_L) {
				block[i].move_left();
				move_save_push(i, MOVE_L);
				search();
				move_save_pop();
				block[i].move_right();
			}
			if (m & MOVE_R) {
				block[i].move_right();
				move_save_push(i, MOVE_R);
				search();
				move_save_pop();
				block[i].move_left();
			}
			if (m & MOVE_U) {
				block[i].move_up();
				move_save_push(i, MOVE_U);
				search();
				move_save_pop();
				block[i].move_down();
			}
			if (m & MOVE_D) {
				block[i].move_down();
				move_save_push(i, MOVE_D);
				search();
				move_save_pop();
				block[i].move_up();
			}
		}
		return;
	}
};

#endif
