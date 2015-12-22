from Tkinter import *
import random
import string
global filename, target, wins, losses, frame
filename = "C:\\Python27\\My Own Creations\\MinesweeperStatistics.txt"

#Initialize Tkinter frame for board
root = Tk()
root.title("Minesweeper")
frame = Frame(root)
frame.grid(row = 0, column = 0)

#Functions needed for menubar
def new_game():
    global frame
    frame.grid_forget()
    frame = Frame(root)
    frame.grid(row = 0, column = 0)
    g = Grid(frame, 15, 15)

def stat_center(win_bool = None):
    def clear_stats():
        wins = 0
        losses = 0
        percentage = 'N/A'
        target = open(filename, 'w+')
        target.write("Wins: {} \n".format(wins))
        target.write("Losses: {} \n".format(losses))
        target.write("Percentage: {}".format(str(percentage) + "%" if percentage != 'N/A' else percentage))
        target.close()
        stat_display.destroy()
        stat_center()
    global filename
    target = open(filename, 'r')
    line_list = target.readlines()
    wins = int(line_list[0].split()[1])
    losses = int(line_list[1].split()[1])
    percentage = line_list[2].split()[1]
    if win_bool != None:
        target.close()
        target = open(filename, 'w+')
    stat_display = Tk()
    stat_display.title("Statistics")
    if win_bool == None:
        w = Label(stat_display, height = 10, width = 30, text = "Wins: {}\nLosses: {}\nPercentage: {}".format(wins, losses, percentage))
    else:
        if win_bool == True:
            wins += 1
            percentage = round((float(wins) / (wins + losses)), 2) * 100
            w = Label(stat_display, height = 10, width = 30, text = "You Win!\nWins: {}\nLosses: {}\nPercentage: {}%".format(wins, losses, percentage))
        elif win_bool == False:
            losses += 1
            percentage = round((float(wins) / (wins + losses)), 2) * 100
            w = Label(stat_display, height = 10, width = 30, text = "You Lose!\nWins: {}\nLosses: {}\nPercentage: {}%".format(wins, losses, percentage))
        target.write("Wins: {} \n".format(wins))
        target.write("Losses: {} \n".format(losses))
        target.write("Percentage: {}".format(str(percentage) + "%" if percentage != 'N/A' else percentage))
    target.close()
    clear_button = Button(stat_display, text = "Clear Statistics", command = clear_stats)
    clear_button.pack()
    new_game_button = Button(stat_display, text = "New Game", command = new_game)
    new_game_button.pack()
    w.pack()
    stat_display.mainloop()

def about():
    about_display = Tk()
    about_display.title("What is Minesweeper?")
    w = Label(about_display, height = 10, width = 50, justify = LEFT, text = "Minesweeper is a game where you try to find the bombs\nlocated on a square grid.  You do so by locating coordinates\nthat you believe are safe. Once you locate all the safe squares,\nyou win.But be careful!  One wrong square, and it's game over!")
    w.pack()
    about_display.mainloop()

def how_to_play():
    how_to_display = Tk()
    how_to_display.title("How to Play Minesweeper")
    w = Label(how_to_display, height = 10, width = 50, justify = LEFT, text = "The goal of Minesweeper is to locate all the safe squares.\nIf you think a square is safe, click on it. If the square is safe,\nit will display a number telling you how many bombs are\nadjacent to that square. If you believe a square has a bomb\non it, right-click the square to flag it. If you change your mind\nabout that flagged square, right-click again to unflag it.")
    w.pack()
    how_to_display.mainloop()

#Create menu bar
menubar = Menu(root)
root.config(menu = menubar)

filemenu = Menu(menubar, tearoff = 0)
filemenu.add_command(label = "New Game", command = new_game)
filemenu.add_command(label = "Statistics", command = stat_center)
filemenu.add_command(label = "Exit", command = root.destroy)
menubar.add_cascade(label = "File", menu = filemenu)


helpmenu = Menu(menubar, tearoff = 0)
helpmenu.add_command(label = "What is Minesweeper?", command = about)
helpmenu.add_command(label = "How to Play Minesweeper", command = how_to_play)
menubar.add_cascade(label = "Help", menu = helpmenu)

#Grid and Grid_Button Classes
class Grid:
    def __init__(self, frame_var, length, width):
        self.length = length
        self.width = width
        self.bomb_count = int(length * width / 8)
        self.bomb_list = []
        self.safe_squares = {}
        self.flagged_squares = []
        self.grid = []
        self.turn_count = 0
        self.max = (self.length * self.width) - self.bomb_count
        for x in xrange(self.width):
            self.grid.append([])
            for y in xrange(self.length):
                b = Grid_Button(frame_var, self, x, y)
                self.grid[x].append(b)
                self.grid[x][y].Button.grid(column = x, row = y)
        self.create_bombs()
        self.create_safe_squares()
    
    def create_bombs(self):
        for count in range(self.bomb_count):
            location = random.choice(range(1,self.length*self.width + 1))
            while self.bomb_list.count(location) == 1:
                location = random.choice(range(1,self.length*self.width + 1))
            self.bomb_list.append(location)
        return self.bomb_list
        
    def create_safe_squares(self):
        for t in range(1,(self.length*self.width)+1):
            if t not in self.bomb_list:
                bomb_num = 0
                for bomb in self.bomb_list:
                    if bomb%self.width == 1:
                        if bomb == 1:
                            if t == bomb + 1 or t == bomb + self.width or t == bomb + self.width + 1:
                                bomb_num += 1
                        elif bomb == 1 + (self.length - 1)*self.width:
                            if t == bomb + 1 or t == bomb - self.width or t == bomb - self.width + 1:
                                bomb_num += 1
                        else:
                            if t == bomb + 1 or t == bomb + self.width or t == bomb + self.width + 1 or t == bomb - self.width or t == bomb - self.width + 1:
                                bomb_num += 1
                    if bomb%self.width in range(2,self.width):
                        if bomb in range(2,self.width):
                            if t == bomb + 1 or t == bomb - 1 or t == bomb + self.width - 1 or t == bomb + self.width or t == bomb + self.width + 1:
                                bomb_num += 1
                        elif bomb in range(2+(self.length-1)*self.width,self.length*self.width):
                            if t == bomb + 1 or t == bomb - 1 or t == bomb - self.width + 1 or t == bomb - self.width or t == bomb - self.width - 1:
                                bomb_num += 1
                        else:
                            if t == bomb + 1 or t == bomb - 1 or t == bomb + self.width - 1 or t == bomb + self.width or t == bomb + self.width + 1 or t == bomb - self.width + 1 or t == bomb - self.width or t == bomb - self.width - 1:
                                bomb_num += 1
                    if bomb%self.width == 0:
                        if bomb == self.width:
                            if t == bomb - 1 or t == bomb + self.width or t == bomb + self.width - 1:
                                bomb_num += 1
                        elif bomb == self.width * self.length:
                            if t == bomb - 1 or t == bomb - self.width - 1 or t == bomb - self.width:
                                bomb_num += 1
                        else:
                            if t == bomb - 1 or t == bomb + self.width or t == bomb + self.width - 1 or t == bomb - self.width - 1 or t == bomb - self.width:
                                bomb_num += 1
                self.safe_squares[t] = bomb_num
        return self.safe_squares
    
    def bomb_display(self):
        for coordinate in self.bomb_list:
            column = coordinate % self.width
            if column == 0:
                column += self.width
            column -= 1
            row = int((coordinate - column) / self.width)
            self.grid[column][row].Button.config(fg = "black", text = 'B')
        for flag_coordinate in self.flagged_squares:
            if flag_coordinate not in self.bomb_list:
                column = flag_coordinate % self.width
                if column == 0:
                    column += self.width
                column -= 1
                row = int((flag_coordinate - column) / self.width)
                self.grid[column][row].Button.config(fg = "black", text = 'X')
        self.disable_buttons()

    def disable_buttons(self):
        for x in xrange(len(self.grid)):
            for y in xrange(len(self.grid[x])):
                self.grid[x][y].Button.unbind("<Button-1>")
                self.grid[x][y].Button.unbind("<Button-3>")
                
class Grid_Button:
    def __init__(self, frame_var, grid_inst, x, y):
        self.grid_inst = grid_inst
        self.coordinate = (y * self.grid_inst.width) + x + 1
        self.Button = Button(frame_var, width = 2, height = 1)
        self.Button.bind("<Button-1>", self.square_check)
        self.Button.bind("<Button-3>", self.square_flagging)
        self.state = "C"
        
    def square_check(self, event):
        if self.coordinate in self.grid_inst.bomb_list:
            self.grid_inst.bomb_display()
            stat_center(False)
        else:
            self.Button.config(fg = "red", text = str(self.grid_inst.safe_squares[self.coordinate]))
            self.grid_inst.turn_count += 1
            if self.grid_inst.turn_count == self.grid_inst.max:
                self.grid_inst.bomb_display()
                stat_center(True)
            else:
                self.Button.unbind("<Button-1>")
                self.Button.unbind("<Button-3>")

    def square_flagging(self, event):
        if self.state == "F":
            self.state = "C"
            self.Button.config(text = "")
            self.Button.bind("<Button-1>", self.square_check)
            del self.grid_inst.flagged_squares[self.grid_inst.flagged_squares.index(self.coordinate)]
        else:
            self.state = "F"
            self.Button.config(fg = "green", text = "F")
            self.Button.unbind("<Button-1>")
            self.grid_inst.flagged_squares.append(self.coordinate)

#Creation of board upon starting program
g = Grid(frame, 15, 15)
root.mainloop()
