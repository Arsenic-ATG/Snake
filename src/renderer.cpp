/**
 * @file renderer.cpp
 * @brief Implementation of rendering utilities for the Snake game
 *
 * This file contains the implementation of all rendering functions
 * that were previously in main.cpp, providing a clean separation
 * between game logic and rendering logic.
 */

#include "renderer.hpp"
#include <algorithm>
#include <cstring>
#include <string>

namespace renderer {

void draw_text(const game_ctx_t *game_ctx, const char *text,
               const SDL_FPoint loc, const SDL_Color color,
               const int size, const bool is_center_alligned) {
  SDL_assert(text);

  int font_size = (size == 0) ? default_font_size : size;
  TTF_SetFontSize(game_ctx->font, font_size);
  SDL_Surface *sdl_surface =
      TTF_RenderText_Blended(game_ctx->font, text, strlen(text), color);

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

void draw_grid(const game_ctx_t *game_ctx) {
  // Draw outer boundary lines in solid white color and internal lines as almost
  // transparent

  /* Draw outer boundary */
  SDL_SetRenderDrawColor(game_ctx->renderer, 216, 222, 233,
                         SDL_ALPHA_OPAQUE); /*#d8dee9*/
  SDL_FRect boundry = {game_ctx->x_offset, game_ctx->y_offset,
                       game_ctx->grid_length, game_ctx->grid_length};
  SDL_RenderRect(game_ctx->renderer, &boundry);

  /* Draw internal lines */
  SDL_SetRenderDrawColor(game_ctx->renderer, 59, 66, 82,
                         SDL_ALPHA_OPAQUE); /*#3b4252*/
  for (auto i = 1u; i < game_ctx->board->get_grid_size(); ++i) {
    // Render rows
    SDL_RenderLine(
        game_ctx->renderer, game_ctx->x_offset + (game_ctx->cell_size * i),
        game_ctx->y_offset, game_ctx->x_offset + (game_ctx->cell_size * i),
        game_ctx->y_offset + game_ctx->grid_length);
    // Render cols
    SDL_RenderLine(game_ctx->renderer, game_ctx->x_offset,
                   game_ctx->y_offset + (game_ctx->cell_size * i),
                   game_ctx->x_offset + game_ctx->grid_length,
                   game_ctx->y_offset + (game_ctx->cell_size * i));
  }
}

SDL_FPoint get_absolute_coords(const game_ctx_t *game_ctx,
                               const game::grid_coords_t grid_coords) {
  SDL_FPoint abs_coords;
  abs_coords.x = game_ctx->x_offset + (grid_coords.x * game_ctx->cell_size);
  abs_coords.y = game_ctx->y_offset + (grid_coords.y * game_ctx->cell_size);
  return abs_coords;
}

void fill_cell(const game_ctx_t *game_ctx,
               const game::grid_coords_t grid_coords) {
  SDL_FPoint coords = get_absolute_coords(game_ctx, grid_coords);
  SDL_FRect cell = {coords.x, coords.y, game_ctx->cell_size,
                    game_ctx->cell_size};
  SDL_RenderFillRect(game_ctx->renderer, &cell);
}

void draw_snake(const game_ctx_t *game_ctx) {
  const auto snake_body = game_ctx->board->get_snake().get_body();

  SDL_SetRenderDrawColor(game_ctx->renderer, 163, 190, 140,
                         SDL_ALPHA_OPAQUE); /* #a3be8c */
  for (auto const &curr_body_co : snake_body) {
    fill_cell(game_ctx, {curr_body_co.x, curr_body_co.y});
  }
}

void draw_food(const game_ctx_t *game_ctx) {
  SDL_SetRenderDrawColor(game_ctx->renderer, 191, 97, 106,
                         SDL_ALPHA_OPAQUE); /* #bf616a */
  auto food_loc = game_ctx->board->get_food_loc();
  fill_cell(game_ctx, food_loc);
}

void draw_score_board(const game_ctx_t *game_ctx) {
  const auto score = game_ctx->board->get_snake().get_size() - 1;

  SDL_Color color = {255, 255, 255, 255}; /* #ffffff */
  SDL_FPoint loc = {game_ctx->x_offset + (game_ctx->grid_length / 2.0f),
                    game_ctx->y_offset - default_font_size / 2};
  draw_text(game_ctx, ("Score: " + std::to_string(score)).c_str(), loc, color,
            default_font_size, CENTER_ALLIGN);
}

void draw_intrupt_bgwindow(const game_ctx_t *game_ctx) {
  /* Interrupt window would be drawn on top of the grid */
  draw_grid(game_ctx);

  /* Draw interrupt window background */
  SDL_SetRenderDrawColor(game_ctx->renderer, 46, 52, 64,
                         200); /* #2e3440 (transparent)*/

  /* Background window should be as big as the grid itself */
  SDL_FRect pause_win = {game_ctx->x_offset, game_ctx->y_offset,
                         game_ctx->grid_length, game_ctx->grid_length};
  SDL_RenderFillRect(game_ctx->renderer, &pause_win);
}

void draw_title_screen(const game_ctx_t *game_ctx) {
  draw_intrupt_bgwindow(game_ctx);
  SDL_Color color;
  SDL_FPoint loc;

  /* Draw title Text */
  color = {235, 203, 139, 255};
  loc = {game_ctx->x_offset + (game_ctx->grid_length / 2.0f),
         game_ctx->y_offset + (game_ctx->grid_length / 4.0f)};
  draw_text(game_ctx, "SNAKE !", loc, color, default_font_size + 300,
            CENTER_ALLIGN);

  /* Draw movement Controls */
  color = {180, 147, 173, 255};
  loc = {game_ctx->x_offset + (game_ctx->grid_length / 4.0f),
         game_ctx->y_offset + (game_ctx->grid_length / 2.0f) + 20};
  draw_text(game_ctx, "W", loc, color, default_font_size, CENTER_ALLIGN);
  loc.y += 30;
  draw_text(game_ctx, "A  S  D", loc, color, default_font_size, CENTER_ALLIGN);
  color = {136, 192, 208, 255};
  loc.y += 35;
  draw_text(game_ctx, "Movement", loc, color, default_font_size, CENTER_ALLIGN);

  /* Draw other Controls */
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
  draw_text(game_ctx, "R -", loc, color);
  loc.x += 40;
  color = {136, 192, 208, 255};
  draw_text(game_ctx, "Reset", loc, color);
  loc.x -= 40;

  color = {180, 147, 173, 255};
  loc.y += 40;
  draw_text(game_ctx, "Esc -", loc, color);
  loc.x += 55;
  color = {136, 192, 208, 255};
  draw_text(game_ctx, "Exit", loc, color);
  loc.x -= 55;
}

void draw_playing_screen(const game_ctx_t *game_ctx) {
  draw_grid(game_ctx);
  draw_snake(game_ctx);
  draw_food(game_ctx);
  draw_score_board(game_ctx);
}

void draw_pause_screen(const game_ctx_t *game_ctx) {
  draw_playing_screen(game_ctx);
  draw_intrupt_bgwindow(game_ctx);

  /* Draw Text */
  SDL_Color color = {235, 203, 139, 255};
  SDL_FPoint loc = {game_ctx->x_offset + (game_ctx->grid_length / 2.0f),
                    game_ctx->y_offset + (game_ctx->grid_length / 2.0f - 30)};
  draw_text(game_ctx, "paused !", loc, color, default_font_size + 30,
            CENTER_ALLIGN);

  color = {136, 192, 208, 255};
  loc = {game_ctx->x_offset + (game_ctx->grid_length / 2.0f),
         game_ctx->y_offset + (game_ctx->grid_length / 2.0f) + 25};
  draw_text(game_ctx, "Press any movement key to continue", loc, color,
            default_font_size, CENTER_ALLIGN);
}

} // namespace renderer