#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cmath>       // for std::round

int my_rating = 1640;
int rating_delta = 0;

class Problem {
public:
    int rating;      // now an int
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
    // Read & parse each data line
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::vector<std::string> cols;
        std::istringstream ss(line);
        std::string field;
        while (std::getline(ss, field, '\t'))
            cols.push_back(field);

        if (cols.size() < 3) {
            std::cerr << "Warning: skipping malformed line: " << line << "\n";
            continue;
        }

        double raw_rating = 0;
        int id = 0;
        try {
            raw_rating = std::stod(cols[0]);
            id         = std::stoi(cols[1]);
        } catch (...) {
            std::cerr << "Warning: invalid numbers on line: " << line << "\n";
            continue;
        }

        // round to nearest integer
        int rounded_rating = static_cast<int>(std::round(raw_rating));
        const std::string& title = cols[2];

        // Only include if not solved
        if (solved.find(id) == solved.end()) {
            problems.emplace_back(rounded_rating, id, title);
        }
    }

    in.close();

    // Print out unsolved problems
    for (const auto& p : problems) {
        std::cout
            << "ID: "       << p.id
            << ", Rating: " << p.rating
            << ", Title: \""<< p.title << "\"\n";
    }

    return 0;
}

