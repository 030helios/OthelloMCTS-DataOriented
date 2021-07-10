#ifndef __node_h__
#define __node_h__
#include "config"
#include <array>
#include <vector>
#include <semaphore.h>
using namespace std;

struct Nodes
{
    sem_t mtx;

    vector<sem_t> sem;
    vector<int8_t> color;
    vector<int8_t> shuffleID;
    vector<int8_t> moveIndex;
    vector<int> totalScore;
    vector<int> totalGames;
    vector<vector<int>> children;
    vector<array<int8_t, BoardSize>> board;

    Nodes(int size);
    //returns last index
    int push_back(int8_t computerColor, array<int8_t, BoardSize> bd);
    float UCB(int id, int parentTotal, int8_t parentColor);
};

#endif