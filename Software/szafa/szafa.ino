#include <BH1750.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20,4);  // Ustawienie adresu ukladu na 0x27
//BH1750 czujnikSwiatla;

#define PIRinput 0b00000001 //0
#define czujnikLewy 0b00000010 //1
#define czujnikPrawy 0b00000100 //2
#define prz1 0b00001000 //3
#define prz2 0b00010000 //4
#define prz3 0b00100000 //5
#define prz4 0b01000000 //6
#define debug_on 0b10000000 //7

#define szafaZewnatrz 0b00000001 //31
#define szafaGora 0b00000010 //30
#define szafaPolka1 0b00000100 //29
#define szafaPolka2 0b00001000 //28
#define szafaPolka3 0b00010000 //27
#define szafaPolka4 0b00100000 //26
#define szafaPolka5 0b01000000 //25
#define szafaPolka6 0b10000000 //24

void setup() {
  //BH1750
  //czujnikSwiatla.configure(BH1750_CONTINUOUS_LOW_RES_MODE);

  //LCD
  lcd.init();     // Inicjalizacja LCD 4x20
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Program: Oswietlenie");
  lcd.setCursor(0,1);
  lcd.print("szafy 2.0");
  delay(500);

  //PINY - INPUT
  //  pinMode(PIRinput, INPUT_PULLUP);
  //  pinMode(czujnikLewy, INPUT_PULLUP);
  //  pinMode(czujnikPrawy, INPUT_PULLUP);
  DDRB |= 0b00000000;
  PORTB |= 0b00000110;

  //PINY - OUTPUT
  //  pinMode(szafaZewnatrz, OUTPUT);
  //  pinMode(szafaGora, OUTPUT);
  //  pinMode(szafaPolka1, OUTPUT);
  //  pinMode(szafaPolka2, OUTPUT);
  //  pinMode(szafaPolka3, OUTPUT);
  //  pinMode(szafaPolka4, OUTPUT);
  //  pinMode(szafaPolka5, OUTPUT);
  //  pinMode(szafaPolka6, OUTPUT);
  DDRA |= 0b11111111;
  PORTA |= 0b00000000;


  for (int i = 0; i < 8; i++) {
    lcd.setCursor(i+6,2);
    lcd.print("*");
    PORTA = (0b00000001 << i);
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(0,0);

}

void diagnostykaLCD(uint16_t lux_in, bool pir_in, bool czl_in, bool czp_in, bool szZ, bool szG, bool sz1, bool sz2, bool sz3, bool sz4, bool sz5, bool sz6) {
  char buffer[20];

  //lcd.clear();
  //stany wejsc na wyswietlacz
  sprintf(buffer, "PIR %01d CZL %01d CZP %01d   ", pir_in, czl_in, czp_in);
  lcd.setCursor(0,0);
  lcd.print(buffer);

  //stany wyjsc na wyswietlacz
  sprintf(buffer, "SZ %01d SG %01d P1 %01d P2 %01d ", szZ, szG, sz1, sz2);
  lcd.setCursor(0,1);
  lcd.print(buffer);
  sprintf(buffer, "P3 %01d P4 %01d P5 %01d P6 %01d ", sz3, sz4, sz5, sz6);
  lcd.setCursor(0,2);
  lcd.print(buffer);

  //czujnik oswietlenia
  sprintf(buffer, "LUX %05d  ", lux_in);
  lcd.setCursor(0,3);
  lcd.print(buffer);
}

void loop() {
  //********************************************************************************************************************************************************************************
  //DEFINICJE
  //********************************************************************************************************************************************************************************

  byte WEJSCIA = PINB;
  byte WYJSCIA_st = PINA;
  byte WYJSCIA_out = 0b00000000;

  //********************************************************************************************************************************************************************************
  //SPRAWDZANIE STANOW
  //********************************************************************************************************************************************************************************

  //WEJSCIA
  uint16_t lux_st/*=czujnikSwiatla.readLightLevel()*/;
  bool PIR_st = (WEJSCIA & PIRinput); //digitalRead(PIRinput);
  bool CZL_st = (WEJSCIA & czujnikLewy); //digitalRead(czujnikLewy);
  bool CZP_st = (WEJSCIA & czujnikPrawy); //digitalRead(czujnikPrawy);

  //WYJSCIA
  bool SZZ_st = (WYJSCIA_st & szafaZewnatrz); //digitalRead(szafaZewnatrz);
  bool SZG_st = (WYJSCIA_st & szafaGora); //digitalRead(szafaGora);
  bool SZ1_st = (WYJSCIA_st & szafaPolka1); //digitalRead(szafaPolka1);
  bool SZ2_st = (WYJSCIA_st & szafaPolka2); //digitalRead(szafaPolka2);
  bool SZ3_st = (WYJSCIA_st & szafaPolka3); //digitalRead(szafaPolka3);
  bool SZ4_st = (WYJSCIA_st & szafaPolka4); //digitalRead(szafaPolka4);
  bool SZ5_st = (WYJSCIA_st & szafaPolka5); //digitalRead(szafaPolka5);
  bool SZ6_st = (WYJSCIA_st & szafaPolka6); //digitalRead(szafaPolka6);

  //********************************************************************************************************************************************************************************
  //PROGRAM
  //********************************************************************************************************************************************************************************

  //Jezeli wykryto ruch:
  WYJSCIA_out |= PIR_st ? szafaZewnatrz : 0b00000000;
  WYJSCIA_out |= (PIR_st & (CZL_st | CZP_st)) ? szafaGora : 0b00000000;
  WYJSCIA_out |= (PIR_st & CZL_st) ? szafaPolka1 + szafaPolka2 : 0b00000000;
  WYJSCIA_out |= (PIR_st & CZP_st) ? szafaPolka3 + szafaPolka4 + szafaPolka5 + szafaPolka6 : 0b00000000;
  //WYJSCIA_out |= (PIR_st & (CZL_st | CZP_st)) ? (0b11111111) : (0b00000000);
  //PORTA ^= szafaPolka1; //digitalWrite(szafaPolka3,!digitalRead(szafaPolka3));


  //********************************************************************************************************************************************************************************
  //WYSTEROWANIE WYJSC
  //********************************************************************************************************************************************************************************

  PORTA = WYJSCIA_out;
  delay(200);

  //diagnostyka stanów na podpiętym LCD
  //f (WEJSCIA & debug_on){
  diagnostykaLCD(lux_st, PIR_st, CZL_st, CZP_st, SZZ_st, SZG_st, SZ1_st, SZ2_st, SZ3_st, SZ4_st, SZ5_st, SZ6_st);
  //}
}
