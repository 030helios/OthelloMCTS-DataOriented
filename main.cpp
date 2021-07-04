#include "config"
#include "func.h"
#include "node.h"
#include <time.h>
#include <thread>
#include <iostream>
#include <algorithm>
using namespace std;

array<array<pair<int8_t, int8_t>, BoardSize>, BoardSize> RdMoves;
//continue exploring until time up
void Countdown(time_t timeLimit, Node *root, int id)
{
    while (time(0) < timeLimit)
        root->explore(id);
}

array<int8_t, BoardSize> GetStep(array<int8_t, BoardSize> target, int &thinkTime, int threadCount, Node *root, int &id)
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

    cout << "Total playouts: " << root->totalgames[id] << endl;
    if (root->gameover[id] == -2 || root->gameover[id] == 0)
    {
        if (root->gameover[id] == 0)
            cout << "Maybe Draw\n";
        cout << (root->color[id] == 1 ? "Black " : "White ");
        id = root->getbest(id);
        float childUtility = float(root->score[id] * root->color[id]) / (2 * root->totalgames[id]);
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

    Node root;
    sem_init(&root.lock, 0, 1);
    sem_init(&root.sem.front(), 0, 1);
    root.sem.emplace_back();
    root.color.emplace_back(computerColor);
    root.RdId.emplace_back(rand() % BoardSize);
    root.moveIndex.emplace_back(BoardSize - 1);
    root.gameover.emplace_back(-2);
    root.score.emplace_back(0);
    root.totalgames.emplace_back(0);
    root.children.emplace_back();
    root.board.emplace_back(board);

    string ImgName = "Output_";
    int index = 0;
    int id = 0;
    while (won(board)[1] == 0)
    {
        cout << "Round " << index << endl;
        printboard(board, ImgName + to_string(index++) + ".jpg");
        board = GetStep(board, timeLimit, threadCount, &root, id);
    }
    printboard(board, ImgName + to_string(index++) + ".jpg");
    cout << "Winner: " << (won(board)[0] == 1 ? "Black" : "White") << endl;
    return 0;
}