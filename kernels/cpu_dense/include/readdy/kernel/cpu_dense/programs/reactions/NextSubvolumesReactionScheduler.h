/**
 * << detailed description >>
 *
 * @file SingleCPUNextSubvolumesReactionScheduler.h
 * @brief << brief description >>
 * @author clonker
 * @date 09.09.16
 */

#ifndef READDY_KERNEL_CPU_NEXTSUBVOLUMESREACTIONSCHEDULER_H
#define READDY_KERNEL_CPU_NEXTSUBVOLUMESREACTIONSCHEDULER_H

#include <readdy/model/programs/Programs.h>
#include "readdy/kernel/cpu_dense/Kernel.h"

namespace readdy {
namespace kernel {
namespace cpu_dense {
namespace programs {
namespace reactions {

class NextSubvolumes : public readdy::model::programs::reactions::NextSubvolumes {
using cell_index_t = unsigned int;
using signed_cell_index_t = typename std::make_signed<cell_index_t>::type;
public:
    NextSubvolumes(const Kernel *const kernel);
    ~NextSubvolumes();

    virtual void execute() override;

    double getMaxReactionRadius() const;
private:
    struct ReactionEvent;
    struct GridCell;

    Kernel const* const kernel;

    // sets up a grid cell (rate, timestamp, next event)
    void setUpCell(GridCell& cell);
    // sets up the computational grid with spacing >= max(reaction radii)
    void setUpGrid();
    // assigns particles to the computational grid
    void assignParticles();
    // schedules the reactions
    void setUpEventQueue();
    // evaluates the collected events
    void evaluateReactions();
    // sets up the neighbor-linked-list structure
    void setUpNeighbors(GridCell& cell);
    GridCell * getCell(const readdy::model::Vec3& particlePosition);
    // fetches a cell at (i,j,k)
    GridCell * getCell(signed_cell_index_t i, signed_cell_index_t j, signed_cell_index_t k);

    // array holding the number of cells in each spatial direction
    std::array<unsigned int, 3> nCells;
    // size of each box
    readdy::model::Vec3 cellSize;

    std::vector<GridCell> cells;
    std::vector<GridCell*> eventQueue;
};

}
}
}
}
}

#endif //READDY_KERNEL_CPU_NEXTSUBVOLUMESREACTIONSCHEDULER_H
