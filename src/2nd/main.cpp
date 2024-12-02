#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

typedef int32_t Level;
typedef std::vector<Level> Report;

namespace ExampleSampleProvider
{
    [[nodiscard]] std::vector<Report> get_reports()
    {
        return
        {
            { 7, 6, 4, 2, 1 },
            { 1, 2, 7, 8, 9 },
            { 9, 7, 6, 2, 1 },
            { 1, 3, 2, 4, 5 },
            { 8, 6, 4, 4, 1 },
            { 1, 3, 6, 7, 9 }
        };
    }
}

namespace FileReportsProvider
{
    [[nodiscard]] std::vector<Report> get_reports(const std::filesystem::path& path, const size_t listSize = 1000)
    {
        std::ifstream fileStream(path);
        std::vector<Report> reports;
        reports.reserve(listSize);

        std::string line;
        while (std::getline(fileStream, line))
        {
            Report report;
            std::istringstream ss(line);
            Level level;

            while (ss >> level)
            {
                report.emplace_back(level);
            }

            reports.emplace_back(report);
        }

        return reports;
    }
}

bool is_safe(const Report& report, bool problemDampener = false)
{
    auto badLevelTolerate = problemDampener;
    enum Direction: uint8_t { Undefined, Increase, Decrease };
    if (report.empty())
    {
        return true;
    }

    auto lastLevel = report[0];
    auto lastDirection = Undefined;

    for (auto i = 1; i < report.size(); ++i)
    {
        #pragma region Delta not too large
        const auto delta = report[i] - lastLevel;
        if (const auto absDelta = std::abs(delta);
            absDelta < 1 || absDelta > 3)
        {
            if (badLevelTolerate)
            {
                badLevelTolerate = false;
                continue;
            }

            return false;
        }
        #pragma endregion

        #pragma region Always increase or decrease
        auto direction = Undefined;

        if (delta > 0)
        {
            direction = Increase;
        }
        else if (delta < 0)
        {
            direction = Decrease;
        }

        if (direction != Undefined && lastDirection != Undefined && lastDirection != direction)
        {
            if (badLevelTolerate)
            {
                badLevelTolerate = false;
                continue;
            }

            return false;
        }
        #pragma endregion

        #pragma region State persistance
        lastDirection = direction;
        lastLevel = report[i];
        #pragma endregion
    }

    return true;
}

bool is_safe_with_problem_dampener(const Report& report)
{
    return is_safe(report, true);
}

bool is_safe_without_problem_dampener(const Report& report)
{
    return is_safe(report, false);
}

int main(const int argc, const char** argv)
{
    const auto start = std::chrono::high_resolution_clock::now();

    auto reports = argc < 2 || !exists(std::filesystem::path(argv[1]))
        ? ExampleSampleProvider::get_reports()
        : FileReportsProvider::get_reports(argv[1], 1000);

    const auto safeCount = std::ranges::count_if(reports, is_safe_without_problem_dampener);
    const auto safeWithProblemDampenerCount = std::ranges::count_if(reports, is_safe_with_problem_dampener);

    std::cout << "Safe reports count without problem dampener: " << safeCount << "\n"
        << "Safe reports count with problem dampener: " << safeWithProblemDampenerCount << "\n\n";

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Total execution time: " << duration << " microseconds." << std::endl;
    return EXIT_SUCCESS;
}