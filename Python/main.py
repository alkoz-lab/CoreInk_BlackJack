from art import logo
from colorama import init, Fore
from score import Score
from deck import Deck
from hand import Hand, BLACKJACK_VALUE
from presenter import Presenter
from blackjack import determine_winner, rules

def pause():
    input("Press Enter to continue...")

init(autoreset=True)
print(Fore.GREEN + logo)
name = ""
while len(name) == 0:
    name = input("Enter your name: ")
max_score  = int(input("Enter the maximum score (e.g. 10): "))
score = Score(max_score)
print("Welcome to Blackjack, " + name + "!")
print("We'll play until the score reaches " + str(max_score) + ".")
pause()
round = 1
while not score.game_over():
    deck = Deck()
    deck.shuffle()
    left = Hand("Dealer")
    right = Hand(name)
    presenter = Presenter(left, right, score)
    left.take_card(deck.deal())
    presenter.one_player_takes_cards(round)
    left.take_card(deck.deal())
    presenter.one_player_takes_cards(round)
    right.take_card(deck.deal())
    presenter.one_player_takes_cards(round)
    right.take_card(deck.deal())
    your_move = ""
    while True:
        if your_move == "hit" or your_move == "h":
            right.take_card(deck.deal())
        presenter.one_player_takes_cards(round)
        if right.is_bust() or right.get_value() == BLACKJACK_VALUE or your_move == "stand" or your_move == "s":
            break
        your_move = input("Type 'help', 'hit' or 'stand'? ").lower()
        if your_move == "help":
            print(rules())
            print("Type 'hit' to take another card.")
            print("Type 'stand' to stop taking cards.")
            print("Type 'help' to see this message again.")
            pause()
    if not right.is_bust():
        your_value = right.get_value()
        while not left.is_bust() and left.get_value() < (BLACKJACK_VALUE - 4):
            left.take_card(deck.deal())
            presenter.another_player_takes_cards(round)
    determine_winner(left, right, score)
    presenter.round_finished(round)
    round += 1
    if not score.game_over():
        pause()