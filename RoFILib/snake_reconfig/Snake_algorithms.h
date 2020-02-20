#pragma once

#include <Configuration.h>
#include <IO.h>
#include "MinMaxHeap.h"
#include "Snake_structs.h"
#include <limits>
#include <queue>
#include <cmath>
#include <memory>

struct AlgorithmStat
{
    unsigned long pathLength = 0;
    unsigned long queueSize = 0;
    unsigned long seenCfgs = 0;

    std::string toString() const
    {
        std::stringstream out;
        out << std::setw(8) << std::left << "length " << pathLength << std::endl;
        out << std::setw(8) << std::left << "queue " << queueSize << std::endl;
        out << std::setw(8) << std::left << "cfgs " << seenCfgs << std::endl;
        return out.str();
    }
};

class ConfigurationPtrHash
{
public:
    std::size_t operator()(const std::unique_ptr<Configuration>& ptr) const
    {
        return ConfigurationHash{}(*ptr);
    }
};

inline bool operator==(const std::unique_ptr<Configuration>& a, const std::unique_ptr<Configuration>& b)
{
    return (*a) == (*b);
}

using ConfigPred = std::unordered_map<const Configuration*, const Configuration*>;
using ConfigEdges = std::unordered_map<const Configuration*, std::vector<const Configuration*>>;
using ConfigValue = std::unordered_map<const Configuration*, double>;
using EvalFunction = double(const Configuration&, const Configuration&);
using DistFunction = double(const Configuration&, const Configuration&);
using EvalPair = std::tuple<double, const Configuration*>;

struct EvalCompare
{
public:
    bool operator()(const EvalPair& a, const EvalPair& b) const
    {
        return std::get<0>(a) < std::get<0>(b);
    }
};

class ConfigPool
{
public:
    typedef std::unordered_set<std::unique_ptr<Configuration>, ConfigurationPtrHash>::iterator iterator;
    typedef std::unordered_set<std::unique_ptr<Configuration>, ConfigurationPtrHash>::const_iterator const_iterator;

    iterator begin() { return pool.begin(); }
    const_iterator begin() const { return pool.begin(); }
    iterator end() { return pool.end(); }
    const_iterator end() const { return pool.end(); }

    size_t size() const
    {
        return pool.size();
    }

    Configuration* insert(const Configuration& config)
    {
        auto tmp = std::make_unique<Configuration>(config);
        auto [ptr, _] = pool.insert(std::move(tmp));
        return ptr->get();
    }

    bool find(const Configuration& config) const
    {
        auto tmp = std::make_unique<Configuration>(config);
        return (pool.find(tmp) != pool.end());
    }

    const Configuration* get(const Configuration& config) const
    {
        auto tmp = std::make_unique<Configuration>(config);
        return pool.find(tmp)->get();
    }

private:
    std::unordered_set<std::unique_ptr<Configuration>, ConfigurationPtrHash> pool;
};

inline double eval(const Configuration& configuration, SpaceGrid& sg) {
    sg.loadConfig(configuration);
    return sg.getDist();
}

inline std::vector<Configuration> createPath(ConfigPred& pred, const Configuration* goal)
{
    std::vector<Configuration> res;
    const Configuration* current = goal;

    while (current != pred.at(current))
    {
        res.push_back(*current);
        current = pred.at(current);
    }
    res.push_back(*current);

    std::reverse(res.begin(), res.end());
    return res;
}

inline std::vector<Configuration> createPath(ConfigEdges& edges, const Configuration* init, const Configuration* goal)
{
    ConfigPred pred;
    std::set<const Configuration*> seen;
    if (init == goal)
    {
        return {*init};
    }

    seen.insert(init);
    pred.insert({init, init});

    std::queue<const Configuration*> queue;
    queue.push(init);

    while (!queue.empty())
    {

        const auto curr = queue.front();
        queue.pop();

        for (const auto* next : edges[curr])
        {
            if (seen.find(next) == seen.end())
            {
                seen.insert(next);
                pred.insert({next, curr});

                if (next == goal)
                {
                    return createPath(pred, goal);
                }

                queue.push(next);
            }
        }
    }

    return {};
}


inline std::vector<Configuration> SnakeStar(const Configuration& init, AlgorithmStat* stat = nullptr, int limit = 5000, double path_pref = 0.5)
{
    unsigned step = 90;
    double free_pref = 1 - path_pref;
    ConfigPred pred;
    ConfigPool pool;

    SpaceGrid grid(init.getIDs().size());
    // Already computed shortest distance from init to configuration.
    ConfigValue initDist;

    // Shortest distance from init through this configuration to goal.
    ConfigValue goalDist;

    double startDist = eval(init, grid);

    if (startDist == 0)
    {
        return {init};
    }

    MinMaxHeap<EvalPair, EvalCompare> queue(limit);

    const Configuration* pointer = pool.insert(init);
    const Configuration* bestConfig = pointer;
    unsigned bestScore = startDist;
    double worstDist = startDist;

    initDist[pointer] = 0;
    goalDist[pointer] = startDist;
    pred[pointer] = pointer;
    int maxQSize = 0;
    int i = 0;
    queue.push( {goalDist[pointer], pointer} );


    while (!queue.empty() && i++ < limit)
    {

        maxQSize = std::max(maxQSize, queue.size());
        const auto [d, current] = queue.popMin();
        double currDist = initDist[current];

        std::vector<Configuration> nextCfgs;
        current->simpleNext(nextCfgs, step);

        for (const auto& next : nextCfgs)
        {
            const Configuration* pointerNext;
            double newEval = eval(next, grid);
            double newDist = path_pref * (currDist + 1) + free_pref * newEval;
            bool update = false;

            if (newEval != 0 && limit <= queue.size() + i) {
                if (newDist > worstDist) {
                    continue;
                }
                worstDist = newDist;
                if (!queue.empty()) {
                    queue.popMax();
                }
            }
            if (newDist > worstDist) {
                worstDist = newDist;
            }

            if (!pool.find(next))
            {
                pointerNext = pool.insert(next);
                initDist[pointerNext] = currDist + 1;
                update = true;
            }
            else
            {
                pointerNext = pool.get(next);
            }

            if (newEval < bestScore) {
                bestScore = newEval;
                bestConfig = pointerNext;
            }

            if ((currDist + 1 < initDist[pointerNext]) || update)
            {
                initDist[pointerNext] = currDist + 1;
                goalDist[pointerNext] = newDist;
                pred[pointerNext] = current;
                queue.push({newDist, pointerNext});
            }

            if (newEval == 0)
            {
                auto path = createPath(pred, pointerNext);
                if (stat != nullptr)
                {
                    stat->pathLength = path.size();
                    stat->queueSize = maxQSize;
                    stat->seenCfgs = pool.size();
                }

                return path;
            }
        }
    }
    std::cout << "Just returning my best!" << std::endl;
    auto path = createPath(pred, bestConfig);
    if (stat != nullptr)
    {
        stat->pathLength = path.size();
        stat->queueSize = maxQSize;
        stat->seenCfgs = pool.size();
    }

    return path;
}

unsigned closestMass(const Configuration& init) {
    Vector mass = init.massCenter();
    unsigned bestID = 0;
    double bestDist = std::numeric_limits<double>::max();
    for (const auto& [id, ms] : init.getMatrices()) {
        for (const auto& matrix : ms) {
            double currDist = sqDistVM(matrix, mass);
            if (currDist < bestDist) {
                bestDist = currDist;
                bestID = id;
            }
        }
    }
    return bestID;
}

class MakeStar
{
public:
    MakeStar(const Configuration& init, unsigned root)
    : mass(init.massCenter())
    , dists()
    , seen()
    , zz_seen()
    , module_count(int.getMatrices().size())
    , dfs_stack() {
        for (const auto& [id, ms] : init.getMatrices()) {
            for (unsigned side = 0; side < 2; ++side) {
                double currDist = sqDistVM(matrix, ms[side]);
                dists.insert({{id,side},currDist});
            }
        }
        dfs_stack.push(root);
    }

    bool empty() {
        return dfs_stack.empty();
    }

private:
    const Vector mass;
    std::unordered_map<std::tuple<unsigned, unsigned>, double> dists;
    std::unordered_set<unsigned> seen;
    std::unordered_set<unsigned> zz_seen;
    int module_count;
    std::stack<unsigned> dfs_stack;
};

using chooseRootFunc = unsigned(const Configuration&);
template<typename Next>
inline Configuration treefy(const Configuration& init, AlgorithmStat* stat = nullptr, chooseRootFunc chooseRoot = closestMass)
{
    unsigned int root = chooseRoot(init);
    Next oracle = Next(init, root);
    while(!oracle.empty()) {

    }
}