#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#define PROGMEM
constexpr int TFT_BLACK = 0;
constexpr int TFT_WHITE = 1;

namespace fonts
{
    inline int AsciiFont8x16;
    inline int Font8x8C64;
    inline int Satisfy_24;
}
namespace textdatum_t
{
    enum Datum { top_left, top_center };
}
namespace epd_mode_t
{
    enum Mode { epd_quality, epd_fast };
}

struct BitmapDraw
{
    int x, y, width, height;
    const unsigned char *bitmap;
};
struct Frame
{
    std::vector<BitmapDraw> cards;
    std::vector<std::string> text;
};
inline std::vector<std::string> displayEvents;

struct TestDisplay
{
    Frame current;
    std::vector<Frame> frames;
    int width() const { return 200; }
    void setEpdMode(epd_mode_t::Mode) {}
    void setTextColor(int) {}
    void setTextWrap(bool, bool) {}
    void setTextDatum(textdatum_t::Datum) {}
    void setFont(const int *) {}
    void setCursor(int, int) {}
    void fillRect(int, int, int, int, int) {}
    void drawRect(int, int, int, int, int) {}
    void fillScreen(int) { current = {}; }
    int textWidth(const char *text) const { return static_cast<int>(std::strlen(text)) * 8; }
    void print(const char *text) { current.text.emplace_back(text); }
    void write(uint8_t) {}
    void drawString(const char *text, int, int) { print(text); }
    void drawBitmap(int x, int y, const unsigned char *bitmap,
                    int width, int height, int, int)
    {
        current.cards.push_back({x, y, width, height, bitmap});
    }
    void display()
    {
        frames.push_back(current);
        displayEvents.emplace_back("display");
    }
    void waitDisplay() { displayEvents.emplace_back("wait"); }
    void wakeup() { displayEvents.emplace_back("wakeup"); }
};

// Button state seen after each M5.update(): every button pressed (default), none, or one.
enum TestPress { PRESS_ALL = -1, PRESS_NONE = -2, PRESS_A = 0, PRESS_B, PRESS_C, PRESS_EXT };
inline TestPress testPressed = PRESS_ALL;
// When non-empty, each M5.update() takes the next state from here instead.
inline std::vector<TestPress> testPressScript;

struct TestButton
{
    TestPress id;
    bool wasPressed() const { return testPressed == PRESS_ALL || testPressed == id; }
};
struct TestPower
{
    int millivolts = 4000;
    int reads = 0;
    int getBatteryVoltage() { ++reads; return millivolts; }
    void powerOff() { displayEvents.emplace_back("powerOff"); }
};
struct TestM5
{
    TestDisplay Display;
    TestPower Power;
    TestButton BtnA{PRESS_A}, BtnB{PRESS_B}, BtnC{PRESS_C}, BtnEXT{PRESS_EXT};
    void update()
    {
        if (!testPressScript.empty())
        {
            testPressed = testPressScript.front();
            testPressScript.erase(testPressScript.begin());
        }
    }
};
inline TestM5 M5;
using M5GFX = TestDisplay;
inline uint32_t testMillis = 0;
inline uint32_t millis() { return testMillis; }
inline void delay(uint32_t milliseconds)
{
    displayEvents.push_back("delay:" + std::to_string(milliseconds));
}
