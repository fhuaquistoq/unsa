#include <iostream>
#include <vector>
#include <queue>

int difficulty = -1;

char playerSymbol = 'X';
char aiSymbol = 'O';

void drawBoard(const char board[3][3])
{
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      std::cout << " " << board[i][j] << " ";
      if (j < 2)
        std::cout << "|";
    }
    std::cout << std::endl;
    if (i < 2)
      std::cout << "-----------" << std::endl;
  }

  std::cout << std::endl;

  std::cout << "Player: " << playerSymbol << " | AI: " << aiSymbol << std::endl;
}

int checkGame(const char board[3][3])
{
  int winner = 10;
  int draw = 0;
  int lose = -10;

  for (int i = 0; i < 3; i++)
  {
    if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
      if (board[i][0] == aiSymbol)
        return winner;
      else
        return lose;
  }

  for (int j = 0; j < 3; j++)
  {
    if (board[0][j] != ' ' && board[0][j] == board[1][j] && board[1][j] == board[2][j])
      if (board[0][j] == aiSymbol)
        return winner;
      else
        return lose;
  }

  if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
    if (board[0][0] == aiSymbol)
      return winner;
    else
      return lose;

  if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
    if (board[0][2] == aiSymbol)
      return winner;
    else
      return lose;

  return 0;
}

int main(int argc, char *argv[])
{

  if (argc > 1)
    difficulty = std::stoi(argv[1]);

  std::cout << "TIC TAC TOE" << std::endl;
  std::cout << "Who starts? Player or AI?" << std::endl;
  std::cout << "1) Player" << std::endl;
  std::cout << "2) AI" << std::endl;
  std::cout << "Enter your choice: ";

  int choice;
  std::cin >> choice;

  std::cout << std::endl;

  if (choice == 2)
  {
    playerSymbol = 'O';
    aiSymbol = 'X';
  }

  char board[3][3];
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      board[i][j] = ' ';
    }
  }

  char winner = ' ';

  while (winner == ' ')
  {
    drawBoard(board);

    std::cout << "Enter your move (row and column): ";
    int row, col;
    std::cin >> row >> col;
  }

  return 0;
}