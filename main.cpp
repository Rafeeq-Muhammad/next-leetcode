#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cmath>       // for std::round
#include <algorithm>   // for std::sort, std::shuffle
#include <random>      // for std::mt19937, std::random_device

int my_rating = 1640;
int rating_delta_min = 100;
int rating_delta_max = 200;
int num_problems = 10;

class Problem {
public:
    int rating;
    int id;
    std::string title;

    Problem(int r, int i, const std::string& t)
      : rating(r), id(i), title(t) {}
};

int main() {
    // Load solved IDs
    std::unordered_set<int> solved;
    std::ifstream solved_in("solved.txt");
    if (solved_in.is_open()) {
        int sid;
        while (solved_in >> sid) {
            solved.insert(sid);
        }
        solved_in.close();
    } else {
        std::cerr << "Warning: could not open solved.txt, assuming none solved\n";
    }

    // Read problems
    const std::string filename = "ratings.txt";
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Error: could not open '" << filename << "'\n";
        return 1;
    }

    std::string line;
    // Skip header
    if (!std::getline(in, line)) {
        std::cerr << "Error: file is empty\n";
        return 1;
    }

    std::vector<Problem> problems;
    // Parse each line
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<std::string> cols;
        std::istringstream ss(line);
        std::string field;
        while (std::getline(ss, field, '\t'))
            cols.push_back(field);
        if (cols.size() < 3) continue;

        double raw_rating = 0;
        int id = 0;
        try {
            raw_rating = std::stod(cols[0]);
            id         = std::stoi(cols[1]);
        } catch (...) {
            continue;
        }
        int rounded_rating = static_cast<int>(std::round(raw_rating));
        const std::string& title = cols[2];

        // Only include if not solved
        if (solved.find(id) == solved.end()) {
            problems.emplace_back(rounded_rating, id, title);
        }
    }
    in.close();

    // Sort by rating ascending
    std::sort(problems.begin(), problems.end(), [](const Problem &a, const Problem &b) {
        return a.rating < b.rating;
    });

    // Partition into in-range and above-range
    int min_rating = my_rating + rating_delta_min;
    int max_rating = my_rating + rating_delta_max;
    std::vector<Problem> in_range;
    std::vector<Problem> above_range;
    for (const auto &p : problems) {
        if (p.rating >= min_rating && p.rating <= max_rating) {
            in_range.push_back(p);
        } else if (p.rating > max_rating) {
            above_range.push_back(p);
        }
    }

    // Randomly shuffle the in-range problems
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(in_range.begin(), in_range.end(), gen);

    // Select problems
    std::vector<Problem> selected;
    for (size_t i = 0; i < in_range.size() && selected.size() < static_cast<size_t>(num_problems); ++i) {
        selected.push_back(in_range[i]);
    }
    // If not enough, fill from above_range (already sorted by rating)
    for (size_t i = 0; i < above_range.size() && selected.size() < static_cast<size_t>(num_problems); ++i) {
        selected.push_back(above_range[i]);
    }

    // Print selected problems
    if (selected.empty()) {
        std::cout << "No unsolved problems found in the specified rating ranges.\n";
        return 0;
    }
    for (const auto &p : selected) {
        std::cout
            << "ID: " << p.id
            // << ", Rating: " << p.rating  NOTE: Comment out to not freak myself out
            << ", Title: \"" << p.title << "\"\n";
    }

    return 0;
}

