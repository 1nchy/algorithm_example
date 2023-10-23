#ifndef _HUFFMAN_CODING_HPP_
#define _HUFFMAN_CODING_HPP_

#include <cassert>

#include <cstdio>
#include <iostream>

#include <functional>
#include <queue>
#include <vector>
#include <unordered_map>

using namespace std;

typedef vector<bool> b_vec;

unordered_map<char, b_vec> huffman_coding(
    const unordered_map<char, size_t>& _freq_table
);

auto huffman_coding(
    const unordered_map<char, size_t>& _freq_table
) -> unordered_map<char, b_vec> {
    struct node {
        char _c; size_t _freq;
        node(size_t _freq) : _freq(_freq) {}
        node(char _c, size_t _freq) : _c(_c), _freq(_freq) {}
    };
    struct btnode : node {
        btnode* _left = nullptr; btnode* _right = nullptr;
        bool _enable = false;
        btnode(size_t _freq) : node(_freq) {}
        btnode(char _c, size_t _freq) : node(_c, _freq), _enable(true) {}
    };
    priority_queue<btnode*, vector<btnode*>, function<bool(const btnode*, const btnode*)>> _q([&](const btnode* _a, const btnode* _b) {
        return _a->_freq > _b->_freq;
    });
    for (const auto& _i : _freq_table) {
        btnode* const _p = new btnode(_i.first, _i.second);
        _q.emplace(_p);
    }
    unordered_map<char, b_vec> _ans;
    while (_q.size() > 1) {
        auto* const _a = _q.top(); _q.pop();
        auto* const _b = _q.top(); _q.pop();
        btnode* const _p = new btnode(_a->_freq + _b->_freq);
        _p->_left = _a; _p->_right = _b;
        _q.emplace(_p);
    }
    b_vec _code; _code.reserve(_freq_table.size());
    function<void(btnode*)> dfs = [&](btnode* _p) { // left 0, right 1
        assert(_p != nullptr);
        if (_p->_enable) {
            _ans[_p->_c] = _code;
        }
        else {
            _code.push_back(0); dfs(_p->_left); _code.pop_back();
            _code.push_back(1); dfs(_p->_right); _code.pop_back();
        }
        delete _p;
    };
    dfs(_q.top());
    return _ans;
};

ostream& operator<<(ostream& _os, const b_vec& _b) {
    _os << '[';
    for (size_t _i = 0; _i < _b.size();) {
        _os << _b[_i++];
        // if (_i != _b.size()) {
        //     _os << ", ";
        // }
    }
    return _os << "]";
}

#endif // _HUFFMAN_CODING_HPP_