
class model {
    bool* wave;
    int* propagator;
    int* compatible;
    int* observed;
    
    std::stack<std::tuple<int, int>> stack;

    size_t FMX, FMY, T;
    bool periodic;

    double* weights;
    double* weightLogWeights;

    int* sumsOfOnes;
    double sumOfWeights, sumOfWeightLogWeights, startingEntropy;
    double* sumsOfWeights, *sumsOfWeightLogWeights, *entropies;

    static constexpr int DX[] = { -1, 0, 1, 0 };
    static constexpr int DY[] = {  0, 1, 0,-1 };

    wee::random random{0};

public:
    model(int w, int h) : FMX(w), FMY(h) {}

    void init() {
        size_t len = FMX * FMY;
        wave = new bool[len * T];
        compatible = new int[len * T * 4];
        weightLogWeights = new double[T];
        sumOfWeights = 0;
        sumOfWeightLogWeights = 0;

        for(auto t : range(T)) {
            weightLogWeights[t] = weights[t] * std::log(weights[t]);
            sumOfWeights += weights[t]; // << uninitialized??
            sumOfWeightLogWeights += weightLogWeights[t];
        }
        startingEntropy = std::log(sumOfWeights) - sumOfWeightLogWeights / sumOfWeights;

        sumsOfOnes = new int[len];
        sumsOfWeights = new double[len];
        sumsOfWeightLogWeights = new double[len];
        entropies = new double[len];

        //stack = new std::stack<std::tuple<int, int> >;
        //stack.reserve(len * T);
    }

    // n-dimensional = on_boundary(const std::array<size_t, D0>&) = 0;
    virtual bool on_boundary(int x, int y) = 0;
    virtual void ban(size_t i, size_t t) {
        size_t len = FMX * FMY;
        array_view<bool, 2> vw(wave, len, T);
        vw[{i, t}] = false;
        int* comp = &array_view<int, 3>(compatible, len, T, 4)[{i, t}];
        for(auto d : range(4)) {
            comp[d] = 0;
        }
        stack.push({i ,t});

        auto sum = sumsOfWeights[i];
        entropies[i] += sumsOfWeightLogWeights[i] / sum - std::log(sum);

        sumsOfOnes[i]               -= 1;
        sumsOfWeights[i]            -= weights[t];
        sumsOfWeightLogWeights[i]   -= weightLogWeights[t];

        sum = sumsOfWeights[i];
        entropies[i] -= sumsOfWeightLogWeights[i] / sum - std::log(sum);
    }

    int observe() {
        double minH = 1E+3;
        int argmin = -1;
        size_t len = FMX * FMY;
        int amount;
        double H;

        for(auto i : range(len)) {
            if(on_boundary(i % FMX, i / FMX))
                continue;
            if(amount = sumsOfOnes[i]; amount == 0)
                return -1;
            if(H = entropies[i]; amount > 1 && H <= minH) {
                if(auto noise = 1E-6 * rnd<decltype(H)>(); H + noise < minH) {
                    minH = H + noise;
                    argmin = i;
                }
            }
        }
        if(argmin == -1) {
            observed = new int[len]; // <<< TODO: hoist!
            for(auto i : range(len)) {
                for(auto t : range(T)) {
                    if(array_view<bool, 2> vw(wave, len, T); vw[{i, t}]) {
                        observed[i] = t;
                    }
                    //if(wave[i + t * len]) {
                    //    observed[i] = t;
                    //}
                }
            }
            return 0;
        }

        double* distribution = new double[T];
        for(auto t : range(T)) {
            distribution[t] = array_view<bool, 2>(wave, len, T)[{
                (size_t)argmin, t
            }] ? weights[t] : 0.0;
        }
        size_t r = rnd<double>();

        bool* w = &wave[argmin]; // <<< correct?
        for(auto t : range(T)) {
            if(w[t] != (t==r)) {
                ban(argmin, t);
            }
        }
        
        return 1;
    }

    void propagate() {
        while(stack.size() > 0) {
            auto e = stack.top();
            stack.pop();

            int i1 = std::get<0>(e);
            int x1 = i1 % FMX;
            int y1 = i1 / FMX;
            
            for(size_t d : range(4)) {
                int dx = DX[d];
                int dy = DY[d];
                int x2 = x1 + dx;
                int y2 = y1 + dy;
                if(on_boundary(x2, y2)) {
                    continue;
                }

                x2 += x2 < 0 ? (int)FMX : x2 >= (int)FMX ? -(int)FMX : 0;
                y2 += y2 < 0 ? (int)FMY : y2 >= (int)FMY ? -(int)FMY : 0;

                int i2 = x2 + y2 * FMX;
                int* p = &array_view<int, 3>(propagator, 4, T, T)[{ 
                    d, 
                    (size_t)std::get<1>(e) 
                }];
                int* compat = &compatible[i2];

                for(auto l : range(T)){ // p.Length == T? <<< correct?
                    int t2 = p[l];
                    int* comp = &compat[t2];
                    comp[d]--;
                    if(comp[d] == 0) {
                        ban(i2, t2);
                    }
                } 
            }
        }
    }

    bool run(int seed, int ) {
        if(wave == nullptr) 
            init();
        clear();
        random = wee::random(seed);

        return false;
    }

    void clear() {
    }

};
