#include "func.h"
#include "node.h"
#include <vector>
#include <thread>
#include <iostream>
using namespace std;

mutex MtxBoard;
vector<array<int8_t, BoardSize>> boards;
array<array<pair<int8_t, int8_t>, BoardSize>, BoardSize> RdMoves;

array<int8_t, BoardSize> *makeNewBoard(array<int8_t, BoardSize> *oldPtr)
{
    lock_guard<mutex> lock(MtxBoard);
    boards.emplace_back(*oldPtr);
    return &boards.back();
}
void delBoard(array<int8_t, BoardSize> *boardPtr)
{
    lock_guard<mutex> lock(MtxBoard);
    *boardPtr = boards.back();
    boards.pop_back();
}

//continue exploring until time up
void Countdown(time_t timeLimit, Node *root)
{
    while (time(0) < timeLimit)
    {
        root->explore();
    }
    return;
}

array<int8_t, BoardSize> GetStep(array<int8_t, BoardSize> board, int &thinkTime, int threadCount, Node *&root)
{
    time_t timeLimit = time(0) + thinkTime;
    //initialize thread
    vector<thread> threadvec;
    for (int i = 0; i < threadCount; i++)
        threadvec.emplace_back(Countdown, timeLimit, root);
    //after 5 seconds
    for (int i = 0; i < threadCount; i++)
        threadvec[i].join();

    cout << "Total playouts: " << root->totalgames << endl;
    if (root->gameover == -2)
    {
        cout << (root->col == 1 ? "Black " : "White ");
        root = root->getbest();
        float winrate = float(root->totalscore) / (2 * root->totalgames) + 0.5;
        cout << "winrate estimate: " << 1 - winrate << endl;
        return *root->board;
    }
    else if (root->gameover == 0)
        cout << "Draw\n";
    else
        cout << "Winner: " << (root->gameover == 1 ? "Black\n" : "White\n");
    root = root->getbest();
    root->clean();
    return *root->board;
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
    for (auto &bd : RdMoves)
        bd = defaultMoves;
    for (auto &bd : RdMoves)
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
    boards.reserve(10000000);
    boards.push_back(board);

    Node Source(&boards.front(), computerColor);
    Node *root = &Source;
    string ImgName = "Output_";
    int index = 0;
    while (won(board)[1] == 0)
    {
        cout << "size " << boards.size() << endl;
        cout << "Round " << index << endl;
        printboard(board, ImgName + to_string(index++) + ".jpg");
        board = GetStep(board, timeLimit, threadCount, root);
    }
    printboard(board, ImgName + to_string(index++) + ".jpg");
    cout << "Winner: " << (won(board)[0] == 1 ? "Black" : "White") << endl;
    return 0;
}