#if 0
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/22, /* data=*/21, /* reset=*/U8X8_PIN_NONE);

#define printx(h, str, n)          \
  u8g2.setFont(u8g2_font_6x10_tf); \
  u8g2.setCursor(h, 10);           \
  u8g2.print(str);                 \
  u8g2.print(": ");                \
  u8g2.print(n)

// 创建贪吃蛇游戏
const int vert = 64, horiz = 128; // 屏幕高度和宽度
const int width = 18;             // 游戏宽度
const int height = 9;             // 游戏高度
const int s = width * height;
const float speed = 10; // 游戏速度
int len = 3;            // 蛇的初始长度
int i = 0;
bool gameover = false;             // 游戏结束标志
char direction = 'd', input = 'd'; // 蛇的移动方向
float fps = 0;
int x = 0, y = 0;                            // 起点
int o_x = (horiz - width * 5) / 2, o_y = 14; // 绘制起点
int smap[s] = {0};                           // 游戏地图
int score = 0;                               // 分数

void game()
{
  u8g2.drawFrame(o_x, o_y, width * 5 + 2, height * 5 + 2); //绘制游戏边框
  input = Serial.read();                                   //读取输入
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

  for (i = 0; i < s; i++)
  {
    if (smap[i] > 0)
      u8g2.drawBox(o_x + i % width * 5, o_y + i / width * 5, 5, 5); //绘制蛇身
    if (smap[i] == -1)
      u8g2.drawBox(o_x + i % width * 5, o_y + i / width * 5, 5, 5); //绘制食物
  }
  score = len - 3;
}

void setup(void)
{
  Serial.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print();
  smap[random(0, s)] = -1; //随机生成食物
}

void loop()
{
  unsigned long start = millis();

  // u8g2绘制区域
  u8g2.clearBuffer();
  printx(0, "FPS", fps);
  printx(65, "Score", score);
  if (!gameover)
    game();
  else
  {
    u8g2.setFont(u8g2_font_7x13_tf);
    u8g2.setCursor(0, 30);
    u8g2.print("Game Over!");
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(0, 50);
    u8g2.print("Press r to restart");
    if (Serial.read() == 'r')
    {
      gameover = false;
      len = 3;
      direction = 'd';
      input = 'd';
      x = 0;
      y = 0;
      score = 0;
      for (i = 0; i < s; i++)
        smap[i] = 0;
      smap[random(0, s)] = -1;
    }
  }
  u8g2.sendBuffer();
  // u8g2绘制区域
  unsigned long end = millis();
  fps = 1000 / (end - start);
  delay(speed * 10); //延时
}
#endif

