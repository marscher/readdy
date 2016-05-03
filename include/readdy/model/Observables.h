/**
 * << detailed description >>
 *
 * @file Observables.h
 * @brief << brief description >>
 * @author clonker
 * @date 26.04.16
 */

#ifndef READDY2_MAIN_OBSERVABLES_H
#define READDY2_MAIN_OBSERVABLES_H

#include <readdy/model/Observable.h>
#include <readdy/model/Vec3.h>
#include <vector>

namespace readdy {
    namespace model {
        class ParticlePositionObservable : public ObservableWithResult<std::vector<Vec3>> {
        public:

            ParticlePositionObservable(Kernel *const kernel, unsigned int stride = 1) : ObservableWithResult(kernel, stride) { }

            virtual ~ParticlePositionObservable() {
            }

            virtual void evaluate(readdy::model::time_step_type t) override;

        };
    }
}

#endif //READDY2_MAIN_OBSERVABLES_H