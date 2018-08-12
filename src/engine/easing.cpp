#include <engine/easing.hpp>
//#include <atom/core/math/math.h>
#include <cmath>

namespace wee {

easing::easing_t easing::ease[max_easing] = {
    &easing::ease_linear_in,
    &easing::ease_linear_out,
    &easing::ease_linear_in_out,
    &easing::ease_sine_in,
    &easing::ease_sine_out,
    &easing::ease_sine_in_out,
    &easing::ease_quadratic_in,
    &easing::ease_quadratic_out,
    &easing::ease_quadratic_in_out,
    &easing::ease_cubic_in,
    &easing::ease_cubic_out,
    &easing::ease_cubic_in_out,
    &easing::ease_quintic_in,
    &easing::ease_quintic_out,
    &easing::ease_quintic_in_out,
    &easing::ease_exp_in,
    &easing::ease_exp_out,
    &easing::ease_exp_in_out,
    &easing::ease_circle_in,
    &easing::ease_circle_out,
    &easing::ease_circle_in_out,
    &easing::ease_back_in,
    &easing::ease_back_out,
    &easing::ease_back_in_out,
    &easing::ease_elastic_in,
    &easing::ease_elastic_out,
    &easing::ease_elastic_in_out,
    &easing::ease_bounce_in,
    &easing::ease_bounce_out,
    &easing::ease_bounce_in_out
};

float easing::ease_linear_in(float p) {
    return p;
}
float easing::ease_linear_out(float p) {
    return  1.f - p;
}
float easing::ease_linear_in_out(float p) {
    return p;
}
float easing::ease_sine_in(float p) {
    return std::sin((p - 1) * M_PI_2) + 1;
}
float easing::ease_sine_out(float p) {
    return std::sin(p * M_PI_2);
}
float easing::ease_sine_in_out(float p) {
    return 0.5f * (1.f - std::cos(p * M_PI));
}
float easing::ease_quadratic_in(float p) {
    return p * p;
}
float easing::ease_quadratic_out(float p) {
    /**
     * Modeled after the parabola y = -x^2 + 2x
     */
    return -(p * (p - 2));
}
float easing::ease_quadratic_in_out(float p) {
    /**
     * Modeled after the piecewise quadratic
     * y = (1/2)((2x)^2)             ; [0, 0.5)
     * y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
     */
    if(p < 0.5f) {
        return 2 * p * p;
    } else {
        return (-2 * p * p) + (4 * p) - 1;
    }
}

float easing::ease_cubic_in(float p) {
    return p * p * p;
}

float easing::ease_cubic_out(float p) {
    float f = (p - 1);
    return f * f * f + 1.0f;
}

float easing::ease_cubic_in_out(float p) {
    if(p < 0.5) {
        return 4 * p * p * p;
    } else {
        float f = ((2 * p) - 2);
        return 0.5f * f * f * f + 1;
    }
}
float easing::ease_quartic_in(float p) {
    return p * p * p * p;
}
float easing::ease_quartic_out(float p) {
    float f = (p - 1);
    return f * f * f * (1 - p) + 1;
}
float easing::ease_quartic_in_out(float p) {
    if(p < 0.5)
    {
        return 8 * p * p * p * p;
    }
    else
    {
        float f = (p - 1);
        return -8 * f * f * f * f + 1;
    }
}
float easing::ease_quintic_in(float p) {
    return p * p * p * p * p;
}
float easing::ease_quintic_out(float p) {
    float f = (p - 1);
    return f * f * f * f * f + 1;
}
float easing::ease_quintic_in_out(float p) {
    if(p < 0.5)
    {
        return 16 * p * p * p * p * p;
    }
    else
    {
        float f = ((2 * p) - 2);
        return  0.5 * f * f * f * f * f + 1;
    }
}
float easing::ease_exp_in(float p) {
    return (p == 0.0) ? p : std::pow(2, 10 * (p - 1));
}
float easing::ease_exp_out(float p) {
    return (p == 1.0) ? p : 1 - std::pow(2, -10 * p);
}
float easing::ease_exp_in_out(float p) {
    if(p == 0.0 || p == 1.0) return p;
    if(p < 0.5) {
        return 0.5 * std::pow(2, (20 * p) - 10);
    } else {
        return -0.5 * std::pow(2, (-20 * p) + 10) + 1;
    }
}

float easing::ease_circle_in(float p) {
    return 1 - std::sqrt(1 - (p * p));
}

float easing::ease_circle_out(float p) {
    return std::sqrt((2 - p) * p);
}

float easing::ease_circle_in_out(float p) {
    if(p < 0.5)
    {
        return 0.5 * (1 - std::sqrt(1 - 4 * (p * p)));
    }
    else
    {
        return 0.5 * (std::sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
    }
}

float easing::ease_back_in(float p) {
    return p * p * p - p * std::sin(p * M_PI);
}
float easing::ease_back_out(float p) {
    float f = (1 - p);
    return 1 - (f * f * f - f * std::sin(f * M_PI));
}
float easing::ease_back_in_out(float p) {
    if(p < 0.5)
    {
        float f = 2 * p;
        return 0.5 * (f * f * f - f * std::sin(f * M_PI));
    }
    else
    {
        float f = (1 - (2*p - 1));
        return 0.5 * (1 - (f * f * f - f * std::sin(f * M_PI))) + 0.5;
    }
}

float easing::ease_elastic_in(float p) {
    return std::sin(13 * M_PI_2 * p) * std::pow(2, 10 * (p - 1));
}

float easing::ease_elastic_out(float p) {
    return std::sin(-13 * M_PI_2 * (p + 1)) * std::pow(2, -10 * p) + 1;
}

float easing::ease_elastic_in_out(float p) {
    if(p < 0.5) {
        return 0.5 * std::sin(13 * M_PI_2 * (2 * p)) * std::pow(2, 10 * ((2 * p) - 1));
    } else  {
        return 0.5 * (std::sin(-13 * M_PI_2 * ((2 * p - 1) + 1)) * std::pow(2, -10 * (2 * p - 1)) + 2);
    }
}

float easing::ease_bounce_in(float p) {
    return 1 - ease_bounce_out(1 - p);
}

float easing::ease_bounce_out(float p) {
    if(p < 4/11.0) {
        return (121 * p * p)/16.0;
    } else if(p < 8/11.0) {
        return (363/40.0 * p * p) - (99/10.0 * p) + 17/5.0;
    } else if(p < 9/10.0) {
        return (4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0;
    } else {
        return (54/5.0 * p * p) - (513/25.0 * p) + 268/25.0;
    }
}

float easing::ease_bounce_in_out(float p) {
    if(p < 0.5) {
        return 0.5 * ease_bounce_in(p*2);
    } else  {
        return 0.5 * ease_bounce_out(p * 2 - 1) + 0.5;
    }
}

}
