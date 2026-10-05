#include "../blackjack/RoundJudge.hpp"
#include "../blackjack/Score.hpp"
#include "check.hpp"
#include <initializer_list>

namespace
{
    Hand makeHand(const char *name, std::initializer_list<int> ranks)
    {
        Hand hand(name);
        for (int rank : ranks)
            hand.takeCard(Card(Card::SUIT_CLUBS, rank));
        return hand;
    }

    void checkRound(const char *name, std::initializer_list<int> dealerRanks,
                    std::initializer_list<int> playerRanks, Outcome expected)
    {
        require(RoundJudge::judge(makeHand("DEALER", dealerRanks),
                                  makeHand("PLAYER", playerRanks)) == expected,
                name);
    }

    void checkScoreRecording()
    {
        Score score(10);
        score.record(Outcome::DealerWins);
        require(score.dealerScore() == 1 && score.isDealerWinner() &&
                    score.toString() == "+1:0",
                "dealer win is recorded and marked");
        score.record(Outcome::PlayerWins);
        require(score.playerScore() == 1 && !score.isDealerWinner() &&
                    score.toString() == "1:1+",
                "player win is recorded and marked");
        score.record(Outcome::Draw);
        require(score.dealerScore() == 1 && score.playerScore() == 1 &&
                    score.isDraw() && score.toString() == "1:1",
                "draw adds no points");
    }
}

int main()
{
    checkRound("equal double bust", {10, 10, 5}, {10, 10, 5}, Outcome::Draw);
    checkRound("dealer bust total higher", {10, 10, 10}, {10, 10, 2}, Outcome::Draw);
    checkRound("player bust total higher", {10, 10, 2}, {10, 10, 10}, Outcome::Draw);
    checkRound("dealer bust only", {10, 10, 5}, {10, 10}, Outcome::PlayerWins);
    checkRound("player bust only", {10, 10}, {10, 10, 5}, Outcome::DealerWins);
    checkRound("equal non-bust totals", {10, 10}, {10, 10}, Outcome::Draw);
    checkRound("dealer higher non-bust total", {10, 1}, {10, 9}, Outcome::DealerWins);
    checkRound("player higher non-bust total", {10, 9}, {10, 1}, Outcome::PlayerWins);
    checkRound("ace adjusted below bust", {10, 10, 1}, {10, 9}, Outcome::DealerWins);
    checkScoreRecording();
    std::puts("All round judge tests passed.");
    return EXIT_SUCCESS;
}