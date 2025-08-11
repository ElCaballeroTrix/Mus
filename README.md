# Mus

This is a prototype of the Spanish card game called "Mus". It has been created in Unreal Engine 5.5, only using widgets and it is a single player game.
In the game "Mus", the goal is to reach a set number of points by winning rounds, having the best hands in four categories: "Grande", "Chica", "Parejas", and "Juego", where players can discard cards and engage in betting and bluffing in the rounds ending in each player revealing their cards.
<img width="1230" height="761" alt="Mus1" src="https://github.com/user-attachments/assets/1621be2a-e8dc-432d-bd2c-311daf5de75c" />

The logic of the game can be found in the "Source/MusModerno/Data" folder where the "MusManager" and "BotAI" classes are the most important and, the logic of the UI elements in the "Source/MusModerno/UI" where the "MusTable" class stands out. 
## Rounds
In each round, the player has a series of actions that it can do depending on the current state of the game. If the cards have just been given to the players, they can decide either to call “Mus”, start the game, or “No Mus”, they want to discard some cards for new ones. If only one of them calls “Mus”, the betting rounds begin. In each betting round, a participant can pass its turn, place a bet, accept the bet, or go all in “Órdago”. Once all categories are over, the winners of each round are announced as well as their price.\ <img width="1227" height="762" alt="Mus2" src="https://github.com/user-attachments/assets/9a97b572-cbbd-4f1e-855f-090b60d90182" />

\
If there is a discard phase, players select which cards to discard. The bots will decide on these cards based on their current cards: if the bot has 2 or more kings, 4 equal cards, a trio of the same card, or a pair and a king, then it will not discard any cards. If it does discard, the kings will never be discarded, and for every card, there is a 40% probability of discarding the card. \
\
In the "Grande" round, the player with the highest cards wins. In comparing two hands, the number of kings they both have is compared. If one has more than the other, it wins, and if they have the same number, the next highest card is compared, and so on. \
In my code, I made a double loop where I compare the cards of the participants that have accepted the bet, starting with the king and going down. First, if the participant has several cards with values the same as the current cards being compared, it is added to a list of cards that made the player win the round for future use. Next, it enters the second loop, where it compares the number of cards a participant has compared to the current possible winner. If it has more cards of the value of the current evaluated card, it will be the possible winner. If it is the same, the loop continues, and if at any iteration, there is no other participant that has the same or highest amount of a card value, the double loop stops with a winner.\
\
The "Chica" round is the opposite of the “Grande” round, the player with the lowest cards wins. So, in terms of programming, it is basically the same except that the comparison of the cards starts from the lowest value and goes up.\
\
In the "Parejas" round, the player with the highest number of cards with the highest cards wins. Where a player with 4 cards of the same value would win a player with 3, but this one would win a participant that has 2 pairs, and 2 pairs wins a pair. Before this round starts, players must say whether they can participate based on their cards. If they have 4 of the same cards, a trio or a pair, they can play this round.\
In my code, the first thing that is done is detect if a participant has a pair, two, a trio of 4 equal cards and save it in a list for future use. Then comes the comparison part, where if there is only one player with 4 equal cards, it wins, only a trio wins and only a pair wins. Else, the cards are compared and the one with the highest value is won (example: a trio of kings wins a trio of sevens).\
\
In the “Juego” round, the sum of the cards of a participant is compared. If it is 31, it wins. The next best is 32, then 40 and then in descending order. Before this round starts, participants must declare whether they have a total sum of 31 or more to participate in the round. If no one can enter this round, then a “Punto” round starts, which basically compares the sum of all cards and the highest wins, with 30 being the highest possible value.\
<img width="1230" height="756" alt="Mus3" src="https://github.com/user-attachments/assets/2583efd8-fb63-4a2b-a046-b795a7915342" />

### Bot Betting
-The first thing calculated is whether a bot can bluff or not, and it is decided randomly by a 50% change. \
-If someone called "Órdago", and someone is close to winning, he also calls "Órdago", otherwise he does not unless he can bluff. If he can bluff, then there is a very small chance of doing so.\
-In other to control that there is not an infinite betting, a bot can only make a higher bet a maximum of 3 times.\
-Depending on the betting round, a bot decides whether to go depending on its cards. If it can bluff before doing so, there is a percentage chance that it will raise the bet without looking at its cards.\
If there is no bluff, there is a percentage of going or not that depends on the cards it has. If the round is “Grande”, the more kings it has, the more likely it is to play the round. If it is “Chica”, number of aces. If it is “Pares”, probability of 1 if you have four pairs of a kind, 0.8 if you have three pairs of a kind, 0.6 if you have two pairs and 0.4 if you have one pair. If it is “Juego”, 1 if the sum of the cards is 31, 0.8 if the sum is 32, 0.6 if the sum is 40, and 0.4 for the rest. Finally, for “Punto”, probability of 1 if the sum is 30, 0.7 if the sum is greater than 25, and 0.5 if the sum is greater than 20 and less than 25.\
-Once the bot decides whether to play or not, either based on its cards or by bluffing, it will go all in if someone or it is close to winning and bet otherwise. If this is not the case and no one has bet, it will bet, and if someone has already bet, there is a probability of responding with a bet or with a “Quiero.”\

