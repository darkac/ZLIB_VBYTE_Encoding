// Last modified: 2014-09-13 07:07:00
 
/**
 * @file: encode.c
 * @author: tongjiancong(lingfenghx@gmail.com)
 * @date:   2014-09-13 05:55:28
 * @brief: A simple example of showing
 *			how to ZLIB/VBYTE-encode a stream of integers
 **/
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

#define cnt 10

static size_t encode_vbyte(uint32_t u32, uint8_t *buffer)
{
    size_t nbytes;
    uint8_t byte;

    nbytes = 0;

    while (u32 >= 128) {
        byte = (u32 & 127) | 128;

        buffer[nbytes] = byte;

        nbytes++;

        u32 >>= 7;
    }

    buffer[nbytes] = (uint8_t)u32;

    nbytes++;

    return nbytes;
}

void encode_block_vbyte(uint32_t *source,
                        size_t nelements_source,
                        void *destination,
                        size_t *nbytes_destination)
{
    size_t i;

    *nbytes_destination = 0;

    for (i = 0; i < nelements_source; i++)
        *nbytes_destination +=
            encode_vbyte(source[i], destination + *nbytes_destination);
}

void encode_block_zlib(uint32_t *source,
                       size_t nbytes_source,
                       void *destination,
                       size_t *nbytes_destination)
{
    size_t nbytes;
    int ret;

    nbytes = nbytes_source * sizeof(uint32_t);

	/* int compress2(Bytef *dest,
	 *					uLongf *destLen,
	 *					const Bytef *source,
	 *					uLong sourceLen, int level);
	 */
    ret = compress2(destination,
                    nbytes_destination,
                    (void*)source,
                    nbytes,
                    Z_BEST_COMPRESSION);
	/* According to the zlib Manual,
	 * the last argument can be one of the followings:
	 *
	 * #define Z_NO_COMPRESSION         0
	 * #define Z_BEST_SPEED             1
	 * #define Z_BEST_COMPRESSION       9
	 * #define Z_DEFAULT_COMPRESSION  (-1)
	 *
	 */

	assert(ret != -1);
}

int main()
{
	//uint32_t *offset = (uint32_t *)malloc(cnt * sizeof(uint32_t));
	//uint32_t *length = (uint32_t *)malloc(cnt * sizeof(uint32_t));
	
	uint32_t offset[cnt] = {492130,	19855691, 16592212,	62126078, 95013208,
		12924177, 97263795, 75519745, 25692602,	2172777};
	uint32_t length[cnt] = {24, 30, 28, 3, 9, 97, 76, 17, 16, 3};
	
	/* Make sure that buffer is large enough to hold the compressed data.
	 * Or, the return value of compress2() will not be 0.
	 * For more details, please refer to http://www.zlib.net/manual.html
	 */
	size_t nbytes_buffer = compressBound(cnt * sizeof(uint32_t));
	void *buffer = (uint32_t *)malloc(nbytes_buffer);//100 * 1048576 * sizeof(uint32_t));
	assert(buffer != NULL);

	
	/* Note that here we only care about the output length - nbytes_buffer,
	 * not the compressed content - buffer
	 */
	encode_block_zlib(offset, cnt, buffer, &nbytes_buffer);
	fprintf(stderr, "output bytes of offset (ZLIB) = %u\n", (uint32_t)nbytes_buffer);
	encode_block_vbyte(offset, cnt, buffer, &nbytes_buffer);
	fprintf(stderr, "output bytes of offset (VBYTE) = %u\n", (uint32_t)nbytes_buffer);
	
	nbytes_buffer = compressBound(cnt * sizeof(uint32_t));
	encode_block_zlib(length, cnt, buffer, &nbytes_buffer);
	fprintf(stderr, "output bytes of length (ZLIB) = %u\n", (uint32_t)nbytes_buffer);
	encode_block_vbyte(length, cnt, buffer, &nbytes_buffer);
	fprintf(stderr, "output bytes of length (VBYTE) = %u\n", (uint32_t)nbytes_buffer);

	free(buffer);

    return 0;
}
