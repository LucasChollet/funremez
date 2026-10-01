//
//  LolRemez — Remez algorithm implementation
//
//  Copyright © 2005–2022 Sam Hocevar <sam@hocevar.net>
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

//
// Powerful arithmetic expression parser/evaluator
//
// Usage:
//   expression e;
//   e.parse(" 2*x^3 + 3 * sin(x - atan(x))");
//   auto y = e.eval("1.5");
//

#include <cstdint>
#include <vector>
#include <tuple>
#include <cassert>

#include "real.h"

namespace grammar
{

enum class id : uint8_t
{
    /* Variables and constants */
    x, y,
    constant,
    /* Unary functions/operators */
    plus, minus, abs,
    sqrt, cbrt,
    exp, expm1, exp2, erf, erfc, erfcx,
    log, log1p, log2, log10,
    gamma, lgamma,
    sin, cos, tan,
    asin, acos, atan,
    sinh, cosh, tanh,
    /* Binary functions/operators */
    add, sub, mul, div, mod,
    atan2, pow,
    min, max,
    fmod,
    /* Conversion functions */
    tofloat, todouble, toldouble,
};

struct State {
    std::vector<id> temp_op;
    std::vector<std::tuple<id, int>> ops;
    std::vector<real> constants;
};

struct expression
{
    /*
     * Evaluate expression at x
     */
    real eval(real const &x) const;

    /*
     * Is expression constant? i.e. does not depend on x
     */
    bool is_constant() const;

    /*
     * Parse arithmetic expression in x, e.g. 2*x+3
     */
    bool parse(std::string const &str);

private:
    State m_state;
};

} /* namespace grammar */

using grammar::expression;

