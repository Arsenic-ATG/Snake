/**@file snake.hpp
 * @brief contains function prototypes for snake game
 *
 * This contains function prototypes responsible for the game logic. the
 * definitions would be present in snake.cpp
 */

#ifndef SNAKE_H
#define SNAKE_H

#include <memory>
#include <vector>

namespace game {

constexpr unsigned int default_grid_size = 20;

/**
 * represents coordinates on the game board
 */
struct grid_coords_t {
  unsigned int x, y;

  grid_coords_t(const unsigned int x = 0, const unsigned int y = 0)
      : x(x), y(y) {}
  bool operator==(const grid_coords_t &other) const {
    return ((x == other.x) && (y == other.y));
  }
};

/**
 * chose center of the grid as default location (grid starts at 0,0 that's why
 * -1)
 */
const grid_coords_t default_snake_pos = {(default_grid_size / 2) - 1,
                                         (default_grid_size / 2) - 1};

/**
 * All the directions a snake can take on the board
 */
enum Direction { north = 0, east, south, west };

/**@class Snake
 * @brief Contains behavior of the snake
 *
 */
class Snake {
  grid_coords_t head; /* coordinates of the head on game grid*/
  std::vector<grid_coords_t> body;
  Direction direction;

public:
  Snake(const grid_coords_t head_coords) : head(head_coords) {
    body.push_back(head);
  }
  Snake(const int x, const int y) : Snake(grid_coords_t(x, y)) {}

  /* getters */
  auto get_body() const { return body; }
  auto get_head() const { return head; }
  auto get_direction() const { return direction; }
  auto get_next_head_location() const {
    auto next_coords = head;
    switch (direction) {
    case north:
      next_coords.y--;
      break;
    case east:
      next_coords.x++;
      break;
    case south:
      next_coords.y++;
      break;
    case west:
      next_coords.x--;
      break;
    }
    return next_coords;
  }

  auto set_direction(const Direction dir) -> void { direction = dir; }

  /* move the snake to the next location. NOTE: this fuction doesn't check for
   * collision and food eating, they must be handled seperately. */
  auto move(const bool has_eaten_food = false) -> void;

  // check if the snake body existin in the given coordnate or not
  auto has_snake(const grid_coords_t here) const -> bool;
};

/**@class Board
 * @brief Contains behavior of the game board
 *
 */
class Board {
  unsigned int grid_size;
  std::vector<grid_coords_t> grid;
  grid_coords_t food_loc;
  grid_coords_t init_snake_coords;
  std::unique_ptr<Snake> snake;

  auto will_collide(const grid_coords_t next_loc) const -> bool;
  auto spawn_new_food() -> void;

public:
  Board(const unsigned int grid_size = default_grid_size,
        const grid_coords_t init_snake_coords = default_snake_pos)
      : grid_size(grid_size), init_snake_coords(init_snake_coords),
        snake(std::make_unique<Snake>(Snake(init_snake_coords))) {
    spawn_new_food();
  }

  /* getters */
  auto get_grid_size() const { return grid_size; }
  auto get_snake() { return *snake; }
  auto get_food_loc() const { return food_loc; }
  auto get_grid() const { return grid; }

  auto update_snake_dir(Direction dir) -> void { snake->set_direction(dir); }
  auto update() -> bool;
  auto reset() -> bool;
};

} /*  namespace game */

#endif /* SNAKE_H */
