#ifndef __node_h__
#define __node_h__
#include "config"
#include <set>
#include <array>
#include <deque>
#include <math.h>
#include <random>
#include <vector>
#include <semaphore.h>
using namespace std;

struct Node
{
    sem_t lock;
    set<int> availible;

    vector<sem_t> sem;
    vector<int8_t> color;
    vector<int8_t> RdId;

    vector<int8_t> moveIndex;
    vector<int8_t> gameover;
    vector<int> score;
    vector<int> totalgames;

    vector<deque<int>> children;

    vector<array<int8_t, BoardSize>> board;

    void clean();
    int newNode(int id, int8_t col);
    void delNode(int id);
    void delNodehelper(int id);

    float UCB(int id, int &N, int8_t co);

    int8_t getNewChild(int id);
    int8_t select(int id);

    int8_t explore(int id);

    int8_t getbest(int id);
    int playermove(int id, array<int8_t, BoardSize> &target);
};

#endif