#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>
#include "constants.h"

///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////
int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

///////////////////////////////////////////////////////////////////////////////
// Declare two game objects for the ball and the paddle
///////////////////////////////////////////////////////////////////////////////
struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

///////////////////////////////////////////////////////////////////////////////
// Function to initialize our SDL window
///////////////////////////////////////////////////////////////////////////////
int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return false;
    }
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function to poll SDL events and process keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game_is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            game_is_running = false;
        }
        // TODO: Set paddle velocity based on left/right arrow keys
        // ...
        if (event.key.keysym.sym == SDLK_LEFT) {
            paddle.vel_x = -300;       

        }

        if (event.key.keysym.sym == SDLK_RIGHT) {
            paddle.vel_x = 300;
        }

        break;
    case SDL_KEYUP:
        // TODO: Reset paddle velocity based on left/right arrow keys
        // ...
        if (event.key.keysym.sym == SDLK_LEFT) {
            
            //this prevents the paddle of getting stuck for a while when the player press left 
            //and right at the same time and then releases any direction key.
            if (paddle.vel_x < 0) {
                paddle.vel_x = 0;              
            }           
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
            if (paddle.vel_x > 0) {
                paddle.vel_x = 0;             
            }
        }   
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Setup function that runs once at the beginning of our program
///////////////////////////////////////////////////////////////////////////////
void setup(void) {
    // Initialize the ball object moving down at a constant velocity
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 10;
    ball.vel_y = 10;
    // Initialize the paddle position at the bottom of the screen
    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}


bool ball_right_wall_collision(void) {

    if (ball.x + ball.width >= WINDOW_WIDTH) {
        return true;
    }       
    return false;
}

bool ball_left_wall_collision(void) {

    if (ball.x <= 0) {
        return true;
    }
    return false;
}

bool ball_top_wall_collision(void) {

    if (ball.y <=0) {
        return true;
    }
    return false;
}

bool ball_bottom_wall_collision(void) {

    if (ball.y+ball.height>=WINDOW_HEIGHT) {
        return true;
    }
    return false;
}


bool ball_paddle_collision(void) {

    if (ball.x < paddle.x + paddle.width &&
        ball.x+ball.width>paddle.x &&
        ball.y<paddle.y+paddle.height &&
        ball.y+ball.height>paddle.y) {

        printf("Collision detected!\n");
        return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// Update function with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void) {
    // Calculate how much we have to wait until we reach the target frame time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    // Only delay if we are too fast too update this frame
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    // Get delta_time factor converted to seconds to be used to update objects
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame to be used in the next one
    last_frame_time = SDL_GetTicks();

    // Update ball position based on its velocity
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;

    // TODO: Update paddle position based on its velocity
    paddle.x += paddle.vel_x * delta_time;

    // TODO: Check for ball collision with the walls  

    if (ball_left_wall_collision() || ball_right_wall_collision()) {
        ball.vel_x *= -1;
    }

    if (ball_top_wall_collision()) {
        ball.vel_y *= -1;
    }

    if (ball_bottom_wall_collision()) {
        game_is_running = false;
    }

    // TODO: Check for ball collision with the paddle
    if (ball_paddle_collision()) {
        ball.vel_y *= -1;
    }

    // TODO: Check for game over when ball hits the bottom part of the screen
    // ...
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw a rectangle for the ball object
    SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    // Draw a rectangle for the paddle object
    SDL_Rect paddle_rect = {
        (int)paddle.x,
        (int)paddle.y,
        (int)paddle.width,
        (int)paddle.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

    SDL_RenderPresent(renderer);
}

///////////////////////////////////////////////////////////////////////////////
// Function to destroy SDL window and renderer
///////////////////////////////////////////////////////////////////////////////
void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* args[]) {
    game_is_running = initialize_window();

    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
