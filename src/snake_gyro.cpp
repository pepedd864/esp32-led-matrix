#if 0
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <MPU6050_light.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI(); //创建TFT对象
MPU6050 mpu(Wire);         //创建MPU6050对象

xSemaphoreHandle xMutex = xSemaphoreCreateMutex();

// 创建贪吃蛇游戏
const int vert = 240, horiz = 240; // 屏幕高度和宽度
const int width = 12;              // 游戏宽度
const int height = 12;             // 游戏高度
const int s = width * height;
const int cube = 20;    // 方块大小
const float speed = 10; // 游戏速度
int len = 3;            // 蛇的初始长度
int i = 0;
bool gameover = false;             // 游戏结束标志
char direction = 'd', input = 'd'; // 蛇的移动方向
float fps = 0;
int x = 0, y = 0;                              // 起点
int o_x = (horiz - width * cube) / 2, o_y = 0; // 绘制起点
int smap[s] = {0};                             // 游戏地图
int score = 0;                                 // 分数
bool vec[4] = {0, 0, 0, 0};                    //  上 右 下 左 顺时针
double xa = 0, ya = 0, za = 0;                 // 陀螺仪数据
double tx = 0, ty = 0, tz = 0;                 // 陀螺仪数据缓存

void drawCube()
{
  for (i = 0; i < s; i++)
  {
    if (smap[i] == -1)
      tft.fillRect(i % width * cube + o_x, i / width * cube + o_y, cube, cube, TFT_GREEN);
    else if (smap[i] > 0)
      tft.fillRect(i % width * cube + o_x, i / width * cube + o_y, cube, cube, TFT_RED);
  }
  delay(70 + speed * 5);
  for (i = 0; i < s; i++)
  {
    if (smap[i] == -1)
      tft.fillRect(i % width * cube + o_x, i / width * cube + o_y, cube, cube, TFT_WHITE);
    else if (smap[i] > 0)
      tft.fillRect(i % width * cube + o_x, i / width * cube + o_y, cube, cube, TFT_WHITE);
  }
}

void mpuTask(void *param)
{
  while (true)
  {
    if (xSemaphoreTake(xMutex, portMAX_DELAY))
    {
      //临界资源处理
      mpu.update();
      xa = mpu.getAngleX();
      ya = mpu.getAngleY();
      za = mpu.getAngleZ();
      xSemaphoreGive(xMutex);
    }
    delay(2);
  };
}

void game()
{
  //临界资源处理
  if (xa - tx > 30)
  {
    input = 'a';
    Serial.println("左");
  }
  else if (xa - tx < -30)
  {
    input = 'd';
    Serial.println("右");
  }
  else if (ya - ty > 30)
  {
    input = 's';
    Serial.println("下");
  }
  else if (ya - ty < -30)
  {
    input = 'w';
    Serial.println("上");
  }
  tx = xa;
  ty = ya;

  if (input)
  {
    if ((input == 'a' || input == 'A') && (direction != 'd'))
      direction = 'a';
    if ((input == 'd' || input == 'D') && (direction != 'a'))
      direction = 'd';
    if ((input == 's' || input == 'S') && (direction != 'w'))
      direction = 's';
    if ((input == 'w' || input == 'W') && (direction != 's'))
      direction = 'w';
  }
  switch (direction) //移动
  {                  //判断是否超出边界
  case 'a':
    x -= (x > 0 ? 1 : -width + 1);
    break;
  case 'd':
    x += (x < width - 1 ? 1 : -width + 1);
    break;
  case 'w':
    y -= (y > 0 ? 1 : -height + 1);
    break;
  case 's':
    y += (y < height - 1 ? 1 : -height + 1);
    break;
  }
  if (smap[y * width + x] > 1)
  {
    Serial.println("Game Over!"); //判断是否吃到自己
    gameover = true;
  }

  if (smap[y * width + x] == -1) //判断是否吃到食物
  {
    len++;
    do
      i = random(0, s);
    while (smap[i]); //保证食物生成位置为空地
    smap[i] = -1;    //生成食物
  }
  else
    for (i = 0; i < s; i++)
      if (smap[i] > 0)
        smap[i]--;           //蛇身移动
  smap[y * width + x] = len; //蛇头赋值
  drawCube();
  tft.setCursor(3, 5);
  tft.setTextColor(TFT_RED, TFT_WHITE);
  tft.setTextSize(2);
  tft.print("Score:");
  tft.print(score);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.print(score);
  score = len - 3;
}

void setup(void)
{
  Serial.begin(9600);

  tft.init();
  tft.fillScreen(TFT_WHITE);

  Wire.begin(21, 22); // sda, scl
  mpu.begin();
  mpu.calcGyroOffsets();

  smap[random(0, s)] = -1; //随机生成食物
  xTaskCreatePinnedToCore(mpuTask, "mpuTask", 4096, NULL, 5, NULL, 1);
}

void loop()
{
  unsigned long start = millis();
  // 绘制区域
  if (!gameover)
    game();
  else
  {
    tft.setCursor(4, 100);
    tft.setTextColor(TFT_RED, TFT_WHITE);
    tft.setTextSize(4);
    tft.print("Game Over!");
    delay(100);
  }
  // 绘制区域
  /***************检测FPS************************/
  unsigned long end = millis();
  /* tft.setCursor(3, 5);
  tft.setTextColor(TFT_RED, TFT_WHITE);
  tft.setTextSize(2);
  tft.print("FPS:");
  tft.print(1000 / (end - start));
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.print(1000 / (end - start)); */
  /***************检测FPS************************/
}
#endif