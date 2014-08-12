#include "block.h"

int main(int argc, char **argv)
{
#if 0
	struct block_s block[] = {
		{3*8 + 1, 2, 1},
		{1*8 + 1, 3, 1},
		{2*8 + 1, 3, 1},
		{1*8 + 4, 2, 1},
		{1*8 + 6, 1, 2},
		{2*8 + 4, 1, 2},
		{3*8 + 3, 1, 2},
		{4*8 + 4, 2, 1},
		{3*8 + 6, 1, 2},
		{5*8 + 1, 1, 2},
		{4*8 + 3, 2, 1},
		{4*8 + 5, 1, 2},
		{5*8 + 2, 3, 1},
		{0, 0, 0},
	};
#else
	struct block_s block[] = {
		{3*8 + 2, 2, 1},
		{3*8 + 4, 1, 3},
		{4*8 + 1, 2, 1},
		{4*8 + 3, 1, 2},
		{4*8 + 5, 1, 3},
		{5*8 + 2, 1, 2},
		{6*8 + 3, 2, 1},
		{0, 0, 0},
	};
#endif

	board_s *b = new board_s(block);
	for (int y = 0; y < 0x8; y++) {
		for (int x = 0; x < 0x8; x++) {
			printf("bit: %d: %d--%d\n", y, x, b->get_board_bit(x, y));
		}
	}
	b->search();
	return 0;
}
