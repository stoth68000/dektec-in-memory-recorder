#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

/* SMPTE 292M Clause 5 */
/* Table 2 - Timing Reference codes. */
#define IS_SAV(n) (((n) & (1 << 6)) == 0)
#define IS_EAV(n) (((n) & (1 << 6)) == 1)
#define CHROMA_SAMPLE(n) ((n) * 2)
#define LUMA_SAMPLE(n) (((n) * 2) + 1)

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <16bit-sdi-rawfilename>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Unable to open %s\n", argv[1]);
		return -2;
	}

	struct stat sb;
	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		close(fd);
		return -2;
	}

	uint8_t *p = mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (p == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return -3;
	}

	uint16_t *d = (uint16_t *)p;
	uint16_t *end = d + (sb.st_size / 2);

	unsigned int linenr, framenr = 0, old_linenr = 0;
	unsigned int crc;
	unsigned int line_discontinuities = 0;
	while (d < end) {

		/* Detect either a SAV or EAV - See Figure 3 SMPTE292m */
		if ((*(d + CHROMA_SAMPLE(0)) == 0x3ff) && (*(d + CHROMA_SAMPLE(1)) == 0x000) && (*(d + CHROMA_SAMPLE(2)) == 0x000))
		{

			if (IS_SAV(*(d + CHROMA_SAMPLE(3)))) {
				printf("\tSAV : ");
			} else {
				printf("\n");
				linenr  = ((*(d + CHROMA_SAMPLE(4)) & 0x1fc) >> 2);
				linenr |= ((*(d + CHROMA_SAMPLE(5)) & 0x3c) << 5);

				/* See if the line is discontinious, output ! error if so. */
				int linedisc = 0;
				if (old_linenr) {
					if (old_linenr == 750) {
						if (linenr != 1) {
							line_discontinuities++;
							linedisc = 1;
						}
					} else
					if ((old_linenr + 1) != linenr) {
						line_discontinuities++;
						linedisc = 1;
					}
				}
				old_linenr = linenr;

				crc     =  (*(d + CHROMA_SAMPLE(6)) & 0x1ff);
				crc    |= ((*(d + CHROMA_SAMPLE(7)) & 0x1ff) << 9);
				if (linenr == 1) {
					framenr++;
				}
				printf("EAV :%c   fr#%05d/li#%05d/crc#%05x ",
					linedisc ? '!' : ' ',
					framenr, linenr, crc);
			}

			int j;
			for (j = 1; j < (end - d); j++) {
				if ((*(d + CHROMA_SAMPLE(j)) == 0x3ff) &&  (*(d + CHROMA_SAMPLE(j + 1)) == 0x000))
					break;
			}

			/* j represents total LUMA or CHROMA samples, we want J to represent all samples. */
			j *= 2;
			
			printf("len %d ", j);
			if (j > 12)
				j = 12;
			for (int i = 0; i < (j * 2); i += 2)
				printf("%03x ", *(d + i));
		}

		d += 2;
	}

	munmap(p, sb.st_size);
	close(fd);

	return 0;
}
