#include "MenuScreen.hpp"
#include "EinkFrame.hpp"
#include "TextUtil.hpp"

namespace
{
    constexpr int FIRST_ITEM_Y = 48;
    constexpr int ITEM_PITCH = 28;
    constexpr int HIGHLIGHT_MARGIN_X = 8;
    constexpr int HIGHLIGHT_PAD_Y = 4;
    constexpr int HIGHLIGHT_HEIGHT = 24;
}

void MenuScreen::draw(const char *title, const char *const *items, int itemCount, int selected)
{
    auto &d = display_;
    EinkFrame::begin(d);
    TextUtil::drawTitle(d, title);
    d.setTextDatum(textdatum_t::top_center);
    for (int i = 0; i < itemCount; ++i)
    {
        const int y = FIRST_ITEM_Y + i * ITEM_PITCH;
        if (i == selected)
            d.fillRect(HIGHLIGHT_MARGIN_X, y - HIGHLIGHT_PAD_Y,
                       d.width() - 2 * HIGHLIGHT_MARGIN_X, HIGHLIGHT_HEIGHT, TFT_BLACK);
        d.setTextColor(i == selected ? TFT_WHITE : TFT_BLACK);
        d.drawString(items[i], d.width() / 2, y);
    }
    d.setTextDatum(textdatum_t::top_left);
    prompt_.draw(d, "\x8C/\x8F  \x8A:Select");
    EinkFrame::present(d);
    d.waitDisplay();
}

int MenuScreen::select(const char *title, const char *const *items, int itemCount)
{
    using Button = Buttons::Button;
    int selected = 0;
    while (true)
    {
        draw(title, items, itemCount, selected);
        switch (buttons_.waitFor({Button::Select, Button::Up, Button::Down}))
        {
        case Button::Select:
            return selected;
        case Button::Up:
            selected = (selected + itemCount - 1) % itemCount;
            break;
        default:
            selected = (selected + 1) % itemCount;
            break;
        }
    }
}
