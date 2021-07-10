#include "config"
#include "func.h"
#include "node.h"
#include "bot.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace chrono;

//calls /Storage/printBoard.py
void printboard(array<int8_t, BoardSize> board, string name)
{
    string str = "./Storage/printBoard.py " + to_string(EdgeSize) + " Storage/" + name + ".jpg ";
    for (int i = 0; i < BoardSize; i++)
        str += to_string(board[i]) + ".";
    char exe[str.length()];
    strcpy(exe, str.c_str());
    system(exe);
}

//An array of shuffled arrays filled with indexes
int8_t Ishuffled[BoardSize * BoardSize];
int8_t Jshuffled[BoardSize * BoardSize];

int main()
{
    int timeLimit, threadCount;
    cout << "timeLimit :";
    cin >> timeLimit;
    cout << "threadCount :";
    cin >> threadCount;

    array<int8_t, BoardSize> board{};
    int d = EdgeSize / 2 + EdgeSize * EdgeSize / 2;
    board[d] = 1;
    board[d - 1] = -1;
    board[d - EdgeSize] = -1;
    board[d - EdgeSize - 1] = 1;
    srand(system_clock::to_time_t(system_clock::now()));

    array<pair<int8_t, int8_t>, BoardSize> moves;
    for (int i = 0; i < EdgeSize; i++)
        for (int j = 0; j < EdgeSize; j++)
            moves[i * EdgeSize + j] = make_pair(i, j);

    for (int ind = 0; ind < BoardSize; ind++)
    {
        random_shuffle(moves.begin(), moves.end());
        for (int i = 0; i < BoardSize; i++)
        {
            Ishuffled[ind * BoardSize + i] = moves[i].first;
            Jshuffled[ind * BoardSize + i] = moves[i].second;
        }
    }
    string ImgName = "Output_";
    int index = 0;
    int computerColor = 1; //black first
    printboard(board, ImgName + to_string(index) + ".jpg");
    while (hasMove(board, 1) || hasMove(board, -1))
    {
        cout << "Round " << index << endl;
        Bot bot(timeLimit, threadCount, board, computerColor);
        board = bot.play();
        computerColor = -computerColor;
        printboard(board, ImgName + to_string(++index) + ".jpg");
    }
    cout << "Winner: " << (score(board) == 1 ? "Black" : "White") << endl;
    return 0;
}