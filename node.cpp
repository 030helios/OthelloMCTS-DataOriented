#include "func.h"
#include "node.h"
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

Node::Node()
{
    sem_init(&mtx, 0, 1);
}

Node::Node(int size)
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
Node::Node(int8_t computerColor, array<int8_t, BoardSize> bd, int size)
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

    sem.push_back(sem_t());
    sem_init(&sem.back(), 0, 1);
    color.emplace_back(computerColor);
    shuffleID.emplace_back(rand() % BoardSize);
    moveIndex.emplace_back(BoardSize - 1);
    totalScore.emplace_back(0);
    totalGames.emplace_back(0);
    children.push_back(vector<int>());
    board.emplace_back(bd);
}
float Node::UCB(int id, int parentTotal, int8_t parentColor)
{
    float a = sqrt(2 * log(parentTotal) / (float)totalGames[id]);
    a += 0.5 + (float)(totalScore[id]) / (parentColor * 2 * totalGames[id]);
    return a;
}