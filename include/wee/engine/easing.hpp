#pragma once

namespace wee {

struct easing {
    enum {
        linear_in        = 0,
        linear_out,
        linear_in_out,

        sine_in,
        sine_out,
        sine_in_out,

        quadratic_in,
        quadratic_out,
        quadratic_in_out,

        cubic_in,
        cubic_out,
        cubic_in_out,

        quintic_in,
        quintic_out,
        quintic_in_out,

        exp_in,
        exp_out,
        exp_in_out,

        circle_in,
        circle_out,
        circle_in_out,

        back_in,
        back_out,
        back_in_out,

        elastic_in,
        elastic_out,
        elastic_in_out,

        bounce_in,
        bounce_out,
        bounce_in_out,

        max_easing
    };

    typedef float(*easing_t)(float);
    static easing_t ease[max_easing];

    /**
     *  linear interpolation (no easing)
     * @param easing
     * @return
     */
    static float ease_linear_in(float);
    static float ease_linear_out(float);
    static float ease_linear_in_out(float);
    /**
     * sine wave easing; sin(p * PI/2).
     * @param in
     * @return
     */
    static float ease_sine_in(float);
    static float ease_sine_out(float);
    static float ease_sine_in_out(float);
    /**
     * quadratic easing; p^2
     * @param in
     * @return
     */
    static float ease_quadratic_in(float);
    static float ease_quadratic_out(float);
    static float ease_quadratic_in_out(float);
    /**
     * quadratic easing; p^3
     * @param in
     * @return
     */
    static float ease_cubic_in(float);
    static float ease_cubic_out(float);
    static float ease_cubic_in_out(float);
    /**
     * quartic easing; p^4
     * @param in
     * @return
     */
    static float ease_quartic_in(float);
    static float ease_quartic_out(float);
    static float ease_quartic_in_out(float);
    /**
     * quintic easing; p^5
     * @param in
     * @return
     */
    static float ease_quintic_in(float);
    static float ease_quintic_out(float);
    static float ease_quintic_in_out(float);
    /**
     * exp easing, base 2
     * @param in
     * @return
     */
    static float ease_exp_in(float);
    static float ease_exp_out(float);
    static float ease_exp_in_out(float);
    /**
     * Circular easing; sqrt(1 - p^2)
     * @param in
     * @return
     */
    static float ease_circle_in(float);
    static float ease_circle_out(float);
    static float ease_circle_in_out(float);
    /**
     * Overshooting cubic easing;
     * @param in
     * @return
     */
    static float ease_back_in(float);
    static float ease_back_out(float);
    static float ease_back_in_out(float);
    /**
     * exply-damped sine wave easing
     * @param in
     * @return
     */
    static float ease_elastic_in(float);
    static float ease_elastic_out(float);
    static float ease_elastic_in_out(float);
    /**
     * exply-decaying bounce easing
     * @param in
     * @return
     */
    static float ease_bounce_in(float);
    static float ease_bounce_out(float);
    static float ease_bounce_in_out(float);
};

}
