#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Screen dimension constants
const int SCREEN_WIDTH = 880;
const int SCREEN_HEIGHT = 800;

// Game element dimensions
const int PADDLE_WIDTH = 400;
const int PADDLE_HEIGHT = 20;
const int BALL_SIZE = 15;
const int BRICK_WIDTH = 80;
const int BRICK_HEIGHT = 30;
const int NUM_BRICKS = 20;

// Paddle movement speed
const int PADDLE_SPEED = 20;

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

// Function to get the best score from the file
int getBestScore() {
    int bestScore = 0;
    FILE *file = fopen("scores.txt", "r");
    if (file) {
        char line[1024];
        if (fgets(line, sizeof(line), file)) {
            bestScore = atoi(line); // Convert string to integer
        }
        fclose(file);
    }
    return bestScore;
}

// Function to update the best score in the file
void updateBestScore(int score) {
    FILE *file = fopen("scores.txt", "w");
    if (file) {
        fprintf(file, "%d\n", score);
        fclose(file);
    }
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
    ball->dx = 10.f; // Horizontal speed
    ball->dy = -10.f; // Vertical speed
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
void drawPaddle(SDL_Renderer *renderer, Paddle *paddle) {
    SDL_Rect rect = { (int)paddle->x, (int)paddle->y, (int)paddle->width, (int)paddle->height };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_RenderFillRect(renderer, &rect);
}

// Function to draw ball
void drawBall(SDL_Renderer *renderer, Ball *ball) {
    SDL_Rect rect = { (int)(ball->x - ball->size / 2), (int)(ball->y - ball->size / 2), (int)ball->size, (int)ball->size };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_RenderFillRect(renderer, &rect);
}

// Function to draw bricks
void drawBricks(SDL_Renderer *renderer, Brick bricks[]) {
    for (int i = 0; i < NUM_BRICKS; ++i) {
        if (bricks[i].active) {
            SDL_Rect rect = { bricks[i].x, bricks[i].y, bricks[i].width, bricks[i].height };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
            SDL_RenderFillRect(renderer, &rect);
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

bool areAllBricksDestroyed(Brick bricks[]) {
    for (int i = 0; i < NUM_BRICKS; ++i) {
        if (bricks[i].active) {
            return false; // If any brick is still active, return false
        }
    }
    return true; // All bricks are destroyed
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
void displayText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int x, int y) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

int main(int argc, char* argv[])
{
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

    // Create renderer for window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load fonts
    TTF_Font* font = TTF_OpenFont("Minecraft.ttf", 28); // Make sure to provide the correct path and size
    if (!font) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int bestScore = getBestScore();
    bool playerWon = false;
    // Set text color as white
    SDL_Color textColor = {255, 255, 255, 255};

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
    bool gameRunning = true;  // Initialize the game as running
    SDL_Event e;
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    paddle.x -= PADDLE_SPEED;
                    if (paddle.x < 0) {
                        paddle.x = 0;
                    }
                    break;
                case SDLK_RIGHT:
                    paddle.x += PADDLE_SPEED;
                    if (paddle.x > SCREEN_WIDTH - PADDLE_WIDTH) {
                        paddle.x = SCREEN_WIDTH - PADDLE_WIDTH;
                    }
                    break;
                    // ... other key handling ...
            }

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_r && !gameRunning) {
                    // Reset the game when 'R' is pressed after Game Over
                    gameRunning = true;
                    gameOver = false;
                    playerWon = false;
                    score = 0;
                    initPaddle(&paddle);
                    initBall(&ball);
                    initBricks(bricks);
                }
            }
        }

        if (gameRunning) {
            // Your existing game logic for handling ball, collisions, and drawing goes here.

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
            if (areAllBricksDestroyed(bricks)) {
                playerWon = true;
                gameOver = true;
                gameRunning = false;
            }
            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
            SDL_RenderClear(renderer);

            // Draw game elements
            drawPaddle(renderer, &paddle);
            drawBall(renderer, &ball);
            drawBricks(renderer, bricks);

            // Display score
            char scoreText[100];
            sprintf(scoreText, "Score: %d", score);
            displayText(renderer, font, scoreText, textColor, 20, 700);

            // Check for game over
            if (gameOver) {
                if (score > bestScore) {
                    bestScore = score;
                    updateBestScore(score); // Update best score in the file
                }
                gameRunning = false;
            }
        }

        // Check for game over
        if (!gameRunning) {
            if (playerWon) {
                displayText(renderer, font, "You Win! Press R to Restart", textColor, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 - 40);
            } else {
                displayText(renderer, font, "Game Over! Press R to Restart", textColor, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 - 40);
            }
            //displayText(renderer, font, "Best Score: %d", textColor, SCREEN_WIDTH - 220, 760);
        }
        // Display best score
        char bestScoreText[100];
        sprintf(bestScoreText, "Best Score: %d", bestScore);
        displayText(renderer, font, bestScoreText, textColor, SCREEN_WIDTH - 220, 700);

        // Update the screen
        SDL_RenderPresent(renderer);

        // Cap the frame rate
        SDL_Delay(16); // Approximately 60 frames per second
    }

    // Cleanup
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
