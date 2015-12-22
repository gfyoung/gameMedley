import math
import string
import random

card_values = {'a': (1, 11), '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
               '8': 8, '9': 9, '10': 10, 'j': 10, 'q': 10, 'k': 10}
def New_Deck(n):
    Deck = {'a': 4 * n, '2': 4 * n, '3': 4 * n, '4': 4 * n, '5': 4 * n,
            '6': 4 * n, '7': 4 * n, '8': 4 * n, '9': 4 * n, '10': 4 * n,
            'j': 4 * n, 'q': 4 * n, 'k': 4 * n}
    return Deck

def Update_Deck(deck, card):
    deck[card] = deck[card] - 1
    if deck[card] == 0:
        del deck[card]
    return deck

def Card_Count(deck):
    num_cards = 0
    for card in deck:
        for count in range(deck[card]):
            num_cards += 1
    return num_cards

def New_Hand(deck):
    hand = {}
    deck_list = []
    for card in deck.keys():
        for count in range(deck[card]):
            deck_list.append(card)
    card1 = deck_list[random.choice(range(len(deck_list)))]
    hand[card1] = 1
    deck = Update_Deck(deck, card1)
    del deck_list[deck_list.index(card1)]
    card2 = deck_list[random.choice(range(len(deck_list)))]
    if card2 == card1:
        hand[card1] += 1
    else:
        hand[card2] = 1
    deck = Update_Deck(deck, card2)
    return hand
    
def add_card(hand, card, deck):
    if card in hand.keys():
        hand[card] += 1
    else:
        hand[card] = 1
    deck = Update_Deck(deck, card)
    return hand

def Lowest_Hand_Value(hand):
    lowest_card_value = 0
    for card in hand.keys():
        for value in range(hand[card]):
            if card == 'a':
                lowest_card_value += 1
            else:
                lowest_card_value += card_values[card]
    return lowest_card_value

def New_Human_Hand_Value(hand):
    hum_card_value = 0
    for card in hand.keys():
        for value in range(hand[card]):
            if card == 'a':
                if hum_card_value + 11 > 21:
                    hum_card_value += 1
                else:
                    value = int(raw_input("Ace Value: 1 or 11?: "))
                    hum_card_value += value
            else:
                hum_card_value += card_values[card]
    return hum_card_value

def Human_Value_Update(card, hand_value):
    if card == 'a':
        if hand_value + 11 > 21:
            hand_value += 1
        else:
            value = int(raw_input("Ace Value: 1 or 11?: "))
            hand_value += value
    else:
        hand_value += card_values[card]
    return hand_value

def get_hand(hand):
    hand_list = ''
    for card in hand.keys():
        for count in range(hand[card]):
            hand_list = hand_list + card + ' '
    return hand_list    

def New_Comp_Hand_Value(hand):
    comp_card_value = 0
    ace_waitlist = []
    for card in hand.keys():
        for count in range(hand[card]):
            if card == 'a':
                ace_waitlist.append(card)
            else:
                comp_card_value += card_values[card]
    for ace in ace_waitlist:
        if comp_card_value + 11 > 21:
            comp_card_value += 1
        else:
            comp_card_value += 11
    return comp_card_value

def Comp_Value_Update(card, hand_value):
    if card == 'a':
        if hand_value + 11 > 21:
            hand_value += 1
        else:
            hand_value += 11
    else:
        hand_value += card_values[card]
    return hand_value

def play_game(n):
    pot = 0
    amount = 1000
    BJ_Deck = New_Deck(n)
    while amount > 0:
        print "Pot:", pot
        print "Amount:", amount
        waiting = True
        while waiting:
            try:
                bet = int(raw_input("Bet: "))
                if pot + bet <= amount:
                    break
                else:
                    print "Invalid bet!"
            except:
                print "Not Valid Bet!"
        pot += bet
        print "New Pot:", pot
        if Card_Count(BJ_Deck) < 2:
            BJ_Deck = New_Deck(n)
        Player_Hand = New_Hand(BJ_Deck)
        print "Your Current Hand:", get_hand(Player_Hand)
        Min_Value = Lowest_Hand_Value(Player_Hand)
        if Min_Value > 21:
            print "Bust! You Lose!"
            amount -= pot
            pot = 0
        if Min_Value <= 21:
            hand_list = []
            updated_list = []
            updated_list_values = []
            if len(Player_Hand) == 1:
                print "Do you want to split?  Type y for yes and n for no"
                decision = str(raw_input("Choice: "))
                while decision != 'y' and decision != 'n':
                    print "Invalid option!"
                    decision = str(raw_input("Choice: "))
                if decision == 'y':
                    hand_list.append({Player_Hand.keys()[0]:1})
                    hand_list.append({Player_Hand.keys()[0]:1})
                if decision == 'n':
                    hand_list.append({Player_Hand.keys()[0]:2})
            else:
                hand_list.append({Player_Hand.keys()[0]:1, Player_Hand.keys()[1]:1})
            for cardhand in hand_list:
                if len(hand_list) == 2:
                    print "Your Current Hand:", get_hand(cardhand)
                cardhand_value = New_Human_Hand_Value(cardhand)
                print "Hit or Stand?  Type h for hit and s for stand."
                choice = str(raw_input("Choice: "))
                while choice != 's':
                    while choice != 'h' and choice != 's':
                        print "Invalid Option!"
                        choice = str(raw_input("Choice: "))
                    while choice == 'h':
                        if Card_Count(BJ_Deck) == 0:
                            BJ_Deck = New_Deck(n)
                        deck_list = []
                        for card in BJ_Deck.keys():
                            for count in range(BJ_Deck[card]):
                                deck_list.append(card)
                        new_card = deck_list[random.choice(range(len(deck_list)))]
                        cardhand = add_card(cardhand, new_card, BJ_Deck)
                        print "Your Current Hand:", get_hand(cardhand)
                        cardhand_value = Human_Value_Update(new_card, cardhand_value)
                        if cardhand_value > 21:
                            print "Bust! Hand Eliminated!"
                            choice = 's'
                            break
                        else:
                            print "Hit or Stand?  Type h for hit and s for stand."
                            choice = str(raw_input("Choice: "))
                if choice == 's' and cardhand_value <= 21:
                    updated_list.append(cardhand)
                    updated_list_values.append(cardhand_value)
        win_count = 0
        loss_count = 0
        tie_count = 0
        while loss_count != len(updated_list) and win_count == 0:
            if Card_Count(BJ_Deck) < 2:
                BJ_Deck = New_Deck(n)
            Comp_Hand = New_Hand(BJ_Deck)
            Comp_Hand_Value = New_Comp_Hand_Value(Comp_Hand)
            print "Computer's Initial Hand:", get_hand(Comp_Hand)
            while Comp_Hand_Value < 17:
                if Card_Count(BJ_Deck) == 0:
                    BJ_Deck = New_Deck(n)
                deck_list = []
                for card in BJ_Deck.keys():
                    for count in range(BJ_Deck[card]):
                        deck_list.append(card)
                new_card = deck_list[random.choice(range(len(deck_list)))]
                Comp_Hand = add_card(Comp_Hand, new_card, BJ_Deck)
                Comp_Hand_Value = Comp_Value_Update(new_card, Comp_Hand_Value)
            print "Computer's Final Hand:", get_hand(Comp_Hand)
            for cardhand in updated_list:
                if Comp_Hand_Value > 21 or updated_list_values[updated_list.index(cardhand)] > Comp_Hand_Value:
                    print "Your Hand Wins!"
                    win_count += 1
                    break
                elif Comp_Hand_Value <= 21 and Comp_Hand_Value > updated_list_values[updated_list.index(cardhand)]:
                    print "Your Hand Loses!"
                    loss_count += 1
                elif Comp_Hand_Value == updated_list_values[updated_list.index(cardhand)]:
                    print "Tie!"
                    tie_count += 1
            if win_count == 0 and tie_count > 0:
                break
        if win_count > 0:
            print "You Win!"
            amount += pot
            pot = 0
        if win_count == 0 and loss_count == len(updated_list):
            print "You Lose!"
            amount -= pot
            pot = 0
        if win_count == 0 and tie_count > 0:
            print "We had a tie!  So we'll deal again!"
    print "Sorry, you lose! Try again next time!"
    game_options()
    
def game_options():
    print "Choose from the following options:"
    print "n: Start a new game"
    print "a: What is Blackjack?"
    print "r: Rules of Blackjack"
    print "s: Statistics"
    print "e: Exit the game"
    print "Note: if you have never played on this Blackjack program before, it is recommended you type 'r' to read the rules before playing a game."
    valid_options = ['n','a','r','s','e']
    option = str(raw_input("Choice: "))
    while option not in valid_options:
        print "Invalid option!"
        option = str(raw_input("Choice: "))
    if option == 'n':
        waiting = True
        while waiting:
            try:
                num_decks = int(raw_input("How many decks?: "))
                break
            except:
                print "Invalid number of decks!"
        play_game(num_decks)
    elif option == 'a':
        print "Blackjack is a card game in which you try to outsmart the dealer by drawing a set of cards that are worth more than the dealer's or do not exceed 21 when the dealer's cards do. The game is continuous, and you try to rack up as much money as possible. However, don't be reckless when you bet! Once you run out, you're done, and it's game over!"
        proceed = raw_input("Type anything or press [ENTER] to continue:")
        print ""
        game_options()
    elif option == 'r':
        print "Rules:"
        print "1) The player starts with an amount of $1000. The player is only allowed to bet dollar amounts. Before each dealing, the player makes a bet. This bet is added to a pool of money known as the pot, which resets to zero after a decisive result (i.e. the dealer or player wins). If the player wins, the pot is added to his amount. If the player loses, the pot is subtracted from his amount. When the amount hits zero, the player loses the game."
        print "2) Each deck contains 13 types of cards: ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, jack, queen, and king"
        print "3) 2 - 10 cards are worth what their number. Jack, queen, and king are worth 10. Ace is worth 1 or 11 depending on the player's choice."
        print "4) If the player's hand exceeds a value of 21, it is called a bust, and the player automotically loses."
        print "5) If the player's hand does not bust, and if the dealer's hand exceeds a value of 21, the dealer automatically loses."
        print "6) If neither bust, and the dealer's hand is greater than that of the player's, the player loses.  Otherwise, the player wins."
        print "7) A tie occurs when the player's card values equal those of the dealer's. In this case, the cards are discarded, and the player draws a new hand. As it is a new round of drawing, the player will have the option of betting more money if he is so inclined. However, the pot cannot exceed the money that the player has."
        print "7) At the start of the dealing, the player is given two cards. Provided they do not bust or are not of the same type, the player will be asked if he wants another card (hit) or not (stand). If the player hits, he will be given another card and will be asked the same question again, provided he does not bust. If he stands, then the dealer will draw its own hand."
        print "8) If the two cards drawn are of the same type (e.g. two jacks), the player has the option of splitting his cards, in which each card becomes a separate hand. The dealer does not have this option. If the player splits his cards, the player will play each hand separately. However, the dealer will not draw cards until the player is finished with the second hand. The dealer wins if the player does a combination of the following with both hands: busting or having a lower card value than that of the dealer's. The player wins if the dealer busts or if one of the player's hands has a higher value than that of the dealer's."
        print "9) Once a game starts, the player can only quit the game by pressing Ctrl + C. This will cause a KeyboardInterrupt in the program, and it will quit."
        proceed = raw_input("Type anything or press [ENTER] to continue:")
        print ""
        game_options()
    elif option == 'e':
        print "Thanks for playing! Bye!"

print "Welcome to Blackjack!"
game_options()
