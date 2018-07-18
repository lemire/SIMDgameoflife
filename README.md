# SIMDgameoflife
Vectorized (AVX) version of the game of life

[Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) is a classical cellular automaton dating back to the 1970s
and still studied to this day. It serves as a crude model for the emergence of life
from an initial chaos.


The rules are simple. From a boolean grid, where things are either alive or dead,
we count the number of "alive" neighbours (up to 8) that each cell has, then we
apply these rules:


* Any live cell with fewer than two live neighbours dies.
* Any live cell with more than three live neighbours dies.
* Any dead cell with exactly three live neighbours becomes a live cell.

Each time we iterate through the grid, we have to visit all cells and apply these rules,
generating a new grid. How quickly could we do so?



## Requirements

This code assumes a Linux/macOS setup with the libpng library installed. A
GCC/clang compiler with make support is necessary.

A recent x64 processor with AVX2 support is best, for this project to make sense.

## Usage

We compute 100 iterations, generating PNG figures for the last two iterations.
The size of the grid is a parameter.

```
make
./gameoflife  10000
./gameoflife  10000 --nosimd
```
