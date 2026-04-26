# C_Life + BSQ
- Life: Conway's Game of Life with dual-buffer tick simulation and pen-command input parsing.
- BSQ: largest obstacle-free square finder using 2D dynamic programming on a configurable-character map format.

# exam05 — Level 01: C Systems Programming Under Exam Conditions

> Two timed C exercises in systems-level programming: Conway's Game of Life simulator with a custom pen-based input protocol, and a largest-square finder using dynamic programming on a configurable map format.

---

## Table of Contents

- [Overview](#overview)
- [Exercises](#exercises)
  - [life — Conway's Game of Life](#life--conways-game-of-life)
  - [bsq — Biggest Square on a Map](#bsq--biggest-square-on-a-map)
- [Key Concepts & Approaches](#key-concepts--approaches)
- [Technologies](#technologies)
- [Setup & Build](#setup--build)
- [Usage Examples](#usage-examples)
- [Real-World Applicability](#real-world-applicability)

---

## Overview

Level 01 of 42's Exam Rank 05 requires building complete, memory-safe C programs from a specification under exam conditions. Both exercises are single-file C programs that demonstrate: struct-based state management, dynamic 2D grid allocation, algorithmic problem-solving, and careful memory cleanup. Allowed functions are strictly restricted — no `printf` in `life`, no library string functions in `bsq`.

| Exercise | Core challenge |
|----------|---------------|
| `life` | Conway's Game of Life with pen-command input, dual-buffer tick simulation |
| `bsq` | Largest clear square on a configurable-character map using 2D dynamic programming |

---

## Exercises

### life — Conway's Game of Life

**Files:** `main.c`

A complete Game of Life simulation: takes board dimensions and iteration count as arguments, reads a sequence of pen movement commands from stdin to set the initial cell pattern, simulates the specified number of generations, and prints the result.

**What it does:**

**Input phase — pen protocol:**
A virtual pen starts at (0,0). Commands arrive character-by-character from stdin via `read()`:
- `w`/`a`/`s`/`d` — move up/left/down/right, clamped to board bounds
- `x` — toggle drawing mode (pen down marks the current cell alive; pen up moves without marking)

This converts a stream of ASCII characters into an initial board state without needing a coordinate format.

**Simulation phase — dual-buffer tick:**
Two `t_board` structs are allocated. Each generation, `perform_action()` reads from `curr` and writes results into `next`, then the pointers are swapped. This avoids updating cells in-place while reading neighbors — a correctness requirement of the Game of Life rules.

**Conway's rules implemented:**
- Live cell with 2 or 3 live neighbors → survives
- Dead cell with exactly 3 live neighbors → becomes alive
- All other cells → die or stay dead

**Output:** Each alive cell is `O`, each dead cell is a space, each row terminated by `\n`.

**Memory:** Both boards are fully heap-allocated with proper cleanup on all exit paths (including mid-allocation failures).

---

### bsq — Biggest Square on a Map

**Files:** `main.c`

Reads one or more map files (or stdin), parses a header that defines the character set, validates the map, finds the largest obstacle-free square using a dynamic programming algorithm, fills it with the "full" character, and prints the result.

**What it does:**

**Configurable character format:**
The first line encodes: `<height><empty_char><obstacle_char><full_char>`. Any printable ASCII character is valid for each role — the program is not hardcoded to `.`, `o`, or `x`. Validation checks that all three are distinct and that every cell on the map contains only empty or obstacle characters.

**Largest-square algorithm (2D DP):**
A `dp[y][x]` table is built where each cell stores the side length of the largest square whose bottom-right corner is at `(x, y)`:
- If `(x, y)` is an obstacle: `dp[y][x] = 0`
- If on the top row or left column: `dp[y][x] = 1`
- Otherwise: `dp[y][x] = 1 + min(dp[y-1][x], dp[y][x-1], dp[y-1][x-1])`

The global maximum during the scan gives the largest square's size and bottom-right corner position, which is then filled with the full character.

**Tiebreaking:** The scan proceeds top-to-bottom, left-to-right, so the first (topmost, then leftmost) maximum found wins — matching the subject's tiebreak requirement.

**Multi-file support:** Multiple file arguments are processed in sequence; each result or `"map error"` is separated by a blank line.

**Memory:** `getline()` manages line buffers; all rows and the dp table are freed on all exit paths.

---

## Key Concepts & Approaches

**Dual-Buffer (Ping-Pong) Pattern**
`life` allocates two identical boards and alternates between them each generation. Reading from `curr` and writing to `next` guarantees that neighbor counts for every cell are computed from the same generation's state. Swapping pointers (`t_board *temp = curr; curr = next; next = temp`) is O(1) and avoids copying the entire grid each tick.

**2D Dynamic Programming**
`bsq`'s DP recurrence computes the largest square bottom-right at each cell in a single left-to-right, top-to-bottom pass — O(width × height) time. This is dramatically more efficient than naive O(width² × height²) brute force, and is the standard algorithm used in image processing, terrain analysis, and grid-based planning.

**Configurable Character Protocol**
Rather than hardcoding map characters, `bsq` reads them from the header and stores them in the map struct. Every cell check uses `map->empty` and `map->obstacle` rather than literal characters. This makes the program a general-purpose map processor — the same binary handles any valid input format.

**Strict Memory Safety**
All allocations check for NULL and free on failure before returning. `getline()` buffers are freed after use. The dp table rows are freed individually before the array itself. Every exit path — including mid-allocation failures and invalid map formats — leaves no leaked memory.

**Pen State Machine**
`life`'s input parser is a minimal state machine: a `t_pen` struct tracks position and a boolean `drawing` flag. The `switch` on each input character transitions state and conditionally marks cells. This avoids parsing a complex coordinate format and models well under the allowed function constraint (`read()` only).

---

## Technologies

| Tool | Purpose |
|------|---------|
| C (C99 standard) | Implementation language |
| `read()` | Byte-level stdin input (life — no buffering) |
| `getline()` | Line-based file input with dynamic buffer (bsq) |
| `malloc` / `calloc` / `free` | Manual heap management |
| `atoi` | Argument parsing |
| `putchar` / `fputs` / `fprintf` | Output (allowed function sets differ per exercise) |
| clang / gcc | Compilation |
| Valgrind | Memory leak verification |

---

## Setup & Build

**Prerequisites:** `gcc` or `clang`, `make` (optional), `valgrind` for leak checking.

```bash
# Clone the repo
git clone https://github.com/gtretiak/exam05.git
cd exam05/lvl_01

# --- life ---
cd life
gcc -Wall -Wextra -Werror main.c -o life

# Run with a glider pattern
echo 'sdxddssaaww' | ./life 5 5 0

# Run for 1 generation
echo 'dxss' | ./life 3 3 1

# --- bsq ---
cd ../bsq
gcc -Wall -Wextra -Werror main.c -o bsq

# Run on a map file
./bsq file.txt

# Run on multiple files
./bsq file1.txt file2.txt

# Read from stdin
echo "3.ox\n...\n...\n..." | ./bsq

# Memory check
valgrind --leak-check=full ./bsq file.txt
```

---

## Usage Examples

### life — Blinker (period-2 oscillator)

```bash
# Draw a vertical line of 3 cells, simulate 1 tick → becomes horizontal
echo 'dxss' | ./life 3 3 0
# Output (iteration 0 — initial):
#  0 
#  0 
#  0 

echo 'dxss' | ./life 3 3 1
# Output (iteration 1):
#    
# 000
#    

echo 'dxss' | ./life 3 3 2
# Output (iteration 2 — back to vertical):
#  0 
#  0 
#  0 
```

### life — Glider seed

```bash
echo 'sdxddssaaww' | ./life 5 5 0
#      
#  000 
#  0 0 
#  000 
#      
```

### bsq — Map file

```
# file.txt contents:
9.ox
...........................
....o......................
............o..............
...........................
....o......................
...............o...........
...........................
......o..............o.....
..o.......o................
```

```bash
./bsq file.txt
# Output:
# .....xxxxxxx...............
# ....oxxxxxxx...............
# .....xxxxxxxo..............
# .....xxxxxxx...............
# ....oxxxxxxx...............
# .....xxxxxxx...o...........
# .....xxxxxxx...............
# ......o..............o.....
# ..o.......o................
```

---

## Real-World Applicability

### 1. Cellular Automaton Engines and Scientific Simulation
`life`'s dual-buffer simulation loop is the exact pattern used in physics simulations, fluid dynamics solvers (lattice Boltzmann methods), and any grid-based cellular automaton. The ping-pong buffer prevents read-write aliasing. Game of Life itself is used in computer architecture research (it is Turing-complete) and in procedural map generation for games.

### 2. Image Processing — Largest Clear Region Detection
`bsq`'s DP algorithm is directly applicable to image processing tasks: finding the largest obstacle-free rectangle/square in a binary image (for object placement, obstacle mapping, or text layout). The same recurrence is used in computer vision pipelines for detecting clear landing zones in aerial imagery, free-space detection in robotics, and optimal content-aware ad placement.

### 3. Terrain Analysis and Robotics Path Planning
The largest-square DP is used in grid-based path planners (like those in autonomous vehicles and warehouse robots) to identify the largest free area for maneuver, vehicle placement, or storage slot allocation. The configurable character format of `bsq` mirrors how robotics occupancy grids encode passable/impassable cells with arbitrary cost labels.

### 4. Network / Data Center Layout Optimization
Finding the largest contiguous free square on a grid maps directly to data center rack layout problems: given a floor map with blocked areas (pillars, cooling units), find the largest contiguous zone for server deployment. The same DP technique is used in EDA (Electronic Design Automation) for finding placement areas on chip floorplans.

### 5. Real-Time State Machines in Embedded Systems
`life`'s pen input parser — a `t_pen` struct with position and drawing state updated by a `switch` on each input byte — is a minimal finite state machine. This pattern is fundamental in embedded firmware: serial protocol parsers, keyboard/touchpad drivers, and industrial control panels all process byte streams through exactly this kind of position-and-mode state machine with bounded memory and no dynamic allocation in the hot path.
