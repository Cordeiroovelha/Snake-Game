#include <raylib.h>
#include <deque>
#include <raymath.h>

constexpr int cellSize{24}, cellCound{20};
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
    void Draw(){
        for (unsigned int i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{x*cellSize, y*cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
        
    }

    void Update(){
        body.pop_back();
        body.push_front(Vector2Add(body[0], direction));
    }
};

class Food {
private:
    Texture2D texture;
public:    
    Vector2 position;

    void Draw(){
        DrawTexture(texture, position.x * cellSize, position.y* cellSize, WHITE);
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
        Image image = LoadImage("Graphics/food.png");
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

    void Draw(){
        food.Draw();
        snake.Draw();
    }

    void Update(){
        snake.Update();
        CheckCollisionsWithFood();
    }

    void CheckCollisionsWithFood(){
        if(Vector2Equals(snake.body[0], food.position)){
            food.position = food.GenerateRandomPos(snake.body);
        }
    }

    Game() : food(snake.body) {}
};


int main() {
    
    // inicialização da tela com as medidas, o nome do programa e o fps
    InitWindow(cellSize * cellCound, cellSize * cellCound, "Snake Game");
    SetTargetFPS(60);

    Game game;

    // Game Loop
    while (!WindowShouldClose()) {
        // Event Handling
        

        // Updating Positions
        if(eventTriggered(0.2))
            game.Update();

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
            game.snake.direction = {0, -1};
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
            game.snake.direction = {0, 1};
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
            game.snake.direction = {-1, 0};
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
            game.snake.direction = {1, 0};
        

        // Drawing
        BeginDrawing();
            ClearBackground(green);
            game.Draw();
        EndDrawing();

    }

    CloseWindow();
    return 0;
}