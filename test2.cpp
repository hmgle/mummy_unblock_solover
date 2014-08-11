#include "block.h"

int main(int argc, char **argv)
{
	struct block_s block[] = {
		{3*8 + 1, 2, 1},
		{0, 0, 0},
	};

	board_s *b = new board_s(block);

	b->search();
	return 0;
}
