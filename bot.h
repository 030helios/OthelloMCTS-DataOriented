#ifndef __bot_h__
#define __bot_h__
#include "node.h"
#include <array>
#include <chrono>
#include <thread>
#include <vector>
#include <cstdint>
using namespace std;
using namespace chrono;

class Bot
{
    sem_t BotSem;
    vector<Node> tree;
    // Default heat value for root exploration.
    // Depends on clock rate, thinkTime and threadCount, which leaves room for improvement.
    float depth;
    milliseconds timeLimit;
    int threadCount;

    static void countdown(system_clock::time_point start, milliseconds thinkTime, float depth, int threadCount, Bot *bot);

    int newNode(int tier, int id, int8_t col);

    int getNewChild(int tier, int id);
    int select(int tier, int id);

public:
    Bot(int tLimit, int thrCount, array<int8_t, BoardSize> board, int8_t color);

    int explore(int tier, int id, int8_t heat);

    void showWinrate(ostream &out);
    array<int8_t, BoardSize> play();
};

#endif