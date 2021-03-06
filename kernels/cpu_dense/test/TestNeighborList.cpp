/********************************************************************
 * Copyright © 2016 Computational Molecular Biology Group,          *
 *                  Freie Universität Berlin (GER)                  *
 *                                                                  *
 * This file is part of ReaDDy.                                     *
 *                                                                  *
 * ReaDDy is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU Lesser General Public License as   *
 * published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU Lesser General Public License for more details.              *
 *                                                                  *
 * You should have received a copy of the GNU Lesser General        *
 * Public License along with this program. If not, see              *
 * <http://www.gnu.org/licenses/>.                                  *
 ********************************************************************/


/**
 * Test different contexts w.r.t. boxsize and periodicity, perform setupBoxes() and see if that worked.
 * Then add a few particles and perform fillBoxes(). The result of this is the actual neighborlist, which is
 * checked as well.
 *
 * @file CPUTestNeighborList.cpp
 * @brief Test the neighborlist object of the CPU kernel.
 * @author chrisfroe
 * @author clonker
 * @date 23.08.16
 */

#include <gtest/gtest.h>
#include <readdy/kernel/cpu_dense/model/CPUDNeighborList.h>
#include <readdy/kernel/cpu_dense/CPUDKernel.h>
#include <readdy/testing/NOOPPotential.h>

namespace cpu = readdy::kernel::cpu_dense;
namespace cpum = cpu::model;
namespace m = readdy::model;

namespace {

using data_t = cpu::model::CPUDParticleData;

struct TestNeighborList : ::testing::Test {

    std::unique_ptr<cpu::CPUDKernel> kernel;
    unsigned int typeIdA;

    TestNeighborList() : kernel(std::make_unique<cpu::CPUDKernel>()) {
        auto &ctx = kernel->getKernelContext();
        ctx.particle_types().add("A", 1.0, 1.0);
        double eductDistance = 1.2;
        ctx.reactions().add(kernel->createFusionReaction("test", "A", "A", "A", 0., eductDistance));
        ctx.potentials().add(std::make_unique<readdy::testing::NOOPPotentialOrder2>("A", "A", 1.1, 0., 0.));
        typeIdA = ctx.particle_types().id_of("A");
        ctx.configure();
    }

};

auto isPairInList = [](readdy::kernel::cpu_dense::model::CPUDNeighborList *pairs, data_t &data,
                       unsigned long idx1, unsigned long idx2) {
    const auto &neighbors1 = pairs->find_neighbors(idx1);
    for (auto &neigh_idx : neighbors1) {
        if (neigh_idx.idx == idx2) {
            return true;
        }
    }
    const auto &neighbors2 = pairs->find_neighbors(idx2);
    for (auto &neigh_idx : neighbors2) {
        if (neigh_idx.idx == idx1) {
            return true;
        }
    }
    return false;
};

auto getNumberPairs = [](const readdy::kernel::cpu_dense::model::CPUDNeighborList &pairs) {
    using val_t = decltype(*pairs.begin());
    return std::accumulate(pairs.begin(), pairs.end(), 0, [](int acc, val_t &x) {
        return acc + x.size();
    });
};

TEST_F(TestNeighborList, ThreeBoxesNonPeriodic) {
    // maxcutoff is 1.2, system is 1.5 x 4 x 1.5, non-periodic, three cells
    auto &ctx = kernel->getKernelContext();
    ctx.setBoxSize(1.5, 4, 1.5);
    ctx.setPeriodicBoundary(false, false, false);

    readdy::util::thread::Config conf;
    readdy::kernel::cpu_dense::model::CPUDParticleData data {&ctx};
    cpum::CPUDNeighborList list(&ctx, data, &conf);

    list.setupCells();
    // Add three particles, two are in one outer box, the third on the other end and thus no neighbor
    const auto particles = std::vector<m::Particle>{
            m::Particle(0, -1.8, 0, typeIdA), m::Particle(0, -1.8, 0, typeIdA), m::Particle(0, 1.8, 0, typeIdA)
    };

    data.addParticles(particles);
    list.fillCells();
    int sum = getNumberPairs(list);
    EXPECT_EQ(sum, 2);
    EXPECT_TRUE(isPairInList(&list, data, 0, 1));
    EXPECT_TRUE(isPairInList(&list, data, 1, 0));
}

TEST_F(TestNeighborList, OneDirection) {
    // maxcutoff is 1.2, system is 4.8 x 5 x 5.1
    auto &ctx = kernel->getKernelContext();
    ctx.setBoxSize(1.2, 1.1, 2.8);
    ctx.setPeriodicBoundary(false, false, true);

    readdy::util::thread::Config conf;
    readdy::kernel::cpu_dense::model::CPUDParticleData data {&ctx};
    cpum::CPUDNeighborList list(&ctx, data, &conf);

    list.setupCells();
    // Add three particles, one of which is in the neighborhood of the other two
    const auto particles = std::vector<m::Particle>{
            m::Particle(0, 0, -1.1, typeIdA), m::Particle(0, 0, .4, typeIdA), m::Particle(0, 0, 1.1, typeIdA)
    };
    data.addParticles(particles);
    list.fillCells();
    int sum = getNumberPairs(list);
    EXPECT_EQ(sum, 4);
    EXPECT_TRUE(isPairInList(&list, data, 0, 2));
    EXPECT_TRUE(isPairInList(&list, data, 2, 0));
    EXPECT_TRUE(isPairInList(&list, data, 1, 2));
    EXPECT_TRUE(isPairInList(&list, data, 2, 1));
    EXPECT_FALSE(isPairInList(&list, data, 0, 1));
    EXPECT_FALSE(isPairInList(&list, data, 1, 0));
}

TEST_F(TestNeighborList, AllNeighborsInCutoffSphere) {
    // maxcutoff is 1.2, system is 4 x 4 x 4, all directions periodic
    auto &ctx = kernel->getKernelContext();
    ctx.setBoxSize(4, 4, 4);
    ctx.setPeriodicBoundary(true, true, true);
    readdy::util::thread::Config conf;
    readdy::kernel::cpu_dense::model::CPUDParticleData data {&ctx};
    cpum::CPUDNeighborList list(&ctx, data, &conf);
    list.setupCells();
    // Create a few particles. In this box setup, all particles are neighbors.
    const auto particles = std::vector<m::Particle>{
            m::Particle(0, 0, 0, typeIdA), m::Particle(0, 0, 0, typeIdA), m::Particle(.3, 0, 0, typeIdA),
            m::Particle(0, .3, -.3, typeIdA), m::Particle(-.3, 0, .3, typeIdA), m::Particle(.3, -.3, 0, typeIdA)
    };

    data.addParticles(particles);
    list.fillCells();
    int sum = getNumberPairs(list);
    EXPECT_EQ(sum, 30);
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = i + 1; j < 6; ++j) {
            EXPECT_TRUE(isPairInList(&list, data, i, j)) << "Particles " << i << " and " << j << " were not neighbors.";
            EXPECT_TRUE(isPairInList(&list, data, j, i)) << "Particles " << j << " and " << i << " were not neighbors.";
        }
    }
}
}