#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdio>

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (!TTF_Init()) {
        SDL_Log("Failed to initialize TTF: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", 800, 600, SDL_WINDOW_METAL);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Enable VSync for smooth rendering
    SDL_SetRenderVSync(renderer, 1);

    // Load logo texture
    SDL_Texture *logo = IMG_LoadTexture(renderer, "assets/sdl.png");
    if (!logo) {
        SDL_Log("Failed to load image: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Scale and center the logo
    float texW = 0, texH = 0;
    SDL_GetTextureSize(logo, &texW, &texH);
    constexpr float scale = 0.4f;
    const SDL_FRect dst = {
        400.0f - (texW * scale) / 2.0f,
        300.0f - (texH * scale) / 2.0f,
        texW * scale,
        texH * scale
    };

    bool running = true;
    bool showLogo = true;
    bool showInfo = false;

    // Load font with higher size for clarity
    constexpr int fontSize = 40;
    TTF_Font *font = TTF_OpenFont("assets/Arial.ttf", static_cast<float>(fontSize));
    if (!font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        SDL_DestroyTexture(logo);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Font rendering configuration
    TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
    TTF_SetFontKerning(font, true);

    // Prepare version and platform text
    constexpr SDL_Color textColor = {0, 0, 0, 255};
    const int version = SDL_GetVersion();
    char versionText[64];
    char platformText[64];
    snprintf(versionText, sizeof(versionText), "SDL Version: %d.%d.%d",
             SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
    snprintf(platformText, sizeof(platformText), "Platform: %s", SDL_GetPlatform());

    // Render text surfaces with blending
    const size_t versionLen = strlen(versionText);
    const size_t platformLen = strlen(platformText);
    SDL_Surface *surface1 = TTF_RenderText_Blended(font, versionText, versionLen, textColor);
    SDL_Surface *surface2 = TTF_RenderText_Blended(font, platformText, platformLen, textColor);

    if (!surface1 || !surface2) {
        SDL_Log("Text rendering failed: %s", SDL_GetError());
        if (surface1) SDL_DestroySurface(surface1);
        if (surface2) SDL_DestroySurface(surface2);
        TTF_CloseFont(font);
        SDL_DestroyTexture(logo);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Convert surfaces to textures
    SDL_Texture *text1 = SDL_CreateTextureFromSurface(renderer, surface1);
    SDL_Texture *text2 = SDL_CreateTextureFromSurface(renderer, surface2);

    // Measure text sizes
    const int width1 = surface1->w;
    const int height1 = surface1->h;
    const int width2 = surface2->w;
    const int height2 = surface2->h;

    SDL_DestroySurface(surface1);
    SDL_DestroySurface(surface2);

    // Position the text elements
    const SDL_FRect pos1 = {
        400 - static_cast<float>(width1) / 2.0f, 250, static_cast<float>(width1), static_cast<float>(height1)
    };
    const SDL_FRect pos2 = {
        400 - static_cast<float>(width2) / 2.0f, 350, static_cast<float>(width2), static_cast<float>(height2)
    };

    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                const auto mx = event.button.x;
                if (auto my = event.button.y; showLogo && mx >= dst.x && mx <= dst.x + dst.w &&
                                              my >= dst.y && my <= dst.y + dst.h) {
                    showLogo = false;
                    showInfo = true;
                } else if (showInfo &&
                           ((mx >= pos1.x && mx <= pos1.x + pos1.w && my >= pos1.y && my <= pos1.y + pos1.h) ||
                            (mx >= pos2.x && mx <= pos2.x + pos2.w && my >= pos2.y && my <= pos2.y + pos2.h))) {
                    showLogo = true;
                    showInfo = false;
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw logo or info depending on state
        if (showLogo) {
            SDL_RenderTexture(renderer, logo, nullptr, &dst);
        }

        if (showInfo) {
            SDL_RenderTexture(renderer, text1, nullptr, &pos1);
            SDL_RenderTexture(renderer, text2, nullptr, &pos2);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Cleanup
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
