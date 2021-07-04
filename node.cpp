#include "func.h"
#include "node.h"
using namespace std;

void Node::clean()
{
    fill(totalgames.begin(), totalgames.end(), 0);
    fill(score.begin(), score.end(), 0);
}
int Node::newNode(int id, int8_t col)
{
    sem_wait(&lock);
    if (availible.size())
    {
        int i = *availible.begin();
        color[i] = col;
        RdId[i] = rand() % BoardSize;
        moveIndex[i] = BoardSize - 1;
        gameover[i] = -2;
        board[i] = board[id];
        deque<int>().swap(children[i]);
        sem_post(&lock);
        return i;
    }
    int i = color.size();
    sem.emplace_back();
    sem_init(&sem.back(), 0, 1);
    color.emplace_back(col);
    RdId.emplace_back(rand() % BoardSize);
    moveIndex.emplace_back(BoardSize - 1);
    gameover.emplace_back(-2);
    score.emplace_back(0);
    totalgames.emplace_back(0);
    children.emplace_back();
    board.emplace_back(board[id]);
    sem_post(&lock);
    return i;
}
void Node::delNode(int id)
{
    sem_wait(&lock);
    delNodehelper(id);
    sem_post(&lock);
}
void Node::delNodehelper(int id)
{
    availible.insert(id);
    for (int child : children[id])
        delNodehelper(child);
}

float Node::UCB(int id, int &N, int8_t parentColor)
{
    if (gameover[id] != -2)
        if (gameover[id] != 0)
            return N * parentColor * gameover[id];
        else
            return 0.5 + sqrt(2 * log(N) / (float)totalgames[id]);
    float a = sqrt(2 * log(N) / (float)totalgames[id]);
    a += 0.5 + (float)(score[id]) / (parentColor * 2 * totalgames[id]);
    return a;
}
int8_t Node::getNewChild(int id)
{
    children[id].emplace_back(newNode(id, -color[id]));
    if (newMove(board[children[id].back()], color[id], RdId[id], moveIndex[id]))
        return children[id].back();
    else if (children.size() > 1) // has children
        children[id].pop_back();
    else if (!hasMove(board[children[id].back()], -color[id])) //won
        children[id].pop_back();
    else //pass
        return children[id].back();
    return -1;
}
int8_t Node::select(int id)
{
    float ucbmax = -INFINITY;
    int8_t best = -1;
    for (auto child : children[id])
    {
        sem_wait(&sem[child]);
        if (totalgames[child] == 0)
        {
            sem_post(&sem[child]);
            return child;
        }
        float ucb = UCB(child, totalgames[id], color[id]);
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

int8_t Node::explore(int id)
{
    int8_t child = -1;
    sem_wait(&sem[id]);
    totalgames[id]++;
    if (gameover[id] != -2 && gameover[id] != 0)
    {
        sem_post(&sem[id]);
        return gameover[id];
    }
    if (moveIndex[id] >= 0)
        child = getNewChild(id);
    if (child == -1)
        child = select(id);
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

    int8_t outcome = explore(child);
    sem_wait(&sem[id]);
    score[id] += outcome;
    if (gameover[id] != -2 && gameover[id] != 0)
    {
        outcome = totalgames[id] * gameover[id] - score[id];
        score[id] = gameover[id] * totalgames[id];
    }
    sem_post(&sem[id]);
    return outcome;
}
//return best board
int8_t Node::getbest(int id)
{
    int8_t best = children[id].front();
    for (auto child : children[id])
        if (totalgames[child] > totalgames[best])
            best = child;
    for (auto child : children[id])
        if (gameover[child] == color[id])
            best = child;
    while (children[id].front() != best)
        delNode(children[id].front());
    children.resize(1);
    return best;
}

//returns child that matches the input
int Node::playermove(int id, array<int8_t, BoardSize> &target)
{
    if (target != board[id])
        while (children[id].size())
            if (board[children[id].front()] != target)
                children[id].pop_front();
            else
                break;
    if (board[children[id].front()] == target)
    {
        children[id].resize(1);
        return children[id][0];
    }
    return id;
}