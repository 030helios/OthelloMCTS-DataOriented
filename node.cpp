#include "func.h"
#include "node.h"
#include <math.h>
using namespace std;

Nodes::Nodes(int size)
{
    sem_init(&mtx, 0, 1);
    sem.reserve(size);
    color.reserve(size);
    shuffleID.reserve(size);
    moveIndex.reserve(size);
    totalScore.reserve(size);
    totalGames.reserve(size);
    children.reserve(size);
    board.reserve(size);
}
int Nodes::push_back(int8_t computerColor, array<int8_t, BoardSize> bd)
{
    sem_wait(&mtx);
    int ret = sem.size();

    sem.emplace_back();
    color.emplace_back(computerColor);
    shuffleID.emplace_back(rand() % moveSize);
    moveIndex.emplace_back(moveSize - 1);
    totalScore.emplace_back(0);
    totalGames.emplace_back(0);
    children.emplace_back();
    board.emplace_back(bd);

    sem_init(&sem.back(), 0, 1);
    sem_post(&mtx);
    return ret;
}
float Nodes::UCB(int id, int parentTotal, int8_t parentColor)
{
    sem_wait(&sem[id]);
    float a = sqrt(2 * log(parentTotal) / (float)totalGames[id]);
    a += 0.5 + (float)(totalScore[id]) / (parentColor * 2 * totalGames[id]);
    sem_post(&sem[id]);
    return a;
}