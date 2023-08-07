#define _GNU_SOURCE

#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SCREEN_W 1280
#define SCREEN_H 1280

#define CUBE_SIZE 5
#define PRECISION 3

typedef struct {
  bool cube[CUBE_SIZE * PRECISION][CUBE_SIZE * PRECISION]
           [CUBE_SIZE * PRECISION];
} map;

map gol;

Vector3 center = (Vector3){0, 0, 0};

Vector3 sphere_pos(float r, float xa, float ya) {
  Vector3 v;

  v.x = r * sinf(xa) * cosf(ya);
  v.z = r * sinf(xa) * sinf(ya);
  v.y = r * cosf(xa);

  return v;
}

void fill_randomly(float density) {
  int s = CUBE_SIZE * PRECISION;
  for (int x = 0; x < s; ++x) {
    for (int y = 0; y < s; ++y) {
      for (int z = 0; z < s; ++z) {
        float p = (float)rand() / RAND_MAX;
        gol.cube[x][y][z] = p < density;
      }
    }
  }
}

void DisplayCube() {
  int s = CUBE_SIZE * PRECISION;

  float size = (float)1 / PRECISION;
  float tr = -(float)CUBE_SIZE / 2 + size / 2;
  for (int x = 0; x < s; ++x) {
    for (int y = 0; y < s; ++y) {
      for (int z = 0; z < s; ++z) {
        if (gol.cube[x][y][z]) {
          Vector3 pos =
              (Vector3){(float)x / PRECISION + tr, (float)y / PRECISION + tr,
                        (float)z / PRECISION + tr};

          DrawCube(pos, size, size, size, DARKBLUE);
          DrawCubeWires(pos, size, size, size, WHITE);
        }
      }
    }
  }

  DrawCubeWires(center, CUBE_SIZE, CUBE_SIZE, CUBE_SIZE, WHITE);
}

int count_neightbors(int x, int y, int z) {

  const int l = CUBE_SIZE * PRECISION;
  int count = 0;

  count += gol.cube[x][y][(z + 1) % l];
  count += gol.cube[x][y][(z - 1 + l) % l];
  count += gol.cube[x][(y + 1) % l][z];
  count += gol.cube[x][(y - 1 + l) % l][z];
  count += gol.cube[x][(y + 1) % l][(z + 1) % l];
  count += gol.cube[x][(y + 1) % l][(z - 1 + l) % l];
  count += gol.cube[x][(y - 1 + l) % l][(z + 1) % l];
  count += gol.cube[x][(y - 1 + l) % l][(z - 1 + l) % l];

  count += gol.cube[(x + 1) % l][y][(z + 1) % l];
  count += gol.cube[(x + 1) % l][y][(z - 1 + l) % l];
  count += gol.cube[(x + 1) % l][(y + 1) % l][z];
  count += gol.cube[(x + 1) % l][(y - 1 + l) % l][z];
  count += gol.cube[(x + 1) % l][(y + 1) % l][(z + 1) % l];
  count += gol.cube[(x + 1) % l][(y + 1) % l][(z - 1 + l) % l];
  count += gol.cube[(x + 1) % l][(y - 1 + l) % l][(z + 1) % l];
  count += gol.cube[(x + 1) % l][(y - 1 + l) % l][(z - 1 + l) % l];

  count += gol.cube[(x - 1 + l) % l][y][(z + 1) % l];
  count += gol.cube[(x - 1 + l) % l][y][(z - 1 + l) % l];
  count += gol.cube[(x - 1 + l) % l][(y + 1) % l][z];
  count += gol.cube[(x - 1 + l) % l][(y - 1 + l) % l][z];
  count += gol.cube[(x - 1 + l) % l][(y + 1) % l][(z + 1) % l];
  count += gol.cube[(x - 1 + l) % l][(y + 1) % l][(z - 1 + l) % l];
  count += gol.cube[(x - 1 + l) % l][(y - 1 + l) % l][(z + 1) % l];
  count += gol.cube[(x - 1 + l) % l][(y - 1 + l) % l][(z - 1 + l) % l];

  return count;
}

void UpdateCube() {

  int s = CUBE_SIZE * PRECISION;
  map newgol;

  for (int x = 0; x < s; ++x) {
    for (int y = 0; y < s; ++y) {
      for (int z = 0; z < s; ++z) {
        int c = count_neightbors(x, y, z);

        newgol.cube[x][y][z] = (c > 1 && c < 10);
      }
    }
  }

  gol = newgol;
}

int main() {
  const int seed = time(0);
  char *seed_text;
  asprintf(&seed_text, "Seed : %d", seed);

  srand(seed);

  InitWindow(SCREEN_W, SCREEN_H, "Game Of Life 3D");
  SetTargetFPS(60);

  float radius = 12.5f;
  float x_angle = PI / 2;
  float y_angle = 0.0f;

  float rot_speed = 0.01f;

  Camera3D cam;
  cam.position = sphere_pos(radius, x_angle, y_angle);
  cam.target = (Vector3){0, 0, 0};
  cam.up = (Vector3){0, 1, 0};
  cam.fovy = 45.0f;
  cam.projection = CAMERA_PERSPECTIVE;

  fill_randomly(0.3f);

  // gol.cube[0][0][0] = true;

  bool play = false;

  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_RIGHT))
      y_angle -= rot_speed;
    if (IsKeyDown(KEY_LEFT))
      y_angle += rot_speed;
    if (IsKeyDown(KEY_UP))
      x_angle -= rot_speed;
    if (IsKeyDown(KEY_DOWN))
      x_angle += rot_speed;

    cam.position = sphere_pos(radius, x_angle, y_angle);

    if (IsKeyPressed(KEY_SPACE))
      play = !play;

    if (play)
      UpdateCube();

    BeginDrawing();

    ClearBackground(DARKGRAY);

    BeginMode3D(cam);

    DrawGrid(10, 10.0f);
    DisplayCube();

    EndMode3D();

    DrawText(seed_text, 10, 10, 64, GREEN);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
