#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cmath>       // for std::round
#include <algorithm>   // for std::sort, std::shuffle, std::find_if
#include <random>      // for std::mt19937, std::random_device

enum OUTPUT_TYPE {
    RANGE = 0,
    CLOSEST = 1,
};

int my_rating = 1640;
int rating_delta_min = 100;
int rating_delta_max = 200;
int num_problems = 10;
OUTPUT_TYPE output_type = CLOSEST;

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

    // Read problems from ratings.txt
    std::vector<Problem> problems;
    std::ifstream in("ratings.txt");
    if (!in.is_open()) {
        std::cerr << "Error: could not open 'ratings.txt'\n";
        return 1;
    }
    std::string line;
    std::getline(in, line); // skip header
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<std::string> cols;
        std::istringstream ss(line);
        std::string field;
        while (std::getline(ss, field, '\t')) cols.push_back(field);
        if (cols.size() < 3) continue;
        double raw_rating;
        int id;
        try {
            raw_rating = std::stod(cols[0]);
            id = std::stoi(cols[1]);
        } catch (...) { continue; }
        int rounded_rating = static_cast<int>(std::round(raw_rating));
        if (solved.count(id) == 0) {
            problems.emplace_back(rounded_rating, id, cols[2]);
        }
    }
    in.close();

    // Sort ascending by rating
    std::sort(problems.begin(), problems.end(), [](auto &a, auto &b) {
        return a.rating < b.rating;
    });

    std::vector<Problem> selected;

    if (output_type == RANGE) {
        // Partition into in-range and above-range
        int min_rating = my_rating + rating_delta_min;
        int max_rating = my_rating + rating_delta_max;
        std::vector<Problem> in_range, above_range;
        for (auto &p : problems) {
            if (p.rating >= min_rating && p.rating <= max_rating) in_range.push_back(p);
            else if (p.rating > max_rating) above_range.push_back(p);
        }
        // Shuffle in-range
        std::mt19937 gen(std::random_device{}());
        std::shuffle(in_range.begin(), in_range.end(), gen);
        // Fill selected
        for (auto &p : in_range) {
            if (selected.size() >= size_t(num_problems)) break;
            selected.push_back(p);
        }
        for (auto &p : above_range) {
            if (selected.size() >= size_t(num_problems)) break;
            selected.push_back(p);
        }
    } else {
        // CLOSEST: take smallest ratings > my_rating
        for (auto &p : problems) {
            if (p.rating > my_rating) {
                selected.push_back(p);
                if (selected.size() >= size_t(num_problems)) break;
            }
        }
    }

    if (selected.empty()) {
        std::cout << "No unsolved problems found for the given criteria.\n";
        return 0;
    }
    for (auto &p : selected) {
        std::cout << "ID: " << p.id
                  << ", Rating: " << p.rating
                  << ", Title: \"" << p.title << "\"\n";
    }
    return 0;
}

