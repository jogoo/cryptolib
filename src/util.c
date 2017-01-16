/**
 *
 */

#include <stdint.h>

void print_array(uint8_t *array, uint16_t size)
{
	uint16_t i;
	for (i = size; i > 0;)
		printf("%02x", array[--i]);
	printf("\n");
}
