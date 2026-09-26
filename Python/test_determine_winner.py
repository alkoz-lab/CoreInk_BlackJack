import unittest
from hand import Hand
from score import Score
from card import Card, RANK_ACE, RANK_JACK, SUIT_CLUBS, SUIT_DIAMONDS, SUIT_HEARTS, SUIT_SPADES
from blackjack import determine_winner


class DetermineWinnerTests(unittest.TestCase):

    def test_21_wins_over_less_than_21(self):
        left = Hand("left")
        left.take_card(Card(SUIT_HEARTS, RANK_JACK))
        left.take_card(Card(SUIT_CLUBS, 10))
        left.take_card(Card(SUIT_HEARTS, RANK_ACE))
        right = Hand("right")
        right.take_card(Card(SUIT_SPADES, RANK_JACK))
        right.take_card(Card(SUIT_DIAMONDS, 5))
        score1 = Score(2)
        score2 = Score(2)
        determine_winner(left, right, score1)
        determine_winner(right, left, score2)
        self.assertTrue(score1.is_left_winner)
        self.assertFalse(score1.is_draw)
        self.assertFalse(score2.is_left_winner)
        self.assertFalse(score2.is_draw)

    def test_draw(self):
        left = Hand("left")
        left.take_card(Card(SUIT_HEARTS, RANK_JACK))
        left.take_card(Card(SUIT_CLUBS, 10))
        right = Hand("right")
        right.take_card(Card(SUIT_SPADES, RANK_JACK))
        right.take_card(Card(SUIT_DIAMONDS, 10))
        score1 = Score(2)
        score2 = Score(2)
        determine_winner(left, right, score1)
        determine_winner(right, left, score2)
        self.assertFalse(score1.is_left_winner)
        self.assertTrue(score1.is_draw)
        self.assertFalse(score2.is_left_winner)
        self.assertTrue(score2.is_draw)

    def test_less_than_21_wins_over_bust(self):
        left = Hand("left")
        left.take_card(Card(SUIT_HEARTS, RANK_JACK))
        left.take_card(Card(SUIT_CLUBS, 10))
        right = Hand("right")
        right.take_card(Card(SUIT_SPADES, RANK_JACK))
        right.take_card(Card(SUIT_DIAMONDS, 10))
        right.take_card(Card(SUIT_HEARTS, 10))
        score1 = Score(2)
        score2 = Score(2)
        determine_winner(left, right, score1)
        determine_winner(right, left, score2)
        self.assertTrue(score1.is_left_winner)
        self.assertFalse(score1.is_draw)
        self.assertFalse(score2.is_left_winner)
        self.assertFalse(score2.is_draw)


if __name__ == '__main__':
    unittest.main()
