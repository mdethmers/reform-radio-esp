#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "AudioTools/Communication/AudioHttp.h"
#include "AudioTools/AudioLibs/A2DPStream.h"

URLStream url("ssid","password");
A2DPStream a2dp;  // Bluetooth A2DP output
MP3DecoderHelix codec;
EncodedAudioStream dec(&a2dp, &codec); // Decoding stream
StreamCopy copier(dec, url); // copy url to decoder

void setup(){
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  // setup Bluetooth A2DP
  auto cfg = a2dp.defaultConfig(TX_MODE);
  cfg.name = "Arduino Audio";
  a2dp.begin(cfg);

  // setup decoder
  dec.begin();

  // mp3 radio
  url.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128","audio/mp3");

  Serial.println("Bluetooth 'Arduino Audio' ready - pair with your phone");
}

void loop(){
  copier.copy();
}
