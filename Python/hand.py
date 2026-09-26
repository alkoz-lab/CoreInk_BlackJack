from card import Card, RANK_ACE, SUIT_HEARTS

BLACKJACK_VALUE = 21

class Hand:
    name = ""
    cards : list[Card] = []

    def __init__(self, name):
        self.name = name
        self.cards = []

    def __str__(self):
        return self.name
    
    def get_cards(self):
        return " ".join([str(card) for card in self.cards])
    
    def get_first_card(self):
        return self.cards[0]
    
    def get_value(self):
        value_no_aces = 0
        aces = []
        # count aces later
        for card in self.cards:
            if (card.rank == RANK_ACE):
                aces.append(card)
            else:
                value_no_aces += card.get_value()
        # count aces
        value_only_aces = 0
        aces_count = len(aces)
        if (aces_count > 0):
            ace = Card(SUIT_HEARTS, RANK_ACE)
            ace_max_value = ace.get_value()
            ace_min_value = ace.get_value_when_over21()

            # check if one of the aces may be counted by max value
            if value_no_aces + ace_min_value * (aces_count - 1) + ace_max_value <= BLACKJACK_VALUE:
                value_only_aces = ace_min_value * (aces_count - 1) + ace_max_value
            else:
                value_only_aces = ace_min_value * aces_count # nope, all aces are counted by min value

        return value_no_aces + value_only_aces
    
    def is_bust(self):
        return self.get_value() > BLACKJACK_VALUE
    
    def take_card(self, card):
        self.cards.append(card)