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
#include <SDL3_ttf/SDL_ttf.h>

#include "./snake.hpp"
#include <algorithm>
#include <cstring>
#include <memory>
#include <string>

/**
 * Defaults
 * TODO: test and see what happens when the window is not square.
 */
constexpr int win_width = 1000;
constexpr int win_height = 1000;
constexpr int default_font_size = 32;

#define CENTER_ALLIGN true

/**
 * Game State
 *
 * current state of the game
 */
enum State {
  paused,
  play,
  title,
  game_over,
};

/**
 * Game Context
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
  TTF_Font *font;

  std::unique_ptr<game::Board> board;

  /* offset at which the board must start from */
  const float x_offset;
  const float y_offset;
  const float grid_length;
  const float cell_size;

  State game_state;
} game_ctx_t;

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

  /*  Init Font ( currntly using EvilVampire-woqBn.ttf)*/
  if (!TTF_Init()) {
    SDL_Log("Couldn't initialize TTF: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  auto font = TTF_OpenFont("assets/EvilVampire-woqBn.ttf", default_font_size);
  if (!font) {
    SDL_Log("Couldn't Load Font : %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Change the blening mode to blend (for alpha value of colors to have an
  // effect when rending stuff)
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  // init board offsets (these are hardcoded for now)
  constexpr auto x_offset = 100;
  constexpr auto y_offset = 100;
  auto board = std::make_unique<game::Board>(game::Board());
  const auto grid_size = board->get_grid_size();
  const auto grid_length =
      std::min(win_width - (2 * x_offset), win_height - (2 * y_offset));
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
  else if (event->type == SDL_EVENT_KEY_DOWN) {
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

/*====== utility functions for rendering ======*/

/**
 * Draw text on screen from C-Style text.
 */
void draw_text(const game_ctx_t *game_ctx, const char *text,
               const SDL_FPoint loc, const SDL_Color color,
               const bool is_center_alligned = false) {
  SDL_assert(text);

  SDL_Surface *sdl_surface =
      TTF_RenderText_Blended(game_ctx->font, text, strlen(text), color);

  // SDL_Color bgcolor = {235, 0, 0, 255};
  // SDL_Surface *sdl_surface =
  //   TTF_RenderText_Shaded(game_ctx->font, text, strlen(text), color,
  //   bgcolor);
  SDL_Texture *texture =
      SDL_CreateTextureFromSurface(game_ctx->renderer, sdl_surface);
  float width, height;
  SDL_GetTextureSize(texture, &width, &height);
  auto final_loc = loc;
  if (is_center_alligned) {
    final_loc.x -= width / 2;
    final_loc.y -= height / 2;
  }
  SDL_FRect dst_rect = {final_loc.x, final_loc.y, width, height};
  SDL_RenderTexture(game_ctx->renderer, texture, nullptr, &dst_rect);
  SDL_DestroyTexture(texture);
  SDL_DestroySurface(sdl_surface);
}

/**
 * draw_grid
 *
 * draw the game grid at default location (presend in GAME_STATE)
 */
void draw_grid(const game_ctx_t *game_ctx) {

  // draw outer boundary lines in solid white color and internal lines as almost
  // transparent

  /*  draw outer boundary */
  SDL_SetRenderDrawColor(game_ctx->renderer, 216, 222, 233,
                         SDL_ALPHA_OPAQUE); /*#d8dee9*/
  SDL_FRect boundry = {game_ctx->x_offset, game_ctx->y_offset,
                       game_ctx->grid_length, game_ctx->grid_length};
  SDL_RenderRect(game_ctx->renderer, &boundry);

  /* draw internal lines. */
  SDL_SetRenderDrawColor(game_ctx->renderer, 59, 66, 82,
                         SDL_ALPHA_OPAQUE); /*#3b4252*/
  for (auto i = 1u; i < game_ctx->board->get_grid_size(); ++i) {
    // render rows
    SDL_RenderLine(
        game_ctx->renderer, game_ctx->x_offset + (game_ctx->cell_size * i),
        game_ctx->y_offset, game_ctx->x_offset + (game_ctx->cell_size * i),
        game_ctx->y_offset + game_ctx->grid_length);
    // render cols
    SDL_RenderLine(game_ctx->renderer, game_ctx->x_offset,
                   game_ctx->y_offset + (game_ctx->cell_size * i),
                   game_ctx->x_offset + game_ctx->grid_length,
                   game_ctx->y_offset + (game_ctx->cell_size * i));
  }
}

/**
 * get_absolute_coords
 *
 * Get absolute screen coordinates of cell (x,y) in the game::Board grid
 */
auto get_absolute_coords(const game_ctx_t *game_ctx,
                         const game::grid_coords_t grid_coords) {
  SDL_FPoint abs_coords;
  abs_coords.x = game_ctx->x_offset + (grid_coords.x * game_ctx->cell_size);
  abs_coords.y = game_ctx->y_offset + (grid_coords.y * game_ctx->cell_size);

  return abs_coords;
}

/**
 * fill_cell
 *
 * fill the grid cell pointed by grid_coords
 */
auto fill_cell(const game_ctx_t *game_ctx,
               const game::grid_coords_t grid_coords) {
  SDL_FPoint coords = get_absolute_coords(game_ctx, grid_coords);
  SDL_FRect cell = {coords.x + 5, coords.y + 5, game_ctx->cell_size - 8,
                    game_ctx->cell_size - 8};
  return SDL_RenderFillRect(game_ctx->renderer, &cell);
}

/**
 * draw_snake
 *
 * draw the snake in the grid
 */
void draw_snake(const game_ctx_t *game_ctx) {
  const auto snake_body = game_ctx->board->get_snake().get_body();

  SDL_SetRenderDrawColor(game_ctx->renderer, 163, 190, 140,
                         SDL_ALPHA_OPAQUE); /* #a3be8c */
  for (auto const &curr_body_co : snake_body) {
    fill_cell(game_ctx, {curr_body_co.x, curr_body_co.y});
  }
}

/**
 * draw_food
 *
 * draw the food in the grid
 */
void draw_food(const game_ctx_t *game_ctx) {
  SDL_SetRenderDrawColor(game_ctx->renderer, 191, 97, 106,
                         SDL_ALPHA_OPAQUE); /* #bf616a */
  auto food_loc = game_ctx->board->get_food_loc();
  fill_cell(game_ctx, food_loc);
}

/**
 * draw_score_board
 *
 * draw the score_board above grid
 */
void draw_score_board(const game_ctx_t *game_ctx) {
  const auto score = game_ctx->board->get_snake().get_size() - 1;

  TTF_SetFontSize(game_ctx->font, default_font_size * 2);
  SDL_Color color = {255, 255, 255, 255}; /* #ffffff */
  SDL_FPoint loc = {game_ctx->x_offset + (game_ctx->grid_length / 2.0f),
                    game_ctx->y_offset - (game_ctx->grid_length / 16.0f)};
  draw_text(game_ctx, ("Score: " + std::to_string(score)).c_str(), loc, color,
            CENTER_ALLIGN);
}
/**
 * draw a transparent background window on top of playing board to show any
 * information about intrupt ( can be title_window, pause screen, game over
 * screen etc)
 */
void draw_intrupt_bgwindow(const game_ctx_t *game_ctx) {
  /* intrupt window would be drawn on top of the grid */
  draw_grid(game_ctx);

  /* draw intrupt window background */
  SDL_SetRenderDrawColor(game_ctx->renderer, 46, 52, 64,
                         200); /* #2e3440 (transparent)*/

  /* pause window should be as big as the grid itself */
  SDL_FRect pause_win = {game_ctx->x_offset, game_ctx->y_offset,
                         game_ctx->grid_length, game_ctx->grid_length};
  SDL_RenderFillRect(game_ctx->renderer, &pause_win);
}

/**
 * draw_pause screen
 */
void draw_title_screen(const game_ctx_t *game_ctx) {

  draw_intrupt_bgwindow(game_ctx);
  SDL_Color color;
  SDL_FPoint loc;

  /*  Draw title Text */
  TTF_SetFontSize(game_ctx->font, default_font_size + 200);
  color = {235, 203, 139, 255};
  loc = {game_ctx->x_offset + (game_ctx->grid_length / 2.0f),
         game_ctx->y_offset + (game_ctx->grid_length / 4.0f)};
  draw_text(game_ctx, "SNAKE !", loc, color, CENTER_ALLIGN);

  /* Draw movement Controls */
  TTF_SetFontSize(game_ctx->font, default_font_size);
  color = {180, 147, 173, 255};
  loc = {game_ctx->x_offset + (game_ctx->grid_length / 4.0f),
         game_ctx->y_offset + (game_ctx->grid_length / 2.0f) + 20};
  draw_text(game_ctx, "W", loc, color, CENTER_ALLIGN);
  loc.y += 30;
  draw_text(game_ctx, "A  S  D", loc, color, CENTER_ALLIGN);
  color = {136, 192, 208, 255};
  loc.y += 35;
  draw_text(game_ctx, "Movement", loc, color, CENTER_ALLIGN);

  /* Draw other Controls */
  TTF_SetFontSize(game_ctx->font, default_font_size);
  color = {180, 147, 173, 255};
  loc = {game_ctx->x_offset + (game_ctx->grid_length * 0.65f),
         game_ctx->y_offset + (game_ctx->grid_length / 2.0f) + 5};
  draw_text(game_ctx, "P - ", loc, color);
  loc.x += 40;
  color = {136, 192, 208, 255};
  draw_text(game_ctx, "Pause", loc, color);
  loc.x -= 40;

  color = {180, 147, 173, 255};
  loc.y += 40;
  draw_text(game_ctx, "Esc -", loc, color);
  loc.x += 55;
  color = {136, 192, 208, 255};
  draw_text(game_ctx, "Exit", loc, color);
  loc.x -= 55;
}

/**
 * draw playing screen
 */
void draw_playing_screen(const game_ctx_t *game_ctx) {
  draw_grid(game_ctx);
  draw_snake(game_ctx);
  draw_food(game_ctx);
  draw_score_board(game_ctx);
}

/**
 * draw_pause screen
 */
void draw_pause_screen(const game_ctx_t *game_ctx) {
  draw_playing_screen(game_ctx);
  draw_intrupt_bgwindow(game_ctx);
  /* Draw Text */
  TTF_SetFontSize(game_ctx->font, default_font_size + 50);
  SDL_Color color = {235, 203, 139, 255};
  SDL_FPoint loc = {game_ctx->x_offset + (game_ctx->grid_length / 2.0f),
                    game_ctx->y_offset + (game_ctx->grid_length / 2.0f - 40)};
  draw_text(game_ctx, "paused !", loc, color, CENTER_ALLIGN);

  color = {136, 192, 208, 255};
  TTF_SetFontSize(game_ctx->font, default_font_size + 10);
  loc = {game_ctx->x_offset + (game_ctx->grid_length / 2.0f),
         game_ctx->y_offset + (game_ctx->grid_length / 2.0f) + 40};
  draw_text(game_ctx, "Press any movement key to continue", loc, color,
            CENTER_ALLIGN);

  /*set the font size back to normal before moving returning*/
  TTF_SetFontSize(game_ctx->font, default_font_size);
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
    draw_playing_screen(game_ctx);
    break;
  case State::title:
  case State::game_over:
    draw_title_screen(game_ctx);
    break;
  case State::paused:
    draw_pause_screen(game_ctx);
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
void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
