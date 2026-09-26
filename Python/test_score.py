from score import Score
import unittest


class ScoreTests(unittest.TestCase):

    def test_when_game_is_over_it_returns_true(self):
        score = Score(1)
        score.left_wins()
        self.assertTrue(score.game_over())

    def test_when_game_is_not_over_it_returns_false(self):
        score = Score(1)
        score.draw()
        self.assertFalse(score.game_over())

    def test_when_left_wins_it_returns_true(self):
        score = Score(1)
        score.left_wins()
        self.assertTrue(score.is_left_winner)

    def test_when_right_wins_it_returns_false(self):
        score = Score(1)
        score.right_wins()
        self.assertFalse(score.is_left_winner)

    def test_when_draw_it_returns_true(self):
        score = Score(1)
        score.draw()
        self.assertTrue(score.is_draw)

    def test_when_left_wins_draw_is_false(self):
        score = Score(1)
        score.draw()
        score.left_wins()
        self.assertTrue(score.is_left_winner)
        self.assertFalse(score.is_draw)

    def test_when_left_wins_it_returns_correct_score(self):
        score = Score(1)
        score.left_wins()
        self.assertEqual(str(score), "+1:0")

    def test_when_right_wins_it_returns_correct_score(self):
        score = Score(1)
        score.right_wins()
        self.assertEqual(str(score), "0:1+")

    def test_when_draw_it_returns_correct_score(self):
        score = Score(1)
        score.draw()
        self.assertEqual(str(score), "0:0")


if __name__ == '__main__':
    unittest.main()
