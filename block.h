#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
 * 棋盘: 6x6, 扩展为 8x8, 有效区域位于中间区域
 *
 * 0x00 0x01 ... 0x0f
 * 0x10 0x11 ... 0x1f
 * .
 * .
 * .
 * 0xf0 0xf1 ... 0xff
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
		this->topleft -= 0x10;
	}
	void move_down() {
		this->topleft += 0x10;
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
public:
	board_s() {
		this->n = 12;
		this->block = (struct block_s *)calloc(n, sizeof(struct block_s));
	}

	board_s(int n) {
		this->n = n;
		this->block = (struct block_s *)calloc(n, sizeof(struct block_s));
	}
};

#endif
