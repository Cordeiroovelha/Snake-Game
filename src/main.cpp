#include <raylib.h>
#include <deque>
#include <raymath.h>

constexpr int cellSize{24}, cellCound{20}, offSet{60};
constexpr Color green = {173, 204, 96, 255};
constexpr Color darkGreen = {43, 51, 24, 255};
double lastUpdateTime{0};

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool ElementInDeck(Vector2 elements, std::deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if(Vector2Equals(deque[i], elements))
            return true;
    }
    return false;
}

class Snake {
private:
    
public:
    Vector2 direction = {1, 0};
    std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    bool addSegment = false;


    void Draw(){
        for (unsigned int i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offSet+ x* cellSize, offSet+ y* cellSize, 
                (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
        
    }

    void Update(){
        body.push_front(Vector2Add(body[0], direction));
        
        if (addSegment == true)    
            addSegment = false;
        else
            body.pop_back();
    }

    void Reset(){
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        direction = {1, 0};
    }
};

class Food {
private:
    Texture2D texture;
public:    
    Vector2 position;

    void Draw(){
        DrawTexture(texture, offSet+ position.x * cellSize, offSet+ position.y* cellSize, WHITE);
    }

    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0, cellCound - 1);
        float y = GetRandomValue(0, cellCound - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody){
        Vector2 position = GenerateRandomCell();
        while (ElementInDeck(position, snakeBody))
            position = GenerateRandomCell();
        
        return position;
    }

    Food(std::deque<Vector2> snakeBody) {
        Image image = LoadImage("Graphics/apple.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food() { UnloadTexture(texture); }

};

class Game {
private:
    
public:
    Snake snake;
    Food food;
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    void Draw(){
        food.Draw();
        snake.Draw();
    }

    void Update(){
        if (running){
            snake.Update();
            CheckCollisionsWithFood();
            CheckCollisionsWithWall();
            CheckCollisionsWithBody();
        }
    }

    void CheckCollisionsWithFood(){
        if(Vector2Equals(snake.body[0], food.position)){
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionsWithWall(){
        if(snake.body[0].x == cellCound || snake.body[0].x == -1)
            GameOver();
        if(snake.body[0].y == cellCound || snake.body[0].y == -1)
            GameOver();
    }

    void CheckCollisionsWithBody(){
        std::deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if(ElementInDeck(snake.body[0], headlessBody))
            GameOver();
    }

    void GameOver(){
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    Game() : food(snake.body) {
        InitAudioDevice();
        eatSound = LoadSound("Sounds/food.mp3");
        wallSound = LoadSound("Sounds/gameover.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }
};

int main() {
    
    // inicialização da tela com as medidas, o nome do programa e o fps
    InitWindow(2* offSet + cellSize* cellCound, 2* offSet + cellSize* cellCound, "Snake Game");
    SetTargetFPS(60);

    Game game;

    // Game Loop
    while (!WindowShouldClose()) {
        // Updating Positions
        if(eventTriggered(0.2))
            game.Update();

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0, 1};
            game.running = true;
        }   
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1, 0};
            game.running = true;
        }
        
        // Drawing
        BeginDrawing();
        DrawRectangleLinesEx(Rectangle{(float)offSet-5, (float)offSet-5, 
            (float)cellSize*cellCound+10, (float)cellSize*cellCound+10},
            5, darkGreen);
        ClearBackground(green);
        // UI
        DrawText("Retro Snake", offSet-5, 16, 32, darkGreen);
        DrawText("Score:", cellCound* cellSize - (offSet + 20), 16, 32, darkGreen);
        DrawText(TextFormat("%i", game.score), cellCound* cellSize + offSet / 2, 16, 32, darkGreen);
        // Objs
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}