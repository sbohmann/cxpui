#include <SDL3/SDL.h>
#include <stdlib.h>

#define NUM_RECTS 1000

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Performance Demo", 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    
    SDL_FRect rects[NUM_RECTS];
    float speeds[NUM_RECTS];
    
    // Initialize random rectangles
    for(int i = 0; i < NUM_RECTS; i++) {
        rects[i].x = random() % 800;
        rects[i].y = random() % 600;
        rects[i].w = 20;
        rects[i].h = 20;
        speeds[i] = 1 + (random() % 5);
    }
    
    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event) && event.type == SDL_EVENT_QUIT) break;
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Update and draw all rectangles
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for(int i = 0; i < NUM_RECTS; i++) {
            rects[i].x += speeds[i];
            if(rects[i].x > 800) rects[i].x = -20;
            SDL_RenderFillRect(renderer, &rects[i]);
        }
        
        SDL_RenderPresent(renderer);
    }
    
    SDL_Quit();
    return 0;
}
