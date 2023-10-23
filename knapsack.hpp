#ifndef _KNAPSACK_HPP_
#define _KNAPSACK_HPP_

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#include <cstdio>

using namespace std;

int knap_sack(
    const vector<int>& _weight,
    const vector<int>& _value,
    int _capacity
);
void knap_sack_traceback(
    const vector<int>& _weight,
    const vector<vector<int>>& _w,
    const vector<vector<int>>& _v
);


auto knap_sack(
    const vector<int>& _weight,
    const vector<int>& _value,
    int _capacity
) -> int {
    // JNS jump nodes set
    assert(_weight.size() == _value.size());
    const size_t _n = _weight.size();
    vector<vector<int>> _w(_n+1); // _w[i][j] means weight of plan %j in %i-th JNS.
    vector<vector<int>> _v(_n+1); // _v[i][j] means value of plan %j in %i-th JNS.
    // assert(_w[_i].size() == _v[_i].size()), elements in _w[_i] and _v[_i] are sorted.
    _w[0].emplace_back(0); _v[0].emplace_back(0);

    for (size_t _i = 0; _i < _n; ++_i) { // S_{i} -> S_{i+1}
        // traverse _w[_i] and _v[_i]
        auto& _wi = _w[_i]; auto& _vi = _v[_i];
        assert(_wi.size() == _vi.size());
        const size_t _ni = _wi.size(); // size of S_{i}
        size_t _k = 0; // index of nodes in S_{i}, which could appear in next JNS.
        auto& _wI = _w[_i+1]; auto& _vI = _v[_i+1]; // JNS
        for (size_t _j = 0; _j < _ni; ++_j) { // traverse S_{i} to generate P
            // calculate the position of each node in S_{i} where they could appear in next JNS.
            // %<_wsi, _vsi> = P
            const int _wsi = _wi[_j] + _weight[_i]; // total weight if selected %_i
            if (_wsi > _capacity) break; // overloaded
            int _vsi = _vi[_j] + _value[_i]; // total value if selected %_i
            while (_k < _ni && _wi[_k] < _wsi) { // nodes in S_{i} on the left of P, they have chance to appear in next JNS.
                _wI.emplace_back(_wi[_k]);
                _vI.emplace_back(_vi[_k]);
                ++_k;
            }
            if (_k < _ni && _wi[_k] == _wsi) { // node in S_{i}, which has the same weight with P, update P if necessary
                _vsi = max(_vsi, _vi[_k]);
                ++_k;
            }
            if (_vI.empty() || _vsi > _vI.back()) { // insert P into next JNS
                _wI.emplace_back(_wsi);
                _vI.emplace_back(_vsi);
            }
            // all nodes on the lower right corner of P would be eliminated
            while (_k < _ni && _vi[_k] < _vI.back()) { // _vI can't be empty
                ++_k;
            }
        }
        while (_k < _ni) { // insert remained nodes in S_{i} into JNS
            _wI.emplace_back(_wi[_k]);
            _vI.emplace_back(_vi[_k]);
            ++_k;
        }
    }
    knap_sack_traceback(_weight, _w, _v);
    return _v[_n].back();
}

auto knap_sack_traceback(
    const vector<int>& _weight,
    const vector<vector<int>>& _w,
    const vector<vector<int>>& _v
) -> void {
    assert(_w.size() == _v.size());
    const size_t _n = _weight.size();
    const size_t _ns = _w.back().size();
    vector<int> _indices(_ns);
    iota(_indices.begin(), _indices.end(), 0);
    vector<int> _serial(_n, 0);

    auto find = [&](int _x, int _k) -> bool {
        const auto& _wk = _w[_k];
        return binary_search(_wk.begin(), _wk.end(), _x);
    };

    int _wx = -1;
    for (size_t _i = _n; _i > 0; --_i) {
        const auto& _wi = _w[_i];
        const auto& _vi = _v[_i];
        assert(_wi.size() == _vi.size() && !_wi.empty());
        if (_wx == -1) _wx = _wi.back();
        if (!find(_wx, _i-1)) {
            _serial[_i-1] = 1;
            _wx -= _weight[_i-1];
        }
        else {
            _serial[_i-1] = 0;
            _wx = -1;
        }
    }
    printf("[");
    for (size_t _i = 0; _i < _serial.size(); ++_i) {
        printf("%d", _serial[_i]);
        if (_i+1 != _serial.size()) {
            printf(", ");
        }
    }
    printf("].\n");
}
#endif // _KNAPSACK_HPP_