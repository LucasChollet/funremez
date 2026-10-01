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

#include "expression.h"

namespace grammar
{

real expression::eval(real const& x) const
{
    /* Use a stack */
    std::vector<real> stack;

    auto pop_val = [&stack]() -> real
    {
        auto ret = stack.back();
        stack.pop_back();
        return ret;
    };

    auto push_val = [&stack](real const& v) -> void
    {
        stack.push_back(v);
    };

    for (size_t i = 0; i < m_ops.size(); ++i)
    {
        /* Rules that do not consume stack elements */
        if (std::get<0>(m_ops[i]) == id::x)
        {
            push_val(x);
            continue;
        }
        else if (std::get<0>(m_ops[i]) == id::y)
        {
            push_val(0); // TODO
            continue;
        }
        else if (std::get<0>(m_ops[i]) == id::constant)
        {
            push_val(m_constants[std::get<1>(m_ops[i])]);
            continue;
        }

        /* All other rules consume at least the head of the stack */
        real head = pop_val();

        switch (std::get<0>(m_ops[i]))
        {
        case id::plus: push_val(head);
            break;
        case id::minus: push_val(-head);
            break;

        case id::abs: push_val(fabs(head));
            break;
        case id::sqrt: push_val(sqrt(head));
            break;
        case id::cbrt: push_val(cbrt(head));
            break;
        case id::exp: push_val(exp(head));
            break;
        case id::expm1: push_val(expm1(head));
            break;
        case id::exp2: push_val(exp2(head));
            break;
        case id::erf: push_val(erf(head));
            break;
        case id::erfc: push_val(erfc(head));
            break;
        case id::erfcx: push_val(erfcx(head));
            break;
        case id::log: push_val(log(head));
            break;
        case id::log1p: push_val(log1p(head));
            break;
        case id::log2: push_val(log2(head));
            break;
        case id::log10: push_val(log10(head));
            break;
        case id::gamma: push_val(gamma(head));
            break;
        case id::lgamma: push_val(lgamma(head));
            break;
        case id::sin: push_val(sin(head));
            break;
        case id::cos: push_val(cos(head));
            break;
        case id::tan: push_val(tan(head));
            break;
        case id::asin: push_val(asin(head));
            break;
        case id::acos: push_val(acos(head));
            break;
        case id::atan: push_val(atan(head));
            break;
        case id::sinh: push_val(sinh(head));
            break;
        case id::cosh: push_val(cosh(head));
            break;
        case id::tanh: push_val(tanh(head));
            break;

        case id::add: push_val(pop_val() + head);
            break;
        case id::sub: push_val(pop_val() - head);
            break;
        case id::mul: push_val(pop_val() * head);
            break;
        case id::div: push_val(pop_val() / head);
            break;

        case id::atan2: push_val(atan2(pop_val(), head));
            break;
        case id::pow: push_val(pow(pop_val(), head));
            break;
        case id::min: push_val(min(pop_val(), head));
            break;
        case id::max: push_val(max(pop_val(), head));
            break;
        case id::mod:
        case id::fmod: push_val(fmod(pop_val(), head));
            break;

        case id::tofloat: push_val(real(float(head)));
            break;
        case id::todouble: push_val(real(double(head)));
            break;
        case id::toldouble: push_val(real(long_double(head)));
            break;

        case id::x:
        case id::y:
        case id::constant:
            /* Already handled above */
            break;
        }
    }

    assert(stack.size() == 1);
    return pop_val();
}

bool expression::is_constant() const
{
    for (auto const& op : m_ops)
        if (std::get<0>(op) == id::x)
            return false;

    return true;
}

bool expression::parse(std::string const& str)
{
    m_ops.clear();
    m_constants.clear();

    tao::pegtl::memory_input<> in(str, "expression");
    try
    {
        tao::pegtl::parse<r_stmt, action>(in, this);
        return true;
    }
    catch (const tao::pegtl::parse_error& ex)
    {
        printf("parse error: %s\n", ex.what());
        return false;
    }
}

} /* namespace grammar */
