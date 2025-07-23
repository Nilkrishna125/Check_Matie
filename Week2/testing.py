# list1 = [1,2,3]
# list2 = [3,1,2]
# list3 = list(set(list1) - set(list2))
# print(list3==[])
import copy
list1 = [[1,2,3], [3,5,6]]
list2 = [3,1,2]
for list_i in list1:
    if set(list_i) == set(list2):
        print("hello")
def hello():
    return not 1
print(hello())

list_1 = [1,3,4]
list_2 = [4,6]
print(list_1+list_2)
print(list_1+[3])
list_3 = list_1
list_4 = copy.deepcopy(list_1)
# list_3 += [5,6]
list_4 += [5,6]
print(list_3, list_1)
print(list_4, list_1)

list_5 = [5,6,3,1]
list_5 += [7,8]
print(list_5)

hist = [1,2,3,4]
j = ""
for i in hist:
    j += str(i)
print(j)


list_1 = {'a':1,'b':3,'c':4}
list_2 = {'d':4,'e':6}
list_3 = list_1 | list_2
print(list_3)

history = [1,0,8,3,4,2,6,5,7]
def is_win():
        # check if the board position is a win for either players
        # Feel free to implement this in anyway if needed
        n = len(history)
        history_of_one = []
        for i in range(n):
            if n % 2 == 0 and i % 2 != 0:
                history_of_one += [history[i]]
            elif n % 2 != 0 and i % 2 ==0:
                history_of_one += [history[i]] 
        m = len(history_of_one)
        if m > 0:
            last_position = history_of_one[-1]
        for i in range(m-1):
            for j in range(i+1,m-1):
                list1 = [history_of_one[i], history_of_one[j], last_position]
                if winning_position(list1):
                     return 1
        return 0   
     
def winning_position(list1):

            #  |_0_|_1_|_2_|
            #  |_3_|_4_|_5_|
            #  |_6_|_7_|_8_|

        list_win = [[0,1,2],[3,4,5],[6,7,8],[0,3,6],[1,4,7],[2,5,8],[0,4,8],[2,4,6]]
        for list_i in list_win:
            if set(list_i) == set(list1):
                return 1
        return 0
print(is_win())


print("___________________________________________")
lis = [1,2,3,4,5]
for i in lis:
     print(i)
     print([i])
     print(str(i))
     print(str([i]))

print("___________________________________________")
print("Q22222222222222222222222222222222222222222Q")


lis = [1,4,2,3,3]
lis1 = [0]
print(sorted(lis))
print(sorted(lis1))

history11 = [0]
visited_histories_list = [[]]
print(visited_histories_list)
print(history11)
print(any(sorted(prev_hist) == sorted(history11) for prev_hist in visited_histories_list))

print([[1,2]]+[[3,4]])