/**
 * << detailed description >>
 *
 * @file KernelContext.cpp
 * @brief Implementation file of the KernelContext.
 * @author clonker
 * @date 18.04.16
 * @todo make proper reference to KernelContext.h, is kBT really indepdendent of t?
 */

#include <readdy/model/KernelContext.h>
#include <boost/make_unique.hpp>

using namespace readdy::model;

struct readdy::model::KernelContext::Impl {
    double kBT = 0;
    std::array<double, 3> box_size{};
    std::array<bool, 3> periodic_boundary{};
};


double KernelContext::getKBT() const {
    return (*pimpl).kBT;
}

void KernelContext::setKBT(double kBT) {
    (*pimpl).kBT = kBT;
}

void KernelContext::setBoxSize(double dx, double dy, double dz) {
    (*pimpl).box_size = {dx, dy, dz};
}

void KernelContext::setPeriodicBoundary(bool pb_x, bool pb_y, bool pb_z) {
    (*pimpl).periodic_boundary = {pb_x, pb_y, pb_z};
}

KernelContext::KernelContext() : pimpl(boost::make_unique<KernelContext::Impl>()) { }

std::array<double, 3> KernelContext::getBoxSize() const {
    return std::array<double, 3>((*pimpl).box_size);
}

std::array<bool, 3> KernelContext::getPeriodicBoundary() const {
    return std::array<bool, 3>((*pimpl).periodic_boundary);
}

KernelContext::KernelContext(const KernelContext &rhs) : pimpl(boost::make_unique<KernelContext::Impl>(*rhs.pimpl)) { }

KernelContext &KernelContext::operator=(const KernelContext &rhs) {
    *pimpl = *rhs.pimpl;
    return *this;
}


KernelContext &KernelContext::operator=(KernelContext &&rhs) = default;

KernelContext::KernelContext(KernelContext &&rhs) = default;

KernelContext::~KernelContext() = default;



