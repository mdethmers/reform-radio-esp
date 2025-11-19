#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "AudioTools/Communication/AudioHttp.h"
#include "AudioTools/AudioLibs/A2DPStream.h"

// Rotary encoder pins
#define ENCODER_CLK 25
#define ENCODER_DT 26
#define ENCODER_SW 27

// CJMCU-1334 DAC uses I2S
#define I2S_BCLK 14
#define I2S_LRC 15
#define I2S_DOUT 22

URLStream url("ssid","password");
A2DPStream a2dp;  // Bluetooth receiver (RX mode)
I2SStream i2s;    // DAC output
MP3DecoderHelix codec;

// Source mode: true = Bluetooth from phone, false = Internet radio
bool useBluetoothInput = false;

EncodedAudioStream dec(&i2s, &codec); // For internet radio decoding
StreamCopy copier_radio(dec, url);    // Internet radio -> decoder -> DAC
StreamCopy copier_bt(i2s, a2dp);      // Bluetooth -> DAC

// Volume control
int volume = 50; // 0-100
int lastCLK;
unsigned long lastButtonPress = 0;

void IRAM_ATTR encoderISR() {
  int clkState = digitalRead(ENCODER_CLK);
  int dtState = digitalRead(ENCODER_DT);
  
  if (clkState != lastCLK && clkState == LOW) {
    if (dtState != clkState) {
      volume = min(100, volume + 5);
    } else {
      volume = max(0, volume - 5);
    }
    Serial.print("Volume: ");
    Serial.println(volume);
    
    // Apply volume to DAC
    i2s.setVolume(volume / 100.0);
  }
  lastCLK = clkState;
}

void IRAM_ATTR buttonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonPress > 300) { // Debounce
    useBluetoothInput = !useBluetoothInput;
    lastButtonPress = currentTime;
    
    Serial.print("Switched to: ");
    Serial.println(useBluetoothInput ? "Bluetooth Input" : "Internet Radio");
    
    // Switch input source
    if (useBluetoothInput) {
      // Stop internet radio
      url.end();
      dec.end();
      
      // Start Bluetooth receiver
      auto a2dp_cfg = a2dp.defaultConfig(RX_MODE);
      a2dp_cfg.name = "Arduino Speaker";
      a2dp.begin(a2dp_cfg);
    } else {
      // Stop Bluetooth
      a2dp.end();
      
      // Start internet radio
      dec.begin();
      url.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128","audio/mp3");
    }
  }
}

void setup(){
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  // Setup rotary encoder
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  lastCLK = digitalRead(ENCODER_CLK);
  
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), buttonISR, FALLING);

  // Setup I2S for CJMCU-1334 DAC (always active)
  auto i2s_cfg = i2s.defaultConfig(TX_MODE);
  i2s_cfg.pin_bck = I2S_BCLK;
  i2s_cfg.pin_ws = I2S_LRC;
  i2s_cfg.pin_data = I2S_DOUT;
  i2s_cfg.sample_rate = 44100;
  i2s_cfg.bits_per_sample = 16;
  i2s_cfg.channels = 2;
  i2s.begin(i2s_cfg);
  i2s.setVolume(volume / 100.0);

  // Start with Internet Radio mode
  dec.begin();
  url.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128","audio/mp3");

  Serial.println("Ready - Internet Radio mode active");
  Serial.println("Rotate encoder: volume control");
  Serial.println("Click encoder: switch Bluetooth/Radio");
}

void loop(){
  if (useBluetoothInput) {
    // Stream Bluetooth audio from phone to DAC
    copier_bt.copy();
  } else {
    // Stream internet radio to DAC
    copier_radio.copy();
  }
}
