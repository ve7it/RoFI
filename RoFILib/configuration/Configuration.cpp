#include "Configuration.h"
#include <stdexcept>

/* * * * * *
 * MODULE  *
 * * * * * */

double Module::getJoint(Joint a) const {
    switch (a) {
        case Joint::Alpha:
            return alpha;
        case Joint::Beta:
            return beta;
        case Joint::Gamma:
            return gamma;
        default:
            throw std::invalid_argument("Passed unknown joint to `Module::getJoint`");
    }
}

bool Module::rotateJoint(Joint joint, double val) {
    double res = 0;
    switch (joint) {
        case Joint::Alpha:
            res = alpha + val;
            break;
        case Joint::Beta:
            res = beta + val;
            break;
        case Joint::Gamma:
            res = gamma + val;
        default:
            throw std::invalid_argument("Passed unknown joint to `Module::rotateJoint`");
    }
    return setJoint(joint, res);
}

double clampGamma(double val) {
    if (val > 180)
        return 360 - val;

    if (val <= -180)
        return 360 + val;
    
    return val;
}

bool Module::setJoint(Joint joint, double val) {
    switch (joint) {
        case Joint::Alpha:
            if ((val > 90) || (val < -90))
                return false;

            alpha = val;
            return true;

        case Joint::Beta:
            if ((val > 90) || (val < -90))
                return false;

            beta = val;
            return true;

        case Joint::Gamma:
            gamma = clampGamma(val);
            return true;

        default:
            throw std::invalid_argument("Passed unknown joint to `Module::setJoint`");
    }
}

bool Module::operator==(const Module& other) const {
    return (id == other.id) &&
            (std::abs(alpha - other.alpha) < threshold) &&
            (std::abs(beta - other.beta) < threshold) &&
            (std::abs(gamma - other.gamma) < threshold);
}

/* * * * *
 * EDGE  *
 * * * * */

bool Edge::operator==(const Edge& other) const {
    return (id1_ == other.id1_) &&
            (id2_ == other.id2_) &&
            (side1_ == other.side1_) &&
            (side2_ == other.side2_) &&
            (dock1_ == other.dock1_) &&
            (dock2_ == other.dock2_) &&
            (ori_ == other.ori_);
}

inline std::array<unsigned, 5> nextEdgeArray(const std::array<unsigned, 5> &edge) {
    const static std::array<unsigned, 5> size = {2,3,4,3,2};
    std::array<unsigned, 5> res = edge;

    for (unsigned i = 0; i < 5; ++i) {
        if (res[i] + 1 < size[i]) {
            res[i] += 1;
            break;
        }
        res[i] = 0;
    }

    return res;
}

inline std::optional<Edge> nextEdge(const Edge& edge) {
    const static std::array<unsigned, 5> init = {0, 0, 0, 0, 0};
    std::array<unsigned, 5> arr = {edge.side1(), edge.dock1(), edge.ori(), edge.dock2(), edge.side2()};
    auto next = nextEdgeArray(arr);

    if (next == init)
        return std::nullopt;

    return {Edge(edge.id1(),
                static_cast<ShoeId>(next[0]),
                static_cast<ConnectorId>(next[1]),
                next[2],
                static_cast<ConnectorId>(next[3]),
                static_cast<ShoeId>(next[4]),
                edge.id2())};
}

/* * * * * *
 * ACTIONS *
 * * * * * */

bool Action::Rotate::operator==(const Rotate &other) const {
    return _id == other._id &&
            _joint == other._joint &&
            std::abs(_angle - other._angle) < threshold;
}

bool Action::Reconnect::operator==(const Reconnect &other) const {
    return _add == other._add &&
            _edge == other._edge;
}

Action Action::divide(double factor) const {
    std::vector<Rotate> division;
    for (const Action::Rotate& rotate : _rotations)
        division.emplace_back(rotate._id, rotate._joint, rotate._angle * factor);

    return {division, _reconnections};
}

inline bool unique(const std::vector<Action::Rotate>& a) {
    std::unordered_map<ID, std::array<bool, 3>> moved;

    for (const auto& rot : a) {
        if (moved.find(rot.id()) == moved.end())
            moved[rot.id()] = {false, false, false};

        if (moved[rot.id()][rot.joint()])
            return false;

        moved[rot.id()][rot.joint()] = true;
    }

    return true;
}

using Rots = std::vector<Action::Rotate>;
template<>
inline void getAllSubsetsRec<Rots>(const Rots& set, std::vector<Rots>& res, const Rots& accum, unsigned index, unsigned count) {
    if ((accum.size() == count) || (index >= set.size())) {
        res.push_back(accum);
        return;
    }

    auto act = set[index];
    bool ok = true;
    for (auto& act2 : accum) {
        if ((act.id() == act2.id()) && (act.joint() == act2.joint()))
            ok = false;
    }
    if (ok) {
        auto next = accum;
        next.push_back(set[index]);
        getAllSubsetsRec<Rots>(set, res, next, index + 1, count);
    }
    getAllSubsetsRec<Rots>(set, res, accum, index + 1, count);
}

template<typename T>
inline void getAllSubsetsRec(const std::vector<T>& set, std::vector<std::vector<T>>& res, const std::vector<T>& accum, unsigned index, unsigned count) {
    if ((accum.size() == count) || (index >= set.size())) {
        res.push_back(accum);
        return;
    }
    auto next = accum;
    next.push_back(set[index]);
    getAllSubsetsRec(set, res, next, index + 1, count);
    getAllSubsetsRec(set, res, accum, index + 1, count);
}

template<typename T>
inline void getAllSubsets(const std::vector<T>& from, std::vector<std::vector<T>>& res, unsigned count) {
    getAllSubsetsRec<T>(from, res, {}, 0, count);
}

/* * * * * * * * * * * * * *
 * TRANSFORMATION MATRICES *
 * * * * * * * * * * * * * */

inline Matrix transformJoint(double alpha, double beta, double gamma) {
    return rotate(alpha, X) * rotate(gamma, Z) * translate(Z) * rotate(M_PI, Y) * rotate(-beta, X);
}

inline Matrix transformConnection(ConnectorId d1, int ori, ConnectorId d2) {
    static const std::array<Matrix, 3> dockFaceUp = {
            rotate(M_PI, Z) * rotate(-M_PI/2, Y), // XPlus
            rotate(M_PI, Z) * rotate(M_PI/2, Y),  // XMinus
            identity  // ZMinus
    };

    static const std::array<Matrix, 3> faceToDock = {
            rotate(M_PI, Z) * rotate(M_PI/2, Y), // XPlus
            rotate(M_PI, Z) * rotate(-M_PI/2, Y),   // XMinus
            identity // ZMinus
    };

    return faceToDock[d1] * rotate(ori * M_PI/2, Z) * translate(-Z) * dockFaceUp[d2] * rotate(M_PI, X);
}

/* * * * * * * * *
 * CONFIGURATION *
 * * * * * * * * */

Module& Configuration::getModule(ID id) {
    auto it = modules.find(id);
    if(it == modules.end())
        throw std::out_of_range("No module with this id: " + std::to_string(id));

    return it->second;
}

const Module& Configuration::getModule(ID id) const {
    auto it = modules.find(id);
    if(it == modules.end())
        throw std::out_of_range("No module with this id: " + std::to_string(id));

    return it->second;
}

std::vector<ID> Configuration::getIDs() const {
    std::vector<ID> ids;
    ids.reserve(modules.size());
    for (auto& [id, m] : modules)
        ids.push_back(id);

    return ids;
}

std::vector<Edge> Configuration::getEdges(ID id) const {
    std::vector<Edge> res;
    for (auto& e : edges.at(id)) {
        if (e.has_value())
            res.push_back(e.value());
    }
    return res;
}

std::vector<Edge> Configuration::getEdges(ID id, const std::unordered_set<ID>& exclude) const {
    std::vector<Edge> res;
    for (auto& e : edges.at(id)) {
        if (e.has_value() && exclude.find(e->id2()) == exclude.end())
            res.push_back(e.value());
    }
    return res;
}

void Configuration::addModule(double alpha, double beta, double gamma, ID id) {
    modules.emplace(std::piecewise_construct,
                    std::forward_as_tuple(id),  // args for key
                    std::forward_as_tuple(alpha, beta, gamma, id));
    edges.emplace(std::piecewise_construct,
                    std::forward_as_tuple(id),  // args for key
                    std::forward_as_tuple());
    if ((modules.size() == 1) || (id < fixedId)) {
        //TODO: je toto opravdu potřeba/proč to chceme
        fixedId = id;
    }
}

bool Configuration::addEdge(const Edge& edge) {
    // Finds edges for both modules.
    EdgeList& set1 = edges.at(edge.id1());
    EdgeList& set2 = edges.at(edge.id2());

    int index1 = edge.side1() * 3 + edge.dock1();
    int index2 = edge.side2() * 3 + edge.dock2();
    if (set1[index1].has_value() || set2[index2].has_value())
        return false;

    set1[index1] = edge;
    set2[index2] = reverse(edge);
    return true;
}

bool Configuration::findEdge(const Edge& edge) const {
    int idx = edge.side1() * 3 + edge.dock1();
    return edges.at(edge.id1())[idx].has_value();
}

bool Configuration::findConnection(const Edge& edge) const {
    int idx = edge.side1() * 3 + edge.dock1();
    const auto& edgeCandidate = edges.at(edge.id1())[idx];
    return edgeCandidate.has_value() && edgeCandidate.value() == edge;
}

void Configuration::setFixed(ID initID, ShoeId initSide, const Matrix& initRotation) {
    fixedId = initID;
    fixedSide = initSide;
    fixedMatrix = initRotation;
}

bool Configuration::isValid() {
    if (connectedVal == Value::Unknown)
        connectedVal = connected() ? Value::True : Value::False;

    return (connectedVal == Value::True) && computeMatrices() && collisionFree();
}

bool Configuration::computeMatrices() {
    if (empty())
        return true;

    matrices = {};
    matrices[fixedId][fixedSide] = fixedMatrix;

    return computeMatricesRec(fixedId, fixedSide);
}

bool Configuration::computeMatricesRec(ID id, ShoeId side) {
    bool result = true;

    // At first, fix other side of the module.
    ShoeId side2 = side == A ? B : A;
    auto& matrixCurr = matrices.at(id);
    matrixCurr[side2] = computeOtherSideMatrix(id, side);

    for (const std::optional<Edge>& edgeOpt : edges.at(id)) {
        if (!edgeOpt.has_value() || edgeOpt.value().onCoeff() < 1)
            continue;
        
        const Edge& edge = edgeOpt.value();

        ID idNext = edge.id2();
        ShoeId sideNext = edge.side2();

        // Compute, where next module should be.
        Matrix matrixCmp = computeConnectedMatrix(edge);

        if (matrices.find(idNext) == matrices.end()) {
            matrices[idNext][sideNext] = matrixCmp;
            result &= computeMatricesRec(idNext, sideNext);
        } else {
            const auto& matrixNext = matrices.at(idNext)[sideNext];
            if (!equals(matrixNext, matrixCmp))
                return false;
        }
    }
    return result;
}

Matrix Configuration::computeOtherSideMatrix(ID id, ShoeId side) const {
    auto const& mod = modules.at(id);
    auto const& matrix = matrices.at(id)[side];
    double alpha = 2 * M_PI * mod.alpha / 360;
    double beta = 2 * M_PI * mod.beta / 360;
    double gamma = 2 * M_PI * mod.gamma / 360;

    if (side == B)
        std::swap(alpha, beta);

    return matrix * transformJoint(alpha, beta, gamma);
}

Matrix Configuration::computeConnectedMatrix(Edge edge) const {
    auto const& matrix = matrices.at(edge.id1())[edge.side1()];
    return matrix * transformConnection(edge.dock1(), edge.ori(), edge.dock2());
}

bool Configuration::connected() const {
    std::unordered_set<ID> seen;
    seen.insert(fixedId);
    dfsID(fixedId, seen);
    return seen.size() == modules.size();
}

void Configuration::dfsID(ID id, std::unordered_set<ID>& seen) const {
    for (const std::optional<Edge> edgeOpt : edges.at(id)) {
        if (!edgeOpt.has_value())
            continue;
        ID nextId = edgeOpt.value().id2();
        if (seen.find(nextId) == seen.end()) {
            seen.insert(nextId);
            dfsID(nextId, seen);
        }
    }
}

bool Configuration::collisionFree() const {
    for (auto it1 = matrices.begin(); it1 != matrices.end(); ++it1) {
        for (auto it2 = it1; it2 != matrices.end(); ++it2) {
            const auto& ms1 = it1->second;
            const auto& ms2 = it2->second;
            if (it1 == it2) {
                if (centerSqDistance(ms1[A], ms1[B]) < 1)
                    return false;
                continue;
            } 
            if (centerSqDistance(ms1[A], ms2[A]) < 1 ||
                centerSqDistance(ms1[B], ms2[B]) < 1 ||
                centerSqDistance(ms1[B], ms2[A]) < 1 ||
                centerSqDistance(ms1[A], ms2[B]) < 1)
                return false;
        }
    }
    return true;
}

Vector Configuration::massCenter() const {
    Vector mass({0,0,0,1});
    for (const auto& [id, ms] : matrices) {
        mass += center(ms[A]);
        mass += center(ms[B]);
    }
    mass /= modules.size()*2;
    return mass;
}

std::optional<Configuration> Configuration::executeIfValid(const Action &action) const {
    int steps = 10;
    Configuration next = *this;

    std::vector<Action::Reconnect> connections;
    std::vector<Action::Reconnect> disconnections;
    for (const auto& res : action.reconnections()) {
        if (res.add())
            connections.push_back(res);
        else
            disconnections.push_back(res);
    }

    Action connect({}, connections);
    Action disconnect({}, disconnections);

    if (!next.execute(disconnect) || !next.connected() || !next.execute(connect))
        return std::nullopt;

    Action rotate(action.rotations(), {});
    Action divided = rotate.divide(1.0/steps);
    for (int i = 1; i <= steps; ++i) {
        if (!next.isValid() || !next.execute(divided))
            return std::nullopt;
    }
    if (!next.isValid())
        return std::nullopt;

    return next;
}

bool Configuration::execute(const Action& action) {
    // TODO: proč nevypadnout hned? když víme, že se to posere.
    bool ok = true;
    for (const Action::Rotate rot : action.rotations())
        ok &= execute(rot);

    for (const Action::Reconnect rec : action.reconnections())
        ok &= execute(rec);

    return ok;
}

void Configuration::generateActions(std::vector<Action>& res, unsigned step, unsigned bound = 1) const {
    // TODO maybe improve this
    std::vector<Action::Rotate> rotations;
    generateRotations(rotations, step);
    std::vector<Action::Reconnect> reconnections;
    generateReconnect(reconnections);

    std::vector<std::vector<Action::Rotate>> resRot;
    std::vector<std::vector<Action::Reconnect>> resRec;

    getAllSubsets(reconnections, resRec, bound);
    getAllSubsets(rotations, resRot, bound);

    for (auto& rotation : resRot) {
        for (auto& reconnection : resRec) {
            if (rotation.size() + reconnection.size() <= bound)
                res.emplace_back(rotation, reconnection);
        }
    }
}

void Configuration::generateSimpleActions(std::vector<Action>& res, unsigned step) const {
    std::vector<Action::Rotate> rotations;
    generateRotations(rotations, step);
    std::vector<Action::Reconnect> reconnections;
    generateReconnect(reconnections);

    for (auto& rotation : rotations)
        res.emplace_back(rotation);

    for (auto& reconnection : reconnections)
        res.emplace_back(reconnection);
}

void Configuration::generateParalyzedActions(std::vector<Action>& res, unsigned step, const std::unordered_set<unsigned>& allowed_indices) const {
    // TODO: improve reconnect to a*n from n*n
    std::vector<Action::Rotate> rotations;
    generateParalyzedRotations(rotations, step, allowed_indices);
    std::vector<Action::Reconnect> reconnections;
    generateReconnect(reconnections);

    for (auto& rotation : rotations)
        res.emplace_back(rotation);

    for (auto& reconnection : reconnections)
        res.emplace_back(reconnection);
}

void Configuration::generateConnections(std::vector<Action::Reconnect>& res) const {
    for (auto& [id1, ms1] : matrices) {
        for (auto& [id2, ms2] : matrices) {
            if (id1 >= id2)
                continue;

            Edge edge(id1, A, XPlus, 0, XPlus, A, id2);
            auto edgeOpt = nextEdge(edge);

            while (edgeOpt.has_value()) {
                Vector center1 = center(ms1[edge.side1()]);
                Vector center2 = center(ms2[edge.side2()]);
                if (distance(center1, center2) != 1) {
                    edge = edgeOpt.value();
                    edgeOpt = nextEdge(edge);
                    continue;
                }

                const Matrix& matrix = ms2[edge.side2()];
                if (equals(matrix, computeConnectedMatrix(edge)) && !findEdge(edge))
                    res.emplace_back(true, edge);

                edge = edgeOpt.value();
                edgeOpt = nextEdge(edge);
            }
        }
    }
}

void Configuration::generateRotations(std::vector<Action::Rotate>& res, unsigned step) const {
    for (auto [id, mod] : modules) {
        for (int d : {-step, step}) {
            for (Joint joint : {Alpha, Beta, Gamma}) {
                if (mod.rotateJoint(joint, d))
                    res.emplace_back(id, joint, d);
            }
        }
    }
}

void Configuration::generateParalyzedRotations(std::vector<Action::Rotate>& res, unsigned step, const std::unordered_set<unsigned>& allowed_indices) const {
    for (const auto& id : allowed_indices) {
        auto it = modules.find(id);
        if (it == modules.end())
            continue;

        Module mod = it->second;
        for (int d : {-step, step}) {
            for (Joint joint : {Alpha, Beta, Gamma}) {
                if (mod.rotateJoint(joint, d))
                    res.emplace_back(id, joint, d);
            }
        }
    }
}

void Configuration::generateDisconnections(std::vector<Action::Reconnect>& res) const {
    for (auto& [id, set] : edges) {
        for (const std::optional<Edge>& edgeOpt : set) {
            if (!edgeOpt.has_value())
                continue;

            const Edge& edge = edgeOpt.value();
            if (edge.id1() < edge.id2())
                res.emplace_back(false, edge);
        }
    }
}

void Configuration::next(std::vector<Configuration>& res, unsigned step, unsigned bound = 1) const {
    std::vector<Action> actions;
    if (bound == 1) {
        generateSimpleActions(actions, step);
    } else {
        generateActions(actions, step, bound);
    }

    for (auto& action : actions) {
        auto cfgOpt = executeIfValid(action);
        if (cfgOpt.has_value())
            res.push_back(cfgOpt.value());
    }
}

void Configuration::simpleNext(std::vector<Configuration>& res, unsigned step) const {
    std::vector<Action> actions;
    generateSimpleActions(actions, step);
    for (auto& action : actions) {
        auto cfgOpt = executeIfValid(action);
        if (cfgOpt.has_value())
            res.push_back(cfgOpt.value());
    }
}

void Configuration::paralyzedNext(std::vector<Configuration>& res, unsigned step, const std::unordered_set<unsigned>& allowed_indices) const {
    std::vector<Action> actions;
    generateParalyzedActions(actions, step, allowed_indices);
    for (auto& action : actions) {
        auto cfgOpt = executeIfValid(action);
        if (cfgOpt.has_value())
            res.push_back(cfgOpt.value());
    }
}

Action Configuration::diff(const Configuration &other) const {
    std::vector<Action::Rotate> rotations;
    std::vector<Action::Reconnect> reconnections;

    ModuleMap moduleMap = other.getModules();
    for (const auto& [id, otherModule] : moduleMap) {
        if (modules.find(id) == modules.end())
            continue;

        const Module &module = modules.at(id);
        for (Joint joint : { Alpha, Beta, Gamma }) {
            auto val = otherModule.getJoint(joint) - module.getJoint(joint);
            if (joint == Gamma) {
                val = clampGamma(val);
            }
            if (val != 0) {
                rotations.emplace_back(id, joint, val);
            }
        }

        const EdgeList &edgeList = edges.at(id);
        const EdgeList &otherEdgeList = other.edges.at(id);
        for (unsigned int i = 0; i < 6; i++) {
            auto &edge = edgeList.at(i);
            auto &otherEdge = otherEdgeList.at(i);

            if (edge == otherEdge)
                continue;

            if (!edge.has_value()) {
                const Edge &edge1 = otherEdge.value();
                if (edge1.id1() < edge1.id2())
                    reconnections.emplace_back(true, otherEdge.value());
            } else if (!otherEdge.has_value()) {
                const Edge &edge1 = edge.value();
                if (edge1.id1() < edge1.id2())
                    reconnections.emplace_back(false, edge.value());
            } else {
                const Edge &edge1 = otherEdge.value();
                if (edge1.id1() < edge1.id2()) {
                    reconnections.emplace_back(true, otherEdge.value());
                    reconnections.emplace_back(false, edge.value());
                }
            }
        }
    }

    return {rotations, reconnections};
}

void Configuration::clearEdges() {
    for (auto& [id, el] : edges) {
        for (auto& opt : el) {
            if (opt)
                opt = std::nullopt;
        }
    }
}

bool Configuration::eraseEdge(const Edge& edge) {
    EdgeList& set1 = edges.at(edge.id1());
    int index1 = edge.side1() * 3 + edge.dock1();

    EdgeList& set2 = edges.at(edge.id2());
    int index2 = edge.side2() * 3 + edge.dock2();

    if (!set1[index1].has_value() || !set2[index2].has_value())
        return false;

    set1[index1] = std::nullopt;
    set2[index2] = std::nullopt;
    connectedVal = Value::Unknown;
}

bool Configuration::execute(const Action::Rotate& action) {
    bool res = modules.at(action.id()).rotateJoint(action.joint(), action.angle());
    if (res)
        matricesVal = Value::Unknown;
    return res;
}

bool Configuration::execute(const Action::Reconnect& action) {
    if (action.add()) {
        return addEdge(action.edge());

    return eraseEdge(action.edge());
}