#ifndef __node_h__
#define __node_h__
#include "config"
#include <mutex>
#include <deque>
#include <math.h>
#include <utility>
#include <algorithm>
using namespace std;

struct Node
{
    mutex mtx;
    int8_t col;
    int8_t RdId;

    int8_t moveIndex = BoardSize - 1;
    int8_t gameover = -2;
    int totalscore = 0;
    int totalgames = 0;
    array<int8_t, BoardSize> *board;
    deque<Node> children;

    Node();
    Node(array<int8_t, BoardSize> *bdPtr, int8_t col);

    float UCB(int &N, int8_t co);

    void clean();
    void erase();
    int8_t explore();
    Node *select();
    Node *getbest();
};
#endif