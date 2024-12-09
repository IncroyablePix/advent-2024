#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <utility>
#include <variant>

namespace FirstExampleSampleProvider
{
    [[nodiscard]] std::string get_memory()
    { return "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))"; }
}

namespace SecondExampleSampleProvider
{
    [[nodiscard]] std::string get_memory()
    { return "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))"; }
}

namespace FileMemoryProvider
{
    [[nodiscard]] std::string get_memory(const std::filesystem::path& path)
    {
        std::stringstream memory;
        std::string line;
        std::ifstream file(path);
        while (std::getline(file, line))
        {
            memory << line << "\n";
        }

        return memory.str();
    }
}

struct Condition
{
    const std::string Value;
    const int64_t Position;
    [[nodiscard]] bool should_ignore() const { return Value == "don't()"; }
    [[nodiscard]] bool should_execute() const { return Value == "do()"; }
};

struct Multiplication
{
    std::pair<short, short> Factors;
    const int64_t Position;

    int operator()() const { return Factors.first * Factors.second; }
    int operator+(const Multiplication& other) const
    { return (*this)() + other(); }
};

std::vector<Multiplication> parse_string(const std::string& memory)
{
    static std::regex multiplicationRegex(R"(mul\((\d{1,3})\,(\d{1,3})\))");

    const std::sregex_iterator begin {memory.begin(), memory.end(), multiplicationRegex};
    const std::sregex_iterator end {};
    std::vector<Multiplication> multiplications;
    std::transform(begin, end, std::back_inserter(multiplications), [](const auto& match) -> Multiplication
    { return { .Factors = { std::stoi(match[1].str()), std::stoi(match[2].str()) }}; });

    return multiplications;
}

std::vector<Multiplication> parse_string_with_conditional_statements(const std::string& memory)
{
    static std::regex multiplicationRegex(R"((don't\(\)|do\(\)|mul\((\d{1,3})\,(\d{1,3})\)))");

    const std::sregex_iterator begin {memory.begin(), memory.end(), multiplicationRegex};
    const std::sregex_iterator end {};
    std::vector<std::variant<Multiplication, Condition>> tokens;
    std::transform(begin, end, std::back_inserter(tokens), [](const auto& match) -> std::variant<Multiplication, Condition>
    {
        if (match[1].str() == "don't()" || match[1].str() == "do()")
            return Condition { .Value = match[1].str(), .Position = match.position() };

        return Multiplication { .Factors = { std::stoi(match[2].str()), std::stoi(match[3].str()) }, .Position = match.position() };
    });

    auto ignore = false;
    std::vector<Multiplication> multiplications;
    for (const auto& token : tokens)
    {
        if (std::holds_alternative<Condition>(token))
        {
            ignore = std::get<Condition>(token).should_ignore();
        }

        if (ignore)
        {
            continue;
        }

        if (std::holds_alternative<Multiplication>(token))
        {
            multiplications.emplace_back(std::get<Multiplication>(token));
        }
    }

    return multiplications;
}

int main(const int argc, const char** argv)
{
    const auto start = std::chrono::high_resolution_clock::now();

    const auto reports = argc < 2 || !exists(std::filesystem::path(argv[1]))
        ? FirstExampleSampleProvider::get_memory()
        : FileMemoryProvider::get_memory(argv[1]);

    const auto reportsWithConditions = argc < 2 || !exists(std::filesystem::path(argv[1]))
        ? SecondExampleSampleProvider::get_memory()
        : FileMemoryProvider::get_memory(argv[1]);

    const auto multiplications = parse_string(reports);
    const auto result = std::accumulate(multiplications.begin(), multiplications.end(), 0, [](int sum, const auto& multiplication) { return sum + multiplication(); });

    const auto multiplicationsWithConditionalStatements = parse_string_with_conditional_statements(reportsWithConditions);
    const auto resultWithConditionalStatements = std::accumulate(multiplicationsWithConditionalStatements.begin(), multiplicationsWithConditionalStatements.end(), 0, [](int sum, const auto& multiplication) { return sum + multiplication(); });

    std::cout << "Sum of multiplications: " << result << "\n"
        << "Sum of multiplications with conditions: " << resultWithConditionalStatements << "\n\n";

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Total execution time: " << duration << " microseconds." << std::endl;
    return EXIT_SUCCESS;
}