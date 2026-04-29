#include <iostream>
#include <vector>

void drawBoard(const char playerSymbol, const char aiSymbol, const std::vector<std::vector<char>> &board)
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

int main(int argc, char *argv[])
{
  int difficulty = -1;

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

  char playerSymbol = 'X';
  char aiSymbol = 'O';

  if (choice == 2)
  {
    playerSymbol = 'O';
    aiSymbol = 'X';
  }

  std::vector<std::vector<char>> board(3, std::vector<char>(3, ' '));

  char winner = ' ';

  while (winner == ' ')
  {
    drawBoard(playerSymbol, aiSymbol, board);

    std::cout << "Enter your move (row and column): ";
    int row, col;
    std::cin >> row >> col;
  }

  return 0;
}