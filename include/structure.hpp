#pragma once

#include <set>
#include <vector>

class Net;
class Cell;


class Net{
public:
    std::set<Cell*> m_cells;
    int m_number;

    Net() = default;
    void printInfo();
    void addCell(Cell *cell);
    void getGroupCounts(int from, int to, int& Fn, int& Tn);
    bool isCut();
};


class Cell{
public:
    std::set<Net*> m_nets;
    int m_group;
    int m_number;
    int m_nPin = 0;

    int m_gain = 0;
    bool m_isLocked = false;
    Cell *m_groupPrev = nullptr, *m_groupNext = nullptr;
    Cell *m_gainPrev = nullptr, *m_gainNext = nullptr;

    Cell() = default;
    void printInfo();
    void addNet(Net *net);
    void getFromToGroups(int &from, int &to);
};


class Groups{
public:
    Cell *m_startCells[2] = {nullptr, nullptr};
    int m_counts[2] = {0, 0};

    Groups() = default;
    Cell* operator[](int groupIdx);
    void moveCell(Cell *cell);
    void clear();
};


class Gains{
public:
    Cell** m_cellLists;
    int m_nPinMax;
    int m_nCell = 0;

    Gains() = default;
    ~Gains();
    Cell* operator[](int gain);
    void printInfo();
    void setNPinMax(int nPinMax);
    void addCell(Cell *cell);
    void removeCell(Cell *cell);
    void updateCell(Cell *cell, int diffGain);
    void clear();
    int length();
    bool isHasCell();
};


struct Move{
    Cell *cell;
    int gain;
    int cumGain = 0;
    int cutSize;

    Move(Cell *cell, int gain, int cumGain, int cutSize)
        : cell(cell), gain(gain), cumGain(cumGain), cutSize(cutSize)
    {}
};