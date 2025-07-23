import json
import copy  # use it for deepcopy if needed
import math  # for math.inf
import logging

logging.basicConfig(format='%(levelname)s - %(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
                    level=logging.INFO)

# Global variables in which you need to store player strategies (this is data structure that'll be used for evaluation)
# Mapping from histories (str) to probability distribution over actions
strategy_dict_x = {}
strategy_dict_o = {}


class History:
    def __init__(self, history=None):
        """
        # self.history : Eg: [0, 4, 2, 5]
            keeps track of sequence of actions played since the beginning of the game.
            Each action is an integer between 0-8 representing the square in which the move will be played as shown
            below.
              ___ ___ ____
             |_0_|_1_|_2_|
             |_3_|_4_|_5_|
             |_6_|_7_|_8_|

        # self.board
            empty squares are represented using '0' and occupied squares are either 'x' or 'o'.
            Eg: ['x', '0', 'x', '0', 'o', 'o', '0', '0', '0']
            for board
              ___ ___ ____
             |_x_|___|_x_|
             |___|_o_|_o_|
             |___|___|___|

        # self.player: 'x' or 'o'
            Player whose turn it is at the current history/board

        :param history: list keeps track of sequence of actions played since the beginning of the game.
        """
        if history is not None:
            self.history = history
            self.board = self.get_board()
        else:
            self.history = []
            self.board = ['0', '0', '0', '0', '0', '0', '0', '0', '0']
        self.player = self.current_player()

    def current_player(self):
        """ Player function
        Get player whose turn it is at the current history/board
        :return: 'x' or 'o' or None
        """
        total_num_moves = len(self.history)
        if total_num_moves < 9:
            if total_num_moves % 2 == 0:
                return 'x'
            else:
                return 'o'
        else:
            return None

    def get_board(self):
        """ Play out the current self.history and get the board corresponding to the history in self.board.

        :return: list Eg: ['x', '0', 'x', '0', 'o', 'o', '0', '0', '0']
        """
        board = ['0', '0', '0', '0', '0', '0', '0', '0', '0']
        for i in range(len(self.history)):
            if i % 2 == 0:
                board[self.history[i]] = 'x'
            else:
                board[self.history[i]] = 'o'
        return board

    def is_win(self):
        # check if the board position is a win for either players
        # Feel free to implement this in anyway if needed
        n = len(self.history)
        history_of_one = []
        for i in range(n):
            if n % 2 ==0 and i % 2 != 0:
                history_of_one += [self.history[i]]
            elif n % 2 != 0 and i % 2 ==0:
                history_of_one += [self.history[i]]    
        m = len(history_of_one)
        if m > 0:
            last_position = history_of_one[-1]
        for i in range(m-1):
            for j in range(i+1,m-1):
                list1 = [history_of_one[i], history_of_one[j], last_position]
                if self.winning_position(list1):
                    return 1
        return 0   
     
    def winning_position(self,list1):

            #  |_0_|_1_|_2_|
            #  |_3_|_4_|_5_|
            #  |_6_|_7_|_8_|

        list_win = [[0,1,2],[3,4,5],[6,7,8],[0,3,6],[1,4,7],[2,5,8],[0,4,8],[2,4,6]]
        for list_i in list_win:
            if set(list_i) == set(list1):
                return 1
        return 0

    def is_draw(self):
        # check if the board position is a draw
        # Feel free to implement this in anyway if needed
        
        if '0' in self.board:
            return 0
        return not self.is_win()

    def get_valid_actions(self):
        # get the empty squares from the board
        # Feel free to implement this in anyway if needed
        list_1 = []
        j = 0
        for i in self.board:
            if i == '0':
                list_1 += [j]
            j += 1
        return list_1

    def is_terminal_history(self):
        # check if the history is a terminal history
        # Feel free to implement this in anyway if needed
        if self.is_win() or self.is_draw():
            return 1
        return 0

    def get_utility_given_terminal_history(self):
        # Feel free to implement this in anyway if needed
        if self.is_win():
            if len(self.history) % 2 == 0:
                return -1
            else:
                return 1
        return 0

    def update_history(self, action):
        # In case you need to create a deepcopy and update the history obj to get the next history object.
        # Feel free to implement this in anyway if needed
        history = copy.deepcopy(self.history)
        history += [action]
        return history
    
def backward_induction(history_obj):
    """
    :param history_obj: Histroy class object
    :return: best achievable utility (float) for th current history_obj
    """
    global strategy_dict_x, strategy_dict_o
    history_id = ""
    for i in history_obj.history:
        history_id += str(i)
    dict_strat = {"0": 0, "1": 0, "2": 0, "3": 0, "4": 0, "5": 0, "6": 0, "7": 0, "8": 0}
    # if history_obj.is_terminal_history():
    #     if history_obj.is_win():
    #         strategy_dict_x = strategy_dict_x | {history_id : {"0": 0, "1": 0, "2": 0, "3": 0, "4": 0, "5": 0, "6": 0, "7": 0, "8": 0}}
    #     else:
    #         strategy_dict_o = strategy_dict_o | {history_id : {"0": 0, "1": 0, "2": 0, "3": 0, "4": 0, "5": 0, "6": 0, "7": 0, "8": 0}}
    
    # if history_obj.current_player() == 'x':
    #     actions = history_obj.get_valid_actions()

    if history_obj.is_terminal_history():
        return history_obj.get_utility_given_terminal_history(), ""
    
    if history_obj.current_player() == 'x':
        bestUtility = -5
        best_action = ""
        for i in history_obj.get_valid_actions():
            utility, string_vector = backward_induction(History(history_obj.history + [i]))
            if utility > bestUtility:
                bestUtility = utility
                string_vector += str(i)
                best_action = str(i)
        dict_strat[best_action] = 1
        strategy_dict_x = strategy_dict_x | { history_id : dict_strat}
        return bestUtility, string_vector  
    
    if history_obj.current_player() == 'o':
        bestUtility = 5
        best_action = ""
        for i in history_obj.get_valid_actions():
            utility, string_vector = backward_induction(History(history_obj.history + [i]))
            if utility < bestUtility:
                bestUtility = utility
                string_vector += str(i)
                best_action = str(i)
        dict_strat[best_action] = 1
        strategy_dict_o = strategy_dict_o | { history_id : dict_strat}
        return bestUtility, string_vector  
    
    # TODO implement
    # (1) Implement backward induction for tictactoe
    # (2) Update the global variables strategy_dict_x or strategy_dict_o which are a mapping from histories to
    # probability distribution over actions.
    # (2a)These are dictionary with keys as string representation of the history list e.g. if the history list of the
    # history_obj is [0, 4, 2, 5], then the key is "0425". Each value is in turn a dictionary with keys as actions 0-8
    # (str "0", "1", ..., "8") and each value of this dictionary is a float (representing the probability of
    # choosing that action). Example: {”0452”: {”0”: 0, ”1”: 0, ”2”: 0, ”3”: 0, ”4”: 0, ”5”: 0, ”6”: 1, ”7”: 0, ”8”:
    # 0}}
    # (2b) Note, the strategy for each history in strategy_dict_x and strategy_dict_o is probability distribution over
    # actions. But since tictactoe is a PIEFG, there always exists an optimal deterministic strategy (SPNE). So your
    # policy will be something like this {"0": 1, "1": 0, "2": 0, "3": 0, "4": 0, "5": 0, "6": 0, "7": 0, "8": 0} where
    # "0" was the one of the best actions for the current player/history.
    # TODO implement


def solve_tictactoe():
    backward_induction(History())
    with open('./policy_x.json', 'w') as f:
        json.dump(strategy_dict_x, f)
    with open('./policy_o.json', 'w') as f:
        json.dump(strategy_dict_o, f)
    return strategy_dict_x, strategy_dict_o


if __name__ == "__main__":
    logging.info("Start")
    solve_tictactoe()
    logging.info("End")
