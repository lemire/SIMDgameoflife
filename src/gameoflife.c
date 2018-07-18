#include "basicautomata.h"
#include "pnghelper.h"

void scalargame(size_t N) {
  uint8_t *states = (uint8_t *)malloc(N * N * sizeof(uint8_t));
  uint32_t *counts = (uint32_t *)malloc((N + 2) * (N + 2) * sizeof(uint32_t));
  printf("Using width %zu height %zu \n", N, N);

  randomize(states, N, N);

  bitmap_t image;

  /* Create an image. */

  image.width = N;
  image.height = N;
  image.pixels = calloc(image.width * image.height, sizeof(pixel_t));

  char cated_string[255];
  size_t t = 0;
  size_t iters = 100;
  clock_t start = clock();
  for (; t < iters - 2; t++) {
    printf(".");
    fflush(NULL);
    gameoflife(counts, states, N, N);
  }
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC * 1.0 / (iters - 2);
  printf("time per iteration %f s \n", seconds);
  for (; t < iters; t++) {
    gameoflife(counts, states, N, N);
    mapdatatoimage(&image, states);
    sprintf(cated_string, "result%zu.png", t);
    printf("saving to %s\n", cated_string);
    save_png_to_file(&image, cated_string);
  }

  free(image.pixels);
  free(states);
  free(counts);
}

#ifdef __AVX2__
void vectorgame(size_t N) {
  size_t block = sizeof(__m256i);
  if ((N % block) != 0) {
    N += block - (N % block);
  }
  printf("Using width %zu height %zu \n", N, N);
  uint8_t *states = (uint8_t *)malloc((N + 2) * (N + 2) * sizeof(uint8_t));
  uint8_t *counts = (uint8_t *)malloc((N + 2) * (N + 2) * sizeof(uint8_t));

  randomize_margin1(states, N, N);

  bitmap_t image;

  /* Create an image. */

  image.width = N;
  image.height = N;
  image.pixels = calloc(image.width * image.height, sizeof(pixel_t));

  char cated_string[255];
  size_t t = 0;
  size_t iters = 100;
  clock_t start = clock();
  for (; t < iters - 2; t++) {
    printf(".");
    fflush(NULL);
    gameoflife8vec(counts, states, N, N);
  }
  clock_t end = clock();
  float seconds = (float)(end - start) / (CLOCKS_PER_SEC)*1.0 / (iters - 2);
  printf("time per iteration %f s \n", seconds);
  for (; t < iters; t++) {
    gameoflife8vec(counts, states, N, N);
    mapdatatoimage_margin1(&image, states);
    sprintf(cated_string, "result%zu.png", t);
    printf("saving to %s\n", cated_string);
    save_png_to_file(&image, cated_string);
  }

  free(image.pixels);
  free(states);
  free(counts);
}
#endif

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("pass a width parameter\n");
    return EXIT_FAILURE;
  }
  size_t N = atoi(argv[1]);
#ifdef __AVX2__
  if (argc > 2) {
    printf("Scalar\n");

    scalargame(N);
  } else {
    printf("Using AVX2\n");
    vectorgame(N);
  }
#else
  scalargame(N);
#endif
  return 0;
}
