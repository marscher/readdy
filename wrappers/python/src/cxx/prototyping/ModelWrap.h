/**
 * << detailed description >>
 *
 * @file ModelWrap.h.h
 * @brief << brief description >>
 * @author clonker
 * @date 08.08.16
 */

#ifndef READDY_MAIN_MODELWRAP_H
#define READDY_MAIN_MODELWRAP_H

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <readdy/kernel/singlecpu/SingleCPUKernelStateModel.h>

namespace bpy = pybind11;

namespace readdy {
namespace py {
class Model : public readdy::kernel::singlecpu::SingleCPUKernelStateModel {

    using super = readdy::kernel::singlecpu::SingleCPUKernelStateModel;
public:

    using super::SingleCPUKernelStateModel;

    virtual void removeParticle(const readdy::model::Particle &p) override {
        PYBIND11_OVERLOAD_NAME(void, super, "remove_particle", removeParticle, p);
    }

    virtual const std::vector<model::Vec3> getParticlePositions() const override {
        PYBIND11_OVERLOAD_NAME(const std::vector<model::Vec3>, super, "get_particle_positions", getParticlePositions,);
    }

    virtual double getEnergy() const override {
        PYBIND11_OVERLOAD_NAME(double, super, "get_energy", getEnergy,);
    }

    virtual void increaseEnergy(double increase) override {
        PYBIND11_OVERLOAD_NAME(void, super, "increase_energy", increaseEnergy, increase);
    }

    virtual kernel::singlecpu::model::SingleCPUParticleData *getParticleData() const override {
        PYBIND11_OVERLOAD_NAME(kernel::singlecpu::model::SingleCPUParticleData*, super, "get_particle_data",
                               getParticleData,);
    }

    virtual const readdy::kernel::singlecpu::model::SingleCPUNeighborList *getNeighborList() const override {
        PYBIND11_OVERLOAD_NAME(const kernel::singlecpu::model::SingleCPUNeighborList*, super, "get_neighbor_list",
                               getNeighborList,);
    }

    virtual const std::vector<model::Particle> getParticles() const override {
        PYBIND11_OVERLOAD_NAME(const std::vector<model::Particle>, super, "get_particles", getParticles,);
    }
};
}
}

#endif //READDY_MAIN_MODELWRAP_H
