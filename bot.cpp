#include "bot.h"
#include "func.h"
#include <math.h>
#include <iostream>
#include <algorithm>

#define child(a) tree[next].a[childID]
#define self(a) tree[tier].a[id]

Bot::Bot(int tLimit, int thrCount, array<int8_t, BoardSize> board, int8_t color) : timeLimit(seconds(tLimit)), threadCount(thrCount)
{
    sem_init(&BotSem, 0, 1);
    depth = log(timeLimit.count() * thrCount) / log(EdgeSize) + 0.5;
    tree.reserve(BoardSize);
    tree.emplace_back(color, board, 1);
}
//continue exploring until time up
void Bot::countdown(system_clock::time_point start, milliseconds thinkTime, float depth, int threadCount, Bot *bot)
{
    milliseconds timepast;
    do
    {
        timepast = duration_cast<milliseconds>(system_clock::now() - start);
        bot->explore(0, 0, round(depth - log(timepast.count() * threadCount) / log(EdgeSize)));
    } while (timepast < thinkTime);
}
void Bot::showWinrate(ostream &out)
{
    out << "Total playouts: " << tree[0].totalGames[0] << endl;
    out << (tree[0].color[0] == 1 ? "Black " : "White ");
    float winrate = 0.5 + float(tree[0].totalScore[0] * tree[0].color[0]) / (2 * tree[0].totalGames[0]);
    out << "winrate estimate: " << winrate << endl;
}
array<int8_t, BoardSize> Bot::play()
{
    system_clock::time_point start = system_clock::now();
    vector<thread> jobs;
    for (int i = 0; i < threadCount; i++)
        jobs.emplace_back(countdown, start, timeLimit, depth, threadCount, this);
    for (int i = 0; i < threadCount; i++)
        jobs[i].join();

    showWinrate(cout);
    int best = tree[0].children[0].front();
    for (auto childID : tree[0].children[0])
        if (tree[1].totalGames[childID] > tree[1].totalGames[best])
            best = childID;
    return tree[1].board[best];
}
//copies node at [tier][id] to next tier
int Bot::newNode(int tier, int id, int8_t col)
{
    int next = tier + 1;
    sem_wait(&BotSem);
    if (tree.size() == next)
    {
        if (log(MaxSize) / log(BoardSize / 2) < next)
            tree.emplace_back(MaxSize);
        else
            tree.emplace_back((int)pow((BoardSize / 2), next));
    }
    sem_post(&BotSem);

    sem_wait(&tree[next].mtx);

    int childID = tree[next].color.size();
    tree[next].sem.push_back(sem_t());
    sem_init(&tree[next].sem.back(), 0, 1);
    tree[next].color.emplace_back(col);
    tree[next].shuffleID.emplace_back(rand() % BoardSize);
    tree[next].moveIndex.emplace_back(BoardSize - 1);
    tree[next].totalScore.emplace_back(0);
    tree[next].totalGames.emplace_back(0);
    tree[next].children.emplace_back(vector<int>());
    tree[next].board.emplace_back(self(board));

    sem_post(&tree[next].mtx);
    return childID;
}
int Bot::getNewChild(int tier, int id)
{
    int next = tier + 1;
    int childID = newNode(tier, id, -self(color));
    self(children).emplace_back(childID);
    if (newMove(child(board), self(color), self(shuffleID), self(moveIndex)))
        return childID;
    else if (self(children).size() > 1) // has children
        self(children).pop_back();
    else if (!hasMove(child(board), -self(color))) //won
        self(children).pop_back();
    else //pass
        return childID;
    return -1;
}
int Bot::select(int tier, int id)
{
    int next = tier + 1;
    float ucbmax = -INFINITY;
    int best = -1;
    for (auto childID : self(children))
    {
        sem_wait(&child(sem));
        float ucb = tree[next].UCB(childID, self(totalGames), self(color));
        if (ucb > ucbmax)
        {
            ucbmax = ucb;
            best = childID;
        }
        sem_post(&child(sem));
    }
    return best;
}

int Bot::explore(int tier, int id, int8_t heat)
{
    int childID = -1;
    sem_wait(&self(sem));
    self(totalGames)++;
    if (heat == 0)
    {
        int outcome = playout(self(board), self(color));
        self(totalScore) += outcome;
        sem_post(&self(sem));
        return outcome;
    }
    if (self(moveIndex) >= 0)
        childID = getNewChild(tier, id);
    if (childID == -1)
        childID = select(tier, id);
    else
        heat--;
    if (childID == -1) //won
    {
        int outcome = score(self(board));
        self(totalScore) += outcome;
        sem_post(&self(sem));
        return outcome;
    }
    sem_post(&self(sem));
    int outcome = explore(tier + 1, childID, heat);

    sem_wait(&self(sem));
    self(totalScore) += outcome;
    sem_post(&self(sem));
    return outcome;
}