#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdint.h>

/*
 * 棋盘: 6x6, 为省去边界判断扩展为 16x16, 有效区域位于中部
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

#endif
