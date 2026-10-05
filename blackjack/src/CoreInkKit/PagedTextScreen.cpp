#include "PagedTextScreen.hpp"
#include "EinkFrame.hpp"
#include "TextUtil.hpp"
#include <cstdio>
#include <cstring>

void PagedTextScreen::drawPage(const char *title, const char *const *lines, int lineCount,
                               int page, int pageCount, int linesPerPage)
{
    auto &d = display_;
    EinkFrame::begin(d);
    char pageTitle[32];
    snprintf(pageTitle, sizeof(pageTitle), "%s %d/%d", title, page + 1, pageCount);
    TextUtil::drawTitle(d, pageTitle);
    for (int i = 0; i < linesPerPage; ++i)
    {
        const int index = page * linesPerPage + i;
        if (index >= lineCount)
            break;
        if (std::strcmp(lines[index], "\n") != 0)
            TextUtil::drawAt(d, 0, TEXT_Y + i * TextUtil::LINE_HEIGHT, lines[index]);
    }
    prompt_.draw(d, "\x8C/\x8F  \x8A:Back");
    EinkFrame::present(d);
    d.waitDisplay();
}

void PagedTextScreen::show(const char *title, const char *const *lines, std::size_t lineCount)
{
    using Button = Buttons::Button;
    const int linesPerPage = (prompt_.y() - TEXT_Y) / TextUtil::LINE_HEIGHT;
    const int count = static_cast<int>(lineCount);
    const int pageCount = count == 0 ? 1 : (count + linesPerPage - 1) / linesPerPage;
    int page = 0;
    bool redraw = true;
    while (true)
    {
        if (redraw)
            drawPage(title, lines, count, page, pageCount, linesPerPage);
        redraw = false;
        switch (buttons_.waitFor({Button::Select, Button::Up, Button::Down}))
        {
        case Button::Select:
            return;
        case Button::Up:
            if (page > 0)
            {
                --page;
                redraw = true;
            }
            break;
        default:
            if (page + 1 < pageCount)
            {
                ++page;
                redraw = true;
            }
            break;
        }
    }
}
