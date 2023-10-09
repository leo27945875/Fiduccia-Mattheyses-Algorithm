#pragma once

#include <iostream>
#include <vector>
#include <climits>
#include "structure.hpp"


class FMAlgo{
public:
    std::vector<Cell*> m_cells;
    std::vector<Net*> m_nets;
    Gains m_gains;
    Groups m_groups;

    int m_nPinMax = 0;
    float m_balanceRatio;

    int m_kthMove, m_MaxCumGain;

    std::vector<Move> m_moveRecords;

    FMAlgo(){};
    FMAlgo(std::string inputFile);

    ~FMAlgo();

    void run(std::string outputFile = "", unsigned int maxLoop = UINT_MAX);

    void outputResult(std::string &outputFile, int kthMove);

    void printNetsInfo();
    void printCellsInfo();
    void printGroupsInfo();
    void printGainsInfo();
    void printLinksInfo();
    void printMovesInfo();
    
    Cell* decideWhichCellToMove();
    void moveCell(Cell* movedCell, bool isReplay = false);

    void findMaxGainKthMove(int &kthMove, int &maxGain);
    void resetGroups();
    void resetGains();
    void replay(int kthMove);
    int getCutSize();

    bool isConstrained(Cell *cell);
    void lockCell(Cell *cell);
    void recordMove(Cell *movedCell);

    void readInputData(std::string inputFile);
    void seperateGroups();
    void buildNPinMaxInfo();
    void buildGainArray();
};