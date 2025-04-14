// main.cpp
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr float LOGO_SCALE = 0.4f;
constexpr int FONT_SIZE = 40;
constexpr SDL_Color TEXT_COLOR = {0, 0, 0, 255};

SDL_Texture *LoadTexture(SDL_Renderer *renderer, const char *path) {
    SDL_Texture *texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        SDL_Log("Failed to load texture '%s': %s", path, SDL_GetError());
    }
    return texture;
}

TTF_Font *LoadFont(const char *path, int size) {
    TTF_Font *font = TTF_OpenFont(path, static_cast<float>(size));
    if (!font) {
        SDL_Log("Failed to load font '%s': %s", path, SDL_GetError());
    }
    return font;
}

SDL_Texture *CreateTextTexture(SDL_Renderer *renderer, TTF_Font *font, const char *text, size_t len, SDL_Color color,
                               SDL_FRect &outRect, float centerX, float y) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, len, color);
    if (!surface) {
        SDL_Log("Text rendering failed: %s", SDL_GetError());
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    outRect = {
        centerX - static_cast<float>(surface->w) / 2.0f,
        y,
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };
    SDL_DestroySurface(surface);
    return texture;
}

int main(int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    if (!TTF_Init()) {
        SDL_Log("Failed to initialize TTF: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Hello SDL3", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_METAL);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderVSync(renderer, 1);

    SDL_Texture *logo = LoadTexture(renderer, "assets/sdl.png");
    if (!logo) return 1;

    float texW = 0, texH = 0;
    SDL_GetTextureSize(logo, &texW, &texH);
    SDL_FRect logoRect = {
        WINDOW_WIDTH / 2.0f - (texW * LOGO_SCALE) / 2.0f,
        WINDOW_HEIGHT / 2.0f - (texH * LOGO_SCALE) / 2.0f,
        texW * LOGO_SCALE,
        texH * LOGO_SCALE
    };

    TTF_Font *font = LoadFont("assets/Arial.ttf", FONT_SIZE);
    if (!font) return 1;
    TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
    TTF_SetFontKerning(font, true);

    char versionText[64];
    char platformText[64];
    const int version = SDL_GetVersion();
    snprintf(versionText, sizeof(versionText), "SDL Version: %d.%d.%d",
             SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
    snprintf(platformText, sizeof(platformText), "Platform: %s", SDL_GetPlatform());

    SDL_FRect versionRect, platformRect;
    SDL_Texture *text1 = CreateTextTexture(renderer, font, versionText, strlen(versionText), TEXT_COLOR, versionRect,
                                           WINDOW_WIDTH / 2.0f, 250);
    SDL_Texture *text2 = CreateTextTexture(renderer, font, platformText, strlen(platformText), TEXT_COLOR, platformRect,
                                           WINDOW_WIDTH / 2.0f, 350);
    if (!text1 || !text2) return 1;

    bool running = true;
    bool showLogo = true;
    bool showInfo = false;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                SDL_FPoint clickPoint = {
                    static_cast<float>(event.button.x),
                    static_cast<float>(event.button.y)
                };

                if (showLogo && SDL_PointInRectFloat(&clickPoint, &logoRect)) {
                    showLogo = false;
                    showInfo = true;
                } else if (showInfo &&
                           (SDL_PointInRectFloat(&clickPoint, &versionRect) ||
                            SDL_PointInRectFloat(&clickPoint, &platformRect))) {
                    showLogo = true;
                    showInfo = false;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (showLogo) {
            SDL_RenderTexture(renderer, logo, nullptr, &logoRect);
        }
        if (showInfo) {
            SDL_RenderTexture(renderer, text1, nullptr, &versionRect);
            SDL_RenderTexture(renderer, text2, nullptr, &platformRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(text1);
    SDL_DestroyTexture(text2);
    SDL_DestroyTexture(logo);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
