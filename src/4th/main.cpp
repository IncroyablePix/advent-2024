#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <vector>

namespace FirstExampleSampleProvider
{
    [[nodiscard]] std::vector<std::string> get_text()
    {
        return {"MMMSXXMASM", "MSAMXMSMSA", "AMXSXMAAMM", "MSAMASMSMX", "XMASAMXAMM",
                "XXAMMXXAMA", "SMSMSASXSS", "SAXAMASAAA", "MAMMMXMMMM", "MXMXAXMASX"};
    }
} // namespace FirstExampleSampleProvider

namespace FileReportsProvider
{
    [[nodiscard]] std::vector<std::string> get_text(const std::filesystem::path& path)
    {
        std::vector<std::string> lines;
        std::ifstream fileStream(path);
        std::string line;
        while (std::getline(fileStream, line))
        {
            lines.emplace_back(line);
        }

        return lines;
    }
} // namespace FileReportsProvider

const std::vector<std::pair<int, int>> directions = {{0, 1},  {1, 0},  {1, 1},   {-1, 1},
                                                     {0, -1}, {-1, 0}, {-1, -1}, {1, -1}};

constexpr int wordLength = 4;
const std::string targetWord = "XMAS";

size_t count_xmas(const std::vector<std::string>& grid)
{
    size_t count = 0;
    const auto rows = grid.size();
    const auto cols = grid[0].size();

    for (int x = 0; x < rows; ++x)
    {
        for (int y = 0; y < cols; ++y)
        {
            for (const auto& [first, second] : directions)
            {
                const auto dx = first;
                const auto dy = second;
                const auto endX = x + (wordLength - 1) * dx;
                const auto endY = y + (wordLength - 1) * dy;

                if (endX >= 0 && endX < rows && endY >= 0 && endY < cols)
                {
                    auto found = true;
                    for (int k = 0; k < wordLength; ++k)
                    {
                        if (grid[x + k * dx][y + k * dy] != targetWord[k])
                        {
                            found = false;
                            break;
                        }
                    }
                    if (found)
                    {
                        ++count;
                    }
                }
            }
        }
    }

    return count;
}

int main(const int argc, const char** argv)
{
    const auto text = argc < 2 || !exists(std::filesystem::path(argv[1])) ? FirstExampleSampleProvider::get_text()
                                                                    : FileReportsProvider::get_text(argv[1]);

    const auto xmasOccurrences = count_xmas(text);
    // const auto masOccurrences = count_masses(text);
    std::cout << "Sum of multiplications: " << xmasOccurrences << "\n";
}
