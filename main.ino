// ============================================================================
// HIGH SCHOOL HACKER v1.00 - FINAL RELEASE
// ============================================================================
// ESP32-S3 USB Keyboard Attack Tool with Web Interface
// Hardware: LilyGO T-Dongle S3
// Display: ST7735 80x160px LCD with rainbow effects
// Features: DuckyScript terminal, 17 pre-made payloads, WiFi AP interface
// ============================================================================

// ============================================================================
// LIBRARIES
// ============================================================================
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <FastLED.h>
#include <USB.h>
#include <USBHIDKeyboard.h>

// ============================================================================
// HARDWARE PIN DEFINITIONS
// ============================================================================
#define BACKLIGHT_PIN 38
#define LED_DATA_PIN 40
#define LED_CLK_PIN 39
#define NUM_LEDS 1

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
TFT_eSPI tft = TFT_eSPI();
CRGB leds[NUM_LEDS];
WebServer server(80);
USBHIDKeyboard Keyboard;

// ============================================================================
// NETWORK CONFIGURATION
// ============================================================================
const char* ssid = "HighSchoolHacker";
const char* password = "hacker2026";

// ============================================================================
// GLOBAL STATE VARIABLES
// ============================================================================
int terminalRuns = 0;
int progressBar = 0;  // 0-100 for progress bar animation

// ============================================================================
// DISPLAY RAINBOW COLOR PALETTE (16 colors)
// ============================================================================
uint16_t rainbowColors[] = {
  TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREENYELLOW,
  TFT_GREEN, TFT_CYAN, TFT_BLUE, TFT_NAVY,
  TFT_PURPLE, TFT_MAGENTA, TFT_PINK, TFT_MAROON,
  TFT_OLIVE, TFT_DARKGREEN, TFT_DARKCYAN, TFT_WHITE
};

// ============================================================================
// HTML WEB INTERFACE
// ============================================================================
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>HIGH SCHOOL HACKER</title>
<style>
@import url('https://fonts.googleapis.com/css2?family=VT323&display=swap');

* { margin: 0; padding: 0; box-sizing: border-box; }
body { 
  background: #000; 
  color: #00ff00; 
  font-family: 'VT323', monospace;
  padding: 20px;
  font-size: 18px;
  text-shadow: 0 0 5px #00ff00;
}
.header { 
  background: #000;
  border: 4px double #00ff00; 
  padding: 20px;
  text-align: center;
  margin-bottom: 20px;
  animation: flicker 3s infinite;
}
@keyframes flicker {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.95; }
}
.header h1 { 
  color: #00ff00; 
  font-size: 48px; 
  margin-bottom: 5px;
  letter-spacing: 5px;
}
.header h1::before {
  content: '[ ';
}
.header h1::after {
  content: ' ]';
}
.version { 
  color: #00aa00; 
  font-size: 20px;
  letter-spacing: 3px;
}
.stats {
  background: #001100;
  border: 2px solid #00ff00;
  padding: 15px;
  margin-bottom: 20px;
  text-align: center;
  font-size: 22px;
}
.section {
  background: #001100;
  border: 2px solid #00ff00;
  padding: 20px;
  margin-bottom: 20px;
}
.section h2 {
  color: #00ff00;
  margin-bottom: 15px;
  font-size: 28px;
}
.section h2::before {
  content: '>> ';
}
button {
  background: #00ff00;
  color: #000;
  border: 2px solid #00ff00;
  padding: 10px 20px;
  font-family: 'VT323', monospace;
  font-size: 20px;
  font-weight: bold;
  cursor: pointer;
  margin: 5px;
  transition: all 0.2s;
  text-transform: uppercase;
}
button:hover {
  background: #000;
  color: #00ff00;
  box-shadow: 0 0 10px #00ff00;
}
textarea {
  background: #000;
  color: #00ff00;
  border: 2px solid #00ff00;
  padding: 10px;
  font-family: 'VT323', monospace;
  font-size: 18px;
  width: 100%;
  min-height: 200px;
  resize: vertical;
  margin: 10px 0;
}
.help-text {
  color: #00aa00;
  font-size: 16px;
  margin-top: 5px;
}
.cheat-sheet {
  background: #000;
  padding: 12px;
  margin: 10px 0;
  border-left: 4px solid #00ff00;
  cursor: pointer;
  transition: all 0.2s;
}
.cheat-sheet:hover {
  background: #001100;
  border-left-width: 8px;
  box-shadow: 0 0 10px #00ff00;
}
.cheat-title {
  color: #00ff00;
  font-weight: bold;
  margin-bottom: 5px;
  font-size: 22px;
}
.cheat-title::before {
  content: '> ';
}
.cheat-code {
  color: #00aa00;
  font-size: 16px;
  margin-left: 10px;
}
.category {
  color: #ff6600;
  font-size: 24px;
  margin: 20px 0 10px 0;
  font-weight: bold;
}
</style>
<script>
function loadCheat(code) {
  document.getElementById('terminal').value = code;
}
</script>
</head>
<body>

<div class="header">
<h1>HIGH SCHOOL HACKER</h1>
<div class="version">VERSION 1.00</div>
</div>

<div class="stats">
TERMINAL RUNS: %RUNS% | STATUS: ONLINE
</div>

<div class="section">
<h2>HACKER TERMINAL</h2>
<form method="POST" action="/terminal">
<textarea name="cmd" id="terminal" placeholder="ENTER DUCKYSCRIPT COMMANDS...">REM Type your custom DuckyScript here
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Hello World!
ENTER</textarea>
<div class="help-text">COMMANDS: GUI, STRING, ENTER, DELAY, CTRL, ALT, SHIFT, TAB, ESC</div>
<button type="submit">EXECUTE SCRIPT</button>
</form>
</div>

<div class="section">
<h2>PAYLOAD CHEAT SHEET</h2>
<div class="help-text">CLICK ANY PAYLOAD TO LOAD INTO TERMINAL</div>

<div class="category">PRANKS</div>

<div class="cheat-sheet" onclick="loadCheat('REM Endless Notepads Loop\nGUI r\nDELAY 500\nSTRING cmd\nENTER\nDELAY 1000\nSTRING :loop\nENTER\nSTRING start notepad\nENTER\nSTRING goto loop\nENTER')">
<div class="cheat-title">ENDLESS NOTEPADS</div>
<div class="cheat-code">Opens infinite notepad windows in a loop</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Rick Roll\nGUI\nDELAY 200\nSTRING chrome.exe\nENTER\nDELAY 2000\nSTRING https://www.youtube.com/watch?v=dQw4w9WgXcQ\nENTER')">
<div class="cheat-title">RICK ROLL</div>
<div class="cheat-code">Opens browser to Rick Astley - Never Gonna Give You Up</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Fake Virus Window\nGUI r\nDELAY 500\nSTRING cmd\nENTER\nDELAY 1000\nSTRING color 0C\nENTER\nSTRING title VIRUS DETECTED\nENTER\nSTRING echo WARNING: SYSTEM INFECTED!\nENTER\nSTRING echo Scanning files...\nENTER\nSTRING pause\nENTER')">
<div class="cheat-title">FAKE VIRUS</div>
<div class="cheat-code">Harmless fake virus warning window</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Browser Bomb\nGUI r\nDELAY 500\nSTRING cmd\nENTER\nDELAY 1000\nSTRING for /L %%i in (1,1,10) do start chrome.exe\nENTER')">
<div class="cheat-title">BROWSER BOMB</div>
<div class="cheat-code">Opens 10 browser windows rapidly</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Upside Down Text\nGUI r\nDELAY 500\nSTRING notepad\nENTER\nDELAY 1000\nSTRING ¡ǝɯ ʞɔɐɥ oʇ pǝıɹʇ noʎ\nENTER')">
<div class="cheat-title">UPSIDE DOWN MESSAGE</div>
<div class="cheat-code">Types upside-down text in notepad</div>
</div>

<div class="category">USEFUL TOOLS</div>

<div class="cheat-sheet" onclick="loadCheat('REM Take Screenshot\nGUI SHIFT s\nDELAY 1000')">
<div class="cheat-title">SCREENSHOT CAPTURE</div>
<div class="cheat-code">Opens Snipping Tool for screenshot</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM System Information\nGUI r\nDELAY 500\nSTRING msinfo32\nENTER')">
<div class="cheat-title">SYSTEM INFO</div>
<div class="cheat-code">Opens detailed PC specs window</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Task Manager\nCTRL SHIFT ESC')">
<div class="cheat-title">TASK MANAGER</div>
<div class="cheat-code">Opens Windows Task Manager</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Quick Restart\nGUI r\nDELAY 500\nSTRING shutdown /r /t 10\nENTER')">
<div class="cheat-title">QUICK RESTART</div>
<div class="cheat-code">Restarts PC in 10 seconds</div>
</div>

<div class="category">VISUAL EFFECTS</div>

<div class="cheat-sheet" onclick="loadCheat('REM Matrix Rain Effect\nGUI r\nDELAY 500\nSTRING cmd\nENTER\nDELAY 1000\nSTRING color 0A\nENTER\nSTRING tree C:\\\\ /A\nENTER')">
<div class="cheat-title">MATRIX RAIN</div>
<div class="cheat-code">Green scrolling directory tree like Matrix</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Green Hacker Terminal\nGUI r\nDELAY 500\nSTRING cmd\nENTER\nDELAY 1000\nSTRING color 0A\nENTER\nSTRING dir /s\nENTER')">
<div class="cheat-title">GREEN TERMINAL</div>
<div class="cheat-code">Green text scrolling terminal</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM CMD Rainbow Mode\nGUI r\nDELAY 500\nSTRING cmd\nENTER\nDELAY 1000\nSTRING :loop\nENTER\nSTRING color %random:~-1%%random:~-1%\nENTER\nSTRING echo HACKER MODE ACTIVATED\nENTER\nSTRING goto loop\nENTER')">
<div class="cheat-title">RAINBOW TERMINAL</div>
<div class="cheat-code">CMD window that rapidly changes colors</div>
</div>

<div class="category">EDUCATIONAL DEMO</div>

<div class="cheat-sheet" onclick="loadCheat('REM Hello World Demo\nGUI r\nDELAY 500\nSTRING notepad\nENTER\nDELAY 1000\nSTRING Hello World!\nENTER\nSTRING This is a DuckyScript demo.\nENTER')">
<div class="cheat-title">HELLO WORLD</div>
<div class="cheat-code">Simple notepad demonstration</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Create Organized Folders\nGUI r\nDELAY 500\nSTRING cmd\nENTER\nDELAY 1000\nSTRING cd Desktop\nENTER\nSTRING mkdir Projects\nENTER\nSTRING cd Projects\nENTER\nSTRING mkdir Code Documents Images\nENTER\nSTRING echo Folders created!\nENTER')">
<div class="cheat-title">FOLDER CREATOR</div>
<div class="cheat-code">Creates organized folder structure on Desktop</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Write Batch File\nGUI r\nDELAY 500\nSTRING notepad hello.bat\nENTER\nDELAY 1000\nSTRING @echo off\nENTER\nSTRING echo Hello from batch file!\nENTER\nSTRING pause\nENTER\nCTRL s\nDELAY 500\nALT F4')">
<div class="cheat-title">BATCH FILE WRITER</div>
<div class="cheat-code">Creates a simple .bat file</div>
</div>

<div class="category">DANGER ZONE</div>

<div class="cheat-sheet" onclick="loadCheat('REM Instant Shutdown\nGUI r\nDELAY 500\nSTRING shutdown /s /t 0\nENTER')">
<div class="cheat-title">INSTANT SHUTDOWN</div>
<div class="cheat-code">Shuts down computer immediately - USE WITH CAUTION!</div>
</div>

<div class="cheat-sheet" onclick="loadCheat('REM Lock Screen\nGUI l')">
<div class="cheat-title">LOCK SCREEN</div>
<div class="cheat-code">Locks the computer</div>
</div>

</div>

<div style="text-align: center; margin-top: 30px; color: #00aa00; font-size: 16px;">
[ SYSTEM READY - %RUNS% SCRIPTS EXECUTED ]
</div>

</body>
</html>
)rawliteral";

// ============================================================================
// FUNCTION: buildHTML
// Generates HTML page with current stats
// ============================================================================
String buildHTML() {
  String html = String(htmlPage);
  html.replace("%RUNS%", String(terminalRuns));
  return html;
}

// ============================================================================
// FUNCTION: updateDisplay
// Updates TFT display with rainbow effects and progress bar
// ============================================================================
void updateDisplay() {
  tft.fillScreen(TFT_BLACK);
  
  // Draw rainbow border (4 pixels thick, each pixel different color)
  for(int i = 0; i < 4; i++) {
    // Top and bottom borders
    for(int x = i; x < 160-i; x++) {
      tft.drawPixel(x, i, rainbowColors[x % 16]);
      tft.drawPixel(x, 79-i, rainbowColors[x % 16]);
    }
    // Left and right borders
    for(int y = i; y < 80-i; y++) {
      tft.drawPixel(i, y, rainbowColors[y % 16]);
      tft.drawPixel(159-i, y, rainbowColors[y % 16]);
    }
  }
  
  // Title line 1 - each letter different color
  String title = "HIGH SCHOOL";
  tft.setTextSize(2);
  int x = 12;
  for(int i = 0; i < title.length(); i++) {
    tft.setTextColor(rainbowColors[i % 16], TFT_BLACK);
    tft.setCursor(x, 12);
    tft.print(title[i]);
    x += 12;
  }
  
  // Title line 2 - each letter different color
  String title2 = "HACKER";
  x = 40;
  for(int i = 0; i < title2.length(); i++) {
    tft.setTextColor(rainbowColors[(i + 6) % 16], TFT_BLACK);
    tft.setCursor(x, 30);
    tft.print(title2[i]);
    x += 12;
  }
  
  // Version - each character different color
  String ver = "v1.00";
  tft.setTextSize(1);
  x = 62;
  for(int i = 0; i < ver.length(); i++) {
    tft.setTextColor(rainbowColors[(i + 12) % 16], TFT_BLACK);
    tft.setCursor(x, 48);
    tft.print(ver[i]);
    x += 6;
  }
  
  // Status line - each character different color
  String status = "RUNS:" + String(terminalRuns);
  tft.setTextSize(1);
  x = 10;
  for(int i = 0; i < status.length(); i++) {
    tft.setTextColor(rainbowColors[i % 16], TFT_BLACK);
    tft.setCursor(x, 58);
    tft.print(status[i]);
    x += 6;
  }
  
  // Progress bar - rainbow filled, gray empty
  int barWidth = 140;
  int barX = 10;
  int barY = 68;
  int fillWidth = (barWidth * progressBar) / 100;
  
  for(int i = 0; i < fillWidth; i++) {
    tft.drawFastVLine(barX + i, barY, 6, rainbowColors[i % 16]);
  }
  for(int i = fillWidth; i < barWidth; i++) {
    tft.drawFastVLine(barX + i, barY, 6, TFT_DARKGREY);
  }
}

// ============================================================================
// HTTP HANDLER: Root page
// ============================================================================
void handleRoot() {
  server.send(200, "text/html", buildHTML());
}

// ============================================================================
// HTTP HANDLER: Terminal execution
// ============================================================================
void handleTerminal() {
  if(server.hasArg("cmd")) {
    String script = server.arg("cmd");
    
    Serial.println("Executing terminal!");
    leds[0] = CRGB::Magenta;
    FastLED.show();
    
    // Animate progress bar during execution
    for(int i = 0; i <= 100; i += 10) {
      progressBar = i;
      updateDisplay();
      delay(50);
    }
    
    // Execute the DuckyScript
    executeDuckyScript(script);
    
    // Update counters and reset display
    terminalRuns++;
    progressBar = 0;
    updateDisplay();
    
    leds[0] = CRGB::Green;
    FastLED.show();
  }
  
  server.send(200, "text/html", buildHTML());
}

// ============================================================================
// FUNCTION: executeDuckyScript
// Parses and executes DuckyScript commands line-by-line
// ============================================================================
void executeDuckyScript(String script) {
  Serial.println("Executing DuckyScript:");
  Serial.println(script);
  
  // Normalize line endings
  script.replace("\r\n", "\n");
  script.replace("\r", "\n");
  
  int startPos = 0;
  int endPos = script.indexOf('\n');
  
  // Process each line
  while (startPos < script.length()) {
    String line;
    
    if (endPos == -1) {
      line = script.substring(startPos);
      startPos = script.length();
    } else {
      line = script.substring(startPos, endPos);
      startPos = endPos + 1;
      endPos = script.indexOf('\n', startPos);
    }
    
    line.trim();
    if (line.length() == 0) continue;
    
    // Parse and execute commands
    if (line.startsWith("REM") || line.startsWith("//")) {
      // Comment - skip
      continue;
    }
    else if (line.startsWith("DELAY ")) {
      // Delay command
      int delayTime = line.substring(6).toInt();
      delay(delayTime);
    }
    else if (line.startsWith("STRING ")) {
      // Type string command
      String text = line.substring(7);
      Keyboard.print(text);
    }
    else if (line == "ENTER") {
      // Enter key
      Keyboard.press(KEY_RETURN);
      Keyboard.releaseAll();
      delay(50);
    }
    else if (line == "TAB") {
      // Tab key
      Keyboard.press(KEY_TAB);
      Keyboard.releaseAll();
      delay(50);
    }
    else if (line.startsWith("GUI ") || line.startsWith("WINDOWS ")) {
      // Windows/GUI key combinations
      String key = line.substring(line.indexOf(' ') + 1);
      key.trim();
      
      Keyboard.press(KEY_LEFT_GUI);
      delay(50);
      
      if (key == "SPACE") {
        Keyboard.press(' ');
      } else if (key == "SHIFT") {
        Keyboard.press(KEY_LEFT_SHIFT);
      } else if (key.length() == 1) {
        Keyboard.press(key.charAt(0));
      }
      delay(50);
      Keyboard.releaseAll();
      delay(100);
    }
    else if (line == "GUI" || line == "WINDOWS") {
      // Windows key alone
      Keyboard.press(KEY_LEFT_GUI);
      delay(100);
      Keyboard.releaseAll();
      delay(200);
    }
    else if (line.startsWith("CTRL ") || line.startsWith("CONTROL ")) {
      // Control key combinations
      String key = line.substring(line.indexOf(' ') + 1);
      key.trim();
      
      Keyboard.press(KEY_LEFT_CTRL);
      delay(50);
      
      if (key == "SHIFT") {
        Keyboard.press(KEY_LEFT_SHIFT);
      } else if (key == "ESC" || key == "ESCAPE") {
        Keyboard.press(KEY_ESC);
      } else if (key.length() == 1) {
        Keyboard.press(key.charAt(0));
      }
      delay(50);
      Keyboard.releaseAll();
      delay(100);
    }
    else if (line.startsWith("ALT ")) {
      // Alt key combinations
      String key = line.substring(4);
      key.trim();
      
      Keyboard.press(KEY_LEFT_ALT);
      delay(50);
      
      if (key == "F4") {
        Keyboard.press(KEY_F4);
      } else if (key.length() == 1) {
        Keyboard.press(key.charAt(0));
      }
      delay(50);
      Keyboard.releaseAll();
      delay(100);
    }
    else if (line.startsWith("SHIFT ")) {
      // Shift key combinations
      String key = line.substring(6);
      key.trim();
      
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(50);
      
      if (key.length() == 1) {
        Keyboard.press(key.charAt(0));
      }
      delay(50);
      Keyboard.releaseAll();
      delay(100);
    }
    else if (line == "ESCAPE" || line == "ESC") {
      // Escape key
      Keyboard.press(KEY_ESC);
      Keyboard.releaseAll();
      delay(50);
    }
    else if (line == "SPACE") {
      // Space key
      Keyboard.press(' ');
      Keyboard.releaseAll();
      delay(50);
    }
    
    delay(10);
  }
}

// ============================================================================
// SETUP: Initialize hardware and start services
// ============================================================================
void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  // Initialize RGB LED
  FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  leds[0] = CRGB::Blue;
  FastLED.show();
  
  // Initialize TFT display
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, LOW);
  
  tft.init();
  tft.setRotation(1);  // 160x80 landscape orientation
  tft.fillScreen(TFT_BLACK);
  
  // Fade in backlight smoothly
  for (int i = 0; i < 255; i += 10) {
    analogWrite(BACKLIGHT_PIN, i);
    delay(5);
  }
  digitalWrite(BACKLIGHT_PIN, HIGH);
  
  // Display initial screen
  updateDisplay();
  
  // Initialize USB HID Keyboard
  USB.begin();
  Keyboard.begin();
  
  // Start WiFi Access Point
  Serial.println("Starting WiFi AP...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(IP);
  
  // Configure web server routes
  server.on("/", handleRoot);
  server.on("/terminal", HTTP_POST, handleTerminal);
  
  // Start web server
  server.begin();
  Serial.println("HTTP server started");
  
  // Set LED to green (ready state)
  leds[0] = CRGB::Green;
  FastLED.show();
}

// ============================================================================
// LOOP: Main program loop
// ============================================================================
void loop() {
  // Keep backlight on
  digitalWrite(BACKLIGHT_PIN, HIGH);
  
  // Handle web server requests
  server.handleClient();
  
  delay(10);
}

// ============================================================================
// END OF CODE
// ============================================================================