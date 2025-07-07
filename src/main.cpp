/**@file main.cpp
 * @brief entery point of the program
 *
 * This contains all the SDL fucntion and mainly responsible for all the
 * rendering and UI stuff, core game logic is present in snake.hpp/cpp files
 *
 * The program uses SDL3's main callback fucntion which takes care of different
 * entry points for different platforms (especially usefull when using
 * emscripten to build this project)
 */

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "./snake.hpp"
#include <algorithm>
#include <memory>

/**
 * Default width and height of the main window.
 * TODO: test and see what happens when the window is not square.
 */
constexpr int win_width = 1000;
constexpr int win_height = 1000;

/**
 * Game State
 *
 * pointer to this structure would be made avaialable by SDL throught the
 * rendering pipeline, a pointer to it would be passed around to all the SDL
 * functions as "appstate". If there is anything that is needed by all the SDL
 * functions, it would most probabbly go here instead of existing as a global
 * variable
 */
typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  std::unique_ptr<game::Board> board;

  /* offset at which the board must start from */
  const int x_offset;
  const int y_offset;
  const float grid_length;
  const float cell_size;

  bool is_paused;
  bool is_game_over;
} game_state_t;

/**
 * SDL_APPInit
 *
 * Intial entry point of for SDL3, this runs first at the begining of the
 * program, so put stuff that is related any kind of initialisation here.
 *
 * Learn more about it here: https://wiki.libsdl.org/SDL3/SDL_AppInit
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("snake", "v.1.0", "com.snake.test");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize : %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  if (!SDL_CreateWindowAndRenderer("snek", win_width, win_height, 0, &window,
                                   &renderer)) {
    SDL_Log("Couldn't Create Window Or Renderer : %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // init board offsets (these are hardcoded for now)
  constexpr auto x_offset = 100;
  constexpr auto y_offset = 100;
  auto board = std::make_unique<game::Board>(game::Board());
  const auto grid_size = board->get_grid_size();
  const auto grid_length =
      std::min(win_width - (2 * x_offset), win_height - (2 * y_offset));
  const auto cell_size = static_cast<float>(grid_length) / grid_size;

  *appstate = new game_state_t{.window = window,
                               .renderer = renderer,
                               .board = std::move(board),
                               .x_offset = x_offset,
                               .y_offset = y_offset,
                               .grid_length = grid_length,
                               .cell_size = cell_size,
                               .is_paused = true,
                               .is_game_over = false};
  return SDL_APP_CONTINUE;
}

/**
 * SDL_APPEvent
 *
 * This handles AppEvents (at this point these are user inputs or keystrokes)
 *
 * Learn more about it here: https://wiki.libsdl.org/SDL3/SDL_AppEvent
 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  game_state_t *game_state = static_cast<game_state_t *>(appstate);
  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;
  else if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.scancode) {
      /* Handle movement keys */
    case SDL_SCANCODE_W:
      game_state->board->update_snake_dir(game::Direction::north);
      game_state->is_paused = false;
      break;
    case SDL_SCANCODE_A:
      game_state->board->update_snake_dir(game::Direction::west);
      game_state->is_paused = false;
      break;
    case SDL_SCANCODE_S:
      game_state->board->update_snake_dir(game::Direction::south);
      game_state->is_paused = false;
      break;
    case SDL_SCANCODE_D:
      game_state->board->update_snake_dir(game::Direction::east);
      game_state->is_paused = false;
      break;
    case SDL_SCANCODE_P:
      game_state->is_paused = true;
      break;

    case SDL_SCANCODE_ESCAPE:
      return SDL_APP_SUCCESS;
    default:
      break;
    }
  }
  return SDL_APP_CONTINUE;
}

/*====== utility functions for rendering ======*/

/**
 * draw_grid
 *
 * draw the game grid at default location (presend in GAME_STATE)
 */
void draw_grid(const game_state_t *game_state) {
  SDL_SetRenderDrawColor(game_state->renderer, 128, 128, 128,
                         SDL_ALPHA_OPAQUE); /* white, full Alpha */

  for (auto i = 0u; i <= game_state->board->get_grid_size(); ++i) {
    // render rows
    SDL_RenderLine(game_state->renderer,
                   game_state->x_offset + (game_state->cell_size * i),
                   game_state->y_offset,
                   game_state->x_offset + (game_state->cell_size * i),
                   game_state->y_offset + game_state->grid_length);
    // render cols
    SDL_RenderLine(game_state->renderer, game_state->x_offset,
                   game_state->y_offset + (game_state->cell_size * i),
                   game_state->x_offset + game_state->grid_length,
                   game_state->y_offset + (game_state->cell_size * i));
  }
}

/**
 * get_absolute_coords
 *
 * Get absolute screen coordinates of cell (x,y) in the game::Board grid
 */
auto get_absolute_coords(const game_state_t *game_state,
                         const game::grid_coords_t grid_coords) {
  SDL_FPoint abs_coords;
  abs_coords.x = game_state->x_offset + (grid_coords.x * game_state->cell_size);
  abs_coords.y = game_state->y_offset + (grid_coords.y * game_state->cell_size);

  return abs_coords;
}

/**
 * fill_cell
 *
 * fill the grid cell pointed by grid_coords
 */
auto fill_cell(const game_state_t *game_state,
               const game::grid_coords_t grid_coords) {
  SDL_FPoint coords = get_absolute_coords(game_state, grid_coords);
  SDL_FRect cell = {coords.x + 5, coords.y + 5, game_state->cell_size - 8,
                    game_state->cell_size - 8};
  return SDL_RenderFillRect(game_state->renderer, &cell);
}

/**
 * draw_snake
 *
 * draw the snake in the grid
 */
void draw_snake(const game_state_t *game_state) {
  const auto snake_body = game_state->board->get_snake().get_body();

  SDL_SetRenderDrawColor(game_state->renderer, 255, 0, 0,
                         SDL_ALPHA_OPAQUE); /* red, full Alpha */
  for (auto const &curr_body_co : snake_body) {
    fill_cell(game_state, {curr_body_co.x, curr_body_co.y});
  }
}

/**
 * draw_food
 *
 * draw the food in the grid
 */
void draw_food(const game_state_t *game_state) {
  SDL_SetRenderDrawColor(game_state->renderer, 0, 255, 0,
                         SDL_ALPHA_OPAQUE); /* red, full Alpha */
  auto food_loc = game_state->board->get_food_loc();
  fill_cell(game_state, food_loc);
}
/*=============== End of Utility function ===============*/

/**
 * SDL_APPIterate
 *
 * This function is equivalant to game loop (or update loop) in other
 * game/rendering engines, this will be called over and over responsible for
 * rendering and updating the frame of the application.
 *
 * Learn more about it here: https://wiki.libsdl.org/SDL3/SDL_AppIteratet
 */
SDL_AppResult SDL_AppIterate(void *appstate) {
  game_state_t *game_state = static_cast<game_state_t *>(appstate);

  if (!game_state->is_paused) {
    if (!game_state->board->update()) {
      /* game over */
      game_state->board->reset();
      game_state->is_paused = true;
    }
  }
  /* draw background*/
  SDL_SetRenderDrawColor(game_state->renderer, 0, 0, 0,
                         SDL_ALPHA_OPAQUE); /* black, full Alpha */
  SDL_RenderClear(game_state->renderer);

  draw_grid(game_state);
  draw_snake(game_state);
  draw_food(game_state);

  SDL_RenderPresent(game_state->renderer);

  SDL_Delay(100);
  return SDL_APP_CONTINUE;
}

/**
 * SDL_APPQuit
 *
 * This function is called before the program terminates, so if there is some
 * cleanup which you want to do, this is the function to use.
 *
 * Learn more about it here: https://wiki.libsdl.org/SDL3/SDL_AppQuit
 */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
