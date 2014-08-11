#include "block.h"

int main(int argc, char **argv)
{
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

	board_s *b = new board_s(block);

	b->search();
	return 0;
}
