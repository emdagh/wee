#include <vector>

int main(int, char**) {
    std::vector<int> vertices = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 0
    };
    std::vector<std::pair<std::vector<int>::const_iterator, int> > obj;
    obj.push_back(std::make_pair(vertices.begin(), 0));
    obj.push_back(std::make_pair(vertices.begin() + 1, 1));
    obj.push_back(std::make_pair(vertices.begin() + 2, 2));
    obj.push_back(std::make_pair(vertices.begin() + 3, 3));

    for(size_t i=0; i < obj.size(); i++) {
        for(size_t j=i+1; j < obj.size() - 1; j++) {
            if(&(*obj[i].first) == &(*obj[j].first)) {
                [[maybe_unused]] int k = 0;
                break;
            }
        }
    }


    return 0;
}
