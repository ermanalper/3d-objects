#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

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

Vec3 rotatePitch(Vec3 p, float angleInRadians) {
    Vec3 result;
    float cosA = cos(angleInRadians);
    float sinA = sin(angleInRadians);

    result.x = p.x; // X değişmez
    result.y = p.y * cosA - p.z * sinA;
    result.z = p.y * sinA + p.z * cosA;

    return result;
}

Vec3 rotateYaw(Vec3 p, float angleInRadians) {
    Vec3 result;
    float cosA = cos(angleInRadians);
    float sinA = sin(angleInRadians);

    result.x = p.x * cosA + p.z * sinA;
    result.y = p.y; // Y değişmez
    result.z = -p.x * sinA + p.z * cosA;

    return result;
}

int main()
{
    SDL_Rect erase_rect = (SDL_Rect) {50, 50, 800, 800};

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
    int rl_rotate = 0;
    int ud_rotate = 0;
    float angle = 0.174f; //10 degrees in radian
    while (running) {
        while (SDL_PollEvent(&e)) {
	    rl_rotate = 0;
	    ud_rotate = 0;
	    if (e.key.keysym.sym == SDLK_RIGHT) {
		rl_rotate = 1;    
	    } else if (e.key.keysym.sym == SDLK_LEFT) {
		rl_rotate = -1;
	    } else if (e.key.keysym.sym == SDLK_UP) {
		ud_rotate = 1;
            } else if (e.key.keysym.sym == SDLK_DOWN) {
	    	ud_rotate = -1;
	    }
	    if (rl_rotate != 0) {
		//yaw
		for (int i = 0; i < 8; i++) {
		    Vec3 p = block.v3[i];
		    block.v3[i] = rotateYaw(p, rl_rotate * angle);
		}
		//erase old
		//redraw
		SDL_FillRect(psurface, &erase_rect, 0x00000000);
		print_object(psurface, block.v3, 8);
		SDL_UpdateWindowSurface(pwindow);
	    } else if (ud_rotate != 0) {
		//pitch
	    	for (int i = 0; i < 8; i++) {
		    Vec3 p = block.v3[i];
		    block.v3[i] = rotatePitch(p, ud_rotate * angle);
		}
		//erase old
		//redraw
			
		SDL_FillRect(psurface, &erase_rect, 0x00000000);
		print_object(psurface, block.v3, 8);
		SDL_UpdateWindowSurface(pwindow);
	    }
    	   	    


            if (e.type == SDL_QUIT)
                running = 0;
        }
    }
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    
    return 0;
}

