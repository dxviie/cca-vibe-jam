// Skeleton raylib game: one source file that builds for desktop and for the web.

#include "raylib.h"

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 450
#define SPRITE_SCALE  4.0f

static Texture2D sprite;
static Vector2 position;
static Vector2 velocity = { 220.0f, 160.0f };
static bool paused = false;

static void UpdateDrawFrame(void);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "cca vibe jam");

#if !defined(__EMSCRIPTEN__)
    // Load assets relative to the executable, wherever it is launched from.
    // On the web, assets/ is preloaded into the virtual filesystem root instead.
    ChangeDirectory(GetApplicationDirectory());
#endif

    sprite = LoadTexture("assets/slime.png");
    position = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };

#if defined(__EMSCRIPTEN__)
    // The browser owns the loop: it calls UpdateDrawFrame once per animation frame.
    // A blocking while loop would freeze the tab.
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) UpdateDrawFrame();
#endif

    UnloadTexture(sprite);
    CloseWindow();
    return 0;
}

static void UpdateDrawFrame(void)
{
    // Update
    float dt = GetFrameTime();
    float w = sprite.width*SPRITE_SCALE;
    float h = sprite.height*SPRITE_SCALE;

    if (IsKeyPressed(KEY_SPACE)) paused = !paused;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) position = GetMousePosition();

    if (!paused)
    {
        position.x += velocity.x*dt;
        position.y += velocity.y*dt;
    }

    if ((position.x - w/2 < 0 && velocity.x < 0) || (position.x + w/2 > SCREEN_WIDTH && velocity.x > 0)) velocity.x *= -1;
    if ((position.y - h/2 < 0 && velocity.y < 0) || (position.y + h/2 > SCREEN_HEIGHT && velocity.y > 0)) velocity.y *= -1;

    // Draw
    BeginDrawing();
        ClearBackground((Color){ 30, 30, 46, 255 });

        DrawTextureEx(sprite, (Vector2){ position.x - w/2, position.y - h/2 }, 0.0f, SPRITE_SCALE, WHITE);

        DrawText("Hello from raylib!", 20, 20, 30, RAYWHITE);
        DrawText("Click to move the slime, SPACE to pause", 20, 60, 20, LIGHTGRAY);
        if (paused) DrawText("PAUSED", SCREEN_WIDTH/2 - MeasureText("PAUSED", 40)/2, SCREEN_HEIGHT - 70, 40, GOLD);
        DrawFPS(SCREEN_WIDTH - 100, 20);
    EndDrawing();
}
