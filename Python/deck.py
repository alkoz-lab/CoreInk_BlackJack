import random
from card import Card, SUIT_CLUBS, SUIT_DIAMONDS, SUIT_HEARTS, SUIT_SPADES

class Deck:
    def __init__(self):
        self.cards = []
        for suit in [SUIT_CLUBS, SUIT_DIAMONDS, SUIT_HEARTS, SUIT_SPADES]:
            for rank in range(1, 13 + 1):
                self.cards.append(Card(suit, rank))

    def shuffle(self):
        random.shuffle(self.cards)

    def deal(self):
        return self.cards.pop()