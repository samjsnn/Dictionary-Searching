#include <iostream>
#include <fstream>
#include <unordered_set>
#include <vector>
#include <regex>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <set>

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

class WordSearcher {
private:
    std::vector<std::unordered_set<std::string>> words;
    std::mutex mtx;

    std::string toRegex(std::string pattern_) {
        std::string regex;
        for (size_t i = 0; i < pattern_.size(); ++i) {
            if (std::isalpha(pattern_[i])) {  // letter
                regex += pattern_[i];
            } else if (std::isdigit(pattern_[i])) {  // digits
                int j = i + 1;
                while (j < pattern_.size() && std::isdigit(pattern_[j])) {
                    ++j;
                }
                int digits = std::stoi(pattern_.substr(i, j - i));
                if (j < pattern_.size() && pattern_[j] == ':') {  // digits:digits
                    int k = j + 1;
                    while (k < pattern_.size() && std::isdigit(pattern_[k])) {
                        ++k;
                    }
                    int second_digits = std::stoi(pattern_.substr(j + 1, k - j - 1));
                    regex += ".{" + std::to_string(digits) + "," + std::to_string(second_digits) + "}";
                    i = k - 1;
                } else {  // just digits
                    regex += ".{" + std::to_string(digits) + "}";
                    i = j - 1;
                }
            } else if (pattern_[i] == '{') {  // {letter...letter}
                int j = i + 1;
                while (j < pattern_.size() && pattern_[j] != '}') {
                    ++j;
                }
                if (j < pattern_.size()) {
                    regex += "[" + pattern_.substr(i + 1, j - i - 1) + "]";
                }
                i = j;
            }
        }
        return regex;
    }
    std::string handleNumber(std::string pattern, int &index) {
        std::string out = ".[a-z]{";
        while (index < pattern.size() and ((pattern[index] >= '0' and pattern[index] <= '9') or pattern[index] == ':')){
            out += pattern[index] == ':' ? ',' : pattern[index];
            index ++;
        }
        out += '}';
        return out;
    }

    void searchWords(const std::string& regexPattern, int length, const std::unordered_set<std::string>& wordsToSearch, std::set<std::string>& matches) {
        std::regex e(regexPattern);
        for (const auto& word : wordsToSearch) {
            if (std::regex_match(word, e) && ((length == -1) || (word.length() == length))) {
                mtx.lock();
                matches.insert(word);
                mtx.unlock();
            }
        }
    }

public:
    WordSearcher(const std::string& filename) {
        auto start = std::chrono::high_resolution_clock::now();
        const unsigned int numThreads = std::thread::hardware_concurrency();
        words.resize(numThreads);

        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Unable to open file");
        }

        std::string line;
        unsigned int i = 0;
        while (std::getline(file, line)) {
            words[i % numThreads].insert(line);
            ++i;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end-start;

        std::cout << "Data loaded in " << diff.count() << " s\n";
    }

    void search(const std::string& pattern, int length = -1) {
        std::set<std::string> matches;
        std::string regexPattern = toRegex(pattern);
        auto start = std::chrono::high_resolution_clock::now();
        const unsigned int numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads(numThreads);
        std::vector<std::set<std::string>> results(numThreads);

        for (unsigned int i = 0; i < numThreads; ++i) {
            threads[i] = std::thread(&WordSearcher::searchWords, this, regexPattern, length, std::ref(words[i]), std::ref(results[i]));
        }

        for (auto& thread : threads) {
            thread.join();
        }

        for (const auto& result : results) {
            for (const auto word : result){
                matches.insert((word));
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end-start;

        std::set<std::string>::iterator itr;
        // Displaying set elements
        for (itr = matches.begin();
            itr != matches.end(); itr++)
        {
            std::cout << *itr << "\n";
        }
        std::cout << "Searched for: " << pattern << "\n";
        std::cout << "Number of matches: " << matches.size() << "\n";
        std::cout << "Search completed in " << diff.count() << " s\n";
    }
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <filename> <pattern>\n";
        return 1;
    }
    int length = -1;
    if (argc == 4) {
        length = std::stoi(argv[3]);
    }
    WordSearcher ws(argv[1]);
    ws.search(argv[2], length);
    return 0;
}
