#include <vector>
#include <iostream>
#include <algorithm>


int max_profit(int m, const std::vector<int>& prices) {
    size_t n = prices.size();
    if(n < 2)
        return 0;

    auto first = std::find_if(prices.begin(), prices.end(), [&] (const int64_t& i) {
        return i <= m;
    });

    auto d= std::distance(prices.begin(), first);

    float profit = 0.f;
    for(size_t i=1 + d; i < prices.size(); i++) {
        profit += std::max(0.f, static_cast<float>(prices[i]) / prices[i-1] * m - m);
    }/*
    for(auto it=std::next(first); it != prices.end(); ++it) {
        profit += std::max(0.f, static_cast<float>(*it) / *std::prev(it) * m - m);
    }*/
    return static_cast<int64_t>(profit); 
}

int main(int, char**) {
    std::cout << max_profit(100, { 200, 300, 330, 100, 1, 2, 1, 400 }) << std::endl;
    
    return 0;
}

// 100 2
// 4 5
// -> 25
/**
 * 100 coins buys 20 widgets
 * sell for 5 coins per widget -> 25 coins
 */
