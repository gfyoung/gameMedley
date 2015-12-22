# Largely adapted from "Making Games with Pythong & Pygame" by Al Sweigart

import random, pygame, sys
from pygame.locals import *

FPS = 15
WINDOWWIDTH = 640
WINDOWHEIGHT = 480
CELLSIZE = 20
assert WINDOWWIDTH % CELLSIZE == 0, "Window width must be a multiple of cell size."
assert WINDOWHEIGHT % CELLSIZE == 0, "Window height must be a multiple of cell size."
CELLWIDTH = int(WINDOWWIDTH / CELLSIZE)
CELLHEIGHT = int(WINDOWHEIGHT / CELLSIZE)

WHITE = (255, 255, 255)
GRAY = (185, 185, 185)
BRIGHTYELLOW = (255, 255, 0)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
DARKGREEN = (0, 155, 0)
DARKGRAY = (40, 40, 40)
BGCOLOR = BLACK

UP = 'up'
DOWN = 'down'
LEFT = 'left'
RIGHT = 'right'

HEAD = 0 #syntactic sugar: index of the worm's head

POISONED_APPLE_COUNT = 20

def main():
    global FPSCLOCK, DISPLAYSURF, BASICFONT, BIGFONT

    pygame.init()
    FPSCLOCK = pygame.time.Clock()
    DISPLAYSURF = pygame.display.set_mode((WINDOWWIDTH, WINDOWHEIGHT))
    BASICFONT = pygame.font.Font('freesansbold.ttf', 18)
    BIGFONT = pygame.font.Font('freesansbold.ttf', 100)
    pygame.display.set_caption('Wormy')

    showStartScreen()
    while True:
        runGame()
        showGameOverScreen()

def runGame():
    #Set a random start point
    startx = random.randint(5, CELLWIDTH - 6)
    starty = random.randint(5, CELLHEIGHT - 6)
    wormCoords = [{'x': startx, 'y': starty}, {'x': startx - 1, 'y': starty}, \
                  {'x': startx - 2, 'y': starty}]
    direction = RIGHT

    #Start the apple and poisoned_apple in a random place
    apple, poisoned_apples = place_fruit(wormCoords)
    
    while True: #main game loop
        for event in pygame.event.get():
            if event.type == QUIT:
                terminate()
                
            elif event.type == KEYDOWN:
                if(event.key == K_LEFT or event.key == K_a) and direction != RIGHT:
                    direction = LEFT
                elif(event.key == K_RIGHT or event.key == K_d) and direction != LEFT:
                    direction = RIGHT
                elif(event.key == K_UP or event.key == K_w) and direction != DOWN:
                    direction = UP
                elif(event.key == K_DOWN or event.key == K_s) and direction != UP:
                    direction = DOWN
                elif event.key == K_ESCAPE:
                    terminate()
                    
            elif event.type == KEYUP and event.key == K_p:
                DISPLAYSURF.fill(BGCOLOR)
                showPauseScreen()
                
        #check if the worm has hit itself or the edge
        if wormCoords[HEAD]['x'] == -1 or wormCoords[HEAD]['x'] == CELLWIDTH or \
           wormCoords[HEAD]['y'] == -1 or wormCoords[HEAD]['y'] == CELLHEIGHT:
            print "Wall Collision"
            return # game over
        
        for wormBody in wormCoords[1:]:
            if wormBody['x'] == wormCoords[HEAD]['x'] and wormBody['y'] == wormCoords[HEAD]['y']:
                print "Body Collision"
                return #game over

        #check if worm has eaten an apple
        if wormCoords[HEAD]['x'] == apple['x'] and wormCoords[HEAD]['y'] == apple['y']:
            #don't remove worm's tail segment
            print "Ate an Apple!"
            apple, poisoned_apples = place_fruit(wormCoords) #place fruit in new positions

        #check if worm has eaten a poisoned apple
        else:
            chopped = False
            for poisoned_apple in poisoned_apples:
                if wormCoords[HEAD]['x'] == poisoned_apple['x'] and wormCoords[HEAD]['y'] == poisoned_apple['y']:
                    if len(wormCoords) <= 3: #worm cannot be smaller than its initial length
                        print "Too Many Poisoned Apples"
                        return #game over
            
                    else: #halve the length of the worm
                        chopped = True
                        WORM_LENGTH = len(wormCoords)

                        if WORM_LENGTH%2 == 0:
                            WORM_LENGTH_HALVED = WORM_LENGTH / 2
                        else:
                            WORM_LENGTH_HALVED = (WORM_LENGTH + 1) / 2

                        print "Halved"
                        wormCoords = wormCoords[:WORM_LENGTH_HALVED]
                        apple, poisoned_apples = place_fruit(wormCoords) #place fruit in new positions

                    break #there can only be one poisoned apple at that location

            if not chopped:
                del wormCoords[-1] #remove worm's tail segment
        
        #move the worm by adding a segment in the direction it is moving
        if direction == UP:
            newHead = {'x': wormCoords[HEAD]['x'], 'y': wormCoords[HEAD]['y'] - 1}
        elif direction == DOWN:
            newHead = {'x': wormCoords[HEAD]['x'], 'y': wormCoords[HEAD]['y'] + 1}
        elif direction == LEFT:
            newHead = {'x': wormCoords[HEAD]['x'] - 1, 'y': wormCoords[HEAD]['y']}
        elif direction == RIGHT:
            newHead = {'x': wormCoords[HEAD]['x'] + 1, 'y': wormCoords[HEAD]['y']}
        wormCoords.insert(0, newHead)
        DISPLAYSURF.fill(BGCOLOR)
        drawGrid()
        drawWorm(wormCoords)
        drawApple(apple)
        drawPoisonedApples(poisoned_apples)
        drawScore(len(wormCoords) - 3)
        pygame.display.update()
        FPSCLOCK.tick(FPS)

def drawPressKeyMsg():
    pressKeySurf = BASICFONT.render('Press a key to play.', True, DARKGRAY)
    pressKeyRect = pressKeySurf.get_rect()
    pressKeyRect.topleft = (WINDOWWIDTH - 200, WINDOWHEIGHT - 30)
    DISPLAYSURF.blit(pressKeySurf, pressKeyRect)

def showPauseScreen():
    #displays large text in the center of the screen until a key is pressed
    #draw the text drop shadow (in GRAY)
    titleSurf, titleRect = makeTextObjs('PAUSED', BIGFONT, GRAY)
    titleRect.center = (int(WINDOWWIDTH / 2), int(WINDOWHEIGHT / 2))
    DISPLAYSURF.blit(titleSurf, titleRect)

    #draw the text (in WHITE)
    #offset of 3 pixels to the left and up creates "drop shadow" effect
    #to make the text look a bit prettier
    titleSurf, titleRect = makeTextObjs('PAUSED', BIGFONT, WHITE)
    titleRect.center = (int(WINDOWWIDTH / 2) - 3, int(WINDOWHEIGHT / 2) - 3)
    DISPLAYSURF.blit(titleSurf, titleRect)

    #draw the additional "Press a key to play" text
    pressKeySurf, pressKeyRect = makeTextObjs('Press a key to play', BASICFONT, WHITE)
    pressKeyRect.center = (int(WINDOWWIDTH / 2), int(WINDOWHEIGHT / 2) + 100)
    DISPLAYSURF.blit(pressKeySurf, pressKeyRect)

    while checkForPauseBreak() == None:
        pygame.display.update()
        FPSCLOCK.tick()

def makeTextObjs(text, font, color):
    surf = font.render(text, True, color)
    return surf, surf.get_rect()

def checkForKeyPress():
    if len(pygame.event.get(QUIT)) > 0:
        terminate()

    keyUpEvents = pygame.event.get(KEYUP)
    if len(keyUpEvents) == 0:
        return None
    if keyUpEvents[0].key == K_ESCAPE:
        terminate()
    return keyUpEvents[0].key

def checkForPauseBreak():
    #go through event queue looking for a KEYUP event
    #grab KEYDOWN events to remove them from the event queue
    checkForQuit()

    for event in pygame.event.get([KEYDOWN, KEYUP]):
        if event.type == KEYDOWN:
            continue
        return event.key
    return None

def checkForQuit():
    for event in pygame.event.get(QUIT): #get all the QUIT events
        terminate() #terminate if any QUIT events are present
    for event in pygame.event.get(KEYUP): #get all the KEYUP events
        if event.key == K_ESCAPE:
            terminate() #terminate if the KEYUP event was for the Esc key
        pygame.event.post(event) #put the other KEYUP event objects back

def showStartScreen():
    titleFont = pygame.font.Font('freesansbold.ttf', 100)
    titleSurf1 = titleFont.render('Wormy!', True, WHITE, DARKGREEN)
    titleSurf2 = titleFont.render('Wormy!', True, GREEN)

    degrees1 = 0
    degrees2 = 0
    while True:
        DISPLAYSURF.fill(BGCOLOR)
        #Create separate rotatedSurf variables instead of overwriting the
        #titleSurf variables because rotations are not completely perfect
        #e.g. rotating an image 10 degrees and back, the image you will
        #get is not exactly the same image you started with (exception
        #are multiples of 90)
        #Rotated image will also be slightly larger than the original image
        #so continual rotation will eventually create an image that will
        #be too large for Python to handle --> error message of pygame.error:
        #Width or height is too large
        rotatedSurf1 = pygame.transform.rotate(titleSurf1, degrees1)
        rotatedRect1 = rotatedSurf1.get_rect()
        rotatedRect1.center = (WINDOWWIDTH / 2, WINDOWHEIGHT / 2)
        DISPLAYSURF.blit(rotatedSurf1, rotatedRect1)

        rotatedSurf2 = pygame.transform.rotate(titleSurf2, degrees2)
        rotatedRect2 = rotatedSurf1.get_rect()
        rotatedRect2.center = (WINDOWWIDTH / 2, WINDOWHEIGHT / 2)
        DISPLAYSURF.blit(rotatedSurf2, rotatedRect2)

        drawPressKeyMsg()

        if checkForKeyPress():
            pygame.event.get() #clear event queue
            return
        pygame.display.update()
        FPSCLOCK.tick(FPS)
        degrees1 += 3 #rotate by 3 degrees each frame
        degrees2 += 7 #rotate by 7 degrees each frame

def terminate():
    pygame.quit()
    sys.exit()

def getRandomLocation():
    return {'x': random.randint(0, CELLWIDTH - 1), 'y': random.randint(0, CELLHEIGHT - 1)}

def place_fruit(wormCoords):
    apple = getRandomLocation()

    while apple in wormCoords:
        apple = getRandomLocation()
    
    poisoned_apples = []
    
    for i in range(POISONED_APPLE_COUNT):
        location = getRandomLocation()

        while location == apple or location in poisoned_apples or location in wormCoords:
            location = getRandomLocation()

        poisoned_apples.append(location)

    return apple, poisoned_apples

def showGameOverScreen():
    gameOverFont = pygame.font.Font('freesansbold.ttf', 150)
    gameSurf = gameOverFont.render('Game', True, WHITE)
    overSurf = gameOverFont.render('Over', True, WHITE)
    gameRect = gameSurf.get_rect()
    overRect = overSurf.get_rect()
    gameRect.midtop = (WINDOWHEIGHT / 2, 10)
    overRect.midtop = (WINDOWWIDTH / 2, gameRect.height + 10 + 25)

    DISPLAYSURF.blit(gameSurf, gameRect)
    DISPLAYSURF.blit(overSurf, overRect)
    drawPressKeyMsg()
    pygame.display.update()
    pygame.time.wait(500) #ensures player doesn't accidentally press a key too
                          #soon, which makes the game more "user" friendly
    checkForKeyPress() #clear out any key presses in the event queue

    while True:
        if checkForKeyPress():
            pygame.event.get() #clear event queue
            return

def drawScore(score):
    scoreSurf = BASICFONT.render('Score: %s' % (score), True, WHITE)
    scoreRect = scoreSurf.get_rect()
    scoreRect.topleft = (WINDOWWIDTH - 120, 10)
    DISPLAYSURF.blit(scoreSurf, scoreRect)

def drawWorm(wormCoords):
    for coord in wormCoords:
        x = coord['x'] * CELLSIZE
        y = coord['y'] * CELLSIZE
        wormSegmentRect = pygame.Rect(x, y, CELLSIZE, CELLSIZE)
        pygame.draw.rect(DISPLAYSURF, DARKGREEN, wormSegmentRect)
        wormInnerSegmentRect = pygame.Rect(x + 4, y + 4, CELLSIZE - 8, CELLSIZE - 8)
        pygame.draw.rect(DISPLAYSURF, GREEN, wormInnerSegmentRect)

def drawApple(coord):
    x = coord['x'] * CELLSIZE
    y = coord['y'] * CELLSIZE
    appleRect = pygame.Rect(x, y, CELLSIZE, CELLSIZE)
    pygame.draw.rect(DISPLAYSURF, RED, appleRect)

def drawPoisonedApples(poisoned_apples):
    for coord in poisoned_apples:
        x = coord['x'] * CELLSIZE
        y = coord['y'] * CELLSIZE
        poisoned_appleRect = pygame.Rect(x, y, CELLSIZE, CELLSIZE)
        pygame.draw.rect(DISPLAYSURF, BRIGHTYELLOW, poisoned_appleRect)

def drawGrid():
    for x in range(0, WINDOWWIDTH, CELLSIZE): #draw vertical lines
        pygame.draw.line(DISPLAYSURF, DARKGRAY, (x, 0), (x, WINDOWHEIGHT))
    for y in range(0, WINDOWHEIGHT, CELLSIZE): #draw horizontal lines
        pygame.draw.line(DISPLAYSURF, DARKGRAY, (0, y), (WINDOWWIDTH, y))

if __name__ == '__main__':
    main()
