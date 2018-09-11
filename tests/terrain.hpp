#pragma once


float randf(float scale_ = 1.0f) {
    return scale_ * (static_cast<float>(rand()) / RAND_MAX);
}
void fractal(const vec2& v0, const vec2& v1, std::vector<vec2>& res) {
    vec2 d = vec2::normalize(v1 - v0);
    float len = vec2::length(v1 - v0);
    float slen = len / res.size();


    for(size_t i=0; i < res.size(); i++) {
        float m = slen * i;
        auto& vec = res[i];
        vec.x = v0.x + d.x * m;
        vec.y = v0.y + d.y * m;
    }

    for(size_t i=0; i < res.size() - 1; i++) {
        auto& a = res[i];
        auto& b = res[i + 1];

        vec2 n = vec2::normal_of(b - a);
        a.x += n.x * randf(slen);
        a.y += n.y * randf(slen);
    }
    res.front() = v0;
    res.back()  = v1;
}

void hills(int num, int pixelStep, int screenWidth, std::vector<vec2>& res) {
    float hillStartY = 140.f + randf() * 100.f;
    float hillWidth = (float)screenWidth / num;
    int hillSlices = (int)(hillWidth / pixelStep);

    for(int i=0; i < num; i++) {
        float randomHeight = randf() * 100.0f;

        if(i != 0)
            hillStartY -= randomHeight;

        for(int j=0; j < hillSlices; j++) {
            float x = j * pixelStep + hillWidth * i;
            float y = hillStartY + randomHeight * std::cos(2 * M_PI / hillSlices * j);
            vec2 a = {
                x,
                y 
            };
            res.push_back(a);
        }

        hillStartY = hillStartY + randomHeight;
    }
}
void sloped(float angle, float len, std::vector<vec2>& res) {
    float slen = len / res.size();

    float frequency = 8.0f;

    float fx = res.size() / frequency;

    for(size_t i=0; i < res.size(); i++) {

        float x = (float) i / res.size();// i * slen;
        float y = -std::sin(M_PI * x);
        

        vec2& vec = res[i];
        vec.x = SCREEN_TO_WORLD(x * len);
        vec.y = SCREEN_TO_WORLD(y * len);
    }
}
