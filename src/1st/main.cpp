#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

typedef int32_t Number;
typedef std::vector<Number> List;

namespace ExampleSampleProvider
{
    [[nodiscard]] std::pair<List, List> get_lists()
    {
        List firstList{
            3, 4, 2, 1, 3, 3,
        };
        List secondList{
            4, 3, 5, 3, 9, 3,
        };
        return {firstList, secondList};
    }
}; // namespace ExampleSampleProvider

namespace FileListsProvider
{
    [[nodiscard]] std::pair<List, List> get_lists(const std::filesystem::path& path, const size_t listSize = 1000)
    {
        std::ifstream fileStream(path);
        List firstList(listSize);
        List secondList(listSize);

        std::string line;
        while (std::getline(fileStream, line))
        {
            std::stringstream ss(line);
            Number number;

            ss >> number;
            firstList.push_back(number);

            ss >> number;
            secondList.push_back(number);
        }

        return {firstList, secondList};
    }
}; // namespace FileListsProvider

int main(int argc, char** argv)
{
    auto [firstList, secondList] = argc < 2 || argv[1] == "example"
        ? ExampleSampleProvider::get_lists()
        : FileListsProvider::get_lists("./src/1st/lists.txt", 1000);

#pragma region Part One
    {
        std::ranges::sort(firstList);
        std::ranges::sort(secondList);

        auto deltas = std::ranges::views::zip(firstList, secondList) |
            std::ranges::views::transform([](std::pair<Number, Number> pair)
                                          { return std::abs(pair.first - pair.second); });

        auto distanceBetweenLists = std::accumulate(deltas.begin(), deltas.end(), 0);

        std::cout << "Distance between lists: " << distanceBetweenLists << std::endl;
    }
#pragma endregion

#pragma region Part Two
    {
        std::unordered_map<Number, size_t> occurences;
        auto sumOfSimilarities = std::accumulate(firstList.begin(), firstList.end(), 0, [&](size_t sum, const auto& number)
        {
            if (occurences.contains(number))
            {
                return sum + number * occurences.at(number);
            }

            return sum + number * (occurences[number] = std::count(secondList.begin(), secondList.end(), number));
        });

        std::cout << "Similarity score: " << sumOfSimilarities << std::endl;
    }
#pragma endregion

    return EXIT_SUCCESS;
}
