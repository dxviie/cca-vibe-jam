// Skeleton raylib game: one source file that builds for desktop and for the web.

#include "raylib.h"

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

// The scene is laid out for a 450px tall screen and scaled by the shorter
// screen side, so it keeps its proportions at any window size or aspect ratio
#define DESIGN_SIZE   450.0f
#define SPRITE_SCALE  4.0f

static Texture2D sprite;
static Vector2 position;
static Vector2 velocity = { 220.0f, 160.0f };   // In design pixels per second
static bool paused = false;

static void UpdateDrawFrame(void);

int main(void)
{
#if defined(__EMSCRIPTEN__)
    // Start at the size of the browser window, raylib keeps the canvas matched to it on resize
    int width = EM_ASM_INT({ return window.innerWidth; });
    int height = EM_ASM_INT({ return window.innerHeight; });
#else
    int width = 800;
    int height = 450;
#endif

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "cca vibe jam");

#if !defined(__EMSCRIPTEN__)
    SetWindowMinSize(320, 240);

    // Load assets relative to the executable, wherever it is launched from.
    // On the web, assets/ is preloaded into the virtual filesystem root instead.
    ChangeDirectory(GetApplicationDirectory());
#endif

    sprite = LoadTexture("assets/slime.png");
    position = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };

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

static float Clampf(float value, float min, float max)
{
    return (value < min)? min : (value > max)? max : value;
}

static void UpdateDrawFrame(void)
{
    // Update
    float dt = GetFrameTime();
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    float unit = ((sw < sh)? sw : sh)/DESIGN_SIZE;   // Screen pixels per design pixel
    float w = sprite.width*SPRITE_SCALE*unit;
    float h = sprite.height*SPRITE_SCALE*unit;

    if (IsKeyPressed(KEY_SPACE)) paused = !paused;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) position = GetMousePosition();   // Taps count as clicks

    if (!paused)
    {
        position.x += velocity.x*unit*dt;
        position.y += velocity.y*unit*dt;
    }

    // Keep the slime on screen when the window shrinks
    position.x = Clampf(position.x, w/2, sw - w/2);
    position.y = Clampf(position.y, h/2, sh - h/2);

    if ((position.x - w/2 <= 0 && velocity.x < 0) || (position.x + w/2 >= sw && velocity.x > 0)) velocity.x *= -1;
    if ((position.y - h/2 <= 0 && velocity.y < 0) || (position.y + h/2 >= sh && velocity.y > 0)) velocity.y *= -1;

    // Draw
    int margin = (int)(20*unit);
    int titleSize = (int)(30*unit);
    int textSize = (int)(20*unit);

    BeginDrawing();
        ClearBackground((Color){ 30, 30, 46, 255 });

        DrawTextureEx(sprite, (Vector2){ position.x - w/2, position.y - h/2 }, 0.0f, SPRITE_SCALE*unit, WHITE);

        DrawText("Hello from raylib!", margin, margin, titleSize, RAYWHITE);
        DrawText("Click or tap to move the slime", margin, margin + titleSize + textSize/2, textSize, LIGHTGRAY);
        DrawText("SPACE to pause", margin, margin + titleSize + 2*textSize, textSize, LIGHTGRAY);
        if (paused) DrawText("PAUSED", (int)sw/2 - MeasureText("PAUSED", 2*textSize)/2, (int)sh - margin - 2*textSize, 2*textSize, GOLD);
        DrawFPS((int)sw - margin - 80, (int)sh - margin - 20);
    EndDrawing();
}
