#ifndef __node_h__
#define __node_h__
#include "config"
#include <set>
#include <array>
#include <deque>
#include <mutex>
#include <math.h>
#include <random>
#include <vector>
#include <semaphore.h>
using namespace std;

struct Nodes
{
    mutex mtx;
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

    Nodes();
    Nodes(int computerColor, array<int8_t, BoardSize> bd);

    void clean();
    int newNode(int id, int8_t col);
    void delNode(int id);
    void delNodehelper(int id);

    float UCB(int id, int N, int8_t co);

    int getNewChild(int id);
    int select(int id);

    int explore(int id);

    int getbest(int id);
    int playermove(int id, array<int8_t, BoardSize> &target);
};

#endif