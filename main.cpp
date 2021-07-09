#include "config"
#include "func.h"
#include "node.h"
#include <time.h>
#include <thread>
#include <iostream>
#include <algorithm>
using namespace std;

array<array<pair<int8_t, int8_t>, BoardSize>, BoardSize> shuffledMoves;
//continue exploring until time up
void Countdown(time_t timeLimit, Nodes *root, int id)
{
    while (time(0) < timeLimit)
        root->explore(id, 2);
}
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
array<int8_t, BoardSize> GetStep(array<int8_t, BoardSize> target, int &thinkTime, int threadCount, Nodes *root, int &id)
{
    time_t timeLimit = time(0) + thinkTime;
    id = root->playermove(id, target);
    root->clean();
    //initialize thread
    vector<thread> threadvec;
    for (int i = 0; i < threadCount; i++)
        threadvec.emplace_back(Countdown, timeLimit, root, id);
    //after 5 seconds
    for (int i = 0; i < threadCount; i++)
        threadvec[i].join();

    cout << "Total playouts: " << root->totalGames[id] << endl;
    if (root->gameover[id] == -2 || root->gameover[id] == 0)
    {
        if (root->gameover[id] == 0)
            cout << "Maybe Draw\n";
        cout << (root->color[id] == 1 ? "Black " : "White ");
        id = root->getbest(id);
        float childUtility = float(root->totalScore[id] * root->color[id]) / (2 * root->totalGames[id]);
        cout << "winrate estimate: " << 0.5 - childUtility << endl;
        return root->board[id];
    }
    else
        cout << "Winner: " << (root->gameover[id] == 1 ? "Black\n" : "White\n");
    id = root->getbest(id);
    return root->board[id];
}

void init(array<int8_t, BoardSize> &board)
{
    int d = EdgeSize / 2 + EdgeSize * EdgeSize / 2;
    board[d] = 1;
    board[d - 1] = -1;
    board[d - EdgeSize] = -1;
    board[d - EdgeSize - 1] = 1;
    srand(time(0));
    array<pair<int8_t, int8_t>, BoardSize> defaultMoves;
    for (int i = 0; i < EdgeSize; i++)
        for (int j = 0; j < EdgeSize; j++)
            defaultMoves[i * EdgeSize + j] = make_pair(i, j);
    for (auto &bd : shuffledMoves)
        bd = defaultMoves;
    for (auto &bd : shuffledMoves)
        random_shuffle(bd.begin(), bd.end());
}

int main()
{
    int timeLimit;
    cout << "timeLimit :";
    cin >> timeLimit;
    //black
    int computerColor = 1;
    int threadCount;
    cout << "threadCount :";
    cin >> threadCount;

    array<int8_t, BoardSize> board{};
    init(board);

    Nodes root(computerColor, board);

    string ImgName = "Output_";
    int index = 0;
    int id = 0;
    while (hasMove(board, 1) || hasMove(board, -1))
    {
        cout << "Round " << index << endl;
        printboard(board, ImgName + to_string(index++) + ".jpg");
        board = GetStep(board, timeLimit, threadCount, &root, id);
    }
    printboard(board, ImgName + to_string(index++) + ".jpg");
    cout << "Winner: " << (score(board) == 1 ? "Black" : "White") << endl;
    return 0;
}