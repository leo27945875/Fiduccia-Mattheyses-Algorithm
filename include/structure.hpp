#pragma once

#include <unordered_set>
#include <vector>
#include "utils.hpp"

class Net;
class Cell;


class Net{
public:
    std::unordered_set<Cell*> m_cells;
    int                       m_number;
    int                       m_cellGroupCount[2]   = {0, 0};
    Cell*                     m_cellUnlockGroups[2] = {nullptr, nullptr};

    static int s_cutSize;

    Net() = default;
    void printInfo();
    void addCell(Cell *cell);
    void updateCut();
    void updateMove(int from, int to);
    void updateGroupCount(int group, int n);
    void getGroupCounts(int from, int to, int& Fn, int& Tn);
    bool isCut();

private:
    bool m_lastIsCut_ = false;
};


class Cell{
public:
    std::unordered_set<Net*> m_nets;
    int                      m_group;
    int                      m_number;
    int                      m_nPin      = 0;
    int                      m_gain      = 0;
    bool                     m_isLocked  = false;

    Cell*                    m_gainPrev = nullptr;
    Cell*                    m_gainNext = nullptr;

#if DEBUG == 1
    Cell *m_groupPrev = nullptr, *m_groupNext = nullptr;
#endif

    Cell() = default;
    void printInfo();
    void addNet(Net *net);
    void updateNetsFGroupCount(int n);
    void updateNetsTGroupCount(int n);
    void updateNetsGroupsCount();
    void getFromToGroups(int &from, int &to);
};


class Groups{
public:
    int m_counts[2] = {0, 0};

#if DEBUG == 1
    Cell* m_startCells[2] = {nullptr, nullptr};
#endif

    Groups() = default;
    Cell* operator[](int groupIdx);
    void  moveCell(Cell *cell);
    void  clear();
};


class Gains{
public:
    Cell** m_cellLists;
    int    m_nPinMax;
    int    m_nCell = 0;

    Gains() = default;
    ~Gains();
    Cell* operator[](int gain);
    void  printInfo();
    void  setNPinMax(int nPinMax);
    void  addCell(Cell *cell);
    void  removeCell(Cell *cell);
    void  updateCell(Cell *cell, int diffGain);
    void  clear();
    int   length();
    bool  isHasCell();
};


struct Move{
    Cell* cell;
    int   gain;
    int   cumGain = 0;
    int   cutSize;

    Move(Cell *cell, int gain, int cumGain, int cutSize)
        : cell(cell), gain(gain), cumGain(cumGain), cutSize(cutSize)
    {}
};