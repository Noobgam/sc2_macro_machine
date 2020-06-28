#include "WorkerManager.h"
#include "CCBot.h"
#include "../util/Util.h"

WorkerManager::WorkerManager(CCBot & bot)
    : m_bot         (bot)
    , m_workerData  (bot)
{

}

void WorkerManager::onStart()
{

}

void WorkerManager::onFrame()
{
    m_workerData.updateAllWorkerData();
    handleGasWorkers();
    handleIdleWorkers();

    drawResourceDebugInfo();
    drawWorkerInformation();

    m_workerData.drawDepotDebugInfo();

    handleRepairWorkers();
}

void WorkerManager::handleGasWorkers()
{
    // for each unit we have
    for (auto & unitPtr : m_bot.UnitInfo().getUnits(Players::Self)) {
        const Unit& unit = *unitPtr;
        // if that unit is a refinery
        if (unit.getType().isRefinery() && unit.isCompleted())
        {
            // get the number of workers currently assigned to it
            int numAssigned = m_workerData.getNumAssignedWorkers(unit);

            // if it's less than we want it to be, fill 'er up
            for (int i=0; i<(3-numAssigned); ++i)
            {
                auto gasWorker = getGasWorker(unit);
                if (gasWorker.has_value())
                {
                    m_workerData.setHarvestJob(gasWorker.value(), WorkerJobs::Gas, unit);
                }
            }
        }
    }
}

void WorkerManager::handleIdleWorkers()
{
    // for each of our workers
    for (auto & worker : m_workerData.getWorkers())
    {
        if (!worker.isValid()) { continue; }

        bool isIdle = worker.isIdle();
        if (worker.isIdle() && 
            (m_workerData.getWorkerJob(worker) != WorkerJobs::Build) && 
            (m_workerData.getWorkerJob(worker) != WorkerJobs::Move) &&
            (m_workerData.getWorkerJob(worker) != WorkerJobs::Scout)) 
        {
            m_workerData.setWorkerJob(worker, WorkerJobs::Idle);
        }

        // if it is idle
        if (m_workerData.getWorkerJob(worker) == WorkerJobs::Idle)
        {
            setMineralWorker(worker);
        }
    }
}

void WorkerManager::handleRepairWorkers()
{
    // TODO
}

std::optional<Unit> WorkerManager::getClosestMineralWorkerTo(const CCPosition & pos) const
{
    std::optional<Unit> closestMineralWorker;
    double closestDist = std::numeric_limits<double>::max();

    // for each of our workers
    for (auto & worker : m_workerData.getWorkers())
    {
        if (!worker.isValid()) { continue; }

        // if it is a mineral worker
        if (m_workerData.getWorkerJob(worker) == WorkerJobs::Minerals)
        {
            double dist = Util::DistSq(worker.getPosition(), pos);

            if (!closestMineralWorker.has_value() || dist < closestDist)
            {
                closestMineralWorker = worker;
                closestDist = dist;
            }
        }
    }

    return closestMineralWorker;
}


// set a worker to mine minerals
void WorkerManager::setMineralWorker(const Unit & unit)
{
    // check if there is a mineral available to send the worker to
    auto depot = getClosestDepot(unit);

    // if there is a valid mineral
    if (depot.has_value())
    {
        // update m_workerData with the new job
        m_workerData.setHarvestJob(unit, WorkerJobs::Minerals, depot.value());
    }
}

std::optional<Unit> WorkerManager::getClosestDepot(Unit worker) const
{
    std::optional<Unit> closestDepot;
    double closestDistance = std::numeric_limits<double>::max();

    for (auto & unitPtr : m_bot.UnitInfo().getUnits(Players::Self)) {
        const Unit& unit = *unitPtr;
        if (!unit.isValid()) { continue; }

        if (unit.getType().isResourceDepot() && unit.isCompleted())
        {
            double distance = Util::Dist(unit, worker);
            if (!closestDepot.has_value() || distance < closestDistance)
            {
                closestDepot = unit;
                closestDistance = distance;
            }
        }
    }

    return closestDepot;
}


// other managers that need workers call this when they're done with a unit
void WorkerManager::finishedWithWorker(const Unit & unit)
{
    if (m_workerData.getWorkerJob(unit) != WorkerJobs::Scout)
    {
        m_workerData.setWorkerJob(unit, WorkerJobs::Idle);
    }
}

std::optional<Unit> WorkerManager::getGasWorker(Unit refinery) const
{
    return getClosestMineralWorkerTo(refinery.getPosition());
}

void WorkerManager::setBuildingWorker(Unit worker, Building & b)
{
    m_workerData.setWorkerJob(worker, WorkerJobs::Build);
}

// gets a builder for BuildingManager to use
// if setJobAsBuilder is true (default), it will be flagged as a builder unit
// set 'setJobAsBuilder' to false if we just want to see which worker will build a building
std::optional<Unit> WorkerManager::getBuilder(Building & b, bool setJobAsBuilder) const
{
    std::optional<Unit> builderWorker = getClosestMineralWorkerTo(Util::GetPosition(b.finalPosition));

    // if the worker exists (one may not have been found in rare cases)
    if (builderWorker.has_value() && setJobAsBuilder)
    {
        m_workerData.setWorkerJob(builderWorker.value(), WorkerJobs::Build);
    }

    return builderWorker;
}

// sets a worker as a scout
void WorkerManager::setScoutWorker(Unit workerTag)
{
    m_workerData.setWorkerJob(workerTag, WorkerJobs::Scout);
}

void WorkerManager::setCombatWorker(Unit workerTag)
{
    m_workerData.setWorkerJob(workerTag, WorkerJobs::Combat);
}

void WorkerManager::drawResourceDebugInfo()
{

    for (auto & worker : m_workerData.getWorkers())
    {
        if (!worker.isValid()) { continue; }

        if (worker.isIdle())
        {
            m_bot.Map().drawText(worker.getPosition(), m_workerData.getJobCode(worker));
        }

//        auto depot = m_workerData.getWorkerDepot(worker);
//        if (depot.has_value())
//        {
//            m_bot.Map().drawLine(worker.getPosition(), depot.value().getPosition());
//        }
    }
}

void WorkerManager::drawWorkerInformation()
{

    std::stringstream ss;
    ss << "Workers: " << m_workerData.getWorkers().size() << "\n";

    int yspace = 0;

    for (auto & worker : m_workerData.getWorkers())
    {
        ss << m_workerData.getJobCode(worker) << " " << worker.getID() << "\n";

        m_bot.Map().drawText(worker.getPosition(), m_workerData.getJobCode(worker));
    }

    m_bot.Map().drawTextScreen(0.75f, 0.2f, ss.str());
}

bool WorkerManager::isFree(Unit worker) const
{
    return m_workerData.getWorkerJob(worker) == WorkerJobs::Minerals || m_workerData.getWorkerJob(worker) == WorkerJobs::Idle;
}

bool WorkerManager::isWorkerScout(Unit worker) const
{
    return (m_workerData.getWorkerJob(worker) == WorkerJobs::Scout);
}

bool WorkerManager::isBuilder(Unit worker) const
{
    return (m_workerData.getWorkerJob(worker) == WorkerJobs::Build);
}

int WorkerManager::getNumMineralWorkers()
{
    return m_workerData.getWorkerJobCount(WorkerJobs::Minerals);
}

int WorkerManager::getNumGasWorkers()
{
    return m_workerData.getWorkerJobCount(WorkerJobs::Gas);

}

int WorkerManager::getTotalWorkerCount() {
    return (int)m_workerData.getWorkers().size();
}
