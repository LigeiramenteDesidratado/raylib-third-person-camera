#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH 800 * 8
#define SCREEN_HEIGHT 600 * 8

#define SPEED 4

int main(int argc, char *argv[]) {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Third Person Camera");

  Camera camera = {
      {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, 0};
  SetCameraMode(camera, CAMERA_THIRD_PERSON); // Set free camera mode

  Model mario = LoadModel("mario.obj"); // Load model
  Texture texture = LoadTexture("mario.png");
  mario.materials[0].maps[MAP_DIFFUSE].texture = texture;

  Vector3 position = {0.0f, 0.0f, 0.0f}; // Set model position

  Vector3 input = {0};

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second

  Quaternion oldRotation = {0};

  while (!WindowShouldClose()) {

    UpdateCamera(&camera);

    if (IsKeyDown(KEY_W))
      input.z = 1;
    else if (IsKeyDown(KEY_S))
      input.z = -1;
    else
      input.z = 0;

    if (IsKeyDown(KEY_A))
      input.x = 1;
    else if (IsKeyDown(KEY_D))
      input.x = -1;
    else
      input.x = 0;

    // Since we're possibily adding a horizontal and a vertical movement, the
    // player would move faster diagonally than if it just moved horizontally.
    Vector3 direction = Vector3Normalize(input);

    if (Vector3Length(direction) > 0) {

      Matrix cm = GetCameraMatrix(camera);

      // Get forward and right from camera and normalize it.
      Vector3 fwd = {cm.m8, 0, -cm.m10};
      Vector3 right = {-cm.m0, 0, cm.m2};

      fwd = Vector3Normalize(fwd);
      right = Vector3Normalize(right);

      direction = Vector3Add(Vector3Scale(fwd, direction.z),
                             Vector3Scale(right, direction.x));

      float yaw = atan2f(-direction.x, direction.z);

      Quaternion rotation = QuaternionFromEuler(0.0f, yaw, 0.0f);

      // Smooth player rotation
      rotation = QuaternionSlerp(oldRotation, rotation, 15 * GetFrameTime());
      oldRotation = rotation;

      mario.transform = QuaternionToMatrix(rotation);
    }

    Vector3 velocity = Vector3Scale(direction, SPEED);
    Vector3 moveamount = Vector3Scale(velocity, GetFrameTime());

    position = Vector3Add(position, moveamount);

    // Smooth camera
    camera.target = Vector3Lerp(camera.target, position, 10 * GetFrameTime());

    BeginDrawing();
    {

      ClearBackground(GetColor(0x121212ff));

      BeginMode3D(camera);
      {

        DrawModel(mario, position, 1.0f, WHITE);

        DrawGrid(8, 1.0f);

        DrawLine3D(Vector3Zero(), (Vector3){.x = 5}, RED);
        DrawLine3D(Vector3Zero(), (Vector3){.y = 5}, GREEN);
        DrawLine3D(Vector3Zero(), (Vector3){.z = 5}, BLUE);
      }
      EndMode3D();

      DrawFPS(10, 10);
    }
    EndDrawing();
  }

  UnloadModel(mario);
  UnloadTexture(texture);
  CloseWindow();
}
