from hand import Hand
from score import Score

def determine_winner(left : Hand, right : Hand, score : Score):
    dealer_value = left.get_value()
    your_value = right.get_value()

    if left.is_bust():
        score.right_wins()
    elif right.is_bust():
        score.left_wins()
    elif dealer_value == your_value:
        score.draw()
    elif dealer_value > your_value:
        score.left_wins()
    else: 
        score.right_wins()

def rules():
    return """ Blackjack is a card game where players try to get a hand value
 of 21 or as close to 21 as possible without going over.
 Players are dealt two cards and can choose to 'hit' for
 additional cards or 'stand' to keep their current hand.
 The dealer also receives two cards, one face-up and one face-down.
 The dealer must hit until their hand value is at least 17.
 If a player's hand value is higher than the dealer's without
 going over 21, the player wins."""