/**@file snake.cpp
 * @brief contains function definitions responsible for entire game logic
 *
 * The entire game logic sits here, this file should not be responsible for any
 * rendering related things
 */

#include "snake.hpp"
#include <cmath>
#include <random>

/*-------------*/
/* Snake Class */
/*-------------*/

/**
 * Snake::move
 *
 * Move the snake in the set direction, this fuction also handles the length of
 * the snake if it have eaten the food in this move.
 */
void game::Snake::move(const bool has_eaten_food) {
  head = get_next_head_location();

  /* update the body of the snake (remember, head is at the end) */
  for (auto i = 1u; i < body.size(); ++i) {
    body[i - 1] = body[i];
  }

  /* also adjust the length of the snake if food has been eaten*/
  if (has_eaten_food)
    body.push_back(head);
  else
    body.back() = head;
}

/**
 * Snake::has_snake
 *
 * check if provided coordinates of the grid is occupied by the snake or not.
 */
bool game::Snake::has_snake(const grid_coords_t here) const {
  /*  linear search snake body for this coordinate */
  bool found = false;
  for (auto i = 0u; i < body.size(); ++i) {
    if (here == body[i]) {
      found = true;
      break;
    }
  }
  return found;
}

/*-------------*/
/* Board Class */
/*-------------*/

/**
 * Board::will_collide
 *
 * Check for collisions with the board edges and the body of the snake.
 */
bool game::Board::will_collide(const grid_coords_t next_loc) const {

  return (
      /* check collision with walls (coordinates are unsigned so no need to
         check for lower bound) */
      ((next_loc.x >= grid_size) || (next_loc.y >= grid_size)) ||
      /*  check collision with body */
      snake->has_snake(next_loc));
}

/**
 * Board::spawn_new_food
 *
 * Spawn food at a random valid location on the board.
 */
void game::Board::spawn_new_food() {
  std::random_device rd;
  std::mt19937 gen(rd());
  // Define a uniform integer distribution for numbers between 0 and grid_size
  std::uniform_int_distribution<> distrib(0, grid_size - 1);

  // make sure the food doesn't spawn on positions where snake would collide
  do {
    food_loc.x = distrib(gen);
    food_loc.y = distrib(gen);
  } while (will_collide(food_loc));
}

/**
 * Board::update_snake_dir
 *
 * Update the moving direction of the snake, make sure the snake of length 2 or
 * more can only take 90 degree turns.
 */
void game::Board::update_snake_dir(const Direction new_direction) {
  if (snake->get_size() > 1) {
    /* Directions are numbered from 0 to 3 in clockwise direction, so difference
     * between the 2 directions can be used to check for a 180 degree turn.*/
    auto curr_direction = snake->get_direction();
    if ((std::abs(new_direction - curr_direction) % 3) > 1)
      return;
  }
  snake->set_direction(new_direction);
}

/**
 * Board::update
 *
 * This is the master udpate function which updates the board and all the
 * entities on it including snake and food on every ineration.
 */
bool game::Board::update() {
  /* get next head location */
  auto next_head_location = snake->get_next_head_location();
  /* check for bad collision */
  if (will_collide(next_head_location))
    return false;

  /* check for collision with food */
  bool has_eaten_food = false;
  if (next_head_location == food_loc) {
    spawn_new_food();
    has_eaten_food = true;
  }
  snake->move(has_eaten_food);
  return true;
}

/**
 * Board::reset
 *
 * Reset the board for a new game.
 */
bool game::Board::reset() {
  auto new_snake = std::make_unique<Snake>(Snake(init_snake_coords));
  snake = std::move(new_snake);
  spawn_new_food();
  return true;
}
