#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <unordered_set>

#include "include/structure.hpp"
#include "include/partition.hpp"
#include "include/inlines.hpp"
#include "include/utils.hpp"


FMAlgo::FMAlgo(std::string inputFile){
    readInputData(inputFile);
    seperateGroups();
    buildNPinMaxInfo();
    buildGainArray();

#if DEBUG == 1
    END_LINE;
    LOG("===================== FM Algo Init Info =====================\n");
    printNetsInfo();
    printCellsInfo();
    printGroupsInfo();
    printGainsInfo();
    printLinksInfo();
    LOGKV("Init cutsize", getCutSize());
    END_LINE;
    DDASH_LINE;
    END_LINE;
#endif
}

FMAlgo::~FMAlgo(){
    for (Cell *cell : m_cells) delete cell;
    for (Net  *net  : m_nets ) delete net ;
}

void FMAlgo::run(std::string outputFile, unsigned int maxLoop){
    std::cout << "\nStart running ... " << std::endl;
    unsigned int innerLoop, outerLoop = 1;
    while (outerLoop <= maxLoop){
        OUTER_LOOP_START(outerLoop);
        innerLoop = 1;
        while (m_gains.isHasCell()){
            INNER_LOOP_START(innerLoop);
            
            Cell *cell = decideWhichCellToMove();
            if (cell == nullptr) // No free cell satisfies the balance constraint.
                break;

            MOVE_INFO(cell);
            moveCell(cell);
            innerLoop++;

#if DEBUG == 1
            END_LINE;
            printNetsInfo();
            printCellsInfo();
            printGroupsInfo();
            printGainsInfo();
            printLinksInfo();
            END_LINE;
#endif
        }

#if DEBUG == 1
        DDASH_LINE;
        printMovesInfo();
#endif
        Cell* maxMovedCell;
        findMaxGainKthMove(m_kthMove, m_MaxCumGain, maxMovedCell);
        replay(m_kthMove);
        buildGainArray();
        FIXLOGKV("Max Move", m_kthMove);
        FIXLOGKV("Max CumGain", m_MaxCumGain);
        FIXLOGKV("Max Moved Cell", maxMovedCell->m_number);
        FIXLOGKV("Moved Cell Group", maxMovedCell->m_group); END_LINE;

#if DEBUG == 1
        printNetsInfo();
        printCellsInfo();
        printGroupsInfo();
        printGainsInfo();
        printLinksInfo();
#endif
        m_moveRecords.clear();
        if (m_MaxCumGain <= 0) break;
        else
            outerLoop++;
    }

    FIXLOGKV("\nStop, total loop", outerLoop);
    if (outputFile.size() > 0)
        outputResult(outputFile, m_kthMove);
}

void FMAlgo::readInputData(std::string inputFile){
    // Purely build cell & net vector from file:
    std::ifstream fp(inputFile);
    std::string   line, word;

    getline(fp, line);
    m_balanceRatio = std::stof(line);

    int maxCellNumber = 0, maxNetNumber = 0;
    while (getline(fp, line, ';')){
        std::stringstream       ss(line);
        std::unordered_set<int> lastCellNumbers, lastNetNumbers;
        Cell*                   nowCell = nullptr;
        Net*                    nowNet = nullptr;
        while (ss >> word)
        {
            if (word == "NET") continue;;
            if (word[0] == 'n'){
                int netNumber = std::stoi(word.substr(1));
                if (lastNetNumbers.count(netNumber))
                    break;

                if (netNumber > maxNetNumber){
                    maxNetNumber = netNumber;
                    m_nets.resize(maxNetNumber);
                }
                if (m_nets[netNumber - 1] == nullptr)
                    m_nets[netNumber - 1] = new Net();

                nowNet = m_nets[netNumber - 1];
                nowNet->m_number = netNumber;
                lastNetNumbers.insert(netNumber);
            }
            else{
                int cellNumber = std::stoi(word.substr(1));
                if (lastCellNumbers.count(cellNumber))
                    continue;

                if (cellNumber > maxCellNumber){
                    maxCellNumber = cellNumber;
                    m_cells.resize(maxCellNumber);
                }
                if (m_cells[cellNumber - 1] == nullptr)
                    m_cells[cellNumber - 1] = new Cell();
                    
                nowCell = m_cells[cellNumber - 1];
                nowNet->addCell(nowCell);
                nowCell->addNet(nowNet);
                nowCell->m_number = cellNumber;
                nowCell->m_nPin++;
                lastCellNumbers.insert(cellNumber);
            }
        }
    }
    fp.close();

    m_moveRecords.reserve(m_cells.size());

    FIXLOG("\nFinished data reading.");
}

void FMAlgo::outputResult(std::string &outputFile, int kthMove){
    std::ofstream     fp(outputFile);
    std::stringstream cellss[2];

    fp << "Cutsize = " << m_moveRecords[kthMove - 1].cutSize << std::endl;

    for (Cell *cell : m_cells)
        cellss[cell->m_group] << "c" << cell->m_number << " ";

    for (int i = 0; i < 2; i++){
        cellss[i] << ";";
        fp << "G" << (i + 1) << " " << m_groups.m_counts[i]  << std::endl;
        fp << cellss[i].str() << std::endl;
    }

    fp.close();
}

#if DEBUG == 1
void FMAlgo::printNetsInfo(){
    std::cout << "Balance Ratio = " << m_balanceRatio << std::endl;
    for (int i = 0; i < m_nets.size(); i++){
        std::cout << "NET" << m_nets[i]->m_number << "  ";
        m_nets[i]->printInfo();
    }
    END_LINE;
}

void FMAlgo::printCellsInfo(){
    std::cout << "Max Pin Number: " << m_nPinMax << std::endl;
    for (int i = 0; i < m_cells.size(); i++){
        std::cout << "CELL" << m_cells[i]->m_number << "  ";
        m_cells[i]->printInfo();
    }
    END_LINE;
}

void FMAlgo::printGroupsInfo(){
    Cell *p = m_groups[0];
    std::cout << "GROUP1  ";
    while (p){
        std::cout << "c" << p->m_number << " ";
        p = p->m_groupNext;
    }
    END_LINE;

    p = m_groups[1];
    std::cout << "GROUP2  ";
    while (p){
        std::cout << "c" << p->m_number << " ";
        p = p->m_groupNext;
    }
    END_LINE;
    END_LINE;
}

void FMAlgo::printGainsInfo(){
    m_gains.printInfo();
}

void FMAlgo::printLinksInfo(){
    LOG("Group Links:");
    for (Cell *cell : m_cells){
        int prev = (cell->m_groupPrev? cell->m_groupPrev->m_number: 0);
        int next = (cell->m_groupNext? cell->m_groupNext->m_number: 0);
        std::cout << "c" << prev << " <- " << "c" << cell->m_number << " -> " << "c" << next << std::endl;
    }
    LOG("Gain Links:");
    for (Cell *cell : m_cells){
        int prev = (cell->m_gainPrev? cell->m_gainPrev->m_number: 0);
        int next = (cell->m_gainNext? cell->m_gainNext->m_number: 0);
        std::cout << "c" << prev << " <- " << "c" << cell->m_number << " -> " << "c" << next;
        if (cell->m_isLocked)
            std::cout << " (LOCK)";
        
        END_LINE;
    }
    END_LINE;
}

void FMAlgo::printMovesInfo(){
    DASH_LINE;
    LOG("| Move | Cell | Gain | CumGain | CutSize |");
    int i = 1;
    for (const Move &move : m_moveRecords){
        printf("%5d  %5d  %5d   %5d     %5d  \n", i, move.cell->m_number, move.gain, move.cumGain, move.cutSize);
        i++;
    }
    DASH_LINE;
    END_LINE;
}
#endif

void FMAlgo::seperateGroups(){

    resetGroups();

    int nCell = m_cells.size();
    int halfNCell = nCell / 2;

    groupSetStartCells(m_groups, m_cells[0], m_cells[halfNCell]);

    m_cells[0]->m_group = 0;
    m_cells[0]->updateNetsFGroupCount(1);

    m_cells[halfNCell]->m_group = 1;
    m_cells[halfNCell]->updateNetsFGroupCount(1);

    m_groups.m_counts[0] = halfNCell;
    m_groups.m_counts[1] = nCell - halfNCell;

    for (int i = 1; i < nCell; i++){
        if (i == halfNCell) continue;
        m_cells[i]->m_group = (i < halfNCell)? 0: 1;
        m_cells[i]->updateNetsFGroupCount(1);
        groupLinkCell(m_cells[i - 1], m_cells[i]);
    }

    FIXLOG("\nFinished seperating group."); 
}

void FMAlgo::buildNPinMaxInfo(){
    for (Cell *cell : m_cells){
        if (cell->m_nPin > m_nPinMax)
            m_nPinMax = cell->m_nPin;
    }
    m_gains.setNPinMax(m_nPinMax);
    FIXLOG("\nFinished setting nPinMax info."); 
}

void FMAlgo::buildGainArray(){

    resetGains();

    for (Cell *cell : m_cells){
        int F, T;
        cell->getFromToGroups(F, T);
        for (Net* net : cell->m_nets){
            int Fn, Tn;
            net->getGroupCounts(F, T, Fn, Tn);
            if      (Fn == 1) cell->m_gain++;
            else if (Tn == 0) cell->m_gain--;
        }
        m_gains.addCell(cell);
    }
    FIXLOG("\nFinished building gain bucket list."); 
}

bool FMAlgo::isConstrained(Cell *cell){
    int   group = cell->m_group;
    float n0 = static_cast<float>(m_groups.m_counts[0] + (group == 0? -1: 1));
    float lower = 0.5f * (1.f - m_balanceRatio) * static_cast<float>(m_groups.m_counts[0] + m_groups.m_counts[1]);
    float upper = 0.5f * (1.f + m_balanceRatio) * static_cast<float>(m_groups.m_counts[0] + m_groups.m_counts[1]);
    return (n0 >= lower) && (n0 <= upper);
}

Cell* FMAlgo::decideWhichCellToMove(){
    for (int i = m_gains.length() - 1; i > -1; i--){
        Cell *cell = m_gains.m_cellLists[i];
        while (cell != nullptr){
            if (isConstrained(cell) && !cell->m_isLocked)
                return cell;
            cell = cell->m_gainNext;
        }
    }
    return nullptr;
}

void FMAlgo::lockCell(Cell *cell){
    cell->m_isLocked = true;
    m_gains.removeCell(cell);
}

void FMAlgo::recordMove(Cell* movedCell){
    m_moveRecords.emplace_back(
        movedCell,
        movedCell->m_gain,
        (m_moveRecords.size() == 0? 0: m_moveRecords.back().cumGain) + movedCell->m_gain,
        getCutSize()
    );
}

void FMAlgo::moveCell(Cell* movedCell, bool isReplay){
    // Move the cell:
    movedCell->updateNetsGroupsCount();
    m_groups.moveCell(movedCell);
    if (isReplay) 
        return;

    // Lock moved cell:
    lockCell(movedCell);
    
    // Record the move:
    recordMove(movedCell);

    // Update gains:
    int F, T;
    movedCell->getFromToGroups(T, F); // Already moved cell, so flip the F & T.

    for (Net* net : movedCell->m_nets){
        int Fn, Tn;
        net->getGroupCounts(F, T, Fn, Tn);
        Fn++; Tn--;  // Already moved cell, so Fn & Tn are the number after moving the cell.
        
        if (Tn == 0){
            for (Cell* cell : net->m_cells)
                if (!cell->m_isLocked)
                    m_gains.updateCell(cell, 1);
        } 
        else if (Tn == 1){
            for (Cell* cell : net->m_cells)
                if (cell->m_group == T && !cell->m_isLocked){
                    m_gains.updateCell(cell, -1);
                    break; // There is only one cell in the T-group.
                }
        }

        Fn--; Tn++;

        if (Fn == 0){
            for (Cell* cell : net->m_cells)
                if (!cell->m_isLocked)
                    m_gains.updateCell(cell, -1);
        }
        else if (Fn == 1){
            for (Cell* cell : net->m_cells)
                if (cell->m_group == F && !cell->m_isLocked){
                    m_gains.updateCell(cell, 1);
                    break; // There is only one cell in the F-group.
                }
        }
    }
}

void FMAlgo::findMaxGainKthMove(int &kthMove, int &maxGain, Cell* &movedCell){
    Cell* movedCell_;
    int   kthMove_, maxGain_ = -1;
    int   i = 1;
    for (const Move &move : m_moveRecords){
        if (move.cumGain > maxGain_){
            maxGain_   = move.cumGain;
            kthMove_   = i;
            movedCell_ = move.cell;
        }
        i++;
    }
    kthMove = kthMove_; maxGain = maxGain_; movedCell = movedCell_;
}

void FMAlgo::resetGroups(){
#if DEBUG == 1
    for (Cell *cell : m_cells)
        cell->m_groupPrev = cell->m_groupNext = nullptr;
#endif
    m_groups.clear();
}

void FMAlgo::resetGains(){
    for (Cell *cell : m_cells){
        cell->m_gain = 0;
        cell->m_isLocked = false;
        cell->m_gainPrev = cell->m_gainNext = nullptr;
    }
    m_gains.clear();
}

void FMAlgo::replay(int kthMove){
    for(int i = m_moveRecords.size() - 1; i > kthMove - 1; i--){
        moveCell(m_moveRecords[i].cell, true);
    }
}

int FMAlgo::getCutSize(){
    return Net::s_cutSize;
}