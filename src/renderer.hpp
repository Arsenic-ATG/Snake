/**
 * @file renderer.hpp
 * @brief Rendering utilities and functions for the Snake game
 *
 * This header contains all the rendering-related functions and utilities
 * that were previously in main.cpp. It provides a clean interface for
 * drawing game elements like the grid, snake, food, and UI screens.
 */

#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "./snake.hpp"

/**
 * Game State enum - moved from main.cpp for renderer access
 */
enum State {
  paused,
  play,
  title,
  game_over,
};

/**
 * Game Context structure - moved from main.cpp for renderer access
 */
typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;

  std::unique_ptr<game::Board> board;

  /*===Board parameters===*/
  float x_offset;
  float y_offset;
  float grid_length;
  float cell_size;

  State game_state;
} game_ctx_t;

// External variables that need to be accessible
extern int default_font_size;

/* Text rendering constants */
#define CENTER_ALLIGN true

namespace renderer {

/**
 * @brief Draw text on screen from C-Style text
 * @param game_ctx Game context containing renderer and font
 * @param text Text to render
 * @param loc Location to draw the text
 * @param color Text color
 * @param size Font size (defaults to default_font_size)
 * @param is_center_alligned Whether to center align the text
 */
void draw_text(const game_ctx_t *game_ctx, const char *text,
               const SDL_FPoint loc, const SDL_Color color,
               const int size = 0, // 0 means use default_font_size
               const bool is_center_alligned = false);

/**
 * @brief Draw the game grid at default location
 * @param game_ctx Game context containing grid parameters
 */
void draw_grid(const game_ctx_t *game_ctx);

/**
 * @brief Get absolute screen coordinates of cell (x,y) in the game::Board grid
 * @param game_ctx Game context containing offset and cell size
 * @param grid_coords Grid coordinates to convert
 * @return Absolute screen coordinates
 */
SDL_FPoint get_absolute_coords(const game_ctx_t *game_ctx,
                               const game::grid_coords_t grid_coords);

/**
 * @brief Fill the grid cell pointed by grid_coords
 * @param game_ctx Game context containing renderer and grid parameters
 * @param grid_coords Grid coordinates of the cell to fill
 */
void fill_cell(const game_ctx_t *game_ctx,
               const game::grid_coords_t grid_coords);

/**
 * @brief Draw the snake in the grid
 * @param game_ctx Game context containing the snake and renderer
 */
void draw_snake(const game_ctx_t *game_ctx);

/**
 * @brief Draw the food in the grid
 * @param game_ctx Game context containing food location and renderer
 */
void draw_food(const game_ctx_t *game_ctx);

/**
 * @brief Draw the score board above the grid
 * @param game_ctx Game context containing snake and grid parameters
 */
void draw_score_board(const game_ctx_t *game_ctx);

/**
 * @brief Draw a transparent background window on top of playing board
 * Used for title screen, pause screen, game over screen etc.
 * @param game_ctx Game context containing renderer and grid parameters
 */
void draw_intrupt_bgwindow(const game_ctx_t *game_ctx);

/**
 * @brief Draw the title screen
 * @param game_ctx Game context containing renderer and grid parameters
 */
void draw_title_screen(const game_ctx_t *game_ctx);

/**
 * @brief Draw the playing screen (grid + snake + food + score)
 * @param game_ctx Game context containing all game elements
 */
void draw_playing_screen(const game_ctx_t *game_ctx);

/**
 * @brief Draw the pause screen
 * @param game_ctx Game context containing renderer and grid parameters
 */
void draw_pause_screen(const game_ctx_t *game_ctx);

} // namespace renderer