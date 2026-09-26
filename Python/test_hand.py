from hand import Hand
from card import Card, RANK_ACE, RANK_JACK, RANK_QUEEN, RANK_KING, SUIT_CLUBS, SUIT_DIAMONDS, SUIT_HEARTS, SUIT_SPADES
import unittest


class HandTests(unittest.TestCase):

    def test_when_hand_is_created_it_has_name(self):
        hand = Hand("Player")
        self.assertEqual(str(hand), "Player")

    def test_when_hand_is_created_it_has_no_cards(self):
        hand = Hand("Player")
        self.assertEqual(hand.get_cards(), "")

    def test_when_hand_is_created_it_has_value_of_zero(self):
        hand = Hand("Player")
        self.assertEqual(hand.get_value(), 0)

    def test_when_hand_is_created_it_is_not_bust(self):
        hand = Hand("Player")
        self.assertFalse(hand.is_bust())

    def test_when_hand_is_given_card_it_has_value_of_card(self):
        hand = Hand("Player")
        hand.take_card(Card(SUIT_CLUBS, RANK_KING))
        self.assertEqual(hand.get_value(), 10)

    def test_when_hand_is_given_two_cards_it_has_value_of_cards(self):
        hand = Hand("Player")
        hand.take_card(Card(SUIT_DIAMONDS, RANK_ACE))
        hand.take_card(Card(SUIT_HEARTS, 2))
        self.assertEqual(hand.get_value(), 13)

    def test_when_both_aces_count_as_1_when_value_is_over_21(self):
        hand = Hand("Player")
        hand.take_card(Card(SUIT_SPADES, RANK_QUEEN))
        hand.take_card(Card(SUIT_SPADES, RANK_ACE))
        hand.take_card(Card(SUIT_CLUBS, RANK_ACE))
        self.assertEqual(hand.get_value(), 12)

    def test_is_bust(self):
        hand = Hand("Player")
        hand.take_card(Card(SUIT_CLUBS, 10))
        hand.take_card(Card(SUIT_DIAMONDS, RANK_JACK))
        hand.take_card(Card(SUIT_HEARTS, 2))
        self.assertTrue(hand.get_value(), 22)
        self.assertTrue(hand.is_bust())

    def test_get_first_card_returns_first_card(self):
        hand = Hand("Player")
        hand.take_card(Card(SUIT_HEARTS, 10))
        hand.take_card(Card(SUIT_CLUBS, 5))
        self.assertEqual(str(hand.get_first_card()), str(Card(SUIT_HEARTS, 10)))

    def test_when_2_aces_on_hand_they_might_be_counted_as_1_or_11(self):
        hand = Hand("Player")
        hand.take_card(Card(SUIT_HEARTS, RANK_ACE))  # 11
        hand.take_card(Card(SUIT_SPADES, RANK_ACE))  # 1
        hand.take_card(Card(SUIT_DIAMONDS, 9))
        self.assertEqual(hand.get_value(), 21)

    def test_when_ace_on_hand_it_should_be_counted_as_1_not_to_bust(self):
        hand = Hand("Player")
        hand.take_card(Card(SUIT_HEARTS, RANK_JACK))
        hand.take_card(Card(SUIT_HEARTS, RANK_QUEEN))
        hand.take_card(Card(SUIT_SPADES, RANK_ACE))  # 1
        self.assertEqual(hand.get_value(), 21)

    def test_all_ranks_add_up_to_expected_amount(self):
        hand = Hand("Player")
        hand.take_card(Card(SUIT_HEARTS, RANK_ACE))
        hand.take_card(Card(SUIT_HEARTS, 2))
        hand.take_card(Card(SUIT_HEARTS, 3))
        hand.take_card(Card(SUIT_HEARTS, 4))
        hand.take_card(Card(SUIT_HEARTS, 5))
        hand.take_card(Card(SUIT_HEARTS, 6))
        hand.take_card(Card(SUIT_HEARTS, 7))
        hand.take_card(Card(SUIT_HEARTS, 8))
        hand.take_card(Card(SUIT_HEARTS, 9))
        hand.take_card(Card(SUIT_HEARTS, 10))
        hand.take_card(Card(SUIT_HEARTS, RANK_JACK))
        hand.take_card(Card(SUIT_HEARTS, RANK_QUEEN))
        hand.take_card(Card(SUIT_HEARTS, RANK_KING))
        self.assertEqual(hand.get_value(), 85)


if __name__ == '__main__':
    unittest.main()
