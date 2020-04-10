#pragma once

#include <vector>
#include <algorithm>
#include <future>
#include <map>
#include <numeric>
#include <cassert>

namespace wee {
    template <typename InputIt>
	auto split(InputIt first, InputIt last, size_t n) {
		std::vector<std::pair<InputIt, InputIt>> ranges;
		if (n == 0) return
			ranges;
		ranges.reserve(n);

		size_t dist = std::distance(first, last);
		n = std::min(n, dist);
		auto chunk = dist / n;
		auto remainder = dist % n;

		for (size_t i = 0; i < n - 1; ++i) {
			auto next_end = std::next(first, chunk + (remainder ? 1 : 0));
			ranges.emplace_back(first, next_end);

			first = next_end;
			if (remainder) remainder -= 1;
		}

		// last chunk
		ranges.emplace_back(first, last);
		return ranges;
	}

	template <typename InputIt, typename F>
	void parallel_foreach_n(InputIt first, InputIt last, const F& fun, size_t n) {

		std::vector<std::future<void> > futures;
		futures.reserve(n);
		auto pairs = split(first, last, n);

		for (auto& pair : pairs) {
			futures.push_back(std::move(
				std::async(std::launch::async, [&]() {
					for (auto it = pair.first; it != pair.second; ++it) {
						fun(*it);
					}
					})
			));
		}
		for (const auto& f : futures) f.wait();
	}
	

	template <typename InputIt, typename F>
	void parallel_foreach(InputIt first, InputIt last, const F& fun) {
		parallel_foreach_n(first, last, fun, std::thread::hardware_concurrency());
	}
}
