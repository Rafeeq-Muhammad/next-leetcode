#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cmath>       // for std::round
#include <algorithm>   // for std::sort, std::find_if

int my_rating = 1640;
int rating_delta = 0;
int num_problems_to_print = 10;

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

    // Find first problem with rating >= threshold
    int threshold = my_rating + rating_delta;
    auto start_it = std::find_if(problems.begin(), problems.end(), [threshold](const Problem &p) {
        return p.rating >= threshold;
    });
    if (start_it == problems.end()) {
        std::cout << "No unsolved problems with rating >= " << threshold << "\n";
        return 0;
    }

    // Print next num_problems_to_print
    int printed = 0;
    for (auto it = start_it; it != problems.end() && printed < num_problems_to_print; ++it) {
        const Problem &p = *it;
        std::cout
            << "ID: " << p.id
            << ", Rating: " << p.rating
            << ", Title: \"" << p.title << "\"\n";
        ++printed;
    }

    return 0;
}

