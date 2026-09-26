from hand import Hand
from score import Score
from card import Card
import os
from time import sleep

class Presenter:
    dealer : Hand
    you : Hand
    score : Score

    def __init__(self, left, right, score):
        self.dealer = left
        self.you = right
        self.score = score

    def one_player_takes_cards(self, round):
        self.__clear_up()
        self.__show_round(round)
        self.__show_score()
        self.__show_player_first_card(self.dealer)
        self.__show_player_cards(self.you)
        print()
        sleep(1)

    def another_player_takes_cards(self, round):
        self.__clear_up()
        self.__show_round(round)
        self.__show_score()
        self.__show_player_cards(self.dealer)
        self.__show_player_cards(self.you)
        print()
        sleep(1)

    def round_finished(self, round):
        self.__clear_up()
        self.__show_round(round)
        self.__show_score()
        self.__open_player_cards(self.dealer, self.score.is_left_winner)
        self.__open_player_cards(self.you, not self.score.is_left_winner)
        print()
        sleep(1)

    def __show_round(self, round):
        print(f"Round {round} ", end = "")

    def __show_score(self):
        print(f"{self.dealer} vs {self.you} score: {self.score}")
        print()

    def __show_player_cards(self, player : Hand):
        value = player.get_value()
        if value > 0:
            print(f"{player}: ".ljust(15) + f"{player.get_cards()} : {value}")
        else:
            print(f"{player}:")

    def __open_player_cards(self, player : Hand, is_winner : bool):
        outcome = ""
        if self.score.is_draw:
            outcome = "DRAW!"
        elif is_winner:
            outcome = "WINS!"
        elif player.is_bust():
            outcome = "BUSTS!"
        print(f"{player}: ".ljust(15) + f"{player.get_cards()} : {player.get_value()}\t{outcome}")

    def __show_player_first_card(self, player : Hand):
        unknown_card = " "
        if len(player.cards) > 1:
            unknown_card += str(Card(suit = "", rank = 0))
        first_card = player.get_first_card()
        print(f"{player}: ".ljust(15) + f"{first_card}{unknown_card} : {first_card.get_value()}+?")

    def __clear_up(self):
        os.system('cls' if os.name == 'nt' else 'clear')