#ifndef _KNAPSACK2_HPP_
#define _KNAPSACK2_HPP_

#include <algorithm>
#include <cassert>
#include <queue>
#include <numeric>
#include <vector>

#include <cstdio>
#include <iostream>

using namespace std;

void value_density_sort(
    vector<size_t>& _weight,
    vector<size_t>& _value
);

size_t knap_sack2(
    const vector<size_t>& _weight,
    const vector<size_t>& _value,
    size_t _capacity
);

auto knap_sack2(
    const vector<size_t>& _weight,
    const vector<size_t>& _value,
    size_t _capacity
) -> size_t {
    const size_t _n = _weight.size();
    const size_t _epsilon = 1;
    size_t _max_value = 0; // max value we've reached
    size_t _pred_vl; // lower bound of predicted value
    size_t _pred_vu; // upper bound of predicted value
    struct node {
        size_t _cc; // current capacity
        size_t _cv; // current value
        size_t _clb; // current lower bound
        size_t _cub; // current upper bound
        size_t _i; // serial number
        node(size_t _c, size_t _v, size_t _i, size_t _clb, size_t _cub)
         : _cc(_c), _cv(_v), _i(_i), _clb(_clb), _cub(_cub) {}
    };
    struct btnode : public node {
        btnode* _left = nullptr; // picked
        btnode* _right = nullptr; // not picked
        btnode* _parent = nullptr;
        btnode(size_t _c, size_t _v, size_t _i, size_t _clb, size_t _cub) : node(_c, _v, _i, _clb, _cub) {}
    };
    auto predict_bound = [&](size_t _cc, size_t _cv, size_t _k) { // predict bound of value
        _pred_vl = _cv;
        for (size_t _i = _k; _i < _n; ++_i) {
            if (_cc < _weight[_i]) { // can't pick a complete item
                _pred_vu = _pred_vl + _cc * _value[_i] / float(_weight[_i]);
                for (size_t _j = _i + 1; _j < _n; ++_j) {
                    if (_cc >= _weight[_j]) {
                        _cc -= _weight[_j]; _pred_vl += _value[_j];
                    }
                }
                return;
            }
            _cc -= _weight[_i]; _pred_vl += _value[_i];
        }
        _pred_vu = _pred_vl;
        assert(_pred_vu >= _pred_vl);
    };
    priority_queue<btnode*, vector<btnode*>, function<bool(const btnode*, const btnode*)>> _q(
        [&](const btnode* _a, const btnode* _b) -> bool {
            return _a->_cub < _b->_cub;
        }
    );
    predict_bound(_capacity, 0, 0);
    _max_value = (_pred_vl <= _epsilon ? 0 : _pred_vl - _epsilon);
    btnode _root(_capacity, 0, 0, _pred_vl, _pred_vu);
    _q.emplace(&_root);
    btnode* _ans_node = nullptr;
    while (!_q.empty()) {
        auto* const _p = _q.top(); _q.pop();
        if (_p->_cub <= _max_value) break;
        size_t _i = _p->_i;
        if (_p->_i == _n) {
            _max_value = _p->_cv; _ans_node = _p; continue;
        }
        size_t _cap = _p->_cc; size_t _val = _p->_cv; btnode* _s = nullptr;
        if (_cap >= _weight[_i]) { // could pick _i
            _s = new btnode(_cap - _weight[_i], _val + _value[_i], _i+1, _p->_clb, _p->_cub);
            _p->_left = _s; _s->_parent = _p;
            _q.emplace(_s);
        }
        // if we haven't picked _i
        predict_bound(_cap, _val, _i+1);
        if (_pred_vu > _max_value) {
            _s = new btnode(_cap, _val, _i+1, _pred_vl, _pred_vu);
            _p->_right = _s; _s->_parent = _p;
            _max_value = max(_max_value, (_pred_vl <= _epsilon ? 0 : _pred_vl - _epsilon));
            _q.emplace(_s);
        }
    }
    auto traceback = [&](const btnode* _s) -> size_t {
        if (_ans_node == nullptr) return 0;
        assert(_ans_node->_i == _n);
        vector<bool> _vb; _vb.reserve(_n);
        size_t _total_value = 0;
        for (btnode* _p = _ans_node; _p != nullptr && _p->_i != 0; _p = _p->_parent) {
            btnode* const _pp = _p->_parent;
            _vb[_pp->_i] = (_pp->_left == _p);
        }
        cout << '[';
        for (size_t _i = 0; _i != _n;) {
            if (_vb[_i]) {
                _total_value += _value[_i];
            }
            cout << _vb[_i++];
            if (_i != _n) cout << ", ";
        }
        cout << ']' << endl;
        return _total_value;
    };
    function<void(btnode*)> dfs = [&](btnode* _p) {
        if (_p == nullptr) return;
        dfs(_p->_left); dfs(_p->_right);
        if (_p != &_root) delete(_p);
    };
    auto _total_value = traceback(_ans_node);
    dfs(&_root);
    return _total_value;
};

auto value_density_sort(
    vector<size_t>& _weight,
    vector<size_t>& _value
) -> void {
    assert(_weight.size() == _value.size());
    const size_t _n = _weight.size();
    vector<size_t> _indices(_n, 0);
    iota(_indices.begin(), _indices.end(), 0);
    sort(_indices.begin(), _indices.end(), [&](const auto& _a, const auto& _b) {
        return _value[_a] / (float)_weight[_a] > _value[_b] / (float)_weight[_b];
    });
    vector<bool> _placed(_n, false);
    for (size_t _i = 0; _i != _n; ++_i) {
        if (_placed[_i]) continue;
        size_t _tmpw = _weight[_i]; size_t _tmpv = _value[_i]; size_t _j;
        for (_j = _i; _indices[_j] != _i; _j = _indices[_j]) {
            _weight[_j] = _weight[_indices[_j]];
            _value[_j] = _value[_indices[_j]];
            _placed[_j] = true;
        }
        _weight[_j] = _tmpw;
        _value[_j] = _tmpv;
        _placed[_j] = true;
    }
}

// int main(void) {
//     vector<size_t> _weight {2,4,6,9};
//     vector<size_t> _value {10,10,12,18};
//     knap_sack2(_weight, _value, 15);
//     return 0;
// }


#endif // _KNAPSACK2_HPP_