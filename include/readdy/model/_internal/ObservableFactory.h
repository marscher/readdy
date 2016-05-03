/**
 * << detailed description >>
 *
 * ... should not be instantiated directly
 *
 * @file ObservableFactory.h
 * @brief << brief description >>
 * @author clonker
 * @date 29.04.16
 */

#ifndef READDY2_MAIN_OBSERVABLEFACTORY_H
#define READDY2_MAIN_OBSERVABLEFACTORY_H

#include <string>
#include <unordered_map>
#include <readdy/model/Observable.h>

namespace readdy {
    namespace model {
        class Kernel;
        namespace _internal {
            class ObservableFactory {
            public:
                ObservableFactory(Kernel *const kernel);

                void registerObservable(const std::string &name, const std::function<Observable *()> create);

                std::unique_ptr<Observable> create(const std::string &name);

                template<typename T>
                std::unique_ptr<T> createAs(const std::string &name) {
                    return std::unique_ptr<T>(factory[name]());
                }

                std::vector<std::string> getRegisteredObservableNames() const;

            private:
                std::unordered_map<std::string, std::function<Observable *()>> factory;
            };
        }
    }
}
#endif //READDY2_MAIN_OBSERVABLEFACTORY_H