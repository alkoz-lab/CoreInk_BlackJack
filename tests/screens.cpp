#include "../blackjack/src/CoreInkKit/MenuScreen.hpp"
#include "../blackjack/src/CoreInkKit/MessageScreen.hpp"
#include "../blackjack/src/CoreInkKit/PagedTextScreen.hpp"
#include "../blackjack/RulesText.hpp"
#include "../blackjack/SplashScreen.hpp"
#include "check.hpp"
#include <algorithm>

// Stub buttons report every button as pressed, so each blocking screen returns at once.
namespace
{
    Buttons buttons;
    PromptBar prompt;

    bool hasText(const char *text)
    {
        const auto &frames = M5.Display.frames;
        if (frames.empty())
            return false;
        const auto &lines = frames.back().text;
        return std::find(lines.begin(), lines.end(), text) != lines.end();
    }

    bool showsPercentage()
    {
        for (const auto &frame : M5.Display.frames)
            for (const auto &line : frame.text)
                if (!line.empty() && line.back() == '%')
                    return true;
        return false;
    }

    void checkNoBattery(const char *screen)
    {
        require(!showsPercentage(), screen);
        require(M5.Power.reads == 0, "screens outside the table never read the battery");
    }
}

int main()
{
    SplashScreen splash(M5.Display, buttons, prompt);
    MenuScreen menu(M5.Display, buttons, prompt);
    PagedTextScreen pages(M5.Display, buttons, prompt);
    MessageScreen messages(M5.Display, buttons, prompt);

    M5.Display = {};
    splash.show();
    require(hasText("Blackjack") && M5.Display.frames.back().cards.size() == 4,
            "splash shows the title over four cards");
    checkNoBattery("splash has no battery");

    M5.Display = {};
    const char *const items[] = {"START GAME", "RULES", "POWER OFF"};
    require(menu.select("Blackjack", items, 3) == 0, "pressing the rocker selects the item");
    require(hasText("Blackjack") && hasText("RULES") && M5.Display.frames.size() == 1,
            "menu draws title and items in one refresh");
    checkNoBattery("menu has no battery");

    M5.Display = {};
    pages.show("Rules", RulesText::LINES, RulesText::LINE_COUNT);
    const int linesPerPage = (prompt.y() - PagedTextScreen::TEXT_Y) / 16;
    const int pageCount = (static_cast<int>(RulesText::LINE_COUNT) + linesPerPage - 1) /
                          linesPerPage;
    char rulesTitle[32];
    std::snprintf(rulesTitle, sizeof(rulesTitle), "Rules 1/%d", pageCount);
    require(hasText(rulesTitle) && hasText(RulesText::LINES[0]), "rules show the first page");
    checkNoBattery("rules have no battery");

    M5.Display = {};
    const char *const lines[] = {"You won!", "Score 1:5"};
    messages.show("Game over", lines, 2, "Press any key");
    require(hasText("Game over") && hasText("You won!") && hasText("Score 1:5"),
            "message screen shows title and lines");
    checkNoBattery("game over has no battery");

    M5.Display = {};
    splash.showPowerOff();
    require(M5.Display.frames.size() == 1 && hasText("Blackjack"),
            "power-off frame is the splash art");
    checkNoBattery("power-off frame has no battery");

    M5.Display = {};
    messages.showFatal("deck is empty");
    require(hasText("FATAL ERROR:") && hasText("deck is empty"), "fatal error shows the message");
    checkNoBattery("fatal error has no battery");

    std::puts("All menu, rules, message and splash screen tests passed.");
}
