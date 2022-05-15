#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>

#include <glad/glad.h>

//#include <SDL_ttf.h>

struct Vector2 {
    float x;
    float y;
};

bool Initialize();

void InitSDLText();

void ShutDown();

void RunLoop();

void ProcessInput();

void UpdateGame();

void GenerateOutput();

void DrawWalls();

void DrawGameObjects();

void InitSDLText();

void ResetGame(Vector2& mBallPos, Vector2& mBallVel, float& moveSpeed, float& ballSpeedTimer);

void CheckForPaddleCollision(Vector2 const& ball, Vector2 const& paddle, Vector2& mBallVel, float const& thickness, float const& paddleH, float const& moveSpeed, float& randomNumber,bool& randomNumberChanged);

void CheckForPoints(Vector2& mBallPos, Vector2& mBallVel, int& playerScore, int& AIScore, float& moveSpeed, bool& startRound, float& AIMoveSpeed, float& ballSpeedTimer);

void FrameRateIndependence(float& diff, float& deltaTime, Uint32& mTicksCount);

void PlayerPaddleMovement (Vector2& mPaddlePos, int& mPaddleDir, float& paddleH, float& deltaTime, int const& thickness);

void BallMovement(Vector2& mBallPos, Vector2& mBallVel, bool& startRound, float& deltaTime);

void AIPaddleMovement(Vector2& mBallPos, Vector2& mAIPaddlePos, Vector2& mBallVel, int const& thickness, float& paddleH, float& AIMoveSpeed, float& deltaTime, float& randomNumber, bool& startRound);

void CollisionDetection(Vector2& mBallPos, Vector2& mBallVel, Vector2& mPaddlePos, Vector2& mAIPaddlePos, int const& thickness, float& paddleH, float& randomFactor,float& moveSpeed,float& randomNumber,bool& randomNumberChanged, float& deltaTime, float& ballSpeedTimer, bool& startRound);

float MinFloat(float a, float b);

Vector2 MoveTowards(Vector2 currentPosition, Vector2 targetPosition, float maxDistanceDelta);

class Game {
    public:
        Game():mWindow(nullptr), mIsRunning(true) {
            mTicksCount = 0;
        }

        bool Initialize();
        void RunLoop();
        void ShutDown();
    
    private:
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        //SDL_Texture* textureText;
        //SDL_Surface* surfaceText;
        //TTF_Font* ourFont;


        Vector2 mBallPos = {501,384};
        Vector2 mPaddlePos = {20,350};
        Vector2 mAIPaddlePos = {984,350};
        Vector2 mBallVel = {-250,0};

        Uint32 mTicksCount;
        
        bool mIsRunning, startRound, goingStraight, movingUp = true, randomNumberChanged = false;

        float moveSpeed = 500.0f, AIMoveSpeed = 200.0f, randomFactor = 1.0f, min = 1.0f, paddleH, diff,deltaTime, ballSpeedTimer, randomNumber;

        const int thickness = 20;
        int playerScore = 0, AIScore = 0, mPaddleDir;

        SDL_Rect mNetRects[25];

        void ProcessInput();
        void UpdateGame();
        void GenerateOutput();
        void DrawWalls();
        void DrawGameObjects();
        void InitSDLText();
};

bool Game::Initialize() {
    //random number generator seed
    srand (time(NULL));

    //initialize SDL library
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if(sdlResult != 0) {
        //initialization failed. output error message to console
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    //if initialization successful, create window
    mWindow = SDL_CreateWindow(
        "Pong", // window title
        100, // top left x-coordinate of window
        100, // top left y-coordinate of windw
        1024, // width of window
        768, // height of window
        0 // Flags
    );

    if(!mWindow) {
        //if creating window failed
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow, //window to create renderer for
        -1, //let SDL decide which graphics driver to use
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC //initialization flags. use accelerated renderer and enable vsync
    );

    //initialize text
    InitSDLText();

    ResetGame(mBallPos,mBallVel,moveSpeed,ballSpeedTimer);

    //if window and initialization successful, return true
    return true;
}

void Game::InitSDLText() {
    //initialize, else load error
    /* if(TTF_Init() == -1) {
        std::cout << "Failed to initialize text.Error: " << TTF_GetError() << std::endl;
    } else {
        std::cout << "Text good to go!" << std::endl;
    }

    //load font file and set size
    ourFont = TTF_OpenFont("./peepo/Peepo.ttf",32);
    //confirm font loaded
    if(ourFont == nullptr) {
        std::cout << "Could not load font" << std::endl;
        exit(1);
    }

    SDL_Color textColor = {255,255,255};

    //pixels from text
    surfaceText = TTF_RenderText_Solid(ourFont,"Hello World!",textColor);

    //set up texture
    textureText = SDL_CreateTextureFromSurface(mRenderer,surfaceText);

    SDL_FreeSurface(surfaceText); */
}

void Game::ShutDown() {
    //destroy SDL_Window
    SDL_DestroyWindow(mWindow);
    //destroy texture
    //SDL_DestroyTexture(textureText);
    //close font
    //TTF_CloseFont(ourFont);
    //destroy SDL_Renderer
    SDL_DestroyRenderer(mRenderer);

    //close SDL
    SDL_Quit();
}

void Game::ProcessInput() {
    SDL_Event event;

    //go through all events and respond as desired/appropriate
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                mIsRunning = false;
                break;
        }
    }

    // Retrieve the state of all of the keys then scan each as desired
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_ESCAPE]){
        mIsRunning = false;
    }
    if(state[SDL_SCANCODE_SPACE] && !startRound) {
        startRound = true;
    }

    //update paddle movement
    mPaddleDir = 0;

    if(state[SDL_SCANCODE_UP]) {
        mPaddleDir -= 1;
    } else if(state[SDL_SCANCODE_DOWN]) {
        mPaddleDir += 1;
    }
}

void Game::UpdateGame() {
    //frame rate independence
    FrameRateIndependence(diff,deltaTime,mTicksCount);

    //update game world
    //player paddle movement
    PlayerPaddleMovement(mPaddlePos,mPaddleDir,paddleH,deltaTime,thickness);

    //update ball position
    BallMovement(mBallPos,mBallVel,startRound,deltaTime);

    //update AI paddle
    //determine AI paddle movement direction
    //move AI Paddle
    AIPaddleMovement(mBallPos,mAIPaddlePos,mBallVel,thickness,paddleH,AIMoveSpeed,deltaTime,randomNumber,startRound);
    
    CollisionDetection(mBallPos,mBallVel,mPaddlePos,mAIPaddlePos,thickness,paddleH,randomFactor,moveSpeed,randomNumber,randomNumberChanged,deltaTime,ballSpeedTimer,startRound);

    CheckForPoints(mBallPos,mBallVel,playerScore,AIScore,moveSpeed,startRound,AIMoveSpeed,ballSpeedTimer);
}

void Game::GenerateOutput() {
    //graphics and audio related code
    DrawWalls();

    DrawGameObjects();

    //create rectangle to draw on
    //SDL_Rect textDisplay = {200,300,thickness,thickness * 5};

    //render text on rectangle
    //SDL_RenderCopy(mRenderer,textureText,NULL,&textDisplay);

    SDL_RenderPresent(mRenderer); //swap front and back buffers

}

void Game::RunLoop() {
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::DrawWalls() {
    SDL_SetRenderDrawColor( //pointer, RGBA
        mRenderer,
        0,
        0,
        0,
        0
    );

    //draw game scene
    SDL_SetRenderDrawColor( //pointer, RGBA
        mRenderer,
        255,
        255,
        255,
        255
    );

    // Draw the net
    for(int i = 0; i < 25; i++) {
        SDL_Rect x  = { 508, 50 * i, thickness / 4.0f, thickness};
        mNetRects[i] = x;
    }

    //walls
    SDL_Rect topWall = { 0, 0, 1024, thickness};
    SDL_Rect bottomWall = { 0, 768 - thickness, 1024, thickness};

    SDL_RenderFillRect(mRenderer, &topWall);
    SDL_RenderFillRect(mRenderer, &bottomWall);
    for(int i = 0; i < 25; i++) {
        SDL_RenderFillRect(mRenderer, &mNetRects[i]);
    }
}

void Game::DrawGameObjects() {
    //ball and paddle
    SDL_Rect ball = {
        static_cast<int>(mBallPos.x),
        static_cast<int>(mBallPos.y),
        thickness,
        thickness
    };

    paddleH = thickness * 5;

    SDL_Rect playerPaddle = {
        static_cast<int>(mPaddlePos.x),
        static_cast<int>(mPaddlePos.y),
        thickness,
        paddleH
    };

    SDL_Rect AIPaddle = {
        static_cast<int>(mAIPaddlePos.x),
        static_cast<int>(mAIPaddlePos.y),
        thickness,
        paddleH
    };

    SDL_RenderFillRect(mRenderer, &ball);
    SDL_RenderFillRect(mRenderer, &playerPaddle);
    SDL_RenderFillRect(mRenderer, &AIPaddle);
}

void CollisionDetection(Vector2& mBallPos, Vector2& mBallVel, Vector2& mPaddlePos, Vector2& mAIPaddlePos, int const& thickness, float& paddleH, float& randomFactor,float& moveSpeed, float& randomNumber,bool& randomNumberChanged, float& deltaTime, float& ballSpeedTimer, bool& startRound) {
    if(startRound) {
        ballSpeedTimer += deltaTime;
    }

    //check for collisions
    CheckForPaddleCollision(mBallPos,mPaddlePos,mBallVel,thickness,paddleH,moveSpeed,randomNumber,randomNumberChanged);
    CheckForPaddleCollision(mBallPos,mAIPaddlePos,mBallVel,thickness,paddleH,moveSpeed,randomNumber,randomNumberChanged);

    if(mBallPos.y <= thickness && mBallVel.y < 0.0f) { //if moving upwards and hit wall
        mBallVel.y *= -1;
        if(ballSpeedTimer >= 5.0f) {
            ballSpeedTimer = 0.0f;
            moveSpeed += 2.5f;
        }
    } 
    if(mBallPos.y >= (768.0f - 2.0f * thickness) && mBallVel.y > 0.0f) { //if moving downards and hit wall
        mBallVel.y *= -1;
    }

}

void BallMovement(Vector2& mBallPos, Vector2& mBallVel, bool& startRound, float& deltaTime) {
    if(!startRound) { return; }
    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;
}

void PlayerPaddleMovement (Vector2& mPaddlePos, int& mPaddleDir, float& paddleH, float& deltaTime, int const& thickness) {
    if(mPaddleDir != 0) {
        mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
        //clamp paddlePos
        if(mPaddlePos.y < thickness) {
            mPaddlePos.y = thickness;
        } else if(mPaddlePos.y > 768.0f - thickness - paddleH) {
            mPaddlePos.y = 768.0f - thickness - paddleH;
        }
    }
}

float MinFloat(float a, float b) {
    if(a < b) {
        return a;
    } else {
        return b;
    }
}

void AIPaddleMovement(Vector2& mBallPos, Vector2& mAIPaddlePos, Vector2& mBallVel, int const& thickness, float& paddleH, float& AIMoveSpeed, float& deltaTime, float& randomNumber, bool& startRound) {

    if(mBallVel.x < 0.0f || !startRound) { return; }

    float paddleMid = mAIPaddlePos.y + (paddleH / 2.0f);
    float ballMid = mBallPos.y + (thickness / 2.0f);

    float targetY = ballMid - randomNumber * thickness;

    Vector2 targetPosition = {mAIPaddlePos.x,targetY};

    mAIPaddlePos = MoveTowards(mAIPaddlePos, targetPosition, AIMoveSpeed * deltaTime);

    if(mAIPaddlePos.y < thickness) {
        mAIPaddlePos.y = thickness;
    } else if(mAIPaddlePos.y > 768.0f - thickness - paddleH) {
        mAIPaddlePos.y = 768.0f - thickness - paddleH;
    }
}

Vector2 MoveTowards(Vector2 currentPosition, Vector2 targetPosition, float maxDistanceDelta) {
     Vector2 a = {targetPosition.x - currentPosition.x, targetPosition.y - currentPosition.y};
     float magnitude = sqrt((a.x)*(a.x) + (a.y)*(a.y));
     if (magnitude <= maxDistanceDelta || magnitude == 0.0f) {
        return targetPosition;
     }
     Vector2 deltaV = {a.x / (magnitude * maxDistanceDelta), (a.y / magnitude) * maxDistanceDelta };
     Vector2 sum = {currentPosition.x, currentPosition.y + deltaV.y};
     return sum;
}

void CheckForPoints(Vector2& mBallPos, Vector2& mBallVel, int& playerScore, int& AIScore, float& moveSpeed, bool& startRound, float& AIMoveSpeed, float& ballSpeedTimer) {
    //check if player outscored
    if(mBallPos.x < -1.0f) {
        //AI scored
        AIScore++;
        if(playerScore < 5 && AIScore < 5) {
            ResetGame(mBallPos,mBallVel,moveSpeed,ballSpeedTimer);
            startRound = false;
        } else if(playerScore >= 5 || AIScore >= 5) {
            AIMoveSpeed = 0.0f;
        }
    } else if(mBallPos.x > 1030.0f) {
        //player scored
        playerScore++;
        AIMoveSpeed += 50.0f;
        if(playerScore < 5 && AIScore < 5) {
            ResetGame(mBallPos,mBallVel,moveSpeed,ballSpeedTimer);
            startRound = false;
        } else if(playerScore >= 5 || AIScore >= 5) {
            AIMoveSpeed = 0.0f;
        }
    }
}

void ResetGame(Vector2& mBallPos, Vector2& mBallVel, float& moveSpeed, float& ballSpeedTimer) {
    mBallPos.x = 501;
    mBallPos.y = 384;

    //stack overflow
    float LO = 200.0f, HI = 250.0F;

    float r1 = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    float r2 = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));

    int r3 = rand() % 3; //random int in [0,2)

    if(r3 == 0) {
        r1 *= -1.0f;
    } else if(r3 == 1) {
        r2 *= -1.0f;
    } else {
        r1 *= -1.0f;
        r2 *= -1.0f;
    }

    mBallVel.x = r1;
    mBallVel.y = r2;

    ballSpeedTimer = 0.0f;
}

void CheckForPaddleCollision(Vector2 const& ball, Vector2 const& paddle, Vector2& mBallVel, float const& thickness, float const& paddleH, float const& moveSpeed, float& randomNumber, bool& randomNumberChanged) {
    float ballLeft = ball.x;
	float ballRight = ball.x + thickness;
	float ballTop = ball.y;
	float ballBottom = ball.y + thickness;

	float paddleLeft = paddle.x;
	float paddleRight = paddle.x + thickness;
	float paddleTop = paddle.y;
	float paddleBottom = paddle.y + paddleH;

    float paddleMid = paddle.y + 2.5f * thickness;
    float ballMid = ball.y + 0.5f * thickness;

	if (ballLeft >= paddleRight) { return; }

	if (ballRight <= paddleLeft) { return; }

	if (ballTop >= paddleBottom) { return; }

	if (ballBottom <= paddleTop) { return; }

    float relativeIntersectY = paddleMid - ballMid; //between -32 and 32

    float normalizedRelativeIntersectionY = (relativeIntersectY/(paddleH/2.0f)); //between -1 and 1 -- normalized

    float MAXBOUNCEANGLE = (5.0f * M_PI) / 12.0f;

    float bounceAngle = normalizedRelativeIntersectionY * MAXBOUNCEANGLE;

    float paddleType = 1.0f;

    if(paddleRight > 500.0f) {
        paddleType = -1.0f;
    } else if(paddleRight < 500.0f) {
        randomNumberChanged = false;
    }

    if(!randomNumberChanged) {
        randomNumberChanged = true;
        randomNumber = 0.75f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(4.25f - 0.75f)));
    }

    mBallVel.x = moveSpeed * cos(bounceAngle) * paddleType;
    mBallVel.y = moveSpeed * sin(bounceAngle) * -1.0f;
}

void FrameRateIndependence(float& diff, float& deltaTime, Uint32& mTicksCount) {
    diff = 0;
    //update frame at fixed intervals (fixedDeltaTime)
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    //get deltaTime
    deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    mTicksCount = SDL_GetTicks();

    //clamp deltaTime
    if(deltaTime > .05f) {
        deltaTime = 0.05f;
    }
}
