#include "config"
#include "func.h"
#include "node.h"
#include "bot.h"
#include <chrono>
#include <thread>
#include <cstring>
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

int8_t *Ishuffled = new int8_t[BoardSize * BoardSize];
int8_t *Jshuffled = new int8_t[BoardSize * BoardSize];
int8_t moveSize;

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

    string ImgName = "Output_";
    int index = 0;
    int computerColor = 1; //black first
    printboard(board, ImgName + to_string(index) + ".jpg");
    do
    {
        vector<pair<int8_t, int8_t>> moves;
        for (int8_t i = 0; i < EdgeSize; i++)
            for (int8_t j = 0; j < EdgeSize; j++)
                if (board[i * EdgeSize + j] == 0)
                    moves.emplace_back(make_pair(i, j));
        moveSize = moves.size();
        for (int ind = 0; ind < moveSize; ind++)
        {
            random_shuffle(moves.begin(), moves.end());
            for (int8_t i = 0; i < moveSize; i++)
            {
                Ishuffled[ind * moveSize + i] = moves[i].first;
                Jshuffled[ind * moveSize + i] = moves[i].second;
            }
        }
        cout << "Round " << index << endl;
        Bot bot(timeLimit, threadCount, board, computerColor);
        board = bot.play();
        computerColor = -computerColor;
        printboard(board, ImgName + to_string(++index) + ".jpg");
    } while (hasMove(board, 1) || hasMove(board, -1));
    cout << "Winner: " << (score(board) == 1 ? "Black" : "White") << endl;
    free(Ishuffled);
    free(Jshuffled);
    return 0;
}