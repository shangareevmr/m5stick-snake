#include <M5StickCPlus.h>

double last_theta = 0;
double last_phi = 0;

int dir = 3;

#define FIELD_WIDTH 130/10
#define FIELD_HEIGHT 230/10

int snake_x[FIELD_WIDTH*FIELD_HEIGHT] = { 0 };
int snake_y[FIELD_WIDTH*FIELD_HEIGHT] = { 0 };
int snake_len = 1;

int apple_x = FIELD_WIDTH/2;
int apple_y = FIELD_HEIGHT/2;

void doDrawSnake(int color) {
    M5.Lcd.drawCircle(snake_x[0] * 10 + 7, snake_y[0] * 10 + 8, 5, color);
    for (int i = 1; i < snake_len; i++) {
      M5.Lcd.fillCircle(snake_x[i] * 10 + 7, snake_y[i] * 10 + 8, 5, color);
    }
}

void drawSnake() {
  doDrawSnake(WHITE);
}

void wipeSnake() {
  doDrawSnake(BLACK);
}

void drawApple() {
  while (apple_x < 0 || apple_y < 0) {
    apple_x = random(FIELD_WIDTH - 2);
    apple_y = random(FIELD_HEIGHT - 2);
    
    for (int i = 0; i < snake_len; ++i) {
      if (snake_x[i] == apple_x && snake_y[i] == apple_y) {
        apple_x = -1;
        apple_y = -1;
        break;
      }
    }
  }
  
  M5.Lcd.fillCircle(apple_x * 10 + 7, apple_y * 10 + 8, 5, RED);
}

bool shiftSnake(int head_x, int head_y) {
  for (int i = snake_len - 1; i > 0; --i) {
    snake_x[i] = snake_x[i - 1];
    snake_y[i] = snake_y[i - 1];
    if (snake_x[i] == head_x && snake_y[i] == head_y)
      return false;
  }

  if (snake_x[0] == head_x && snake_y[0] == head_y)
    return false;
      
  snake_x[0] = head_x;
  snake_y[0] = head_y;
  
  return true;
}

void moveSnake(void* pvParameters) {
  while (1) {
    wipeSnake();

    int head_x = snake_x[0];
    int head_y = snake_y[0];
  
    if (dir == 1) {
      ++head_y;
      if (head_y >= FIELD_HEIGHT)
        break;
    }

    if (dir == 2) {
      --head_y;
      if (head_y < 0)
        break;
    }
    
    if (dir == 3) {
      ++head_x;
      if (head_x >= FIELD_WIDTH)
        break;
    }
    
    if (dir == 4) {
      --head_x;
      if (head_x < 0)
        break;
    }

    if (head_x == apple_x && head_y == apple_y) {
      ++snake_len;
      apple_x = -1;
      apple_y = -1;
    }

    if (!shiftSnake(head_x, head_y))
      break;

    drawSnake();
    drawApple();

    delay(500);
  }

  M5.Lcd.fillScreen(BLACK);
  delay(100);
  M5.Lcd.fillScreen(RED);
  delay(100);
  M5.Lcd.fillScreen(BLACK);
  delay(100);
  M5.Lcd.fillScreen(RED);
  delay(100);
  M5.Lcd.fillScreen(BLACK);
  delay(100);
  M5.Lcd.fillScreen(RED);
  delay(1000);
}

void setup() {
  M5.begin();
  M5.Imu.Init();
  M5.Lcd.fillScreen(BLACK);
  
  xTaskCreatePinnedToCore(moveSnake, "move", 4096, NULL, 1, NULL, 0);
}

void loop() {
  double theta = 0;
  double phi = 0;
  double alpha = 0.2;

  float accX = 0;
  float accY = 0;
  float accZ = 0;
  M5.Imu.getAccelData(&accX, &accY, &accZ);

  if ((accX < 1) && (accX > -1)) {
    theta = asin(-accX) * 57.295;
  }
  if (accZ != 0) {
    phi = atan(accY / accZ) * 57.295;
  }

  theta = alpha * theta + (1 - alpha) * last_theta;
  phi   = alpha * phi + (1 - alpha) * last_phi;

  if (phi > 20) {
    dir = 1;
  } else if (phi < -20) {
    dir = 2;
  } else if (theta > 20) {
    dir = 3;
  } else if (theta < -20) {
    dir = 4;
  }

  last_theta = theta;
  last_phi   = phi;
}
