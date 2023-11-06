/*
pico - cc1101 module
GP16 - MISO (GDO1, SO)
GP17 - CSn
GP18 - SCK
GP19 - MOSI (SI)
GP20 - GDO0
GP21 - GDO2


*/
#include <Arduino.h>
#include <SPI.h>
#include <CC1101_RF.h>

CC1101 lora;
#define GDO0 20
bool request_slave(const char *s_id, byte *_packet);

const char *slave_ids[] = {"1234", "5678"};
int slave_cnt = 0;

void blink_led();
#define BUTTON 15
bool button_state = true;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GDO0, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  SPI.begin();         // mandatory
  lora.begin(433.4e6); // 433.4 MHz

  lora.setRXstate();

  Serial.begin(9600);
}

void loop()
{
  static unsigned long t = 0;
  blink_led();
  byte packet[64];

  uint8_t pkt_size = 0;
  if (digitalRead(GDO0))
  {
    pkt_size = lora.getPacket(packet);
  }
  if (pkt_size > 0 && lora.crcok())
  {
    Serial.write(packet, pkt_size);
    Serial.println();
  }

  if (!digitalRead(BUTTON) || millis() - t > 1000)
  {
    if (button_state)
    {
      delayMicroseconds(10);
      button_state = false;

      if (request_slave(slave_ids[slave_cnt], packet))
      {
        Serial.println(String(slave_ids[slave_cnt]) + ": packet request");
      }
      slave_cnt += 1;
      slave_cnt %= 2;
    }
    t = millis();
  }
  else
  {
    button_state = true;
  }
}

void blink_led()
{
  static unsigned long t = 0;
  static int s = 0;
  if (millis() - t < 500)
    return;
  s ^= true;
  digitalWrite(LED_BUILTIN, s);
  // Serial.println(lora.getState(), HEX);
  t = millis();
  return;
}

bool request_slave(const char *s_id, byte *_packet)
{
  strcpy((char *)_packet, s_id);
  bool rtn = lora.sendPacket((const char *)_packet);

  return rtn;
}
