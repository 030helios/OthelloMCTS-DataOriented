#include "func.h"
#include "node.h"
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

Nodes::Nodes() {}

Nodes::Nodes(int computerColor, array<int8_t, BoardSize> bd)
{

    sem_t newSem;
    sem.push_back(newSem);
    sem_init(&sem.back(), 0, 1);
    color.emplace_back(computerColor);
    shuffleID.emplace_back(rand() % BoardSize);
    moveIndex.emplace_back(BoardSize - 1);
    gameover.emplace_back(-2);
    totalScore.emplace_back(0);
    totalGames.emplace_back(0);
    children.emplace_back(deque<int>());
    board.emplace_back(bd);
}

void Nodes::clean()
{
    fill(totalGames.begin(), totalGames.end(), 0);
    fill(totalScore.begin(), totalScore.end(), 0);
}
int Nodes::newNode(int id, int8_t col)
{
    lock_guard<mutex> lock(mtx);
    if (availible.size())
    {
        int i = availible.back();
        availible.pop_back();
        color[i] = col;
        shuffleID[i] = rand() % BoardSize;
        moveIndex[i] = BoardSize - 1;
        gameover[i] = -2;
        totalScore[i] = 0;
        totalGames[i] = 0;
        children[i].clear();
        board[i] = board[id];
        return i;
    }
    int i = color.size();
    sem_t newSem;
    sem.push_back(newSem);
    sem_init(&sem.back(), 0, 1);
    color.emplace_back(col);
    shuffleID.emplace_back(rand() % BoardSize);
    moveIndex.emplace_back(BoardSize - 1);
    gameover.emplace_back(-2);
    totalScore.emplace_back(0);
    totalGames.emplace_back(0);
    deque<int> newchildren;
    children.push_back(newchildren);
    board.emplace_back(board[id]);
    return i;
}
void Nodes::delNode(int id)
{
    lock_guard<mutex> lock(mtx);
    delNodehelper(id);
}
void Nodes::delNodehelper(int id)
{
    availible.push_back(id);
    push_heap(availible.begin(), availible.end());
    for (int child : children[id])
        delNodehelper(child);
}

float Nodes::UCB(int id, int N, int8_t parentColor)
{
    if (gameover[id] != -2)
        if (gameover[id] != 0)
            return N * parentColor * gameover[id];
        else
            return 0.5 + sqrt(2 * log(N) / (float)totalGames[id]);
    float a = sqrt(2 * log(N) / (float)totalGames[id]);
    a += 0.5 + (float)(totalScore[id]) / (parentColor * 2 * totalGames[id]);
    return a;
}
int Nodes::getNewChild(int id)
{
    int childID = newNode(id, -color[id]);
    children[id].emplace_back(childID);
    if (newMove(board[childID], color[id], shuffleID[id], moveIndex[id]))
        return childID;
    else if (children.size() > 1) // has children
    {
        delNode(childID);
        children[id].pop_back();
    }
    else if (!hasMove(board[childID], -color[id])) //won
    {
        delNode(childID);
        children[id].pop_back();
    }
    else //pass
        return childID;
    return -1;
}
int Nodes::select(int id)
{
    float ucbmax = -INFINITY;
    int best = -1;
    for (auto child : children[id])
    {
        sem_wait(&sem[child]);
        if (totalGames[child] == 0)
        {
            sem_post(&sem[child]);
            return child;
        }
        float ucb = UCB(child, totalGames[id], color[id]);
        if (ucb > ucbmax)
        {
            ucbmax = ucb;
            best = child;
            gameover[id] = gameover[child];
        }
        sem_post(&sem[child]);
    }
    return best;
}

int Nodes::explore(int id, int8_t heat)
{
    int child = -1;
    sem_wait(&sem[id]);
    totalGames[id]++;
    if (gameover[id] != -2 && gameover[id] != 0)
    {
        sem_post(&sem[id]);
        return gameover[id];
    }
    if (heat == 0)
    {
        totalScore[id] = playout(board[id], color[id]);
        sem_post(&sem[id]);
        return totalScore[id];
    }
    if (moveIndex[id] >= 0)
        child = getNewChild(id);
    if (child == -1)
        child = select(id);
    else
        heat--;
    if (child == -1)
    { //won
        gameover[id] = 0;
        for (auto stone : board[id])
            gameover[id] += stone;
        gameover[id] = (gameover[id] > 0) - (gameover[id] < 0);
        sem_post(&sem[id]);
        return gameover[id];
    }
    sem_post(&sem[id]);

    int outcome = explore(child, heat);
    sem_wait(&sem[id]);
    totalScore[id] += outcome;
    if (gameover[id] != -2 && gameover[id] != 0)
    {
        outcome = totalGames[id] * gameover[id] - totalScore[id];
        totalScore[id] = gameover[id] * totalGames[id];
    }
    sem_post(&sem[id]);
    return outcome;
}
//return best board
int Nodes::getbest(int id)
{
    int best = children[id].front();
    for (auto child : children[id])
        if (totalGames[child] > totalGames[best])
            best = child;
    for (auto child : children[id])
        if (gameover[child] == color[id])
            best = child;
    for (auto child : children[id])
        if (child != best)
            delNode(child);
    deque<int>().swap(children[id]);
    return best;
}

//returns child that matches the input
int Nodes::playermove(int id, array<int8_t, BoardSize> &target)
{
    if (target != board[id])
        while (children[id].size())
            if (board[children[id].front()] != target)
            {
                delNode(children[id].front());
                children[id].pop_front();
            }
            else
                break;
    if (children[id].size())
        if (board[children[id].front()] == target)
        {
            children[id].resize(1);
            return children[id][0];
        }
    return id;
}