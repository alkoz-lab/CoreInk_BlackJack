#include "MessageScreen.hpp"
#include "EinkFrame.hpp"
#include "StatusBar.hpp"
#include "TextUtil.hpp"

void MessageScreen::show(const char *title, const char *const *lines, int lineCount,
                         const char *prompt)
{
    auto &d = display_;
    EinkFrame::begin(d);
    TextUtil::drawTitle(d, title);
    d.setTextDatum(textdatum_t::top_center);
    d.setFont(&fonts::Satisfy_24);
    for (int i = 0; i < lineCount; ++i)
        d.drawString(lines[i], d.width() / 2, FIRST_LINE_Y + i * LINE_PITCH);
    d.setTextDatum(textdatum_t::top_left);
    prompt_.draw(d, prompt);
    EinkFrame::present(d);
    d.waitDisplay();
    buttons_.waitForAny();
}

void MessageScreen::showFatal(const char *message)
{
    auto &d = display_;
    EinkFrame::begin(d);
    StatusBar(0).draw(d, "FATAL ERROR:");
    d.setTextWrap(true, false);
    TextUtil::drawAt(d, 0, TextUtil::LINE_HEIGHT, message);
    EinkFrame::present(d);
}
