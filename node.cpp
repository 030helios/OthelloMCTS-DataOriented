#include "func.h"
#include "node.h"
using namespace std;

Node::Node() {}
Node::Node(array<int8_t, BoardSize> *bdPtr, int8_t co) : board(bdPtr), col(co), RdId(rand() % BoardSize) {}
float Node::UCB(int &N, int8_t parentColor)
{
    if (gameover != -2)
    {
        if (gameover == 0)
            if (!totalgames)
                return 512;
            else
                return 0.5 + sqrt(2 * log(N) / (float)totalgames);
        return 1024 * parentColor * gameover;
    }
    if (!totalgames)
        return 512;
    float a = sqrt(2 * log(N) / (float)totalgames);
    a += parentColor * ((float)(totalscore) / (totalgames * 2)) + 0.5;
    return a;
}
//marks the pointers as availible
void Node::erase()
{
    delBoard(board);
    for (auto &child : children)
        child.erase();
}
//removes data without removing the structure
void Node::clean()
{
    totalscore = 0;
    totalgames = 0;
    if (moveIndex < 0)
        delBoard(board);
    for (auto &child : children)
        child.clean();
}
int8_t Node::explore()
{
    Node *child;
    {
        lock_guard<mutex> lock(mtx);
        if (gameover != -2)
            return gameover;
        totalgames++;
        if (moveIndex >= 0)
        {
            children.emplace_back(makeNewBoard(board), -col);
            child = &children.back();
            if (!newMove(*child->board, col, RdId, moveIndex))
                if (children.size() > 1)
                { // has children
                    children.pop_back();
                    child = select();
                }
                else if (!hasMove(*board, -col))
                { //won
                    gameover = 0;
                    for (auto stone : *board)
                        gameover += stone;
                    gameover = (gameover > 0) - (gameover < 0);
                    return gameover;
                }
        }
        else
            child = select();
    }
    int8_t outcome = child->explore();
    lock_guard<mutex> lock(mtx);
    totalscore += outcome;
    return outcome;
}
//return greatest UCB child number
Node *Node::select()
{
    float ucbmax = -INFINITY;
    Node *best = nullptr;
    for (auto &child : children)
    {
        lock_guard<mutex> lock(child.mtx);
        float ucb = child.UCB(totalgames, col);
        if (ucb > ucbmax)
        {
            ucbmax = ucb;
            best = &child;
            gameover = best->gameover;
        }
        else if (ucb == ucbmax)
            if (rand() % 2)
            {
                ucbmax = ucb;
                best = &child;
            }
    }
    return best;
}
//return best board
Node *Node::getbest()
{
    int imax = 0;
    for (int i = 0; i < children.size(); i++)
    {
        if (children[i].gameover == col)
        {
            imax = i;
            break;
        }
        if (children[i].totalgames > children[imax].totalgames)
            imax = i;
    }
    for (int i = 0; i < imax; i++)
    {
        children.front().erase();
        children.pop_front();
    }
    while (children.size() != 1)
    {
        children.back().erase();
        children.pop_back();
    }
    return &children.front();
}