#include "swap.h"

void Swap(char *left, char *right)
{
	char sym = *left;
	*left = *right;
	*right = sym;
}
