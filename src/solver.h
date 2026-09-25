//
//  LolRemez — Remez algorithm implementation
//
//  Copyright © 2005–2023 Sam Hocevar <sam@hocevar.net>
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

//
// The remez_solver class
// ----------------------
//

#include "real.h"

#include <array>
#include <thread>
#include <vector>

#include "expression.h"
#include "polynomial.h"
#include "ThreadQueue.h"

enum class root_finder
{
    bisect,
    regula_falsi,
    illinois,
    pegasus,
    ford,
};

class remez_solver
{
public:
    remez_solver();
    ~remez_solver();

    enum class format
    {
        gnuplot,
        cpp,
    };

    void set_order(int order);
    void set_digits(int digits);
    void set_range(real xmin, real xmax);
    void set_func(expression const &expr);
    void set_weight(expression const &expr);
    void set_root_finder(root_finder rf);

    bool check_sanity() const;

    void do_init();
    bool do_step();

    polynomial<real> get_estimate() const;
    real get_error() const { return m_error; }

    bool show_stats = false;
    bool show_debug = false;

private:
    void remez_init();
    void remez_step();

    void find_zeros();
    void find_extrema();

    void worker_thread();

    real eval_estimate(real const &x);
    real eval_func(real const &x);
    real eval_weight(real const &x);
    real eval_error(real const &x);

private:
    /* User-defined parameters */
    expression m_func, m_weight;
    real m_xmin = -real::R_1();
    real m_xmax = +real::R_1();
    int m_order = 4;
    int m_digits = 40;
    bool m_has_weight = false;
    root_finder m_rf = root_finder::pegasus;

    /* Solver state */
    polynomial<real> m_estimate;

    std::vector<real> m_zeros;
    std::vector<real> m_control;

    real m_k1, m_k2, m_epsilon, m_error;

    struct point
    {
        real x, err;
    };

    std::vector<std::array<point, 3>> m_zeros_state;
    std::vector<std::array<point, 3>> m_extrema_state;

    /* Threading information */
    std::vector<std::jthread> m_workers;
    ThreadQueue<int> m_questions, m_answers;
};

