#include <stdio.h>
#include <SDL2/SDL.h>

#define WIDTH 900
#define HEIGHT 600
#define SCALE 1000
typedef struct Vec3 {
    float x, y, z;
} Vec3;

typedef struct Vec2 {
    float x, y;
} Vec2;
struct Block {
    Vec3 v3[8];	    
	
};
Vec2 project(Vec3 p) {
    float d = 4.0f;
    return (Vec2) {
	p.x / (p.z + d),
	p.y / (p.z + d)
    };
}
void scale_and_print(Vec2 p, SDL_Surface* psurface) {
    //prints single point
    int sx = (int)(p.x * SCALE + WIDTH / 2);
    int sy = (int)(p.y * (-1) * SCALE + HEIGHT / 2);
    SDL_Rect pix = (SDL_Rect){sx, sy, 3, 3};
    SDL_FillRect(psurface, &pix, 0xFFFFFFFF);

}
void print_object(SDL_Surface* psurface, Vec3 points[], int n) {
    for (int i = 0; i < n; i++) {
    	Vec3 p = points[i];
	p.z += 200;
	Vec2 projection = project(p);
   	scale_and_print(projection, psurface);
    }
}


int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* pwindow = SDL_CreateWindow(
        "3D Objects",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        0
    );

    if (!pwindow) {
        printf("Window Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
	
    SDL_Surface* psurface = SDL_GetWindowSurface(pwindow);
    
    struct Block block = {
    .v3 = {
            {24, 24, 24}, {24, 24, -24}, {24, -24, 24}, {-24, 24, 24},
            {24, -24, -24}, {-24, 24, -24}, {-24, -24, 24}, {-24, -24, -24}
        }
   };

    print_object(psurface, block.v3, 8);

    SDL_UpdateWindowSurface(pwindow);
	

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;
        }
    }
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    
    return 0;
}

