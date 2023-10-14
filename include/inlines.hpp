#include "structure.hpp"


inline void groupLinkCell(Cell *prev, Cell *next){
#if DEBUG == 1
    prev->m_groupNext = next;
    next->m_groupPrev = prev;
#endif
}

inline void groupSetStartCells(Groups &group, Cell *cell0, Cell *cell1){
#if DEBUG == 1
    group.m_startCells[0] = cell0;
    group.m_startCells[1] = cell1;
#endif
}