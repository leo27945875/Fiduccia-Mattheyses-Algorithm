#include <iostream>
#include <stdexcept>
#include "include/structure.hpp"
#include "include/utils.hpp"


void Net::printInfo(){
    std::cout << "Counts: (" << m_cellGroupCount[0] << ", " << m_cellGroupCount[1] << ") ";
    std::cout << "Cells: ";
    for (Cell* cell : m_cells){
        std::cout << "c" << cell->m_number << "<" << cell << ">" << "(P<" << cell->m_nPin << ">, " << "G<" << cell->m_group << ">) ";
    }
    std::cout << std::endl;
}

void Net::addCell(Cell *cell){
    m_cells.insert(cell);
}

void Net::getGroupCounts(int from, int to, int &Fn, int &Tn){
    Fn = m_cellGroupCount[from];
    Tn = m_cellGroupCount[to];

    // int fromN = 0, toN = 0;
    // for (Cell *cell : m_cells)
    //     if (cell->m_group == from) 
    //         fromN++;
    //     else
    //         toN++;
    
    // Fn = fromN; 
    // Tn = toN;
}

bool Net::isCut(){
    int group = -1;
    for (Cell *cell : m_cells){
        if (group == -1)
            group = cell->m_group;
        else if (cell->m_group != group)
            return true;
    }
    return false;
}


void Cell::printInfo(){
    std::cout << "Group: " << m_group << ", Gain: " << m_gain << ", Pins: " << m_nPin << ", Lock: " << m_isLocked << ", Nets: ";
    for (Net* net : m_nets){
        std::cout << "n" << net->m_number << "<" << net << "> ";
    }
    std::cout << std::endl;
}

void Cell::addNet(Net *net){
    m_nets.insert(net);
}

void Cell::updateNetsFGroupCount(int n){
    for (Net *net : m_nets)
        net->m_cellGroupCount[m_group] += n;
}

void Cell::updateNetsTGroupCount(int n){
    int g = static_cast<int>(!m_group);
    for (Net *net : m_nets)
        net->m_cellGroupCount[g] += n;
}

void Cell::updateNetsGroupsCount(int fn, int tn){
    updateNetsFGroupCount(fn);
    updateNetsTGroupCount(tn);
}

void Cell::getFromToGroups(int &from, int &to){
    if (m_group == 0){from = 0; to = 1;} 
    else             {from = 1; to = 0;}
}

#if DEBUG == 1
Cell* Groups::operator[](int groupIdx){
    return m_startCells[groupIdx];
}
#endif

void Groups::moveCell(Cell *cell){
    int F, T;
    cell->getFromToGroups(F, T);

#if DEBUG == 1
    if(cell->m_groupNext) cell->m_groupNext->m_groupPrev = cell->m_groupPrev;
    if(cell->m_groupPrev) cell->m_groupPrev->m_groupNext = cell->m_groupNext;
    if(m_startCells[F] == cell) 
        m_startCells[F] = cell->m_groupNext;

    cell->m_groupPrev = nullptr;
    cell->m_groupNext = m_startCells[T];
    if(m_startCells[T]) m_startCells[T]->m_groupPrev = cell;
    m_startCells[T] = cell;
#endif

    cell->m_group = T;
    m_counts[F]--;
    m_counts[T]++;
}


void Groups::clear(){
#if DEBUG == 1
    std::fill(m_startCells, m_startCells + 1, nullptr);
#endif
    std::fill(m_counts, m_counts + 1, 0);
}


Gains::~Gains(){
    delete[] m_cellLists;
}

void Gains::printInfo(){
    int gain;
    for (int i = (2 * m_nPinMax); i > -1; i--){
        gain = i - m_nPinMax;
        if (gain < 0)
            std::cout << "GAIN " << "= "  << i - m_nPinMax << " -> ";
        else
            std::cout << "GAIN " << "=  " << i - m_nPinMax << " -> ";

        Cell *cell = m_cellLists[i];
        while (cell){
            std::cout << "c" << cell->m_number << "<" << cell << ">" << " ";
            cell = cell->m_gainNext;
        }
        END_LINE;
    }
    END_LINE;
}

Cell* Gains::operator[](int gain){
    return m_cellLists[gain + m_nPinMax];
}

void Gains::setNPinMax(int nPinMax){
    m_nPinMax = nPinMax;
    m_cellLists = new Cell*[2 * nPinMax + 1];
    clear();
}

void Gains::addCell(Cell *cell){
    int i = cell->m_gain + m_nPinMax;
    if (m_cellLists[i] != nullptr){
        m_cellLists[i]->m_gainPrev = cell;
        cell->m_gainNext = m_cellLists[i];
        cell->m_gainPrev = nullptr;
    }
    m_cellLists[i] = cell;
    m_nCell++;
}

void Gains::removeCell(Cell *cell){
    if(cell->m_gainNext) cell->m_gainNext->m_gainPrev = cell->m_gainPrev;
    if(cell->m_gainPrev) cell->m_gainPrev->m_gainNext = cell->m_gainNext;
    if((*this)[cell->m_gain] == cell) 
        m_cellLists[cell->m_gain + m_nPinMax] = cell->m_gainNext;

    cell->m_gainNext = nullptr;
    cell->m_gainPrev = nullptr;
    m_nCell--;
}

void Gains::updateCell(Cell *cell, int diffGain){
    removeCell(cell);
    int newGain = (cell->m_gain += diffGain);
#if DEBUG == 1
    if (newGain > m_nPinMax || newGain < -m_nPinMax)
        throw std::runtime_error("[Gains::updateCell] Invalid value of [newGain].");
#endif
    addCell(cell);
}

void Gains::clear(){
    std::fill(m_cellLists, m_cellLists + (2 * m_nPinMax + 1), nullptr);
}

int Gains::length(){
    return 2 * m_nPinMax + 1;
}

bool Gains::isHasCell(){
    return m_nCell > 0;
}