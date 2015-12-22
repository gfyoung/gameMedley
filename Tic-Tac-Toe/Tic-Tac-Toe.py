from Tkinter import *

class Grid:
    def __init__(self, frame_var, dimension):
        self.turn_count = 1
        self.max = dimension**2 + 1
        self.grid = []
        for x in xrange(dimension):
            self.grid.append([])
            for y in xrange(dimension):
                b = Grid_Button(frame_var, self, x, y)
                self.grid[x].append(b)
                self.grid[x][y].Button.grid(column = x, row = y)

    def win_check(self, x, y):
        if(y == x) and (self.grid[0][0].get_color() != None):
            if self.grid[0][0].get_color() == self.grid[1][1].get_color() == self.grid[2][2].get_color():
                return True
        elif(y == 2 - x) and (self.grid[0][2].get_color() != None):
            if self.grid[0][2].get_color() == self.grid[1][1].get_color() == self.grid[2][0].get_color():
                return True
        elif(self.grid[x][0].get_color() != None):
            if self.grid[x][0].get_color() == self.grid[x][1].get_color() == self.grid[x][2].get_color():
                return True
        elif(self.grid[0][y].get_color() != None):
            if self.grid[0][y].get_color() == self.grid[1][y].get_color() == self.grid[2][y].get_color():
                return True
        else:
            return False

    def disable_buttons(self):
        for x in xrange(len(self.grid)):
            for y in xrange(len(self.grid[x])):
                self.grid[x][y].Button.config(state = DISABLED)
                
class Grid_Button:
    def __init__(self, frame_var, grid_inst, x, y):
        self.row = y
        self.column = x
        self.Button = Button(frame_var, width = 12, height = 6, command = self.color_change)
        self.grid_inst = grid_inst
        self.color = None
        
    def color_change(self):
        if self.grid_inst.turn_count%2 == 0:
            self.color = "Black"
            self.Button.config(bg = "Black", state = DISABLED)
            self.grid_inst.turn_count += 1
        else:
            self.color = "Red"
            self.Button.config(bg = "Red", state = DISABLED)
            self.grid_inst.turn_count += 1
        if Grid.win_check(self.grid_inst, self.column, self.row) == True:
            Grid.disable_buttons(self.grid_inst)
            result = Tk()
            w = Label(result, text = "{} is the winner!".format(self.color))
            w.pack()
            result.mainloop()
        else:
            if self.grid_inst.turn_count == self.grid_inst.max:
                Grid.disable_buttons(self.grid_inst)
                result = Tk()
                w = Label(result, text = "It's a tie!")
                w.pack()
                result.mainloop()

    def get_color(self):
        return self.color
            
root = Tk()
frame = Frame(root)
frame.grid(row = 0, column = 0)

g = Grid(frame, 3)

root.mainloop()
