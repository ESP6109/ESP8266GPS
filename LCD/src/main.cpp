#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <u8g2lib.h>
U8G2_ST7920_128X32_F_SW_SPI
// u8g2(U8G2_R0, E , RW , RS , RST);
u8g2(U8G2_R0, 16, 5, 4, U8X8_PIN_NONE);
// D0 D1 D2 D6

#define TXP 15
#define RXP 13
SoftwareSerial S(RXP, TXP);

TinyGPSPlus GPS;
TinyGPSCustom V(GPS, "GPRMC", 2);
TinyGPSCustom NS(GPS, "GPRMC", 4);
TinyGPSCustom WE(GPS, "GPRMC", 6);

int Yea = 0, Mon = 0, Day = 0;
int Hou = 0, Min = 0, Sec = 0;
int Wee = 0;
float Lng = 0, Lat = 0;
int Spe = 0;
int Cou = 0;
char Val[2];
struct Key
{
  const uint8_t Pin;
  bool pressed;
};
Key Button = {0, 0};
unsigned long BT1 = 0, BT2 = 0;

void IRAM_ATTR button()
{
  BT1 = millis();
  if (BT1 - BT2 > 250)
  {
    Button.pressed = 1;
    BT2 = BT1;
  }
}

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(0, INPUT);
  attachInterrupt(digitalPinToInterrupt(0), button, FALLING);
  Serial.begin(115200);
  S.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print();
}

void get()
{
  Yea = GPS.date.year();
  Mon = GPS.date.month();
  Day = GPS.date.day();
  Hou = GPS.time.hour();
  Min = GPS.time.minute();
  Sec = GPS.time.second();
  Lng = GPS.location.lng(); // 经度
  Lat = GPS.location.lat(); // 纬度
  Spe = GPS.speed.kmph();
  Cou = GPS.course.deg();
}

void time()
{
  if (Hou + 8 >= 24)
  {
    Day++;
    Hou %= 24;
  }
  Hou += 8;
  if ((Mon == 1 || Mon == 3 || Mon == 5 || Mon == 7 || Mon == 8 || Mon == 10 || Mon == 12) && (Day > 31))
  {
    Mon++;
    Day %= 31;
  }
  else if ((Mon == 4 || Mon == 6 || Mon == 9 || Mon == 11) && (Day > 30))
  {
    Mon++;
    Day %= 30;
  }
  else if (Mon == 2)
  {
    if ((Yea % 4 == 0 || Yea % 400 == 0) && (Yea % 100 != 0) && Day > 29)
    {
      Mon++;
      Day %= 29;
    }
    else if ((Yea % 4 != 0 || Yea % 100 == 0) && Day > 28)
    {
      Mon++;
      Day %= 28;
    }
  }
  if (Mon > 12)
  {
    Yea++;
    Mon %= 12;
  }
}

void week()
{
  if (Mon == 1 || Mon == 2)
    Wee = (Day + 2 * (Mon + 12) + 3 * (Mon + 12 + 1) / 5 + (Yea - 1) + (Yea - 1) / 4 - (Yea - 1) / 100 + (Yea - 1) / 400) % 7;
  else
    Wee = (Day + 2 * Mon + 3 * (Mon + 1) / 5 + Yea + Yea / 4 - Yea / 100 + Yea / 400) % 7;
  Wee++;
}

void LCD(int i)
{
  switch (i)
  {
  case 0:
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.setCursor(0, 14);
    u8g2.printf("%02d.%02d.%02d  %d  %3d\n", (Yea % 100), Mon, Day, Wee, Spe);
    u8g2.setCursor(10, 30);
    u8g2.printf("%02d:%02d\n", Hou, Min);
    u8g2.setCursor(65, 30);
    u8g2.printf("%2d %3d\n", int(Lat), int(Lng));
    u8g2.sendBuffer();
    break;
  }
  case 1:
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.setCursor(14, 14);
    u8g2.printf("%4d.%02d.%02d %d\n", Yea, Mon, Day, Wee);
    u8g2.setCursor(29, 30);
    u8g2.printf("%02d:%02d:%02d\n", Hou, Min, Sec);
    u8g2.sendBuffer();
    break;
  }
  case 2:
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.setCursor(0, 14);
    u8g2.printf("%2d%f\n", (int(Lat) / 10), (Lat - int(Lat) / 10 * 10));
    u8g2.setCursor(100, 14);
    u8g2.printf("%s", NS.value());
    u8g2.setCursor(0, 30);
    u8g2.printf("%2d%f\n", (int(Lng) / 10), (Lng - int(Lng) / 10 * 10));
    u8g2.setCursor(100, 30);
    u8g2.printf("%s", WE.value());
    u8g2.sendBuffer();
    break;
  }
  case 3:
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB12_tr);
    if ((Cou > 338 && Cou < 360) || (Cou >= 0 && Cou < 22)) // N
    {

      u8g2.setCursor(0, 30);
      u8g2.printf("N");
    }
    else if (Cou >= 22 && Cou <= 68) // NE
    {
      u8g2.setCursor(0, 30);
      u8g2.printf("NE");
    }
    else if (Cou > 68 && Cou < 112) // E
    {
      u8g2.setCursor(0, 30);
      u8g2.printf("E");
    }
    else if (Cou >= 112 && Cou <= 158) // SE
    {
      u8g2.setCursor(0, 30);
      u8g2.printf("SE");
    }
    else if (Cou > 158 && Cou < 202) // S
    {
      u8g2.setCursor(0, 30);
      u8g2.printf("S");
    }
    else if (Cou >= 202 && Cou <= 248) // SW
    {
      u8g2.setCursor(0, 30);
      u8g2.printf("SW");
    }
    else if (Cou > 248 && Cou < 292) // W
    {
      u8g2.setCursor(0, 30);
      u8g2.printf("W");
    }
    if (Cou >= 292 && Cou <= 338) // NW
    {
      u8g2.setCursor(0, 30);
      u8g2.printf("NW");
    }
    u8g2.setCursor(0, 14);
    u8g2.printf("%d\n", Cou);
    u8g2.setFont(u8g2_font_logisoso32_tn);
    u8g2.setCursor(64, 32);
    u8g2.printf("%03d\n", Spe);
    u8g2.sendBuffer();
    break;
  }
  }
}

void loop()
{
  digitalWrite(10, 1);
  static int i = 0;
  for (unsigned long m = millis(); millis() - m <= 500;)
  {
    while (S.available())
    {
      GPS.encode(S.read());
    }
    while (Button.pressed)
    {
      i++;
      i %= 4;
      Button.pressed = 0;
    }
    Button.pressed = 0;
  }
  strcpy(Val, V.value());
  if (Val[0] == 'A')
    digitalWrite(2, 0);
  else if (Val[0] == 'V')
    digitalWrite(2, 1);
  get();
  time();
  week();
  LCD(i);
  delay(1);
}

// u8g2.drawBox(8,8,16,16);
// u8g2.drawTriangle(16, 32, 8, 24, 24, 24);
// u8g2.drawTriangle(0, 16, 8, 8, 8, 24);
/*u8g2.drawTriangle(16, 4, 16, 20, 4, 28);
u8g2.drawTriangle(16, 4, 16, 20, 28, 28);*/
// u8g2.drawTriangle(0, 32, 17, 0, 33, 32);
// u8g2.drawTriangle(16, 28, 16, 12, 28, 4);
// u8g2.drawTriangle(32, 16, 24, 24, 24, 8);
/*u8g2.drawDisc(16, 16, 4, U8G2_DRAW_ALL);
u8g2.setCursor(16, 32);
u8g2.printf("S");
u8g2.setCursor(0, 22);
u8g2.printf("W");
u8g2.setCursor(28, 22);
u8g2.printf("E");*/

/*while (i)
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_16_mf);
    u8g2.setCursor(0, 13);
    u8g2.printf("GPS%4d.%2d.%2d \n", Yea, Mon, Dat);
    u8g2.setCursor(0, 30);
    u8g2.printf("%2d:%2d\n", Hou, Min);
    u8g2.sendBuffer();
    i = 0;
  }*/
// boolean newData = false;
// digitalWrite(2, 1);
// Serial.printf("\n");
// delay(1);
/*newData = true;
        Yea = GPS.date.year();
        Mon = GPS.date.month();
        Dat = GPS.date.day();
        Hou = GPS.time.hour();
        Min = GPS.time.minute();
        Lng = GPS.location.lng(); // 经度
        Lat = GPS.location.lat(); // 纬度
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_t0_16_mf);
        u8g2.setCursor(0, 13);
        u8g2.printf("GPS%2d.%2d.%2d \n", Yea, Mon, Dat);
        u8g2.setCursor(0, 30);
        u8g2.printf("%2d:%2d %4.1d %4.1d\n", Hou, Min, Lng, Lat);
        u8g2.sendBuffer();
        digitalWrite(2, 1);*/
// get();
// Serial.printf("TEXT");
// Serial.printf("%c", S.read());

/*i = S.read();
a[j] = i;
++j;
u8g2.clearBuffer();
u8g2.setFont(u8g2_font_t0_16_mf);
u8g2.setCursor(0, 30);
u8g2.println(S.read());
u8g2.setCursor(2, 16);
u8g2.printf("%d", i);
u8g2.setCursor(36, 16);
u8g2.println("CONNECTING");
u8g2.setCursor(66, 13);
u8g2.printf("%d",j);

break;

u8g2.sendBuffer();*/
/*if (S.read() == 'Y')
  digitalWrite(2, 0);
  else digitalWrite(2,1);*/

// delay(5);
/*u8g2.clearBuffer();
u8g2.setFont(u8g2_font_t0_12_mf);
u8g2.setCursor(0, 13);
u8g2.printf("%s", a);
u8g2.sendBuffer();
j = 0;*/
// Serial.printf("\n");
// u8g2.printf("%s", a);// U8G2_KS0108_ERM19264_F u8g2(U8G2_R0, 8, 9, 10, 11, 4, 5, 6, 7, /*enable=*/ 18, /*dc=*/ 17, /*cs0=*/ 14, /*cs1=*/ 15, /*cs2=*/ 16, /* reset=*/  U8X8_PIN_NONE); 	// Set R/W to low!
// u8x8_SetPin_KS0108(getU8x8(), d0, d1, d2, d3, d4, d5, d6, d7, enable, dc, cs0, cs1, cs2, reset);

/*u8g2.setFont(u8g2_font_unifont_t_86);
u8g2.drawGlyph(0, 24, 11111);
u8g2.drawGlyph(18, 24, 11112);*/

/*u8g2.setCursor(93, 13);
    u8g2.printf("%3d\n", Spe);
    u8g2.setCursor(50, 29);
    u8g2.printf("%3d %3d\n", int(Lat), int(Lng));*/

/*int button()
{
  static bool bo_st = 1;
  delay(20);
  if (digitalRead(0) && !bo_st)
  {
    bo_st = digitalRead(0);
    return 1;
  }
  else
  {
    bo_st = digitalRead(0);
    return 0;
  }
}*/

/*for (unsigned long m = millis(); millis() - m <= 500;)
{
}*/