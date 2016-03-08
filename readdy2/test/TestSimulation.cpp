#include <Simulation.h>
#include "gtest/gtest.h"

using namespace readdy;

namespace {
    class TestSimulation : public ::testing::Test {
    protected:
        Simulation simulation;

        virtual void SetUp() { }

        virtual void TearDown() { }

    };

    TEST_F(TestSimulation, Foo) {
		simulation.setKBT(42);
        EXPECT_EQ(42, simulation.getKBT());
        simulation.setPeriodicBoundary(false, false, false);
    }
}