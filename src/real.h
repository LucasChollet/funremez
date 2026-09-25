//
//  LolRemez — Remez algorithm implementation
//
//  Copyright © 2005–2026 Sam Hocevar <sam@hocevar.net>
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//
//  This file was imported from the Lol Engine and the `lol::real`
//  class is now a local `real` class in the global namespace. The
//  original implementation used custom big-integer arithmetic; ours
//  is backed by the GNU MPFR library, which provides arbitrary
//  precision floating-point arithmetic through the C functions of
//  <mpfr.h>. The default precision is 512 bits (16 32-bit bigits),
//  matching the original `lol::real`.
//

#pragma once

#include <ostream>      // std::ostream
#include <string>       // std::to_string()
#include <type_traits>  // std::is_arithmetic_v

#include <mpfr.h>

class real
{
public:
    /* Default construction gives zero, at the current global precision */
    real()
    {
        mpfr_init2(m_v, s_precision);
    }

    /* Implicit construction from any arithmetic type */
    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    real(T v)
    {
        mpfr_init2(m_v, s_precision);
        if constexpr (std::is_integral_v<T>)
            (void)mpfr_set_str(m_v, std::to_string(v).c_str(), 10, MPFR_RNDN);
        else
            (void)mpfr_set_ld(m_v, static_cast<long double>(v), MPFR_RNDN);
    }

    /* Parse a number from a string, e.g. "1.5", "1e-3" or "0x1.8p1" */
    real(char const *s)
    {
        mpfr_init2(m_v, s_precision);
        (void)mpfr_set_str(m_v, s, 0, MPFR_RNDN);
    }

    /* Copy semantics */
    real(real const &o)
    {
        mpfr_init2(m_v, mpfr_get_prec(o.m_v));
        mpfr_set(m_v, o.m_v, MPFR_RNDN);
    }

    real(real &&o) noexcept
    {
        mpfr_init2(m_v, s_precision);
        mpfr_swap(m_v, o.m_v);
    }

    real &operator=(real const &o)
    {
        if (this != &o)
        {
            mpfr_set_prec(m_v, mpfr_get_prec(o.m_v));
            mpfr_set(m_v, o.m_v, MPFR_RNDN);
        }
        return *this;
    }

    real &operator=(real &&o) noexcept
    {
        if (this != &o)
            mpfr_swap(m_v, o.m_v);
        return *this;
    }

    ~real() { mpfr_clear(m_v); }

    /* Explicit conversions back to native floating-point types */
    explicit operator float() const       { return mpfr_get_flt(m_v, MPFR_RNDN); }
    explicit operator double() const      { return mpfr_get_d(m_v, MPFR_RNDN); }
    explicit operator long double() const { return mpfr_get_ld(m_v, MPFR_RNDN); }

    /* Boolean conversion, e.g. for `if (!a[i][i])` */
    explicit operator bool() const { return !mpfr_zero_p(m_v); }

    /* Equality and ordering */
    friend bool operator==(real const &a, real const &b) { return mpfr_cmp(a.m_v, b.m_v) == 0; }
    friend bool operator!=(real const &a, real const &b) { return mpfr_cmp(a.m_v, b.m_v) != 0; }
    friend bool operator< (real const &a, real const &b) { return mpfr_cmp(a.m_v, b.m_v) <  0; }
    friend bool operator<=(real const &a, real const &b) { return mpfr_cmp(a.m_v, b.m_v) <= 0; }
    friend bool operator> (real const &a, real const &b) { return mpfr_cmp(a.m_v, b.m_v) >  0; }
    friend bool operator>=(real const &a, real const &b) { return mpfr_cmp(a.m_v, b.m_v) >= 0; }

    /* Arithmetic */
    real &operator+=(real const &o) { mpfr_add(m_v, m_v, o.m_v, MPFR_RNDN); return *this; }
    real &operator-=(real const &o) { mpfr_sub(m_v, m_v, o.m_v, MPFR_RNDN); return *this; }
    real &operator*=(real const &o) { mpfr_mul(m_v, m_v, o.m_v, MPFR_RNDN); return *this; }
    real &operator/=(real const &o) { mpfr_div(m_v, m_v, o.m_v, MPFR_RNDN); return *this; }

    real operator+() const { return *this; }
    real operator-() const { real r; mpfr_neg(r.m_v, m_v, MPFR_RNDN); return r; }

    friend real operator+(real const &a, real const &b) { real r; mpfr_add(r.m_v, a.m_v, b.m_v, MPFR_RNDN); return r; }
    friend real operator-(real const &a, real const &b) { real r; mpfr_sub(r.m_v, a.m_v, b.m_v, MPFR_RNDN); return r; }
    friend real operator*(real const &a, real const &b) { real r; mpfr_mul(r.m_v, a.m_v, b.m_v, MPFR_RNDN); return r; }
    friend real operator/(real const &a, real const &b) { real r; mpfr_div(r.m_v, a.m_v, b.m_v, MPFR_RNDN); return r; }

    /* Output, honouring the stream’s precision: same "%g" semantics as the
     * default floating-point format, with as many significant digits as the
     * caller requested through std::setprecision(). */
    friend std::ostream &operator<<(std::ostream &os, real const &r)
    {
        char *buf = nullptr;
        if (mpfr_asprintf(&buf, "%.*Rg", os.precision(), r.m_v) < 0 || buf == nullptr)
            return os;
        os << buf;
        mpfr_free_str(buf);
        return os;
    }

    /* Predicates */
    bool is_zero() const { return mpfr_zero_p(m_v); }
    bool is_negative() const { return mpfr_sgn(m_v) < 0; }

    /* Constants */
    static real R_0()  { return real(0); }
    static real R_1()  { return real(1); }
    static real R_10() { return real(10); }
    static real R_E()
    {
        real r(1);
        mpfr_exp(r.m_v, r.m_v, MPFR_RNDN);
        return r;
    }
    static real R_PI()
    {
        real r;
        mpfr_const_pi(r.m_v, MPFR_RNDN);
        return r;
    }
    static real R_TAU()
    {
        real r;
        mpfr_const_pi(r.m_v, MPFR_RNDN);
        mpfr_mul_ui(r.m_v, r.m_v, 2, MPFR_RNDN);
        return r;
    }

    /* Set the global precision for subsequently created real numbers, in
     * 32-bit bigits, mirroring the original `lol::real` interface. */
    static void global_bigit_count(int bigits)
    {
        s_precision = (mpfr_prec_t)bigits * 32;
    }

    /* Number of 32-bit bigits for the default 512-bit precision; used to
     * derive how many significant digits should be printed. */
    static constexpr int DEFAULT_BIGIT_COUNT = 16;

    /* The free mathematical functions below need direct access to m_v */
    friend real fabs(real const &);
    friend real sqrt(real const &);
    friend real cbrt(real const &);
    friend real exp(real const &);
    friend real expm1(real const &);
    friend real exp2(real const &);
    friend real erf(real const &);
    friend real erfc(real const &);
    friend real erfcx(real const &);
    friend real log(real const &);
    friend real log1p(real const &);
    friend real log2(real const &);
    friend real log10(real const &);
    friend real gamma(real const &);
    friend real lgamma(real const &);
    friend real sin(real const &);
    friend real cos(real const &);
    friend real tan(real const &);
    friend real asin(real const &);
    friend real acos(real const &);
    friend real atan(real const &);
    friend real sinh(real const &);
    friend real cosh(real const &);
    friend real tanh(real const &);
    friend real atan2(real const &, real const &);
    friend real pow(real const &, real const &);
    friend real fmod(real const &, real const &);

private:
    mpfr_t m_v;
    inline static mpfr_prec_t s_precision = 512;
};

// Elementary functions, provided by MPFR
inline real fabs(real const &x) { real r; mpfr_abs(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real sqrt(real const &x) { real r; mpfr_sqrt(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real cbrt(real const &x) { real r; mpfr_cbrt(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real exp(real const &x)  { real r; mpfr_exp(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real expm1(real const &x) { real r; mpfr_expm1(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real exp2(real const &x) { real r; mpfr_exp2(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real erf(real const &x)  { real r; mpfr_erf(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real erfc(real const &x) { real r; mpfr_erfc(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real log(real const &x)  { real r; mpfr_log(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real log1p(real const &x) { real r; mpfr_log1p(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real log2(real const &x) { real r; mpfr_log2(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real log10(real const &x) { real r; mpfr_log10(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real lgamma(real const &x) { real r; mpfr_lngamma(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real gamma(real const &x) { real r; mpfr_gamma(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real sin(real const &x)  { real r; mpfr_sin(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real cos(real const &x)  { real r; mpfr_cos(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real tan(real const &x)  { real r; mpfr_tan(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real asin(real const &x) { real r; mpfr_asin(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real acos(real const &x) { real r; mpfr_acos(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real atan(real const &x) { real r; mpfr_atan(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real sinh(real const &x) { real r; mpfr_sinh(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real cosh(real const &x) { real r; mpfr_cosh(r.m_v, x.m_v, MPFR_RNDN); return r; }
inline real tanh(real const &x) { real r; mpfr_tanh(r.m_v, x.m_v, MPFR_RNDN); return r; }

inline real atan2(real const &y, real const &x) { real r; mpfr_atan2(r.m_v, y.m_v, x.m_v, MPFR_RNDN); return r; }
inline real pow(real const &b, real const &e)   { real r; mpfr_pow(r.m_v, b.m_v, e.m_v, MPFR_RNDN); return r; }
inline real fmod(real const &a, real const &b)  { real r; mpfr_fmod(r.m_v, a.m_v, b.m_v, MPFR_RNDN); return r; }

/* Scaled complementary error function: erfcx(x) = exp(x²) · erfc(x) */
inline real erfcx(real const &x)
{
    real r;
    real t;
    mpfr_sqr(r.m_v, x.m_v, MPFR_RNDN);    /* x² */
    mpfr_exp(r.m_v, r.m_v, MPFR_RNDN);    /* exp(x²) */
    mpfr_erfc(t.m_v, x.m_v, MPFR_RNDN);   /* erfc(x) */
    mpfr_mul(r.m_v, r.m_v, t.m_v, MPFR_RNDN);
    return r;
}

/* Return -1, 0 or +1 depending on the sign of x */
inline real sign(real const &x)
{
    return x.is_negative() ? real(-1) : (real(0) < x ? real(1) : real(0));
}

inline real min(real const &a, real const &b) { return a < b ? a : b; }
inline real max(real const &a, real const &b) { return a > b ? a : b; }