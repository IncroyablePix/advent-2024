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

namespace FirstExampleSampleProvider
{
    [[nodiscard]] std::pair<List, List> get_lists()
    {
        List firstList { 3, 4, 2, 1, 3, 3, };
        List secondList { 4, 3, 5, 3, 9, 3, };
        return {firstList, secondList};
    }
}

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
}

Number get_distance_between_lists(List& list1, List& list2)
{
    std::ranges::sort(list1);
    std::ranges::sort(list2);

    auto deltas = std::ranges::views::zip(list1, list2) |
        std::ranges::views::transform([](const std::pair<Number, Number> pair){ return std::abs(pair.first - pair.second); });

    return std::accumulate(deltas.begin(), deltas.end(), 0);
}

Number get_similarity_score(const List& list1, const List& list2)
{
    std::unordered_map<Number, size_t> occurrences;
    return std::accumulate(list1.begin(), list1.end(), 0, [&](size_t sum, const auto& number)
    {
        if (occurrences.contains(number))
        {
            return sum + number * occurrences.at(number);
        }

        return sum + number * (occurrences[number] = std::count(list2.begin(), list2.end(), number));
    });
}

int main(const int argc, const char** argv)
{
    const auto start = std::chrono::high_resolution_clock::now();
    auto [firstList, secondList] =
        argc < 2 || !exists(std::filesystem::path(argv[1]))
        ? FirstExampleSampleProvider::get_lists()
        : FileListsProvider::get_lists(argv[1], 1000);

    const auto distanceBetweenLists = get_distance_between_lists(firstList, secondList);

    const auto similarityScore = get_similarity_score(firstList, secondList);
    std::cout << "Distance between lists: " << distanceBetweenLists << "\n"
              << "Similarity score: " << similarityScore << "\n\n";

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Total execution time: " << duration << " microseconds." << std::endl;

    return EXIT_SUCCESS;
}
