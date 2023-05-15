#if 1
/* 
 * 16 x 16 矩阵
 */
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>

#define NUM_LEDS 256   // 灯珠数
#define xres 16        // Total number of  columns in the display
#define yres 16         // Total number of  rows in the display
#define BRIGHTNESS 250 // LED 亮度
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define LED_PIN 26
//定义矩阵
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(xres, yres, 1, 1, LED_PIN,
                            NEO_MATRIX_TOP  + NEO_TILE_LEFT  + NEO_MATRIX_COLUMNS   + NEO_MATRIX_PROGRESSIVE +
                            NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB + NEO_KHZ800);

// 8列为一组 共四组
int columnIndex[8];
int stepIndex[8];
int glow[8];
bool Running[8]={false, false, false, false, false, false, false, false};
int stepsTillNext[8] = {0, 6, 2, 11, 4, 10, 7, 1};
int hue = 120;//默认绿色

void setup() {
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
  matrix.show();
}

void movingPixel(int x, int y, int colorh, int backgroundGlow = 0){
 
    int pixel = y;
    matrix.drawPixel(x, pixel, hsv2rgb(colorh, 100, 100 ));
    if(pixel-1 >= 0){ matrix.drawPixel(x, pixel-1, hsv2rgb(colorh, 70, 70 ));}   
    if(pixel-2 >= 0){ matrix.drawPixel(x, pixel-2, hsv2rgb(colorh, 50, 50 ));}  
    if(pixel-3 >= 0){ matrix.drawPixel(x, pixel-3, hsv2rgb(colorh, 25, 25 ));}  
    if(pixel-4 >= 0){ matrix.drawPixel(x, pixel-4, hsv2rgb(colorh, 10, 10 ));}   
    if(pixel-5 >= 0){ matrix.drawPixel(x, pixel-5, matrix.Color(0, 0, 0));}   
    
}

void loop() {  
  for(int i=0; i<8; i++){
    if(stepIndex[i] > stepsTillNext[i]){
      Running[i] = true;
      stepsTillNext[i] = 12;  // 进行数组初始化
      columnIndex[i] = random((i*4), ((i+1)*4));    
      glow[i] = random(0, 2);     //随机背景置灰值    
      stepIndex[i] = 0;
    }
    if(Running[i] == true){
      movingPixel(columnIndex[i], stepIndex[i], hue, glow[i]);
      if(stepIndex[i] == 12){
        Running[i] = false;
      }
    }
    stepIndex[i] += 1;
  }
  matrix.show();
  delay(100);//决定下落速度
}

// hsv转rgb值
uint16_t hsv2rgb(uint16_t hue, uint8_t saturation, uint8_t value)
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    uint16_t hi = (hue / 60) % 6;
    uint16_t F = 100 * hue / 60 - 100 * hi;
    uint16_t P = value * (100 - saturation) / 100;
    uint16_t Q = value * (10000 - F * saturation) / 10000;
    uint16_t T = value * (10000 - saturation * (100 - F)) / 10000;

    switch (hi)
    {
    case 0:
        red = value;
        green = T;
        blue = P;
        break;
    case 1:
        red = Q;
        green = value;
        blue = P;
        break;
    case 2:
        red = P;
        green = value;
        blue = T;
        break;
    case 3:
        red = P;
        green = Q;
        blue = value;
        break;
    case 4:
        red = T;
        green = P;
        blue = value;
        break;
    case 5:
        red = value;
        green = P;
        blue = Q;
        break;
    default:
        return matrix.Color(255, 0, 0);
    }
    red = red * 255 / 100;
    green = green * 255 / 100;
    blue = blue * 255 / 100;
    return matrix.Color(red, green, blue);
}
#endif