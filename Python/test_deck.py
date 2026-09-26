from deck import Deck
import unittest


class DeckTests(unittest.TestCase):

    def test_deck_has_52_cards(self):
        self.assertEqual(len(Deck().cards), 52)

    def test_deck_is_reduced_when_dealt(self):
        deck = Deck()
        for _ in range(52):
            deck.deal()
        self.assertEqual(len(deck.cards), 0)


if __name__ == '__main__':
    unittest.main()
