#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_ttf.h>

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
int bestScore = 0;

// Function to display the end screen
void drawEndScreen(SDL_Surface *surface, int bestScore, TTF_Font *font) {
    // Clear the screen
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));

    // Display game over message
    SDL_Color textColor = { 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Game Over", textColor);
    SDL_Rect textRect = { (SCREEN_WIDTH - textSurface->w) / 2, SCREEN_HEIGHT / 2 - textSurface->h, textSurface->w, textSurface->h };
    SDL_BlitSurface(textSurface, NULL, surface, &textRect);
    SDL_FreeSurface(textSurface);

    // Display the best score
    char bestScoreText[50];
    sprintf(bestScoreText, "Best Score: %d", bestScore);
    textSurface = TTF_RenderText_Solid(font, bestScoreText, textColor);
    textRect = { (SCREEN_WIDTH - textSurface->w) / 2, SCREEN_HEIGHT / 2, textSurface->w, textSurface->h };
    SDL_BlitSurface(textSurface, NULL, surface, &textRect);
    SDL_FreeSurface(textSurface);

    // Display restart instructions
    textSurface = TTF_RenderText_Solid(font, "Press 'R' to Restart", textColor);
    textRect = { (SCREEN_WIDTH - textSurface->w) / 2, SCREEN_HEIGHT / 2 + textSurface->h, textSurface->w, textSurface->h };
    SDL_BlitSurface(textSurface, NULL, surface, &textRect);
    SDL_FreeSurface(textSurface);
}

// Function to handle restart
void restartGame(Paddle *paddle, Ball *ball, Brick bricks[]) {
    // Initialize game elements
    initPaddle(paddle);
    initBall(ball);
    initBricks(bricks);
    score = 0;
    gameOver = false;
}

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
    ball->dx = 0.2; // Reduced horizontal speed
    ball->dy = -0.2; // Reduced vertical speed
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
    SDL_Rect rect = { paddle->x, paddle->y, paddle->width, paddle->height };
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));
}

// Function to draw ball
void drawBall(SDL_Surface *surface, Ball *ball) {
    SDL_Rect rect = { ball->x - ball->size / 2, ball->y - ball->size / 2, ball->size, ball->size };
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));
}

// Function to draw bricks
void drawBricks(SDL_Surface *surface, Brick bricks[]) {
    for (int i = 0; i < NUM_BRICKS; ++i) {
        if (bricks[i].active) {
            SDL_Rect rect = { bricks[i].x, bricks[i].y, bricks[i].width, bricks[i].height };
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
            break;  // Break after the first collision to avoid multiple collisions at once
        }
    }
}

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    screenSurface = SDL_GetWindowSurface(window);

    // Initialize TTF
    if (TTF_Init() < 0) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("your_font.ttf", 36); // Replace "your_font.ttf" with your font file path
    if (font == NULL) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        TTF_Quit();
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
            if (gameOver && state[SDL_SCANCODE_R]) {
                restartGame(&paddle, &ball, bricks);
            }
        }

        if (!gameOver) {
            // Update ball position
            ball.x += ball.dx;
            ball.y += ball.dy;

            // Collision with walls
            if (ball.x <= 0 || ball.x >= SCREEN_WIDTH - ball.size) {
                ball.dx = -ball.dx;
            }
            if (ball.y <= 0) {
                ball.dy = -ball.dy;
            }

            // Collision with paddle
            if (ball.x >= paddle.x && ball.x <= paddle.x + paddle.width && ball.y >= paddle.y && ball.y <= paddle.y + paddle.height) {
                ball.dy = -ball.dy;
            }

            // Handle ball-brick collisions
            handleBallBrickCollisions(&ball, bricks);

            // Ball lost
            if (ball.y >= SCREEN_HEIGHT) {
                // Check if the current score is better than the best score
                if (score > bestScore) {
                    bestScore = score; // Update the best score
                    // writeBestScore(bestScore); // Write the best score to the file (implement this function)
                    printf("New Best Score: %d\n", bestScore);
                }

                // Restart the game
                gameOver = true;
            }
        }

        // Clear screen
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

        if (gameOver) {
            // Display end screen
            drawEndScreen(screenSurface, bestScore, font);
        }
        else {
            // Draw game elements
            drawPaddle(screenSurface, &paddle);
            drawBall(screenSurface, &ball);
            drawBricks(screenSurface, bricks);

            // Update the surface
            SDL_UpdateWindowSurface(window);
        }
    }

    // Destroy window and quit SDL
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
