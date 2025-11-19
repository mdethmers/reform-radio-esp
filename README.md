# ESP32 Internet Radio with Bluetooth

## Hardware Setup

### Connections

**CJMCU-1334 DAC (UDA1334A) to ESP32:**
- VIN → 3.3V
- GND → GND
- WSEL (LRC) → GPIO 26
- DIN → GPIO 25
- BCLK → GPIO 27

**KY-040 Rotary Encoder to ESP32:**
- CLK → GPIO 32
- DT → GPIO 33
- SW → GPIO 34
- + → 3.3V
- GND → GND

**WS2812 LED:**
- Built-in on GPIO 48 (ESP32-WROOM DevKit)

## Required Libraries

Install these via Arduino IDE Library Manager:
1. **Arduino Audio Tools** by Phil Schatzmann
2. **libhelix** by Phil Schatzmann (for MP3 decoding)
3. **Adafruit NeoPixel** by Adafruit

## Configuration

Edit these lines in `reform.ino`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* radioURL = "YOUR_RADIO_STREAM_URL";
```

## Features

- **WiFi Radio Streaming**: Connects to internet radio station
- **Volume Control**: Rotate encoder to adjust volume (0-21)
- **Mode Switch**: Press encoder button to toggle between radio and Bluetooth
- **LED Status Indicators**:
  - Red: Starting up
  - Green: WiFi connected
  - Purple: Radio streaming
  - Blue: Bluetooth mode

## Usage

1. Power on the ESP32
2. Wait for green LED (WiFi connected)
3. Radio starts automatically (purple LED)
4. Rotate encoder to adjust volume
5. Press encoder button to switch to Bluetooth mode (blue LED)
6. Connect your phone to "ESP32_Radio" Bluetooth device
7. Press encoder again to return to radio mode

## Notes

- Uses Arduino Audio Tools library for unified audio handling
- Adjust LED_PIN if your board uses a different GPIO for the WS2812
- Volume range is 0-100% (adjusted in 5% increments per encoder click)
- Bluetooth device name is "ESP32_Radio" - you can change this in the code
- Both radio and Bluetooth modes use the same I2S pins (no rewiring needed)
