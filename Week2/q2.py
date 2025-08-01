import copy  # use it for deepcopy if needed
import math
import logging

logging.basicConfig(format='%(levelname)s - %(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
                    level=logging.INFO)

# Global variable to keep track of visited board positions. This is a dictionary with keys as self.boards as str and
# value represents the maxmin value. Use the get_boards_str function in History class to get the key corresponding to
# self.boards.
board_positions_val_dict = {}
# Global variable to store the visited histories in the process of alpha beta pruning.
visited_histories_list = []
v=2
lol=0

class History:
    def __init__(self, num_boards=2, history=None):
        """
        # self.history : Eg: [0, 4, 2, 5]
            keeps track of sequence of actions played since the beginning of the game.
            Each action is an integer between 0-(9n-1) representing the square in which the move will be played as shown
            below (n=2 is the number of boards).

             Board 1
              ___ ___ ____
             |_0_|_1_|_2_|
             |_3_|_4_|_5_|
             |_6_|_7_|_8_|

             Board 2
              ____ ____ ____
             |_9_|_10_|_11_|
             |_12_|_13_|_14_|
             |_15_|_16_|_17_|

        # self.boards
            empty squares are represented using '0' and occupied squares are 'x'.
            Eg: [['x', '0', 'x', '0', 'x', 'x', '0', '0', '0'], ['0', 0', '0', 0', '0', 0', '0', 0', '0']]
            for two board game

            Board 1
              ___ ___ ____
             |_x_|___|_x_|
             |___|_x_|_x_|
             |___|___|___|

            Board 2
              ___ ___ ____
             |___|___|___|
             |___|___|___|
             |___|___|___|

        # self.player: 1 or 2
            Player whose turn it is at the current history/board

        :param num_boards: Number of boards in the game of Notakto.
        :param history: list keeps track of sequence of actions played since the beginning of the game.
        """
        self.num_boards = num_boards
        if history is not None:
            self.history = history
            self.boards = self.get_boards()
        else:
            self.history = []
            self.boards = []
            for i in range(self.num_boards):
                # empty boards
                self.boards.append(['0', '0', '0', '0', '0', '0', '0', '0', '0'])
        # Maintain a list to keep track of active boards
        self.active_board_stats = self.check_active_boards()
        self.current_player = self.get_current_player()

    def get_boards(self):
        """ Play out the current self.history and get the boards corresponding to the history.

        :return: list of lists
                Eg: [['x', '0', 'x', '0', 'x', 'x', '0', '0', '0'], ['0', 0', '0', 0', '0', 0', '0', 0', '0']]
                for two board game

                Board 1
                  ___ ___ ____
                 |_x_|___|_x_|
                 |___|_x_|_x_|
                 |___|___|___|

                Board 2
                  ___ ___ ____
                 |___|___|___|
                 |___|___|___|
                 |___|___|___|
        """
        boards = []
        for i in range(self.num_boards):
            boards.append(['0', '0', '0', '0', '0', '0', '0', '0', '0'])
        for i in range(len(self.history)):
            board_num = math.floor(self.history[i] / 9)
            play_position = self.history[i] % 9
            boards[board_num][play_position] = 'x'
        return boards

    def check_active_boards(self):
        """ Return a list to keep track of active boards

        :return: list of int (zeros and ones)
                Eg: [0, 1]
                for two board game

                Board 1
                  ___ ___ ____
                 |_x_|_x_|_x_|
                 |___|_x_|_x_|
                 |___|___|___|

                Board 2
                  ___ ___ ____
                 |___|___|___|
                 |___|___|___|
                 |___|___|___|
        """
        active_board_stat = []
        for i in range(self.num_boards):
            if not self.is_board_win(self.boards[i]):
                active_board_stat.append(i)
        return active_board_stat

    @staticmethod
    def is_board_win(board):
        for i in range(3):
            if board[3 * i] == board[3 * i + 1] == board[3 * i + 2] != '0':
                return True

            if board[i] == board[i + 3] == board[i + 6] != '0':
                return True

        if board[0] == board[4] == board[8] != '0':
            return True

        if board[2] == board[4] == board[6] != '0':
            return True
        return False

    def get_current_player(self):
        """
        Get player whose turn it is at the current history/board
        :return: 1 or 2
        """
        total_num_moves = len(self.history)
        if total_num_moves % 2 == 0:
            return 1
        else:
            return 2

    def get_boards_str(self):
        boards_str = ""
        for i in range(self.num_boards):
            boards_str = boards_str + ''.join([str(j) for j in self.boards[i]])
        return boards_str

    def is_win(self):
        # Feel free to implement this in anyway if needed
        if len(self.check_active_boards()) != 0:
            return 0
        return 1

    def get_valid_actions(self):
        # Feel free to implement this in anyway if needed
        actions = []
        for board in self.check_active_boards():
            for i in range(9):
                if self.get_boards_str()[board*9 + i] == '0':
                    actions += [board*9 + i]
        return actions
    

    def is_terminal_history(self):
        # Feel free to implement this in anyway if needed
        if len(self.check_active_boards()):
            return 0
        return 1

    def get_value_given_terminal_history(self):
        # Feel free to implement this in anyway if needed
        pass


def alpha_beta_pruning(history_obj: History, alpha, beta, max_player_flag):
    
    global v,lol
    lol += 1
    """
        Calculate the maxmin value given a History object using alpha beta pruning. Use the specific move order to
        speedup (more pruning, less memory).

    :param history_obj: History class object
    :param alpha: -math.inf
    :param beta: math.inf
    :param max_player_flag: Bool (True if maximizing player plays)
    :return: float
    """
    # These two already given lines track the visited histories.
    global visited_histories_list, board_positions_val_dict

    if any(sorted(history_obj.history) == sorted(hist) for hist in visited_histories_list):
        if max_player_flag:
            return alpha, ""
        return beta, ""

    if not len(history_obj.check_active_boards()):
        visited_histories_list  += [history_obj.history]
        if max_player_flag:
            return 1, ""
        return -1, ""

    if max_player_flag:
        visited_histories_list += [history_obj.history]
        best_string_vec = ""
        for action in history_obj.get_valid_actions():
            if alpha >= beta:
                break
            val, string_vec = alpha_beta_pruning(History(num_boards=v, history=history_obj.history + [action]), alpha, beta, 0)
            if val > alpha:
                alpha = val
                best_string_vec = string_vec + str(action)   
        return alpha, best_string_vec


    if not max_player_flag:
        visited_histories_list += [history_obj.history]
        best_string_vec = ""
        for action in history_obj.get_valid_actions():
            if alpha >= beta:
                break
            value, string_vec = alpha_beta_pruning(History(num_boards=v, history=history_obj.history + [action]),alpha,beta,1)
            if value < beta:
                beta = value
                best_string_vec = string_vec + str(action)
        return beta, best_string_vec
            
    # TODO implement
    # TODO implement


def solve_alpha_beta_pruning(history_obj, alpha, beta, max_player_flag):
    # global visited_histories_list
    val, vec = alpha_beta_pruning(history_obj, alpha, beta, max_player_flag)
    return val, vec[-1]


if __name__ == "__main__":
    logging.info("start")
    logging.info("alpha beta pruning")
    histry = input("Enter history, as a single number:")
    if histry == "u":
        histry_list = []
    else:
        histry_list = histry.split(',')
        histry_list = [int(i) for i in histry_list]
    print(histry_list)
    val, vec = solve_alpha_beta_pruning(History(history=histry_list, num_boards=v), -math.inf, math.inf, True)
    # print(board_positions_val_dict)
    print(vec)
    print(lol)
    # logging.info("maxmin value {}".format(value))
    # logging.info("Number of histories visited {}".format(visited_histories))
    logging.info("end")
