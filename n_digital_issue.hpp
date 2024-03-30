#ifndef _N_DIGITAL_ISSUE_HPP_
#define _N_DIGITAL_ISSUE_HPP_

#include <iostream>
#include <termios.h>
#include <cmath>
#include <iomanip>

#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <random>

#include <cassert>

enum direct_t {
    up, down, left, right
};

int get_char();

template <size_t _N> class table_t {
    typedef unsigned element_type;
    struct point_t {
        point_t(size_t _x = 0, size_t _y = 0) : _x(_x), _y(_y) {}
        point_t(const point_t&) = default;
        point_t& operator=(const point_t&) = default;
        size_t _x; size_t _y;
    };
public:
    table_t(std::initializer_list<std::initializer_list<element_type>> _ill);
    table_t(const table_t<_N>&) = default;
    table_t<_N>& operator=(const table_t<_N>&) = default;
    std::vector<direct_t> n_digital_issue();
    bool solvable() const;
    void demo();
    void shuffle();

private:
    element_type& operator[](const point_t& _p) { return _data[_p._x][_p._y]; }
    const element_type& operator[](const point_t& _p) const { return _data[_p._x][_p._y]; }

    bool left();
    bool right();
    bool up();
    bool down();

    // 可采纳性
    size_t evaluate() const;
    // signature
    size_t signature() const;

    void print() const;
    void clear_print() const;
    bool solved() const { return evaluate() == 0; }

    std::vector<std::vector<element_type>> _data;
    point_t _blank;
    size_t _max_digit_num;
};

template <size_t _N> table_t<_N>::table_t(std::initializer_list<std::initializer_list<element_type>> _ill) {
    assert(_ill.size() == _N);
    bool _blank_found = false;
    std::vector<bool> _digit_map(_N * _N, 0);
    size_t _i = 0;
    for (const auto& _il : _ill) {
        assert(_il.size() == _N);
        size_t _j = 0;
        for (const auto& _k : _il) {
            assert(_k < _digit_map.size());
            assert(!_digit_map[_k]);
            if (_k == 0) {
                assert(!_blank_found);
                _blank = {_i, _j}; _blank_found = true;
            }
            _digit_map[_k] = true;
            ++_j;
        }
        _data.emplace_back(_il); ++_i;
    }
    _max_digit_num = (std::log10(_N * _N) + 1);
}

template <size_t _N> auto table_t<_N>::up() -> bool {
    if (_blank._x == _N - 1) return false;
    auto _new_blank = _blank; ++_new_blank._x;
    std::swap((*this)[_blank], (*this)[_new_blank]);
    _blank = _new_blank;
    return true;
};
template <size_t _N> auto table_t<_N>::down() -> bool {
    if (_blank._x == 0) return false;
    auto _new_blank = _blank; --_new_blank._x;
    std::swap((*this)[_blank], (*this)[_new_blank]);
    _blank = _new_blank;
    return true;
};
template <size_t _N> auto table_t<_N>::left() -> bool {
    if (_blank._y == _N - 1) return false;
    auto _new_blank = _blank; ++_new_blank._y;
    std::swap((*this)[_blank], (*this)[_new_blank]);
    _blank = _new_blank;
    return true;
};
template <size_t _N> auto table_t<_N>::right() -> bool {
    if (_blank._y == 0) return false;
    auto _new_blank = _blank; --_new_blank._y;
    std::swap((*this)[_blank], (*this)[_new_blank]);
    _blank = _new_blank;
    return true;
};

template <size_t _N> auto table_t<_N>::evaluate() const -> size_t {
    size_t _cost = 0;
    for (size_t _i = 0; _i < _N; ++_i) {
        for (size_t _j = 0; _j < _N; ++_j) {
            const size_t _k = (_i * _N + _j + 1) % (_N * _N);
            if (_data[_i][_j] != _k) {
                ++_cost;
            }
        }
    }
    return _cost;
};
template <size_t _N> auto table_t<_N>::signature() const -> size_t {
    size_t _seed = _N * _N;
    for (const auto& _vi : _data) {
        for (const auto& _i : _vi) {
            _seed ^= _i + 0x9e3779b9 + (_seed << 6) + (_seed >> 2);
        }
    }
    return _seed;
};

template <size_t _N> auto table_t<_N>::n_digital_issue()
-> std::vector<direct_t> {
    struct node {
        node() = delete;
        node(const table_t<_N>& _t) : _t(_t) { _cost = _t.evaluate(); }
        node(const node&) = default;
        node& operator=(const node&) = default;
        size_t _step = 0;
        size_t _cost = 0;
        table_t<_N> _t;
    };
    struct tree_node : node {
        tree_node() = delete;
        tree_node(const table_t<_N>& _t) : node(_t) {}
        tree_node(const tree_node& _rhs) : node(_rhs) {}
        tree_node& operator=(const tree_node&) = delete;
        tree_node* _parent = nullptr;
        tree_node* _up = nullptr;
        tree_node* _down = nullptr;
        tree_node* _left = nullptr;
        tree_node* _right = nullptr;
    };
    std::priority_queue<tree_node*, std::vector<tree_node*>,
                        std::function<bool(const tree_node*, const tree_node*)>>
                        _q ([&](const tree_node* _a, const tree_node* _b) {
                            return _a->_cost > _b->_cost;
                        });
    tree_node _root(*this);
    auto last_action = [&](const tree_node* const _p, direct_t _dir) -> bool {
        if (_p->_parent == nullptr) return false;
        const tree_node* const _pp = _p->_parent;
        if (_dir == direct_t::up && _p == _pp->_up) return true;
        if (_dir == direct_t::down && _p == _pp->_down) return true;
        if (_dir == direct_t::left && _p == _pp->_left) return true;
        if (_dir == direct_t::right && _p == _pp->_right) return true;
        return false;
    };
    auto leaf_node = [&](const tree_node* const _p) -> bool {
        assert(_p != nullptr);
        return _p->_up == nullptr && _p->_down == nullptr && _p->_left == nullptr && _p->_right == nullptr;
    };
    _q.push(&_root);
    std::unordered_map<size_t, size_t> _visited; // {signature, cost}
    _visited[_root._t.signature()] = _root._cost;
    const tree_node* _target = nullptr;
    size_t _max_cost = std::numeric_limits<size_t>::max();
    while (!_q.empty()) {
        tree_node* const _s = _q.top(); _q.pop();
        assert(leaf_node(_s));
        if (_s->_cost >= _max_cost) continue;
        const auto _s_sign = _s->_t.signature();
        if (_visited.count(_s_sign) && _visited[_s_sign] < _s->_cost) continue;
        if (_s->_cost == _s->_step) {
            _target = _s;
            _max_cost = std::min(_max_cost, _s->_cost);
            continue;
        }
        if (!last_action(_s, direct_t::down)) {
            tree_node* const _up = new tree_node(*_s);
            if (_up->_t.up()) {
                ++_up->_step;
                _up->_cost = _up->_t.evaluate() + _up->_step;
                const auto _sign = _up->_t.signature();
                if ((_visited.count(_sign) && _visited[_sign] < _up->_cost) || _up->_cost >= _max_cost) {
                    delete _up;
                }
                else {
                    assert(leaf_node(_up));
                    _up->_parent = _s; _s->_up = _up;
                    _q.push(_up);
                    _visited[_sign] = _up->_cost;
                }
            }
            else delete _up; 
        }
        if (!last_action(_s, direct_t::up)) {
            tree_node* const _down = new tree_node(*_s);
            if (_down->_t.down()) {
                ++_down->_step;
                _down->_cost = _down->_t.evaluate() + _down->_step;
                const auto _sign = _down->_t.signature();
                if ((_visited.count(_sign) && _visited[_sign] < _down->_cost) || _down->_cost >= _max_cost) {
                    delete _down;
                }
                else {
                    assert(leaf_node(_down));
                    _down->_parent = _s; _s->_down = _down;
                    _q.push(_down);
                    _visited[_sign] = _down->_cost;
                }
            }
            else delete _down;
        }
        if (!last_action(_s, direct_t::right)) {
            tree_node* const _left = new tree_node(*_s);
            if (_left->_t.left()) {
                ++_left->_step;
                _left->_cost = _left->_t.evaluate() + _left->_step;
                const auto _sign = _left->_t.signature();
                if ((_visited.count(_sign) && _visited[_sign] < _left->_cost) || _left->_cost >= _max_cost) {
                    delete _left;
                }
                else {
                    assert(leaf_node(_left));
                    _left->_parent = _s; _s->_left = _left;
                    _q.push(_left);
                    _visited[_sign] = _left->_cost;
                }
            }
            else delete _left;
        }
        if (!last_action(_s, direct_t::left)) {
            tree_node* const _right = new tree_node(*_s);
            if (_right->_t.right()) {
                ++_right->_step;
                _right->_cost = _right->_t.evaluate() + _right->_step;
                const auto _sign = _right->_t.signature();
                if ((_visited.count(_sign) && _visited[_sign] < _right->_cost) || _right->_cost >= _max_cost) {
                    delete _right;
                }
                else {
                    assert(leaf_node(_right));
                    _right->_parent = _s; _s->_right = _right;
                    _q.push(_right);
                    _visited[_sign] = _right->_cost;
                }
            }
            else delete _right;
        }
    }
    if (_target == nullptr) {
        return {};
    }
    assert(leaf_node(_target));
    std::cout << "visited.size() = " << _visited.size() << std::endl;
    std::vector<direct_t> _path; _path.reserve(_target->_step);
    for (const tree_node* _i = _target; _i != &_root; _i = _i->_parent) {
        const tree_node* const _ip = _i->_parent;
        assert(_ip != nullptr);
        if (_i == _ip->_left) {
            _path.push_back(direct_t::left);
        }
        else if (_i == _ip->_right) {
            _path.push_back(direct_t::right);
        }
        else if (_i == _ip->_up) {
            _path.push_back(direct_t::up);
        }
        else if (_i == _ip->_down) {
            _path.push_back(direct_t::down);
        }
        else {
            assert(false);
        }
    }
    std::reverse(_path.begin(), _path.end());
    std::function<void(tree_node*)> dfs = [&](tree_node* _p) {
        if (_p == nullptr) return;
        dfs(_p->_up); _p->_up = nullptr;
        dfs(_p->_down); _p->_down = nullptr;
        dfs(_p->_left); _p->_left = nullptr;
        dfs(_p->_right); _p->_right = nullptr;
        if (_p != &_root) {
            delete _p;
        }
    };
    dfs(&_root);
    return _path;
};

template <size_t _N> auto table_t<_N>::solvable() const -> bool {
    size_t _tau = ((_blank._x + _blank._y) % 2 == 0 || _N % 2 == 1 ? 0 : 1);
    auto less_cnt = [&](size_t _i, size_t _j) -> size_t {
        const size_t _k = (this->_data[_i][_j] == 0 ? _N * _N : this->_data[_i][_j]);
        size_t _cnt = 0;
        for (; _i < _N; ++_i) {
            for (; _j < _N; ++_j) {
                if (this->_data[_i][_j] != 0 && this->_data[_i][_j] < _k) {
                    ++_cnt;
                }
            }
        }
        return _cnt;
    };
    size_t _sum = _tau;
    for (size_t _i = 0; _i < _N; ++_i) {
        for (size_t _j = 0; _j < _N; ++_j) {
            _sum += less_cnt(_i, _j);
        }
    }
    return _sum % 2 == 0;
};

template <size_t _N> auto table_t<_N>::demo() -> void {
    system("stty cooked");
    termios _new_setting, _init_setting;
    tcgetattr(fileno(stdin), &_init_setting);
    _new_setting = _init_setting;
    _new_setting.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), TCSANOW, &_new_setting);
    this->print();
    while (!solved()) {
        int _c = get_char();
        if (_c == 'a') { // left
            if (!this->left()) {
                continue;
            }
        }
        else if (_c == 'd') { // right
            if (!this->right()) {
                continue;
            }
        }
        else if (_c == 'w') { // up
            if (!this->up()) {
                continue;
            }
        }
        else if (_c == 's') { // down
            if (!this->down()) {
                continue;
            }
        }
        else {
            break;
        }
        this->clear_print();
        this->print();
    }
    tcsetattr(fileno(stdin), TCSANOW, &_init_setting);
    system("stty raw");
    if (this->solved()) {
        std::cout << "solve successfully" << std::endl;
    }
    else {
        std::cout << "solve failed" << std::endl;
    }
};

template <size_t _N> auto table_t<_N>::shuffle() -> void {
    auto serial_2_point = [&](size_t _s) -> point_t {
        return point_t(_s / _N, _s % _N);
    };
    std::random_device _rd;
    std::mt19937 _gen(_rd());
    for (size_t _i = 0; _i < _N * _N; ++_i) {
        std::uniform_int_distribution<size_t> _distrib(_i, _N * _N - 1);
        const size_t _r = _distrib(_gen);
        std::swap((*this)[serial_2_point(_i)], (*this)[serial_2_point(_r)]);
        const point_t _p = serial_2_point(_i);
        std::swap((*this)[_p], (*this)[serial_2_point(_r)]);
        if ((*this)[_p] == 0) {
            _blank = _p;
        }
    }
};


template <size_t _N> auto table_t<_N>::print() const -> void {
    for (size_t _i = 0; _i < _N; ++_i) {
        std::cout << "[";
        for (size_t _j = 0; _j < _N;) {
            std::cout << std::setw(_max_digit_num + 1) << _data[_i][_j++];
            // if (_j < _N) { std::cout << "\t"; }
        }
        std::cout << " ]" << std::endl;
    }
};
template <size_t _N> auto table_t<_N>::clear_print() const -> void {
    // std::cout << "\b \b";
    for (size_t _i = 0; _i < _N; ++_i) {
        std::cout << "\033[A";
    }
};

int get_char() {
    int in;
    struct termios new_settings;
    struct termios stored_settings;
    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0,&stored_settings);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);

    in = getchar();

    tcsetattr(0,TCSANOW,&stored_settings);
    return in;
}

#endif // _N_DIGITAL_ISSUE_HPP_