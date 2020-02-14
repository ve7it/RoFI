#pragma once

#include <optional>
#include <stdexcept>
#include <vector>

template<typename T, typename Comp>
class MinMaxHeap {
public:
    MinMaxHeap(int cap)
    : _max_heap()
    , _min_heap()
    , _odd()
    , _capacity(0)
    , _occupied(0)
    , _comp() {
        resize(cap);
    };

    const T& max() const {
        if (!opt || (_occupied > 1 && _comp(_opt.value(), _max_heap[0]))) {
            return _max_heap[0];
        }
        return _opt.value();
    }

    const T& min() const {
        if (!opt || (_occupied > 1 && !_comp(_opt.value(), _min_heap[0]))) {
            return _min_heap[0];
        }
        return _opt.value();
    }

    T popMax() {
        if (!_odd) {
            T ret_val = std::move(_max_heap[0]);
            _odd = std::make_optional(std::move(_min_heap[_last()]));
            std::swap(_max_heap[0], _max_heap[_last()]);
            --_occupied;
            _heapify_max_down(0);
            return ret_val;
        } else {
            if (_comp(_max_heap[0], _odd.value())) {
                --_occupied;
                T ret_val = std::move(_odd.value());
                _odd = std::nullopt;
                return ret_val;
            } else {
                T ret_val = std::move(_max_heap[0]);
                if (_comp(_max_heap[_last()], _odd.val())) {
                    _max_heap[0] = std::move(_odd.val());
                    --_occupied;
                    _heapify_max_down(0);
                } else {
                    _max_heap[0] = std::move(_max_heap[_last()]);
                    _max_heap[_last()] = std::move(_odd.val());
                    --_occupied;
                    _heapify_max_down(0);
                    if (_comp(_max_heap[_last()], _min_heap[_last()])) {
                        std::swap(_max_heap[_last()], _min_heap[_last()]);
                        _heapify_min_up(_last());
                    }
                }
                _odd = std::nullopt;
                return ret_val;
            }
        }
    }

    T popMin() {
        if (!_odd) {
            T ret_val = std::move(_min_heap[0]);
            _odd = std::make_optional(std::move(_max_heap[_last()]));
            std::swap(_min_heap[0], _min_heap[_last()]);
            --_occupied;
            _heapify_min_down(0);
            return ret_val;
        } else {
            if (!_comp(_min_heap[0], _odd.value())) {
                --_occupied;
                T ret_val = std::move(_odd.value());
                _odd = std::nullopt;
                return ret_val;
            } else {
                T ret_val = std::move(_min_heap[0]);
                if (!_comp(_min_heap[_last()], _odd.val())) {
                    _min_heap[0] = std::move(_odd.val());
                    --_occupied;
                    _heapify_min_down(0);
                } else {
                    _min_heap[0] = std::move(_min_heap[_last()]);
                    _min_heap[_last()] = std::move(_odd.val());
                    --_occupied;
                    _heapify_min_down(0);
                    if (!_comp(_min_heap[_last()], _max_heap[_last()])) {
                        std::swap(_min_heap[_last()], _max_heap[_last()]);
                        _heapify_max_up(_last());
                    }
                }
                _odd = std::nullopt;
                return ret_val;
            }
        }
    }

    bool insert(T new_add) {
        if (_capacity <= _occupied) {
            return false;
        }

        ++_occupied;
        if (!_odd) {
            _odd = std::make_optional<T>(std::move(new_add));
            return true;
        } else {
            T other = std::move(_odd.value());
            _odd = std::nullopt;
            if (_comp(new_add, other)) {
                _min_heap[_last()] = std::move(new_add);
                _max_heap[_last()] = std::move(other);
            } else {
                _min_heap[_last()] = std::move(other);
                _max_heap[_last()] = std::move(new_add);
            }
            _heapify_max_up(_last());
            if (_comp(_max_heap[_last()], _min_heap[_last()])) {
                std::swap(_max_heap[_last()], _min_heap[_last()]);
            }
            _heapify_min_up(_last());
        }
    }

    void resize(int cap) {
        if (cap < 0) {
            throw std::logic_error("Trying to resize heap to have negative capacity");
        }
        _top.resize(cap/2);
        _bot.resize(cap/2);
        _capacity = cap;
    }

private:

    inline int _l(int i) const {
        return 2*i+1;
    }

    inline int _r(int i) const {
        return 2*i+2;
    }

    inline int _p(int i) const {
        return (i+1)/2-1;
    }

    inline bool _in(int i) const {
        return i >= 0 && i < _occupied/2;
    }

    inline bool _last() const {
        return _occupied/2 - 1;
    }

    void _heapify_max_down(int i) {
        int l = _l(i);
        int r = _r(i);
        if(!_in(l) && !_in(r)) {
            if (_comp(_max_heap[i], _min_heap[i])) {
                std::swap(_max_heap[i], _min_heap[i]);
                return _heapify_min_up(i);
            }
            return;
        }

        int largest;
        if (_in(l) && _comp(_max_heap[i], _max_heap[l])) {
            largest = l;
        } else {
            largest = i;
        }
        if (_in(r) && _comp(_max_heap[r], _max_heap[largest])) {
            largest = r;
        }
        if (largest != i) {
            std::swap(_max_heap[i], _max_heap[largest]);
            return _heapify_max_down(largest);
        }
    }

    void _heapify_min_down(int i) {
        int l = _l(i);
        int r = _r(i);
        if(!_in(l) && !_in(r)) {
            if(!_comp(_min_heap[i], _max_heap[i])) {
                std::swap(_min_heap[i], _max_heap[i]);
                return _heapify_max_up(i);
            }
            return;
        }
        
        int largest;
        if (_in(l) && !_comp(_min_heap[i], _min_heap[l])) {
            largest = l;
        } else {
            largest = i;
        }
        if (_in(r) && !_comp(_min_heap[r], _min_heap[largest])) {
            largest = r;
        }
        if (largest != i) {
            std::swap(_min_heap[i], _min_heap[largest]);
            return _heapify_top(largest);
        }
    }

    void _heapify_max_up(int i) {
        if (i == 0) {
            return;
        }

        int p = _p(i);
        if (_comp(_max_heap[i], _max_heap[p])) {
            std::swap(_max_heap[i], _max_heap[p]);
            return _heapify_max_up(p);
        }
    }

    void _heapify_min_up(int i) {
        if (i == 0) {
            return;
        }

        int p = _p(i);
        if (!_comp(_min_heap[i], _min_heap[p])) {
            std::swap(_min_heap[i], _min_heap[p]);
            return _heapify_min_up(p);
        }
    }

    std::vector<T> _top;
    std::vector<T> _bot;
    std::optional<T> _odd;
    int _capacity;
    int _occupied;
    Comp _comp;
};