#pragma once

template <typename T, size_t N>
struct wave_propagator;

template <typename T, size_t N, typename R = std::vector<size_t> >
struct basic_constraint {
    virtual ~basic_constraint() = default;
    virtual void init(const wave_propagator<T, N>&, R*) = 0;
    virtual void check(const wave_propagator<T, N>&, size_t,  R*) = 0;
};
