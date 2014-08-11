#include "block.h"

int main(int argc, char **argv)
{
	struct block_s block[] = {
		{3*8 + 1, 2, 1},
		{2*8 + 3, 1, 3},
		{2*8 + 4, 1, 3},
		{0, 0, 0},
	};

	board_s *b = new board_s(block);

	b->search();
	return 0;
}
