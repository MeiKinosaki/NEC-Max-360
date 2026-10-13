#include "machine/scheduler.h"
#include "machine/model_config.h"
#include <cassert>
int main() {
    pc6001::Scheduler scheduler;
    assert(scheduler.totalCycles() == 0);
    scheduler.advance(4);
    scheduler.advance(7);
    assert(scheduler.totalCycles() == 11);
    scheduler.reset();
    assert(scheduler.totalCycles() == 0);
    const auto model = pc6001::configFor(pc6001::Model::PC6001);
    assert(model.model == pc6001::Model::PC6001);
    assert(model.cpuClockHz > 0);
    return 0;
}
