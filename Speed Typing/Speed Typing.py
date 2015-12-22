import random, sys, pygame, string
from pygame.locals import *

FPS = 30
WINDOW_WIDTH = 1000
WINDOW_HEIGHT = 600
HALF_WINDOW_WIDTH = WINDOW_WIDTH / 2
HALF_WINDOW_HEIGHT = WINDOW_HEIGHT / 2

WHITE = (255, 255, 255)
GRAY = (185, 185, 185)
DARK_GRAY = (40, 40, 40)
RED = (255, 0, 0)
BLACK = (0, 0, 0)
BG_COLOR = BLACK

LETTERS = string.ascii_lowercase
LETTER_KEYS = [K_a, K_b, K_c, K_d, K_e, K_f, K_g, K_h, K_i, K_j, K_k, K_l, K_m,
               K_n, K_o, K_p, K_q, K_r, K_s, K_t, K_u, K_v, K_w, K_x, K_y, K_z]
LETTER_DICT = {LETTER_KEYS[i]: LETTERS[i] for i in range(len(LETTER_KEYS))}

INVADER_COUNT = 10
INVADER_WIDTH = WINDOW_WIDTH / INVADER_COUNT
INVADER_HEIGHT = 100
INVADER_DICT = {i : None for i in range(1, INVADER_COUNT + 1)}

VERTICAL_VELOCITY_INCREMENT = 0.4 #amount to increment 'VERTICAL_VELOCITY' by
                                  #after 'WORD_THRESHOLD' number of words have
                                  #been successfully typed
VERTICAL_BOUNDARY = WINDOW_HEIGHT - (INVADER_HEIGHT / 2)

englishWordFilename = "englishWordsList.txt"
englishWordFile = open(englishWordFilename, 'r')
englishWords = englishWordFile.readlines()

for i in range(len(englishWords)):
    englishWords[i] = englishWords[i].strip('\n')

WORD_THRESHOLD = 5 #number of correct words needed
                   #to type before increasing velocity

def main():
    global FPS_CLOCK, DISPLAY_SURF, BASIC_FONT, BIG_FONT, INVADER
    
    pygame.init()
    FPS_CLOCK = pygame.time.Clock()
    DISPLAY_SURF = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
    BASIC_FONT = pygame.font.Font('freesansbold.ttf', 18)
    BIG_FONT = pygame.font.Font('freesansbold.ttf', 90)
    pygame.display.set_caption('Speed Typing')

    INVADER = pygame.image.load('invader.png')
    
    showStartScreen()
    
    while True:
        runGame()
        showGameOverScreen()

def showStartScreen():
    titleSurf = BIG_FONT.render('Speed Typing!', True, RED)
    titleRect = titleSurf.get_rect()
    titleRect.center = (HALF_WINDOW_WIDTH, HALF_WINDOW_HEIGHT - 50)
    
    howToPlaySurf = BASIC_FONT.render('Type as fast as you can before the invaders land!', True, WHITE)
    howToPlayRect = howToPlaySurf.get_rect()
    howToPlayRect.center = (HALF_WINDOW_WIDTH, HALF_WINDOW_HEIGHT + 10)

    nbSurf = BASIC_FONT.render('(N.B.: To pause the game, hit the PAUSE key)', True, WHITE)
    nbRect = nbSurf.get_rect()
    nbRect.center = (HALF_WINDOW_WIDTH, HALF_WINDOW_HEIGHT + 30)
    
    while True:
        DISPLAY_SURF.fill(BG_COLOR)
        DISPLAY_SURF.blit(titleSurf, titleRect)
        DISPLAY_SURF.blit(howToPlaySurf, howToPlayRect)
        DISPLAY_SURF.blit(nbSurf, nbRect)
        
        drawPressKeyMsg()

        if checkForKeyPress():
            pygame.event.get()
            return

        pygame.display.update()
        FPS_CLOCK.tick(FPS)

def drawPressKeyMsg():
    pressKeySurf = BASIC_FONT.render('Press a key to play.', True, DARK_GRAY)
    pressKeyRect = pressKeySurf.get_rect()
    pressKeyRect.topleft = (WINDOW_WIDTH - 200, WINDOW_HEIGHT - 30)
    DISPLAY_SURF.blit(pressKeySurf, pressKeyRect)
    
def checkForKeyPress():
    if len(pygame.event.get(QUIT)) > 0:
        terminate()

    keyUpEvents = pygame.event.get(KEYUP)

    if len(keyUpEvents) == 0:
        return None

    if keyUpEvents[0].key == K_ESCAPE:
        terminate()

    return keyUpEvents[0].key

def runGame():
    global VERTICAL_VELOCITY
    VERTICAL_VELOCITY = 1
    
    RANDOM_WORD = newRandomWord()

    USER_STRING = ''
    
    SCORE = 0
    WORD_COUNT = 0

    while True:
        if USER_STRING == RANDOM_WORD: #word successfully typed
                                       #before invasion occurred
            SCORE += 1
            WORD_COUNT += 1

            if WORD_COUNT == WORD_THRESHOLD:
                WORD_COUNT = 0
                VERTICAL_VELOCITY += VERTICAL_VELOCITY_INCREMENT
            
            USER_STRING = ''
            RANDOM_WORD = newRandomWord()
            
            destroyInvaders()
            
        if successfulInvasion():
            destroyInvaders() #reset invaders for new game
            return

        for event in pygame.event.get():
            if event.type == QUIT:
                terminate()

            if event.type == KEYUP:
                if event.key == K_PAUSE:
                    DISPLAY_SURF.fill(BG_COLOR)
                    showPauseScreen()

                elif event.key == K_ESCAPE:
                    terminate()

                elif event.key == K_BACKSPACE and len(USER_STRING) != 0:
                    USER_STRING = USER_STRING[:-1]

                elif event.key in LETTER_KEYS:
                    USER_STRING = USER_STRING + LETTER_DICT[event.key]

        DISPLAY_SURF.fill(BG_COLOR)
        moveInvaders()
        addInvader()
        drawInvaders()
        drawRandomWord(RANDOM_WORD)
        drawUserString(USER_STRING)
        drawScore(SCORE)
        drawWordCount(WORD_COUNT)
        pygame.display.update()
        FPS_CLOCK.tick(FPS)

def showPauseScreen():
    titleSurf = BIG_FONT.render('PAUSED', True, GRAY)
    titleRect = titleSurf.get_rect()
    titleRect.center = (HALF_WINDOW_WIDTH, HALF_WINDOW_HEIGHT)
    DISPLAY_SURF.blit(titleSurf, titleRect)

    titleSurf = BIG_FONT.render('PAUSED', True, WHITE)
    titleRect = titleSurf.get_rect()
    titleRect.center = (HALF_WINDOW_WIDTH - 3, HALF_WINDOW_HEIGHT - 3)
    DISPLAY_SURF.blit(titleSurf, titleRect)

    drawPressKeyMsg()

    while checkForKeyPress() == None:
        pygame.display.update()
        FPS_CLOCK.tick(FPS)
        
def showGameOverScreen():
    gameOverFont = pygame.font.Font('freesansbold.ttf', 150)
    gameSurf = gameOverFont.render('Game', True, WHITE)
    overSurf = gameOverFont.render('Over', True, WHITE)
    gameRect = gameSurf.get_rect()
    overRect = overSurf.get_rect()
    gameRect.midtop = (HALF_WINDOW_WIDTH, HALF_WINDOW_HEIGHT - 150)
    overRect.midtop = (HALF_WINDOW_WIDTH, HALF_WINDOW_HEIGHT - 25)

    DISPLAY_SURF.blit(gameSurf, gameRect)
    DISPLAY_SURF.blit(overSurf, overRect)
    drawPressKeyMsg()
    pygame.display.update()
    pygame.time.wait(500)
    checkForKeyPress()

    while True:
        if checkForKeyPress():
            pygame.event.get()
            return

def newRandomWord():
    randomIndex = random.randint(0, len(englishWords) - 1)
    return englishWords[randomIndex]

def drawRandomWord(randomWord):
    randomWordString = "New Word: {}".format(randomWord)
    randomWordStringSurf = BASIC_FONT.render(randomWordString, True, WHITE)
    randomWordStringRect = randomWordStringSurf.get_rect()
    randomWordStringRect.topleft = (25, 25)
    DISPLAY_SURF.blit(randomWordStringSurf, randomWordStringRect)

def drawUserString(string):
    userString = "User-Typed String: {}".format(string)
    userStringSurf = BASIC_FONT.render(userString, True, WHITE)
    userStringRect = userStringSurf.get_rect()
    userStringRect.topleft = (25, 45)
    DISPLAY_SURF.blit(userStringSurf, userStringRect)

def drawScore(score):
    scoreString = "Score: {}".format(score)
    scoreStringSurf = BASIC_FONT.render(scoreString, True, WHITE)
    scoreStringRect = scoreStringSurf.get_rect()
    scoreStringRect.topright = (WINDOW_WIDTH - 25, 25)
    DISPLAY_SURF.blit(scoreStringSurf, scoreStringRect)

def drawWordCount(wordCount):
    wordCountString = "Word Count: {}".format(wordCount)
    wordCountSurf = BASIC_FONT.render(wordCountString, True, WHITE)
    wordCountRect = wordCountSurf.get_rect()
    wordCountRect.topright = (WINDOW_WIDTH - 25, 45)
    DISPLAY_SURF.blit(wordCountSurf, wordCountRect)

def drawInvaders():
    for i in INVADER_DICT.keys():
        if INVADER_DICT[i] == None:
            continue

        invaderSurf = INVADER_DICT[i]['surf']
        center_x = INVADER_DICT[i]['center_x']
        center_y = INVADER_DICT[i]['center_y']
        invaderRect = invaderSurf.get_rect()
        invaderRect.center = (center_x, center_y)
        DISPLAY_SURF.blit(invaderSurf, invaderRect)

def moveInvaders():
    global INVADER_DICT
    
    for i in INVADER_DICT.keys():
        if INVADER_DICT[i] == None or INVADER_DICT[i]['center_y'] == VERTICAL_BOUNDARY:
            continue

        elif INVADER_DICT[i]['center_y'] + VERTICAL_VELOCITY >= VERTICAL_BOUNDARY:
            INVADER_DICT[i]['center_y'] = VERTICAL_BOUNDARY

        else:
            INVADER_DICT[i]['center_y'] += VERTICAL_VELOCITY

def addInvader():
    global INVADER_DICT

    if None not in INVADER_DICT.values():
        return

    probability = 0.25
    randomValue = random.random()

    if randomValue >= probability:
        return

    missingValues = []

    for i in INVADER_DICT.keys():
        if INVADER_DICT[i] == None:
            missingValues.append(i)

    randomIndex = random.randint(0, len(missingValues) - 1)
    chosenIndex = missingValues[randomIndex]

    INVADER_DICT[chosenIndex] = {}
    INVADER_DICT[chosenIndex]['surf'] = pygame.transform.scale(INVADER, (INVADER_WIDTH, INVADER_HEIGHT))
    INVADER_DICT[chosenIndex]['center_x'] = (INVADER_WIDTH / 2) * ((2 * chosenIndex) - 1)
    INVADER_DICT[chosenIndex]['center_y'] = INVADER_HEIGHT / 2

def destroyInvaders():
    global INVADER_DICT

    INVADER_DICT = {i : None for i in range(1, INVADER_COUNT + 1)}

def successfulInvasion():
    for i in INVADER_DICT.keys():
        if INVADER_DICT[i] == None or INVADER_DICT[i]['center_y'] != VERTICAL_BOUNDARY:
            return False

    return True

def terminate():
    pygame.quit()
    sys.exit()

if __name__ == '__main__':
    main()
