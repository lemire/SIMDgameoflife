#ifndef BASICAUTOMATA_H
#define BASICAUTOMATA_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
void clear(uint32_t *counts, size_t width, size_t height) {
  memset(counts, 0, width * height * sizeof(uint32_t));
  /*for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      counts[i * width + j] = 0;
    }
  }*/
}
void clear8(uint8_t *counts, size_t width, size_t height) {
  memset(counts, 0, width * height * sizeof(uint8_t));
  /*for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      counts[i * width + j] = 0;
    }
  }*/
}
void randomize(uint8_t *states, size_t width, size_t height) {
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      states[i * width + j] = rand() & 1;
    }
  }
}

void randomize_margin1(uint8_t *states, size_t width, size_t height) {
  memset(states, 0, (width + 2) * (height + 2) * sizeof(uint8_t));
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      states[(i + 1) * (width + 2) + (j + 1)] = rand() & 1;
    }
  }
}

void computecounts(uint32_t *counts, const uint8_t *states, size_t width,
                   size_t height) {
  clear(counts, width + 2, height + 2);
  size_t awidth = width + 2;
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      uint32_t val = states[i * width + j];
      counts[(i + 1 + 1) * awidth + j + 1] += val;
      counts[(i - 1 + 1) * awidth + j + 1] += val;
      counts[(i + 1) * awidth + j + 1 + 1] += val;
      counts[(i + 1) * awidth + j - 1 + 1] += val;
      counts[(i + 1 + 1) * awidth + (j + 1) + 1] += val;
      counts[(i - 1 + 1) * awidth + (j - 1) + 1] += val;
      counts[(i - 1 + 1) * awidth + j + 1 + 1] += val;
      counts[(i + 1 + 1) * awidth + (j - 1) + 1] += val;
    }
  }
}

void gameoflife(uint32_t *counts, uint8_t *states, size_t width,
                size_t height) {
  computecounts(counts, states, width, height);
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      size_t coord = i * width + j;
      bool currentvalue = states[coord];
      uint32_t neighbors = counts[(i + 1) * (width + 2) + j + 1];
      if (currentvalue == 1) {
        if (neighbors < 2 || neighbors > 3) {
          states[coord] = 0;
        }
      } else {
        if (neighbors == 3) {
          states[coord] = 1;
        }
      }
    }
  }
}


#ifdef __AVX2__
#include <x86intrin.h>
void computecounts8(uint8_t *counts, const uint8_t *states, size_t width,
                    size_t height) {
  clear8(counts, width + 2, height + 2);
  size_t awidth = width + 2;
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      uint32_t val = states[(i + 1) * awidth + (j + 1)];
      counts[(i + 1 + 1) * awidth + j + 1] += val;
      counts[(i - 1 + 1) * awidth + j + 1] += val;
      counts[(i + 1) * awidth + j + 1 + 1] += val;
      counts[(i + 1) * awidth + j - 1 + 1] += val;
      counts[(i + 1 + 1) * awidth + (j + 1) + 1] += val;
      counts[(i - 1 + 1) * awidth + (j - 1) + 1] += val;
      counts[(i - 1 + 1) * awidth + j + 1 + 1] += val;
      counts[(i + 1 + 1) * awidth + (j - 1) + 1] += val;
    }
  }
}
// this is going to be entirely load bound
void computecounts8vec(uint8_t *counts, const uint8_t *states, size_t width,
                       size_t height) {
  size_t awidth = width + 2;
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j += sizeof(__m256i)) {
      //  __m256i c = _mm256_lddqu_si256((const __m256i *)(states + (i + 1 ) *
      //  awidth + j + 1));
      __m256i c =
          _mm256_setzero_si256(); // could save this step and the next add
      c = _mm256_add_epi8(
          c, _mm256_lddqu_si256(
                 (const __m256i *)(states + (i + 1 + 1) * awidth + j + 1)));
      c = _mm256_add_epi8(
          c, _mm256_lddqu_si256(
                 (const __m256i *)(states + (i - 1 + 1) * awidth + j + 1)));
      c = _mm256_add_epi8(
          c, _mm256_lddqu_si256(
                 (const __m256i *)(states + (i + 1) * awidth + j + 1 + 1)));
      c = _mm256_add_epi8(
          c, _mm256_lddqu_si256(
                 (const __m256i *)(states + (i + 1) * awidth + j - 1 + 1)));
      c = _mm256_add_epi8(
          c,
          _mm256_lddqu_si256(
              (const __m256i *)(states + (i + 1 + 1) * awidth + (j + 1) + 1)));
      c = _mm256_add_epi8(
          c,
          _mm256_lddqu_si256(
              (const __m256i *)(states + (i - 1 + 1) * awidth + (j - 1) + 1)));
      c = _mm256_add_epi8(
          c, _mm256_lddqu_si256(
                 (const __m256i *)(states + (i - 1 + 1) * awidth + j + 1 + 1)));
      c = _mm256_add_epi8(
          c,
          _mm256_lddqu_si256(
              (const __m256i *)(states + (i + 1 + 1) * awidth + (j - 1) + 1)));
      _mm256_storeu_si256((__m256i *)(counts + (i + 1) * awidth + (j + 1)), c);
    }
  }
}

void gameoflife8vec(uint8_t *counts, uint8_t *states, size_t width,
                    size_t height) {
  assert(width % (sizeof(__m256i)) == 0);
  size_t awidth = width + 2;

  computecounts8vec(counts, states, width, height); //
  // computecounts8vec(counts, states, width, height);

  __m256i shufmask =
      _mm256_set_epi8(0, 0, 0, 0, 0, 1, 1, 0,
                      // 1,1,1,1, 1,1,1,1,
                      //    1,1,1,1, 1,1,1,1,
                      0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                      // 1,1,1,1, 1,1,1,1,
                      0, 0, 0, 0, 0, 1, 0, 0
                      //  1,1,1,1, 1,1,1,1
      );
  /*    printf("states before:\n");
           for (size_t i = 0; i < height; i++) {
            for (size_t j = 0; j < width; j++) {
              printf("%d ",*(states + (i + 1 ) * awidth + j + 1));
            }
            printf("\n");
          }

    printf("counts:\n");
     for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
          printf("%d ",*(counts + (i + 1 ) * awidth + j + 1));
        }
        printf("\n");
      }*/
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j += sizeof(__m256i)) {
      __m256i c = _mm256_lddqu_si256(
          (const __m256i *)(counts + (i + 1) * awidth + j + 1));
      //    printf("count :");
      //  print(c);
      // printf("\n");
      c = _mm256_subs_epu8(
          c, _mm256_set1_epi8(
                 1)); // max was 8 = 0b1000, make it 7, 1 becomes 0, 0 remains 0
                      //  printf("count-:");
                      //  print(c);
      //      printf("\n");
      // printf("oldstate :");

      __m256i oldstate = _mm256_lddqu_si256(
          (const __m256i *)(states + (i + 1) * awidth + j + 1));
      //    print(oldstate);

      //              printf("\n");
      //  printf("xor mask :");
      // print(_mm256_or_si256(c,_mm256_slli_epi16(oldstate,3)));
      // printf("\n");
      __m256i newstate = _mm256_shuffle_epi8(
          shufmask, _mm256_or_si256(c, _mm256_slli_epi16(oldstate, 3)));
      _mm256_storeu_si256((__m256i *)(states + (i + 1) * awidth + (j + 1)),
                          newstate);
      // printf("newstate :");
      //  print(newstate);
      //  printf("\n");
    }
  } /*
   printf("states after:\n");
        for (size_t i = 0; i < height; i++) {
         for (size_t j = 0; j < width; j++) {
           printf("%d ",*(states + (i + 1 ) * awidth + j + 1));
         }
         printf("\n");
       }*/
}
#endif
#endif
