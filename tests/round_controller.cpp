#include "../blackjack/RoundController.hpp"
#include "check.hpp"
#include <deque>
#include <string>
#include <vector>

namespace
{
    // Deals a scripted sequence: dealer, dealer, player, player, then hits in order.
    class StackedCards : public ICardSource
    {
    public:
        std::deque<int> ranks;
        int shuffles = 0;
        void shuffleNewDeck() override { ++shuffles; }
        Card deal() override
        {
            require(!ranks.empty(), "test deck has enough cards");
            const int rank = ranks.front();
            ranks.pop_front();
            return Card(Card::SUIT_HEARTS, rank);
        }
    };

    class RecordingView : public ITableView
    {
    public:
        std::vector<std::string> events;
        Outcome lastOutcome = Outcome::Draw;

        void showInitialDeal(const TableState &table) override
        {
            events.push_back("deal " + std::to_string(table.dealer.size()) + "/" +
                             std::to_string(table.player.size()));
        }
        void showHit(const TableState &table, Side side) override
        {
            events.push_back(side == Side::Dealer
                                 ? "dealer hit " + std::to_string(table.dealer.size())
                                 : "player hit " + std::to_string(table.player.size()));
        }
        void showRoundResult(const TableState &table, Outcome outcome) override
        {
            lastOutcome = outcome;
            events.push_back("result #" + std::to_string(table.round) + " " +
                             table.score.toString());
        }
    };

    class ScriptedInput : public IPlayerInput
    {
    public:
        std::deque<Decision> decisions;
        int asked = 0;
        Decision askHitOrStand() override
        {
            ++asked;
            require(!decisions.empty(), "input script has enough decisions");
            const Decision decision = decisions.front();
            decisions.pop_front();
            return decision;
        }
    };

    using D = IPlayerInput::Decision;

    void playerHitsThenStandsDealerDraws()
    {
        StackedCards cards;
        cards.ranks = {10, 5, 9, 2, 8, 3}; // dealer 15, player 11 -> hit 8 = 19; dealer hits 3 = 18
        RecordingView view;
        ScriptedInput input;
        input.decisions = {D::Hit, D::Stand};
        RoundController game(5, cards, view, input);

        game.playRound();

        const std::vector<std::string> expected = {
            "deal 2/2", "player hit 3", "dealer hit 3", "result #1 0:1+"};
        require(view.events == expected, "round flows deal, hits, result in order");
        require(view.lastOutcome == Outcome::PlayerWins, "higher player total wins");
        require(cards.shuffles == 1, "every round starts with a fresh shuffled deck");
        require(game.round() == 2, "round counter advances");
    }

    void playerBustStopsAskingAndDoubleBustDraws()
    {
        StackedCards cards;
        cards.ranks = {10, 6, 10, 2, 10, 10}; // player 12 + 10 = bust; dealer 16 + 10 = bust
        RecordingView view;
        ScriptedInput input;
        input.decisions = {D::Hit};
        RoundController game(5, cards, view, input);

        game.playRound();

        require(input.asked == 1, "a busted player is not asked again");
        require(view.lastOutcome == Outcome::Draw, "double bust is a draw");
        require(game.score().dealerScore() == 0 && game.score().playerScore() == 0,
                "draw adds no points");
    }

    void dealerStandsOnPolicyValue()
    {
        StackedCards cards;
        cards.ranks = {10, 7, 10, 10}; // dealer 17 stands, player 20 stands
        RecordingView view;
        ScriptedInput input;
        input.decisions = {D::Stand};
        RoundController game(1, cards, view, input);

        game.playRound();

        require(view.events.size() == 2, "dealer on 17 does not hit");
        require(game.isGameOver(), "reaching max score ends the game");
        game.reset(3);
        require(!game.isGameOver() && game.round() == 1 && game.score().maxScore() == 3,
                "reset starts a new game");
    }

    void playerOn21StandsAutomatically()
    {
        StackedCards cards;
        cards.ranks = {10, 8, 1, 10}; // dealer 18; player ace + 10 = 21 on the deal
        RecordingView view;
        ScriptedInput input;
        RoundController natural(5, cards, view, input);
        natural.playRound();
        require(input.asked == 0, "a dealt 21 stands without asking");
        require(view.lastOutcome == Outcome::PlayerWins, "21 beats 18");

        StackedCards hitCards;
        hitCards.ranks = {10, 8, 9, 2, 10}; // player 11, hits 10 = 21
        RecordingView hitView;
        ScriptedInput hitInput;
        hitInput.decisions = {D::Hit};
        RoundController hitTo21(5, hitCards, hitView, hitInput);
        hitTo21.playRound();
        require(hitInput.asked == 1, "hitting to 21 stands without asking again");
        require(hitView.lastOutcome == Outcome::PlayerWins, "hit to 21 beats 18");
    }

    void customDealerPolicy()
    {
        StackedCards cards;
        cards.ranks = {10, 7, 10, 10, 2}; // dealer 17 hits under a stand-on-18 policy
        RecordingView view;
        ScriptedInput input;
        input.decisions = {D::Stand};
        RoundController game(5, cards, view, input, DealerPolicy(18));

        game.playRound();

        require(view.events[1] == "dealer hit 3", "dealer policy is injectable");
    }
}

int main()
{
    playerHitsThenStandsDealerDraws();
    playerBustStopsAskingAndDoubleBustDraws();
    dealerStandsOnPolicyValue();
    playerOn21StandsAutomatically();
    customDealerPolicy();
    std::puts("All round controller tests passed.");
    return EXIT_SUCCESS;
}