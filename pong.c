#include "raylib.h"
#include "raymath.h"
#include <bits/getopt_core.h>
#include <stdlib.h>
#include <getopt.h>

#define CLIB_IMPLEMENTATION
#include "clib.h"

#define GAME_NAME "Pong"
#define GAME_VERSION "1.0.0"

#define TARGET_FPS 60
#define TARGET_DT (1.0f/TARGET_FPS)
#define BALL_RADIUS 15.f
#define BALL_VELOCITY 300.0f
#define PLAYER_DISTANCE_FROM_WALL 120.0f
#define PLAYER_LINE_LENGTH 100.0f
#define PLAYER_LINE_THICCNESS 20.0f
#define PLAYER_STEP 20.0f
#define GAME_ENDS_AT 5

typedef struct {
    Vector2 starting;
    Vector2 ending;
} Line;

void move_line(Line* line, float offset){
    line->starting.y += offset;
    line->ending.y += offset;
}

int ball_collides_with_player(Line line, Vector2 ball){
    float paddle_left = line.starting.x - PLAYER_LINE_THICCNESS / 2;
    float paddle_right = line.starting.x + PLAYER_LINE_THICCNESS / 2;
    return (ball.x - BALL_RADIUS <= paddle_right && ball.x + BALL_RADIUS >= paddle_left) &&
           (ball.y + BALL_RADIUS >= line.starting.y && ball.y - BALL_RADIUS <= line.ending.y);
}

typedef struct {
    size_t p1;
    size_t p2;
} Score;

int main(int argc, char** argv){
    size_t game_ends_at = GAME_ENDS_AT;
    size_t width = 800;

    struct option long_options[] = {
        {"points", required_argument, NULL, 'p'},
        {"width", required_argument, NULL, 'w'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int opt;
	while ((opt = getopt_long(argc, argv, "vp:w:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'v':
                printf("%s v%s by KDesp73\n", GAME_NAME, GAME_VERSION);
                exit(0);
                break;
            case 'h':
                printf("Usage: %s [-p <points>] [-w <width>] [-v | -h]\n", argv[0]);

                printf("\n");
                printf("-h, --help\t\t\tPrints this message\n");
                printf("-v, --version\t\t\tPrints the version of this program\n");
                printf("-p, --points\t\t\tSets the number of points that end the game\n");
                printf("-w, --width\t\t\tSets the width of the window (>=500)\n");
                printf("\n");

                printf("Made by KDesp73\n");
                exit(0);
            case 'p':
                game_ends_at = atoi(optarg);
                break;
            case 'w':
                width = atoi(optarg);
                if(width < 500){
                    ERRO("Width too small. Should be above 500");
                    exit(1);
                }
                break;
            default:
                INFO("Usage: %s [-p <points>] [-w <width>] [-v | -h]", argv[0]);
                exit(1);
        }
    }


    InitWindow(width, width / 1.6f, GAME_NAME);
    SetTargetFPS(TARGET_FPS);
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    int game_over = 0;

    Score score = {0};

    Camera2D camera = {
        .zoom = 1.0f
    };

    BeginMode2D(camera);
    
    Vector2 ball = { w/2.0f, h/2.0f };
    Line p1 = {
        .starting = (Vector2) {PLAYER_DISTANCE_FROM_WALL, PLAYER_DISTANCE_FROM_WALL},
        .ending = (Vector2) {PLAYER_DISTANCE_FROM_WALL, PLAYER_DISTANCE_FROM_WALL + PLAYER_LINE_LENGTH}
    };
    Line p2 = {
        .starting = (Vector2) {w-PLAYER_DISTANCE_FROM_WALL, h-(PLAYER_DISTANCE_FROM_WALL + PLAYER_LINE_LENGTH)},
        .ending = (Vector2) {w-PLAYER_DISTANCE_FROM_WALL, h-PLAYER_DISTANCE_FROM_WALL}
    };

    Vector2 velocity = {BALL_VELOCITY, BALL_VELOCITY};
    while (!WindowShouldClose()) {
        BeginDrawing();
        if(!game_over){
            if(score.p1 == game_ends_at || score.p2 == game_ends_at) game_over = 1;

            if(IsKeyDown(KEY_W)){
                move_line(&p1, -PLAYER_STEP);
            } else if (IsKeyDown(KEY_S)) {
                move_line(&p1, PLAYER_STEP);
            }

            if(IsKeyDown(KEY_UP)){
                move_line(&p2, -PLAYER_STEP);
            } else if (IsKeyDown(KEY_DOWN)) {
                move_line(&p2, PLAYER_STEP);
            }

            p1.starting = Vector2Clamp(p1.starting, (Vector2){0,0}, (Vector2){w,h - PLAYER_LINE_LENGTH});
            p1.ending = Vector2Clamp(p1.ending, (Vector2){0, PLAYER_LINE_LENGTH}, (Vector2){w,h});

            p2.starting = Vector2Clamp(p2.starting, (Vector2){0,0}, (Vector2){w,h - PLAYER_LINE_LENGTH});
            p2.ending = Vector2Clamp(p2.ending, (Vector2){0, PLAYER_LINE_LENGTH}, (Vector2){w,h});

            float nx = ball.x + velocity.x * TARGET_DT;
            float ny = ball.y + velocity.y * TARGET_DT;

            if (nx - BALL_RADIUS <= 0) {
                score.p2++;
                velocity.x = BALL_VELOCITY;
                ball.x = w/2.0f;
                ball.y = h/2.0f;
            } else if (nx + BALL_RADIUS >= w) {
                score.p1++;
                velocity.x = -BALL_VELOCITY;
                ball.x = w/2.0f;
                ball.y = h/2.0f;
            } else if (ball_collides_with_player(p1, ball)) {
                ball.x = p1.starting.x + BALL_RADIUS + PLAYER_LINE_THICCNESS / 2 + 1.0f;
                velocity.x = BALL_VELOCITY;
            } else if (ball_collides_with_player(p2, ball)) {
                ball.x = p2.starting.x - BALL_RADIUS - PLAYER_LINE_THICCNESS / 2 - 1.0f;
                velocity.x = -BALL_VELOCITY;
            } else {
                ball.x = nx;
            }

            if (ny - BALL_RADIUS <= 0) {
                ball.y = BALL_RADIUS;
                velocity.y = BALL_VELOCITY;
            } else if (ny + BALL_RADIUS >= h) {
                ball.y = h - BALL_RADIUS;
                velocity.y = -BALL_VELOCITY;
            } else {
                ball.y = ny;
            }

            ClearBackground(BLACK);

            DrawCircleV(ball, BALL_RADIUS, WHITE);

            DrawLineEx(p1.starting, p1.ending, PLAYER_LINE_THICCNESS, WHITE);
            DrawLineEx(p2.starting, p2.ending, PLAYER_LINE_THICCNESS, WHITE);

            DrawText(TextFormat("%zu", score.p1), w/2-w/4 - MeasureText(TextFormat("%zu", score.p1), 120) / 2, 80, 120, WHITE);
            DrawText(TextFormat("%zu", score.p2), w/2+w/4 - MeasureText(TextFormat("%zu", score.p2), 120) / 2, 80, 120, WHITE);
        } else {
            if(IsKeyPressed(KEY_SPACE)){
                game_over = 0;
                score = (Score) {0, 0};
            }

            ClearBackground(BLACK);
            const char* first_text = TextFormat("Player %d won %zu-%zu", (score.p1 < score.p2) + 1, score.p1, score.p2);
            size_t first_size = MeasureText(first_text, w/16);

            const char* second_text = "Press SPACE to play again";
            size_t second_size = MeasureText(second_text, w/23);

            DrawText(first_text, w/2.0f - first_size/2.0f, h/2 - 100/2, w/16, WHITE);
            DrawText(second_text, w/2.0f - second_size/2.0f, h/2 + 100, w/23, WHITE);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
