#include <stdint.h>
#include <LTC68032.h>
#include <Linduino.h>
#include <LT_SPI.h>
#include <SPI.h>
uint8_t config[6];
int8_t pec_error = 0;
uint8_t cell_codes[7];
uint16_t cellv[7];
uint8_t UV;
uint8_t OV;
void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  LTC6803_initialize();  //! 1) Configure the spi port for 1MHz SCK. That's the frequency the chip is rated for.
  //Afterwards we set the initial configuration (cf. Rapport)
  
  config[0] = 0xF2;
  config[1] = 0x00;
  config[2] = 0x00;
  config[3] = 0xF8;
  //UnderVoltage flag = 2.5V
  config[4] = 0x88;
  //OverVoltage flag=4.5V
  config[5] = 0xDB;
  LTC6803_wrcfgp(&config[0]);
  //Serial.println("Setted up");

}

void loop() {
  
  //Now, we calculate the flag for all cells and we check with the leds if they are or not out the flag (between 2.5V and 4.5V)
  int cutCurrent = 0;
  int cutCurrCh = 0;
  int cutCurrDisch = 0;
  boolean openwire = false;
  boolean muxfailerror = false;
  openwire = LTC6803_owcomp(&pec_error);
  if (openwire == true) {
    openwire = LTC6803_owcomp(&pec_error);
    if (openwire == true) {
      openwire = LTC6803_owcomp(&pec_error);
      if (openwire == true) {
        cutCurrent++;
      }
    }
  }
  LTC6803_selft1();
  pec_error = LTC6803_rdcvp(&cellv[0]);
  if (pec_error == -1) {

    cutCurrent++;
  }
  for (int j = 0; j < 7; j++) {
    if ((cellv[j] * 1.5) != 1279.5) {
      cutCurrent++;
    }
  }

  LTC6803_selft2();
  pec_error = LTC6803_rdcvp(&cellv[0]);
  if (pec_error == -1) {
    cutCurrent++;
  }

  for (int j = 0; j < 7; j++) {
    if ((cellv[j] * 1.5) != 3327) {

      cutCurrent++;
    }
  }
  LTC6803_dagnp();

  muxfailerror = LTC6803_rddgnrp(&pec_error);
  if (pec_error == -1 || muxfailerror == true) {
    cutCurrent++;
  }
  LTC6803_clear();
  LTC6803_stcvadp();
  pec_error = LTC6803_flgp(&cell_codes[0]);
  if (pec_error == -1) {
    cutCurrent++;
  }
  for (int j = 0; j < 7; j++) {
    uint8_t i = 1;
    for (int p = 0; p < j; p++) {
      i *= 2;
    }
    UV = (cell_codes[j]) & 0x01;
    OV = (cell_codes[j]) & 0x02;
    if (OV == 2) {
      Serial.println("OV");
      cutCurrCh++;
      config[0] = 0xF2;
      config[1] |= i;
      config[2] = 0x00;
      config[3] = 0xF8;
      config[4] = 0x88;
      config[5] = 0xDB;
      LTC6803_wrcfgp(&config[0]);
    }
    if (OV == 0) {
      config[0] = 0xF2;
      config[1] &= (~i);
      config[2] = 0x00;
      config[3] = 0xF8;
      config[4] = 0x88;
      config[5] = 0xDB;
      LTC6803_wrcfgp(&config[0]);
    }
    if (UV == 1) {
      Serial.println("UV");
      cutCurrDisch++;
    }
  }
  readT(&cutCurrent);
  //LTC6803_rdcvp(&cellv[12]);

  if (cutCurrCh != 0) {
    cutCharge();
  }
  else if (cutCurrCh == 0) {
    restoreCharge();
  }
  if (cutCurrDisch != 0) {
    cutDischarge();
  }
  else if (cutCurrDisch == 0) {
    restoreDischarge();
  }
  if (cutCurrent != 0) {
    cutError();
  }
  else if (cutCurrent == 0) {
    restore();
  }
}


void readT(int* cutCurrent) {
  int ntc1;
  int ntc2;
  int ntc3;
  int ntc4;
  int ntc5;
  int ntc6;
  int ntc7;
  int ntc8;
  int ntc9;
  int vmax = 784;
  int vmin = 311;
  boolean error = false;
  //Serial.println(*cutCurrent);
  ntc9 = analogRead(A7);
  //Serial.println(ntc9);
  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  ntc1 = analogRead(A6);
  //Serial.println(ntc9);
  digitalWrite(200, HIGH);
  ntc2 = analogRead(A6);
  //Serial.println(ntc2);
  digitalWrite(4, LOW);
  digitalWrite(3, HIGH);
  ntc3 = analogRead(A6);
  //Serial.println(ntc3);
  digitalWrite(4, HIGH);
  ntc4 = analogRead(A6);
  //Serial.println(ntc4);
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  ntc5 = analogRead(A6);
  //Serial.println(ntc5);
  digitalWrite(4, HIGH);
  ntc6 = analogRead(A6);
  //Serial.println(ntc6);
  digitalWrite(4, LOW);
  digitalWrite(3, HIGH);
  ntc7 = analogRead(A6);
  //Serial.println(ntc7);
  digitalWrite(4, HIGH);
  ntc8 = analogRead(A6);
  //Serial.println(ntc8);
  if ((ntc1 > vmax) || (ntc1 < vmin)) {
    //Serial.println("Error ntc1");
    error = true;
  }
  else  if ((ntc2 > vmax) || (ntc2 < vmin)) {
    error = true;
  }
  else  if ((ntc3 > vmax) || (ntc3 < vmin)) {
    error = true;
  }
  else  if ((ntc4 > vmax) || (ntc4 < vmin)) {
    error = true;
  }
  else  if ((ntc5 > vmax) || (ntc5 < vmin)) {
    error = true;
  }
  else  if ((ntc6 > vmax) || (ntc6 < vmin)) {
    error = true;
  }
  else  if ((ntc7 > vmax) || (ntc7 < vmin)) {
    error = true;
  }
  else  if ((ntc8 > vmax) || (ntc8 < vmin)) {
    error = true;
  }
  else  if ((ntc9 > vmax) || (ntc9 < vmin)) {
    error = true;
  }
  if (error == true) {
    //Serial.println("Error NTCs");
    //Serial.println(*cutCurrent);
    *cutCurrent+=1;
    //Serial.println(*cutCurrent);
  }
  //Serial.println(*cutCurrent);
  return;
}
void cutError() {
  Serial.println("Cut Error");
  digitalWrite(5, LOW);
  return;
}
void restore() {
  
  digitalWrite(5, HIGH);
  return;
}
void cutCharge() {
  //Serial.println("Cut Charge");
  digitalWrite(6, LOW);
  return;
}
void restoreCharge() {
  
  digitalWrite(6, HIGH);
  return;
}
void cutDischarge() {
  //Serial.println("Cut Discharge");
  digitalWrite(7, LOW);
  return;
}
void restoreDischarge() {
  
  digitalWrite(7, HIGH);
  return;
}
