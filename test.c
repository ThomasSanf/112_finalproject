#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

// Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Game element dimensions
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int BALL_SIZE = 15;
const int BRICK_WIDTH = 80;
const int BRICK_HEIGHT = 30;
const int NUM_BRICKS = 20;

// Paddle movement speed
const int PADDLE_SPEED = 30;

// Structures for game elements
typedef struct {
    float x, y;
    float width, height;
} Paddle;

typedef struct {
    float x, y;
    float dx, dy;
    float size;
} Ball;

typedef struct {
    int x, y;
    int width, height;
    bool active;
} Brick;

bool gameOver = false;
int score = 0;

// Initialize game elements
void initPaddle(Paddle *paddle) {
    paddle->width = PADDLE_WIDTH;
    paddle->height = PADDLE_HEIGHT;
    paddle->x = (SCREEN_WIDTH - PADDLE_WIDTH) / 2;
    paddle->y = SCREEN_HEIGHT - PADDLE_HEIGHT - 10;
}

void initBall(Ball *ball) {
    ball->size = BALL_SIZE;
    ball->x = SCREEN_WIDTH / 2;
    ball->y = SCREEN_HEIGHT - PADDLE_HEIGHT - BALL_SIZE - 20;
    ball->dx = 0.2; // Horizontal speed
    ball->dy = -0.2; // Vertical speed
}

void initBricks(Brick bricks[]) {
    for (int i = 0; i < NUM_BRICKS; ++i) {
        bricks[i].width = BRICK_WIDTH;
        bricks[i].height = BRICK_HEIGHT;
        bricks[i].x = (i % 10) * (BRICK_WIDTH + 5) + 15;
        bricks[i].y = (i / 10) * (BRICK_HEIGHT + 5) + 15;
        bricks[i].active = true;
    }
}

// Function to draw paddle
void drawPaddle(SDL_Surface *surface, Paddle *paddle) {
    SDL_Rect rect = {paddle->x, paddle->y, paddle->width, paddle->height};
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));
}

// Function to draw ball
void drawBall(SDL_Surface *surface, Ball *ball) {
    SDL_Rect rect = {ball->x - ball->size / 2, ball->y - ball->size / 2, ball->size, ball->size};
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));
}

// Function to draw bricks
void drawBricks(SDL_Surface *surface, Brick bricks[]) {
    for (int i = 0; i < NUM_BRICKS; ++i) {
        if (bricks[i].active) {
            SDL_Rect rect = {bricks[i].x, bricks[i].y, bricks[i].width, bricks[i].height};
            SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 0, 0));
        }
    }
}

// Function to check collision between ball and brick
bool checkCollision(Ball ball, Brick brick) {
    if (ball.x + ball.size > brick.x &&
        ball.x < brick.x + brick.width &&
        ball.y + ball.size > brick.y &&
        ball.y < brick.y + brick.height) {
        return true;
    }
    return false;
}

// Function to handle ball-brick collisions
void handleBallBrickCollisions(Ball *ball, Brick bricks[]) {
    for (int i = 0; i < NUM_BRICKS; ++i) {
        if (bricks[i].active && checkCollision(*ball, bricks[i])) {
            bricks[i].active = false;  // Deactivate the brick
            ball->dy = -ball->dy;      // Change the ball's direction
            score++;                   // Increase score
        }
    }
}

// Function to display text
void displayText(SDL_Surface *surface, TTF_Font *font, const char *text, SDL_Color color, int x, int y) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_BlitSurface(textSurface, NULL, surface, &textRect);
    SDL_FreeSurface(textSurface);
}

int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Get window surface
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    // Load fonts
    TTF_Font* font = TTF_OpenFont("dvs.ttf", 1); // Replace with your font path
    if (!font) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Game elements
    Paddle paddle;
    Ball ball;
    Brick bricks[NUM_BRICKS];

    // Initialize game elements
    initPaddle(&paddle);
    initBall(&ball);
    initBricks(bricks);

    // Main game loop
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Handle paddle movement
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if (state[SDL_SCANCODE_LEFT]) {
                paddle.x -= PADDLE_SPEED;
                if (paddle.x < 0) {
                    paddle.x = 0;
                }
            }
            if (state[SDL_SCANCODE_RIGHT]) {
                paddle.x += PADDLE_SPEED;
                if (paddle.x > SCREEN_WIDTH - paddle.width) {
                    paddle.x = SCREEN_WIDTH - paddle.width;
                }
            }
        }

        // Update ball position
        ball.x += ball.dx;
        ball.y += ball.dy;

        // Collision with walls
        if (ball.x <= 0 || ball.x >= SCREEN_WIDTH - BALL_SIZE) {
            ball.dx = -ball.dx;
        }
        if (ball.y <= 0) {
            ball.dy = -ball.dy;
        }
        if (ball.y >= SCREEN_HEIGHT - BALL_SIZE) {
            gameOver = true;
        }

        // Collision with paddle
        if (ball.y + BALL_SIZE >= paddle.y &&
            ball.x + BALL_SIZE > paddle.x &&
            ball.x < paddle.x + PADDLE_WIDTH) {
            ball.dy = -ball.dy;
        }

        // Handle ball-brick collisions
        handleBallBrickCollisions(&ball, bricks);

        // Clear screen
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));

        // Draw game elements
        drawPaddle(screenSurface, &paddle);
        drawBall(screenSurface, &ball);
        drawBricks(screenSurface, bricks);

        // Display score
        char scoreText[100];
        sprintf(scoreText, "Score: %d", score);
        displayText(screenSurface, font, scoreText, (SDL_Color){255, 255, 255, 255}, 10, 10);

        // Check for game over
        if (gameOver) {
            displayText(screenSurface, font, "Game Over! Press R to Restart", (SDL_Color){255, 255, 255, 255}, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2);

            // Restart game if 'R' is pressed
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if (state[SDL_SCANCODE_R]) {
                gameOver = false;
                score = 0;
                initBall(&ball);
                initBricks(bricks);
            }
        }

        // Update the surface
        SDL_UpdateWindowSurface(window);
    }

    // Cleanup
    TTF_CloseFont(font);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
