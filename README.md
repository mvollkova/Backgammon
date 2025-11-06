# Backgammon — Console Game in C

## Overview

**Backgammon** is a console-based implementation of the classic board game, developed in the **C programming language**.  
This project was created as part of a **university programming assignment** to demonstrate practical knowledge of:

- Procedural programming in C,  
- Dynamic data structures and arrays,  
- Input/output (I/O) operations and file handling,  
- Game loop design and real-time user interaction,  
- Modular and maintainable code architecture.

The game reproduces the fundamental logic of traditional **Backgammon** — a two-player strategy game combining luck and skill — in a simple, text-based environment.


## Game Concept

Backgammon is one of the oldest known board games, where two players race to move all their pieces into their home board and then bear them off before the opponent does.

In this simplified **console version**, the board is represented using ASCII symbols arranged in a **matrix**, and the game logic manages dice rolls, piece movements, and player turns.

The program simulates dice rolls and validates moves according to the game’s rules, ensuring that no illegal actions are performed.


## Objective

The goal is to **move all your checkers into your home area** and **remove them (bear off)** before your opponent does.  
The first player to bear off all their checkers wins the game.
This version focuses on the **core gameplay mechanics** rather than graphical presentation — ideal for studying how logic, data structures, and algorithms can model real-world games in C.

## Player Color Assignment

| Player | Color | Direction |
|------|--------|-------------------------------------|
| **Player 1** | White| Moves **clockwise** |
| **Player 2** | Black |Moves **counterclockwise** |

Each player takes turns rolling dice and moving their checkers according to the rolled numbers.  
Captured pieces are re-entered into play, and the game continues until one player successfully removes all their checkers from the board.
##Controls

| Key | Function |
|------|-----------|
| **F** |See who goes first|
| **S** | Generate numbers |
| **C** |Choose Pawn Move |
| **Q** | Quit the game |


## Saving System & Game State Persistence

The game automatically stores the entire game state in a text file named |**`gamedata.txt`** | located in the project directory.
When you make moves during the game, all changes to the board (such as the player’s position or collected items) are recorded in memory.
Once you exit the game using the Q key, the program saves the current state of the matrix, so that the next time you start the game, it will restore all positions exactly as they were before.

To start a new game from scratch, simply delete the generated|**`gamedata.txt`** | file from your project directory — the program will then initialize a fresh board with default settings.

|**`moves.txt`** | Records a chronological history of player moves (useful for debugging or analyzing gameplay). |
