import random


N_CELL = 4000
N_NET  = 5000

N_MAX_CELL_IN_NET = 10

R = 0.5

SAVE_FILENAME = "data/input4.in"


if __name__ == '__main__':

    cellCounts = [0] * N_CELL
    checkIter  = N_MAX_CELL_IN_NET // 2

    out, lastICellCKPT, iPin = f"{R}\n", 0, 0
    for iNet in range(1, N_NET + 1):
        out += f"NET n{iNet} "
        nCellInNet = random.randint(2, N_MAX_CELL_IN_NET)
        iCellList = [None] * nCellInNet
        for i in range(nCellInNet):
            iPin += 1
            if iPin % checkIter == 0:
                try:
                    iCell = cellCounts.index(0, lastICellCKPT) + 1
                    lastICellCKPT = iCell
                except ValueError:
                    iCell = random.randint(1, N_CELL)
            else:
                iCell = random.randint(1, N_CELL)

            cellCounts[iCell - 1] += 1
            iCellList[i] = iCell

        for iCell in iCellList:
            out += f"c{iCell} "
        
        out += ";\n"
    
    with open(SAVE_FILENAME, "w") as f:
        f.write(out)