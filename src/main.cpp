/**@file main.cpp
 * @brief entry point of the program
 *
 * This contains all the SDL function and mainly responsible for all the
 * SDL initialization, event handling, and main game loop.
 * Core game logic is present in snake.hpp/cpp files.
 * Rendering logic is now separated into renderer.hpp/cpp files.
 *
 * The program uses SDL3's main callback function which takes care of different
 * entry points for different platforms (especially useful when using
 * emscripten to build this project)
 */

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "./snake.hpp"
#include "./renderer.hpp"
#include <algorithm>
#include <cstring>
#include <memory>
#include <string>

/**
 * Defaults (ignore later when the window resizes)
 * TODO: test and see what happens when the window is not square.
 */
int win_width = 650;
int win_height = 650;
int default_font_size = 32;

/**
 * SDL_APPInit
 *
 * Initial entry point for SDL3, this runs first at the beginning of the
 * program, so put stuff that is related to any kind of initialization here.
 *
 * Learn more about it here: https://wiki.libsdl.org/SDL3/SDL_AppInit
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("snake", "v.1.0", "com.snake.test");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize : %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  /* === Handle Window === */
  SDL_Window *window =
      SDL_CreateWindow("snek", win_width, win_height,
                       SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    SDL_Log("Couldn't Create Window : %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  /* on some displays the maximization happens asynchronously, so try to sync
   * window before taking in the new dimensions */
  if (!SDL_SyncWindow(window))
    SDL_Log("Warning: Windows sync failed !");

  /* ===  Handle Renderer === */
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    SDL_Log("Couldn't Create Renderer : %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Change the blending mode to blend (for alpha value of colors to have an
  // effect when rendering stuff)
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  /*  === Handle Fonts === */
  /*  Init Font (currently using EvilVampire-woqBn.ttf)*/
  if (!TTF_Init()) {
    SDL_Log("Couldn't initialize TTF: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  auto font = TTF_OpenFont("assets/EvilVampire-woqBn.ttf", default_font_size);
  if (!font) {
    SDL_Log("Couldn't Load Font : %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // FAILSAFE
  // Note: these values would be rewritten as soon as the window resizes (check
  // SDL_AppEvent())
  constexpr auto x_offset = 32;
  constexpr auto y_offset = 32;
  auto board = std::make_unique<game::Board>(game::Board());
  const auto grid_size = board->get_grid_size();
  const auto grid_length =
      std::min(win_width - (2.0f * x_offset), win_height - (2.0f * y_offset));
  const auto cell_size = static_cast<float>(grid_length) / grid_size;

  *appstate = new game_ctx_t{.window = window,
                             .renderer = renderer,
                             .font = font,
                             .board = std::move(board),
                             .x_offset = x_offset,
                             .y_offset = y_offset,
                             .grid_length = grid_length,
                             .cell_size = cell_size,
                             .game_state = State::title};

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
  game_ctx_t *game_ctx = static_cast<game_ctx_t *>(appstate);
  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;
  // handle all the parameters when the window resizes
  else if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED ||
           event->type == SDL_EVENT_WINDOW_RESIZED) {

    // this will change the values of dimensions to updated ones
    SDL_GetRenderOutputSize(game_ctx->renderer, &win_width, &win_height);
    auto remaining_y = win_height;
    auto remaining_x = win_width;

    // top buffer for length (3% from the top)
    auto top_y_buffer = (3 / 100.0f) * (remaining_y);
    remaining_y -= top_y_buffer;

    auto new_grid_length = (std::min(remaining_x, remaining_y));
    new_grid_length -= (2 / 100.0f) * new_grid_length;

    auto new_x_offset = (remaining_x - new_grid_length) / 2;
    auto new_y_offset = ((remaining_y - new_grid_length) / 2) + top_y_buffer;

    auto new_cell_size =
        static_cast<float>(new_grid_length) / game_ctx->board->get_grid_size();

    // update the game context with the update parameters
    game_ctx->x_offset = new_x_offset;
    game_ctx->y_offset = new_y_offset;
    game_ctx->grid_length = new_grid_length;
    game_ctx->cell_size = new_cell_size;
    // default font size is 32% of the grid length
    default_font_size = (0.032) * new_grid_length;

  } else if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (game_ctx->game_state) {
    case State::play:
      switch (event->key.scancode) {
      case SDL_SCANCODE_W:
        game_ctx->board->update_snake_dir(game::Direction::north);
        break;
      case SDL_SCANCODE_A:
        game_ctx->board->update_snake_dir(game::Direction::west);
        break;
      case SDL_SCANCODE_S:
        game_ctx->board->update_snake_dir(game::Direction::south);
        break;
      case SDL_SCANCODE_D:
        game_ctx->board->update_snake_dir(game::Direction::east);
        break;
      case SDL_SCANCODE_P:
        game_ctx->game_state = State::paused;
        break;
      case SDL_SCANCODE_R:
        game_ctx->board->reset();
        game_ctx->game_state = State::title;
        break;
      case SDL_SCANCODE_ESCAPE:
        return SDL_APP_SUCCESS;
      default:
        break;
      }
      break;

    case State::paused:
      switch (event->key.scancode) {
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_D:
        game_ctx->game_state = State::play;
        if (event->key.scancode == SDL_SCANCODE_W)
          game_ctx->board->update_snake_dir(game::Direction::north);
        else if (event->key.scancode == SDL_SCANCODE_A)
          game_ctx->board->update_snake_dir(game::Direction::west);
        else if (event->key.scancode == SDL_SCANCODE_S)
          game_ctx->board->update_snake_dir(game::Direction::south);
        else if (event->key.scancode == SDL_SCANCODE_D)
          game_ctx->board->update_snake_dir(game::Direction::east);
        break;
      case SDL_SCANCODE_P:
        game_ctx->game_state = State::play;
        break;
      case SDL_SCANCODE_R:
        game_ctx->board->reset();
        game_ctx->game_state = State::title;
        break;
      case SDL_SCANCODE_ESCAPE:
        return SDL_APP_SUCCESS;
      default:
        break;
      }
      break;

    case State::title:
    case State::game_over:
      switch (event->key.scancode) {
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_D:
        game_ctx->game_state = State::play;
        if (event->key.scancode == SDL_SCANCODE_W)
          game_ctx->board->update_snake_dir(game::Direction::north);
        else if (event->key.scancode == SDL_SCANCODE_A)
          game_ctx->board->update_snake_dir(game::Direction::west);
        else if (event->key.scancode == SDL_SCANCODE_S)
          game_ctx->board->update_snake_dir(game::Direction::south);
        else if (event->key.scancode == SDL_SCANCODE_D)
          game_ctx->board->update_snake_dir(game::Direction::east);
        break;
      case SDL_SCANCODE_ESCAPE:
        return SDL_APP_SUCCESS;
      default:
        break;
      }
      break;
    }
  }
  return SDL_APP_CONTINUE;
}

/**
 * SDL_APPIterate
 *
 * This function is equivalent to game loop (or update loop) in other
 * game/rendering engines, this will be called over and over responsible for
 * rendering and updating the frame of the application.
 *
 * Learn more about it here: https://wiki.libsdl.org/SDL3/SDL_AppIterate
 */
SDL_AppResult SDL_AppIterate(void *appstate) {
  game_ctx_t *game_ctx = static_cast<game_ctx_t *>(appstate);

  /* draw background*/
  SDL_SetRenderDrawColor(game_ctx->renderer, 46, 52, 64,
                         SDL_ALPHA_OPAQUE); /* #2e3440 */
  SDL_RenderClear(game_ctx->renderer);

  switch (game_ctx->game_state) {
  case State::play:
    if (!game_ctx->board->update()) {
      /* game over */
      game_ctx->board->reset();
      game_ctx->game_state = State::game_over;
    }
    renderer::draw_playing_screen(game_ctx);
    break;
  case State::title:
  case State::game_over:
    renderer::draw_title_screen(game_ctx);
    break;
  case State::paused:
    renderer::draw_pause_screen(game_ctx);
    break;
  }

  SDL_RenderPresent(game_ctx->renderer);

  /* TODO: this is ugly, find a better way to manage speed. */
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
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  game_ctx_t *game_ctx = static_cast<game_ctx_t *>(appstate);
  
  // Clean up resources
  if (game_ctx) {
    if (game_ctx->font) {
      TTF_CloseFont(game_ctx->font);
    }
    if (game_ctx->renderer) {
      SDL_DestroyRenderer(game_ctx->renderer);
    }
    if (game_ctx->window) {
      SDL_DestroyWindow(game_ctx->window);
    }
    delete game_ctx;
  }
  
  TTF_Quit();
  SDL_Quit();
}