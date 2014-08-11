#include "block.h"

int main(int argc, char **argv)
{
	board_s *b = new board_s(14);

	b->search();
	return 0;
}
