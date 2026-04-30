#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>

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

  std::cout << "Humano: " << playerSymbol << " | IA: " << aiSymbol << std::endl;
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

int minimax(char board[3][3], int depth, bool isMaximizing)
{
  int score = checkGame(board);
  if (score == 10)
    return score;
  if (score == -10)
    return score;

  bool hasEmpty = false;
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      if (board[i][j] == ' ')
      {
        hasEmpty = true;
        break;
      }
    }
    if (hasEmpty)
      break;
  }

  if (!hasEmpty)
    return 0;

  if (difficulty != -1 && depth >= difficulty)
    return score;

  if (isMaximizing)
  {
    int bestScore = -1000;
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (board[i][j] == ' ')
        {
          board[i][j] = aiSymbol;
          int current = minimax(board, depth + 1, false);
          bestScore = std::max(current, bestScore);
          board[i][j] = ' ';
        }
      }
    }
    return bestScore;
  }
  else
  {
    int bestScore = 1000;
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (board[i][j] == ' ')
        {
          board[i][j] = playerSymbol;
          int current = minimax(board, depth + 1, true);
          bestScore = std::min(current, bestScore);
          board[i][j] = ' ';
        }
      }
    }
    return bestScore;
  }
}

bool isBoardFull(const char board[3][3])
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (board[i][j] == ' ')
        return false;
  return true;
}

void aiMove(char board[3][3])
{
  int bestScore = -1000;
  int bestR = -1, bestC = -1;
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      if (board[i][j] == ' ')
      {
        board[i][j] = aiSymbol;
        int score = minimax(board, 1, false);
        board[i][j] = ' ';
        if (score > bestScore)
        {
          bestScore = score;
          bestR = i;
          bestC = j;
        }
      }
    }
  }

  if (bestR != -1)
    board[bestR][bestC] = aiSymbol;
}

int main(int argc, char *argv[])
{

  if (argc > 1)
    difficulty = std::stoi(argv[1]);

  std::cout << "TIC TAC TOE" << std::endl;
  std::cout << "Quien comienza?" << std::endl;
  std::cout << "1) Humano" << std::endl;
  std::cout << "2) IA" << std::endl;
  std::cout << "Decide quien comienza: ";

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
  bool aiTurn = (choice == 2);

  while (true)
  {
    drawBoard(board);

    int gameScore = checkGame(board);
    if (gameScore == 10)
    {
      winner = aiSymbol;
      break;
    }
    else if (gameScore == -10)
    {
      winner = playerSymbol;
      break;
    }

    if (isBoardFull(board))
    {
      break;
    }

    if (aiTurn)
    {
      aiMove(board);
      aiTurn = false;
    }
    else
    {
      int row = -1, col = -1;
      while (true)
      {
        std::cout << "Coloca tu movimiento (fila y columna, 1-3): ";
        if (!(std::cin >> row >> col))
        {
          std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          std::cout << "Entrada inválida. Por favor, ingresa dos numeros." << std::endl;
          continue;
        }
        row -= 1;
        col -= 1;
        if (row < 0 || row >= 3 || col < 0 || col >= 3)
        {
          std::cout << "Coordenadas fuera de rango. Usa 1-3." << std::endl;
          continue;
        }
        if (board[row][col] != ' ')
        {
          std::cout << "Celda ya ocupada. Elige otra." << std::endl;
          continue;
        }
        board[row][col] = playerSymbol;
        break;
      }
      aiTurn = true;
    }
  }

  drawBoard(board);
  if (winner == aiSymbol)
    std::cout << "IA Gana!" << std::endl;
  else if (winner == playerSymbol)
    std::cout << "Humano Gana!" << std::endl;
  else
    std::cout << "Empate!" << std::endl;

  return 0;
}