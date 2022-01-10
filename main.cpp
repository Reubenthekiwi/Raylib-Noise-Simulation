#include "raylib.h"
#include "FastNoiseLite.h"
#include <iostream>
#include <array>
#include <time.h>
#include <string.h>
#include <sstream>

// initialize the basic window needs
constexpr int   WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720,
                FPS = 1280,
                FONT_SIZE = WINDOW_WIDTH/20,
                TILE_SIDE_LENGTH = 5,
                TILE_COUNT_X = WINDOW_WIDTH/TILE_SIDE_LENGTH,
                TILE_COUNT_Y = WINDOW_HEIGHT/TILE_SIDE_LENGTH,
                TILE_COUNT = TILE_COUNT_X * TILE_COUNT_Y;

constexpr char title[] = "Noise Simulation v0.1";
char generatingText[] = "GENERATING... - ";

struct TILE {
    int x, y;
    float height = 100;
    Color color = BLACK;
};

void continueTileGeneration(TILE *tiles, FastNoiseLite &noise, int index, int x, int y); // called every frame

int main() {

    SetRandomSeed(time(0));
    
    int index = 0, index_x = 0, index_y = 0;
    float progress = 0;
    char progressText[100];

    bool isGenerating = false;
    float timeElapsed = 0;

    // noise initialization
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(GetRandomValue(1, 2000000000));

    // array map declaration
    TILE tiles[TILE_COUNT_Y * TILE_COUNT_X];
    
    // init the window
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);
    SetExitKey(KEY_NULL);
    SetTargetFPS(FPS);

    std::cout << "\n\nPress [ENTER] or [SPACE] to generate a new Noise Map!\n" << std::endl;

    // the main loop
    while(!WindowShouldClose()) {

        // debug
        //std::cout << "FPS: " << GetFPS() << std::endl;

        // input
        if(IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            if (!isGenerating) {
                std::cout << "Generating Noise Map..." << std::endl;

                //start timer
                timeElapsed = 0;

                noise.SetSeed(GetRandomValue(1, 2000000000));
                isGenerating = true;
                progress = 0;
                index = 0, index_y = 0, index_x = 0;
            }
        } else if(IsKeyPressed(KEY_S)) {
            std::stringstream stream; stream.precision(0);
            stream << std::fixed; stream << GetRandomValue(0, 2000000000);

            char fileName[99] = "screenshot";
            strcat(fileName, stream.str().c_str());

            TakeScreenshot(strcat(fileName, ".png"));
        }

        // updating
        if(isGenerating) {
            if(index_y < TILE_COUNT_Y) {
                if(index_x < TILE_COUNT_X) {
                    // GENERATE THE TILES EVERY FRAME
                    continueTileGeneration(tiles, noise, index++, index_x, index_y);
                    progress = ((index_y * TILE_COUNT_X) + (index_x + 1));

                    // CONVERT FLOAT TO STRING
                    std::stringstream stream1; std::stringstream stream2;
                    stream1.precision(0); stream2.precision(0);
                    stream1 << std::fixed; stream2 << std::fixed;
                    stream1 << progress; stream2 << TILE_COUNT;
                    std::string prog_temp  = stream1.str();
                    std::string count_temp = stream2.str();

                    strcpy(progressText, prog_temp.c_str());
                    strcat(progressText, " / ");
                    strcat(progressText, count_temp.c_str());

                    index_x++;

                    timeElapsed += GetFrameTime();
                } else {
                    index_x = 0;
                    index_y++;
                }
            } else {
                std::cout << "Map Successfully Generated in:\n" << timeElapsed << " seconds!\n\nPress S to take a screenshot!\n" << std::endl;
                isGenerating = false;
            }
        }

        // rendering
        BeginDrawing();

        ClearBackground(BLACK);

        if(!isGenerating) {
            for(TILE tile : tiles) {
                DrawRectangle(tile.x, tile.y, TILE_SIDE_LENGTH, TILE_SIDE_LENGTH, tile.color);
            }
        } else {
            DrawText(generatingText, 10, 10, FONT_SIZE, BLUE);
            DrawText(progressText, 10 + MeasureText(generatingText, FONT_SIZE), 10, FONT_SIZE, BLUE);
        }

        EndDrawing();

    }

    CloseWindow();
    
    return 0;
}
void continueTileGeneration(TILE *tiles, FastNoiseLite &noise, int index, int x, int y) {
    // set the coordinates
    tiles[index].x = x * TILE_SIDE_LENGTH; tiles[index].y = y * TILE_SIDE_LENGTH;
    // set the height from the noise
    float new_height    = 1 * noise.GetNoise(1 * (float) x, 1 * (float) y)
                        + 0.5 * noise.GetNoise(2 * (float) x, 2 * (float) y)
                        + 0.25 * noise.GetNoise(4 * (float) x, 4 * (float) y);
    
    tiles[index].height = new_height / (1 + 0.5 + 0.25);

    if(tiles[index].height < 0.05) {
        tiles[index].color = DARKBLUE;
    } else if(tiles[index].height < 0.1) {
        tiles[index].color = BLUE;
    } else if(tiles[index].height < 0.2) {
        tiles[index].color = BEIGE;
    } else if(tiles[index].height < 0.4) {
        tiles[index].color = GREEN;
    } else if(tiles[index].height < 0.6) {
        tiles[index].color = DARKGREEN;
    } else if(tiles[index].height < 0.8) {
        tiles[index].color = LIGHTGRAY;
    } else if(tiles[index].height < 1) {
        tiles[index].color = WHITE;
    }
                        
    //std::cout << "Generating Tile at: (" << tiles[y][x].x << ", " << tiles[y][x].y << " ) with the height of: " << tiles[y][x].height << std::endl;
}