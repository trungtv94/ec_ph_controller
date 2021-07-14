#include <U8g2lib.h>
#include <U8x8lib.h>
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);



void setup() {

  Serial.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setColorIndex(1);
}

void loop() {
  u8g2.firstPage();
  do {
    draw();
  } while ( u8g2.nextPage() );
}

void draw_cover_box() {
  u8g2.drawFrame(0, 0, 64, 33);
  u8g2.drawFrame(63, 0, 64, 33);  
  u8g2.drawFrame(0, 32, 64, 32);
  u8g2.drawFrame(63, 32, 64, 32);
}

void draw_base_text() {
  u8g2.setFont(u8g2_font_helvB10_tf);  
  u8g2.drawUTF8(100, 29, DEGREE_SYMBOL);
  
  u8g2.setFont(u8g2_font_profont11_tr);
  u8g2.drawStr(4, 10, "TDS (400)");
  u8g2.drawStr(44, 25, "PPM");
  u8g2.drawStr(66, 10, "Temp.");  
  u8g2.drawStr(107, 25, "C");
  u8g2.drawStr(4, 42, "pH  (6.0)");
  u8g2.drawStr(66, 42, "Trung Tran");
}

void draw_value(String tds_val, String tmp_val, String pH_val) {
  u8g2.setFont(u8g2_font_t0_15b_mn);
  
  int tds_start = 34 - (tds_val.length() - 1)*8;
  u8g2.drawStr(tds_start, 28, tds_val.c_str());

  int tmp_start = 90 - (tmp_val.length() - 1)*8;
  u8g2.drawStr(tmp_start, 28, tmp_val.c_str());

  int pH_start = 34 - (pH_val.length() - 1)*8;
  u8g2.drawStr(pH_start, 56, pH_val.c_str());
  
}

void draw() {
  u8g2.clearBuffer();
  draw_cover_box();
  draw_base_text(); 
  draw_value("998", "25", "7.0");
}
