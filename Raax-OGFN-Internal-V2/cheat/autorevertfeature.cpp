#include "AutoRevertFeature.h"

namespace AutoRevertFeature {

// --- Public Initialization & Tick Functions ------------------------

void Tick() {
    auto It = g_AutoRevertFeatures.begin();
    while (It != g_AutoRevertFeatures.end()) {
        if (!(*It)->Tick())
            It = g_AutoRevertFeatures.erase(It);
        else
            ++It;
    }
}

void Destroy() {
    g_AutoRevertFeatures.clear();
}

// --- Global Variables ----------------------------------------------

std::vector<std::unique_ptr<IAutoRevertFeature>> g_AutoRevertFeatures;

} // namespace AutoRevertFeature
