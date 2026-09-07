#include <string.h>
#include "revert_string.h"

void RevertString(char *str)
{
	size_t len = strlen(str);
	size_t left = 0;
	size_t right = len - 1;
	while (left < right)
	{
		char sym = str[left];
		str[left] = str[right];
		str[right] = sym;
		left++;
		right--;
	}
}