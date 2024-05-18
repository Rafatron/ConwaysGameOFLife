import pygame
import sys
import tkinter as tk
import threading

# Initialize Pygame
pygame.init()

# Constants
SCREEN_WIDTH = 1250 
SCREEN_HEIGHT = 500  
GRID_SIZE = 10
GRID_ROWS = SCREEN_HEIGHT // GRID_SIZE
GRID_COLS = SCREEN_WIDTH // GRID_SIZE
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

# Function to convert mouse coordinates to grid cell coordinates
def get_grid_pos(pos):
    x, y = pos
    grid_x = x // GRID_SIZE
    grid_y = y // GRID_SIZE
    return grid_x, grid_y

# Function to draw the grid
def draw_grid(screen):
    for x in range(0, SCREEN_WIDTH, GRID_SIZE):
        pygame.draw.line(screen, WHITE, (x, 0), (x, SCREEN_HEIGHT))
    for y in range(0, SCREEN_HEIGHT, GRID_SIZE):
        pygame.draw.line(screen, WHITE, (0, y), (SCREEN_WIDTH, y))

# Function to save grid cords to file
def saveListToFile(list, filename):
    file = open(f"./Programmes/{filename}.gol", "w", encoding="ascii")
    for i in list:
        file.write(f"{i[0]},{i[1]}\n")

def main():
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Grid Drawing")

    grid = [[BLACK for i in range(GRID_COLS)] for i in range(GRID_ROWS)] #for identification purposes
    clicked_points = []

    #the tk window
    def managemenet():
        wn = tk.Tk()
        wn.geometry("270x25")
        wn.title("Control Console")

        entry = tk.Entry()

        button = tk.Button(text="save file as .gol", width=50, command= lambda : saveListToFile(clicked_points, entry.get()))

        entry.pack(side="left")
        button.pack(side="right")
        wn.mainloop()

    running = True
    threading.Thread(target=managemenet).start() #start the tk window
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:  # Left mouse button
                    grid_x, grid_y = get_grid_pos(event.pos)
                    if grid[grid_y][grid_x] == BLACK:
                        grid[grid_y][grid_x] = WHITE
                        clicked_points.append([grid_x, grid_y])
                    else:
                        grid[grid_y][grid_x] = BLACK
                        clicked_points.remove([grid_x, grid_y])
        


        screen.fill(BLACK) #this makes all the pixels from white to bacl
        draw_grid(screen)

        # mkae all clicked cells white
        for point in clicked_points:
            x, y = point
            pygame.draw.rect(screen, WHITE, (x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE))  

        pygame.display.flip()

    pygame.quit()
    sys.exit()

if __name__ == "__main__":
    main()