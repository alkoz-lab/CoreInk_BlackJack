from colorama import Fore

class Score:
    def __init__(self, max_score):
        self.max_score = max_score
        self.left_score = 0
        self.right_score = 0
        self.is_left_winner = False
        self.is_draw = False

    def __str__(self):
        if self.is_draw or self.left_score == 0 and self.right_score == 0:
            return f"{self.left_score}:{self.right_score}"
        if self.is_left_winner:
            return f"+{self.left_score}:{self.right_score}"
        else:
            return f"{self.left_score}:{self.right_score}+"
    
    def game_over(self):
        return self.left_score >= self.max_score or self.right_score >= self.max_score
    
    def left_wins(self):
        self.left_score += 1
        self.is_left_winner = True
        self.is_draw = False

    def right_wins(self):
        self.right_score += 1
        self.is_left_winner = False
        self.is_draw = False

    def draw(self):
        self.is_draw = True