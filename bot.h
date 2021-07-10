#ifndef __bot_h__
#define __bot_h__
#include "node.h"
#include <chrono>
#include <vector>
#include <cstdint>
using namespace std;
using namespace chrono;

struct Bot
{
    sem_t BotSem;
    vector<Nodes> tree;
    float depth;
    int threadCount;
    milliseconds timeLimit;

    Bot(int tLimit, int thrCount, array<int8_t, BoardSize> board, int8_t color);

    static void countdown(system_clock::time_point start, milliseconds thinkTime, float depth, int threadCount, Bot *bot);
    void showWinrate(ostream &out);
    array<int8_t, BoardSize> play();

    int newNode(int tier, int id, int8_t col);

    int getNewChild(int tier, int id);
    int select(int tier, int id);
    int explore(int tier, int id, int8_t heat);

};

#endif