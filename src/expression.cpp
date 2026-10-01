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

bool expression::is_constant() const
{
    for (auto const& op : m_ops)
        if (std::get<0>(op) == id::x)
            return false;

    return true;
}

} /* namespace grammar */
