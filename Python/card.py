from colorama import Fore, Back
from enum import Enum

RANK_ACE = 1
RANK_JACK = 11
RANK_QUEEN = 12
RANK_KING = 13

SUIT_CLUBS = "\x03"
SUIT_DIAMONDS = "\x04"
SUIT_HEARTS = "\x05"
SUIT_SPADES = "\x06"

class Card:
    def __init__(self, suit, rank):
        self.suit = suit
        self.rank = rank

    def __str__(self):
        rank = self.get_rank_as_string()
        suit = self.get_suit_as_string()
        return f"{rank}{suit}"
    
    def get_rank_as_string(self):
        rank = self.rank
        if rank == 0:
            rank = "?"
        if rank == RANK_JACK:
            rank = "J"
        elif rank == RANK_QUEEN:
            rank = "Q"
        elif rank == RANK_KING:
            rank = "K"
        elif rank == RANK_ACE:
            rank = "A"
        return f"{Back.WHITE}{Fore.BLACK}{rank}{Fore.RESET}{Back.RESET}"

    def get_suit_as_string(self):
        suit = self.suit
        if self.suit == SUIT_CLUBS:
            suit = f"{Fore.BLACK}♣"
        elif self.suit == SUIT_DIAMONDS:
            suit = f"{Fore.RED}♦"
        elif self.suit == SUIT_HEARTS:
            suit = f"{Fore.RED}♥"
        elif self.suit == SUIT_SPADES:
            suit = f"{Fore.BLACK}♠"
        return f"{Back.WHITE}{suit}{Fore.RESET}{Back.RESET}"

    def get_value(self):
        if self.rank == RANK_ACE:
            return 11
        elif self.rank >= 10:
            return 10
        else:
            return self.rank

    def get_value_when_over21(self):
        if self.rank == RANK_ACE:
            return 1
        elif self.rank >= 10:
            return 10
        else:
            return self.rank