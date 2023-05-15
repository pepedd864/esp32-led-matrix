#if 0

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <MPU6050_light.h>
#include <ArduinoEigen.h>
using namespace Eigen;

struct Line
{
  uint8_t start;
  uint8_t end;
};

TFT_eSPI tft = TFT_eSPI(); //创建TFT对象
MPU6050 mpu(Wire);         //创建MPU6050对象

MatrixXd pt(8, 4);  //立方体顶点
MatrixXd tp(12, 2); //立方体边
MatrixXd tm0(4, 4); //放大60位
MatrixXd tm1(4, 4); //平移
double xa, ya, za;  //角度

Matrix4d txr; //x轴
Matrix4d tyr; //y轴
Matrix4d tzr; //z轴

void mpuTask(void *param)
{
  //临界资源处理
  mpu.update();
  xa = mpu.getAngleX();
  ya = mpu.getAngleY();
  za = mpu.getAngleZ();
}

void drawTube(uint16_t color)
{
  int nr = tp.rows(); //行数
  for (int i = 0; i < nr; i++)
  {
    auto start = pt.row(tp(i, 0));
    auto end = pt.row(tp(i, 1));
    tft.drawLine(start(0), start(1), end(0), end(1), color);
    // tft.drawWideLine(start(0), start(1), end(0), end(1), 2, color, color);
  }
}

void drawTube(const MatrixXd &m, uint16_t color)
{
  int nr = tp.rows(); //行数
  for (int i = 0; i < nr; i++)
  {
    auto start = m.row(tp(i, 0));
    auto end = m.row(tp(i, 1));
    tft.drawLine(start(0), start(1), end(0), end(1), color);
    // tft.drawWideLine(start(0), start(1), end(0), end(1), 2, color, color);
  }
}

void setZRotationTranslation(double angle)
{
  double tmp = 3.1415926 * angle / 180.0;
  double c = cos(tmp);
  double s = sin(tmp);
  tzr(0, 0) = c;
  tzr(0, 1) = s;
  tzr(1, 0) = -s;
  tzr(1, 1) = c;
}

void setXRotationTranslation(double angle)
{
  double tmp = 3.1415926 * angle / 180.0;
  double c = cos(tmp);
  double s = sin(tmp);
  tzr(1, 1) = c;
  tzr(1, 2) = s;
  tzr(2, 1) = -s;
  tzr(2, 2) = c;
}

void setYRotationTranslation(double angle)
{
  double tmp = 3.1415926 * angle / 180.0;
  double c = cos(tmp);
  double s = sin(tmp);
  tzr(0, 0) = c;
  tzr(0, 2) = s;
  tzr(2, 0) = -s;
  tzr(2, 2) = c;
}

void setup()
{
  Serial.begin(9600);
  tft.init();
  tft.fillScreen(TFT_BLACK);

  Wire.begin(21, 22); // sda, scl
  mpu.begin();
  mpu.calcGyroOffsets();

  pt << -1, -1, 1, 1, //A 0
      1, -1, 1, 1,    //B 1
      1, 1, 1, 1,     //C 2
      -1, 1, 1, 1,    //D 3
      -1, -1, -1, 1,  //E 4
      1, -1, -1, 1,   //F 5
      1, 1, -1, 1,    //G 6
      -1, 1, -1, 1;   //H 7

  tp << 0, 1, //A-B
      1, 2,   //B-C
      2, 3,   //C-D
      3, 0,   //D-A
      0, 4,   //A-E
      1, 5,   //B-F
      2, 6,   //C-G
      3, 7,   //D-H
      4, 5,   //E-F
      5, 6,   //F-G
      6, 7,   //G-H
      7, 4;   //H-E

  tm0 << 60, 0, 0, 0, //放大60倍
      0, 60, 0, 0,
      0, 0, 60, 0,
      0, 0, 0, 1;

  tm1 << 1, 0, 0, 0, //平移
      0, 1, 0, 0,
      0, 0, 1, 0,
      120, 120, 120, 1;

  txr.setIdentity(4, 4);
  tyr.setIdentity(4, 4);
  tzr.setIdentity(4, 4);

  pt *= tm0; //这个就是我们最初的大个立方体
}

void loop()
{
  unsigned long start = millis();
  /***********************************/
  MatrixXd tmp(8, 4);
  double xxa, yya, zza;
  double txa = 0, tya = 0, tza = 0;
  mpuTask(NULL);
  xxa = xa;
  yya = ya;
  zza = za;

  setXRotationTranslation(xxa);
  setYRotationTranslation(yya);
  setZRotationTranslation(zza);

  tmp = pt;   //每次都从最初的大立方体开始
  tmp *= txr; //x轴旋转
  tmp *= tyr; //y轴旋转
  tmp *= tzr; //z轴旋转
  tmp *= tm1; //平移

  drawTube(tmp, TFT_WHITE);
  delay(25);
  drawTube(tmp, TFT_BLACK);
  /***********************************/
  unsigned long end = millis();
  tft.setCursor(3, 5);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(2);
  tft.print("FPS:");
  tft.print(1000 / (end - start));
  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  tft.print(1000 / (end - start));
  /***********************************/
  txa = xxa;
  tya = yya;
}

#endif

#if 0
// ESP32多核代码
#include <ArduinoEigen.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <MPU6050_light.h>

using namespace Eigen;

struct Line
{
  uint8_t start;
  uint8_t end;
};

MatrixXd pt(8, 4);  //立边体顶点
MatrixXi tp(12, 2); //立方体边

MatrixXd tm0(4, 4); //放大60位
MatrixXd tm1(4, 4); //平移
Matrix4d txr;
Matrix4d tyr;
Matrix4d tzr;

double xa;
double ya;
double za;

xSemaphoreHandle xMutex = xSemaphoreCreateMutex();

TFT_eSPI tft = TFT_eSPI();
// TFT_eSprite spr = TFT_eSprite(&tft);/

void drawTube(uint16_t color)
{
  int nr = tp.rows(); //行数
  for (int i = 0; i < nr; i++)
  {
    auto start = pt.row(tp(i, 0));
    auto end = pt.row(tp(i, 1));
    tft.drawLine(start(0), start(1), end(0), end(1), color);
    // tft.drawWideLine(start(0), start(1), end(0), end(1), 2, color, color);
  }
}

void drawTube(const MatrixXd &m, uint16_t color)
{
  int nr = tp.rows(); //行数
  for (int i = 0; i < nr; i++)
  {
    auto start = m.row(tp(i, 0));
    auto end = m.row(tp(i, 1));
    tft.drawLine(start(0), start(1), end(0), end(1), color);
    // tft.drawWideLine(start(0), start(1), end(0), end(1), 2, color, color);
  }
}

//angle是角度
void SetXRotationTranslation(double angle)
{
  double tmp = 3.1415926 * angle / 180.0;
  double c = cos(tmp);
  double s = sin(tmp);
  txr(1, 1) = c;
  txr(1, 2) = s;
  txr(2, 1) = -s;
  txr(2, 2) = c;
}

//angle是角度
void SetYRotationTranslation(double angle)
{
  double tmp = 3.1415926 * angle / 180.0;
  double c = cos(tmp);
  double s = sin(tmp);
  tyr(0, 0) = c;
  tyr(0, 2) = -s;
  tyr(2, 0) = s;
  tyr(2, 2) = c;
}

//angle是角度
void SetZRotationTranslation(double angle)
{
  double tmp = 3.1415926 * angle / 180.0;
  double c = cos(tmp);
  double s = sin(tmp);
  tzr(0, 0) = c;
  tzr(0, 1) = s;
  tzr(1, 0) = -s;
  tzr(1, 1) = c;
}

MPU6050 mpu(Wire);

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

void setup()
{
  Serial.begin(115200);
  delay(1000);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  Wire.begin(21, 22);
  mpu.begin();
  mpu.calcGyroOffsets(); // gyro and accelero
  Serial.println("Done!\n");

  pt << -1, -1, 1, 1, //A 0
      1, -1, 1, 1,    //B 1
      1, 1, 1, 1,     //c 2
      -1, 1, 1, 1,    //d 3
      -1, -1, -1, 1,  //e 4
      1, -1, -1, 1,   //f 5
      1, 1, -1, 1,    //g 6
      -1, 1, -1, 1;   //h 7

  tp << 0, 1, //A-B
      1, 2,   //b-c
      2, 3,   //c-d
      3, 0,   //d-a
      0, 4,   //a-e
      1, 5,   //b-f
      2, 6,   //c-g
      3, 7,   //d-h
      4, 5,   //e-f
      5, 6,   //f-g
      6, 7,   //g-h
      7, 4;   //h-e

  tm0 << 60, 0, 0, 0,
      0, 60, 0, 0,
      0, 0, 60, 0,
      0, 0, 0, 1;

  tm1 << 1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      120, 120, 120, 1;

  txr.setIdentity(4, 4);
  tyr.setIdentity(4, 4);
  tzr.setIdentity(4, 4);

  pt *= tm0; //这个就是我们最初的大个立方体

  xTaskCreatePinnedToCore(mpuTask, "mpuTask", 4096, NULL, 5, NULL, 1);
}

void loop()
{
  unsigned long start = millis();
  /***********************************/
  MatrixXd tmp(8, 4);
  double xxa, yya, zza;
  if (xSemaphoreTake(xMutex, portMAX_DELAY))
  {
    //临界资源处理
    xxa = xa;
    yya = ya;
    zza = za;
    xSemaphoreGive(xMutex);
  }

  SetXRotationTranslation(xxa);
  SetYRotationTranslation(yya);
  SetZRotationTranslation(zza);

  tmp = pt;
  tmp *= txr;
  tmp *= tyr;
  tmp *= tzr;
  tmp *= tm1;

  tft.startWrite();
  drawTube(tmp, TFT_WHITE);
  delay(15);
  drawTube(tmp, TFT_BLACK);
  /***********************************/
  unsigned long end = millis();
  tft.setCursor(3, 5);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(2);
  tft.print("FPS:");
  tft.print(1000 / (end - start));
  tft.endWrite();
}

#endif

#if 0

#include <TFT_eSPI.h>
#include "BluetoothSerial.h"

BluetoothSerial bt;

static const uint16_t screenWidth = 240, screenHeight = 240;
TFT_eSPI tft = TFT_eSPI();

// 认证请求回调
void BTConfirmRequestCallback(uint32_t numVal)
{
  //numVal 是主机发来的识别码
  Serial.printf("recv pin :%d \r\n", numVal);
  //这里对识别码进行判断，是否和主机一样，然后再判断是否确定
  bt.confirmReply(true);
}

//认证结果回调函数
void BTAuthCompleteCallback(boolean success)
{
  if (success)
    Serial.println("Pairing success!");
  else
    Serial.println("Pairing failed, rejected by user!");
}

//接受数据
void RecvData(const uint8_t *buffer, size_t size)
{
  if (size > 0)
  {
    Serial.write(buffer, size);
    bt.write(buffer, size);
    bt.println(" -slave");
  }
}

void setup()
{
  bt.enableSSP(); //开启SSP认证
  bt.onConfirmRequest(BTConfirmRequestCallback);
  bt.onAuthComplete(BTAuthCompleteCallback);
  Serial.begin(11520);
  bt.onData(RecvData); //注册接收回调函数
  bt.begin("ESP32");
}

void loop()
{
}

#endif

#if 0

void setup()
{

}

void loop()
{

}

#endif