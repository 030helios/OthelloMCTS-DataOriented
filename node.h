#ifndef __node_h__
#define __node_h__
#include "config"
#include <array>
#include <mutex>
#include <math.h>
#include <random>
#include <vector>
#include <semaphore.h>
using namespace std;

struct Node
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

    Node();
    Node(int size);
    Node(int8_t computerColor, array<int8_t, BoardSize> bd, int size);

    float UCB(int id, int parentTotal, int8_t parentColor);
};

#endif