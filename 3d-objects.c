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

typedef struct Edge {
    Vec3 *pv1, *pv2;
} Edge;

struct Block {
    Vec3 v3[8];	    
	
};
Vec2 project(Vec3 p) {
    float d = 4.0f;
    p.z += 200;
    float project_x = p.x / (p.z + d);
    float project_y = p.y / (p.z + d);
    //printf("Before projection: x=%f, y=%f, z=%f. After projection: x=%f, y=%f \n", p.x, p.y, p.z, project_x, project_y);
    Vec2 res;
    res.x = project_x;
    res.y = project_y;
    return res;
    
}
void scale_and_print(Vec2 p, SDL_Surface* psurface) {
    //prints single point
    int sx = (int)(p.x * SCALE + WIDTH / 2);
    int sy = (int)(p.y * (-1) * SCALE + HEIGHT / 2);
    SDL_Rect pix = (SDL_Rect){sx, sy, 3, 7};
    SDL_FillRect(psurface, &pix, 0xFFFFFFFF);
   // printf("point printed at %d %d of the screen\n", sx, sy);

}
void print_object(SDL_Surface* psurface, Vec3 points[], int n) {
    for (int i = 0; i < n; i++) {
    	Vec3 p = points[i];
	Vec2 projection = project(p);
   	scale_and_print(projection, psurface);
    }
}

Vec3 rotatePitch(Vec3* pp, float angleInRadians) {
    float cosA = cos(angleInRadians);
    float sinA = sin(angleInRadians);
    Vec3 result;

    result.x = pp->x;
    result.y = pp->y * cosA - pp->z * sinA;
    result.z = pp->y * sinA + pp->z * cosA;
    return result;
}

Vec3 rotateYaw(Vec3* pp, float angleInRadians) {
    float cosA = cos(angleInRadians);
    float sinA = sin(angleInRadians);
    Vec3 result;

    result.x = pp->x * cosA + pp->z * sinA;
    result.y = pp->y;
    result.z = -pp->x * sinA + pp->z * cosA;

    return result;
    
}
void draw_edge(SDL_Surface* psurface, Edge e, Uint32 color) {
    ////printf("e.pv1 is %f %f %f, e.pv2 is %f %f %f \n", e.pv1->x, e.pv1->y, e.pv1->z, e.pv2->x, e.pv2->y, e.pv2->z);
    Vec2 v1 = project(*e.pv1); 
    Vec2 v2 = project(*e.pv2);
   // printf("v1 is %f %f, v2 is %f %f \n", v1.x, v1.y, v2.x, v2.y);
    
    //printf("3D coordinates of edge: v1=%f %f %f  v2=%f %f %f Projected coordinates: v1=%f %f  v2=%f %f \n", e.pv1->x, e.pv1->y, e.pv1->z, e.pv2->x, e.pv2->y, e.pv2->z, v1.x, v1.y, v2.x, v2.y);
    float xi = v1.x;
    float yi = v1.y;
    float xf = v2.x;
    float yf = v2.y;
    xi = xi * SCALE + WIDTH / 2;
    xf = xf * SCALE + WIDTH / 2;
    yi = (-1) * yi * SCALE + HEIGHT / 2;
    yf = (-1) * yf * SCALE + HEIGHT / 2;

    float dx = xf - xi;
    float dy = yf - yi;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if (steps == 0) return;

    float x_inc = dx / (float)steps;
    float y_inc = dy / (float)steps;

    float x = xi + x_inc, y = yi + y_inc;
    //printf("Edge started at: %d %d, ended at %d %d \n", xi, yi, xf, yf);
    for(int i = 0; i < steps - 1; i++) {
        SDL_Rect pix = {(int)x, (int)y, 3, 3,};
	SDL_FillRect(psurface, &pix, color);
	x += x_inc;
	y += y_inc;
    }



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
    
    struct Block* pblock = &(struct Block) {
    .v3 = {
            {24, 24, 24}, {24, 24, -24}, {24, -24, 24}, {-24, 24, 24},
            {24, -24, -24}, {-24, 24, -24}, {-24, -24, 24}, {-24, -24, -24}
        }
   };
    Edge edges[28];
    int edge_ix = 0;
    for(int start = 0; start < 8; start++) {
    	for(int end = start + 1; end < 8; end++) {
	    Vec3 *pv1 = &(pblock->v3[start]);
	    Vec3 *pv2 = &(pblock->v3[end]);
	    edges[edge_ix++] = (Edge) {pv1, pv2};
	}

    }




    print_object(psurface, pblock->v3, 8);
    for (int e = 0; e < 28; e++) {
         Edge edge = edges[e];
	 draw_edge(psurface, edge, (0x00FF00 + (e * 600000)) % 0xFFFFFF);
    }

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
		    Vec3* pp = &(pblock->v3[i]);
		    *pp = rotateYaw(pp, rl_rotate * angle);
		}
		//erase old
		//redraw
		SDL_FillRect(psurface, &erase_rect, 0x00000000);
		print_object(psurface, pblock->v3, 8);
		for (int e = 0; e < 28; e++) {
		    Edge edge = edges[e];
		    draw_edge(psurface, edge, (0x0FF000 + (e * 600000)) % 0xFFFFFF);
		}
		SDL_UpdateWindowSurface(pwindow);
	    } else if (ud_rotate != 0) {
		//pitch
	    	for (int i = 0; i < 8; i++) {
		    Vec3* pp = &(pblock->v3[i]);
		    *pp = rotatePitch(pp, ud_rotate * angle);
		}
		//erase old
		//redraw
			
		SDL_FillRect(psurface, &erase_rect, 0x00000000);
		print_object(psurface, pblock->v3, 8);
		for (int e = 0; e < 28; e++) {
		    Edge edge = edges[e];
		    draw_edge(psurface, edge, (0x00FF00 + (e * 600000)) % 0xFFFFFF);
		}
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

