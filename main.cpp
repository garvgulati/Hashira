#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

int charToDigit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1; // invalid
}

unsigned long long parseBase(const std::string& s, int base) {
    unsigned long long result = 0;
    for (char c : s) {
        int d = charToDigit(c);
        if (d < 0 || d >= base) {
            std::cerr << "Invalid digit '" << c << "' for base " << base << "\n";
            exit(1);
        }
        result = result * base + d;
    }
    return result;
}

struct Share {
    int x;
    unsigned long long y;
};

std::string findValue(const std::string& text, const std::string& key) {
    size_t pos = text.find(key);
    if (pos == std::string::npos) return "";
    pos = text.find(":", pos);
    if (pos == std::string::npos) return "";
    pos++;
    // skip spaces and quotes
    while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\"')) pos++;
    size_t endPos = text.find_first_of(",\"\n}", pos);
    if (endPos == std::string::npos) return "";
    return text.substr(pos, endPos - pos);
}

int main() {
    std::ifstream infile("roots.json");
    if (!infile.is_open()) {
        std::cerr << "Cannot open roots.json\n";
        return 1;
    }
    std::string jsonText((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    int n = std::stoi(findValue(jsonText, "\"n\""));
    int k = std::stoi(findValue(jsonText, "\"k\""));

    std::vector<Share> shares;
    for (int i = 1; i <= n; ++i) {
        std::string prefix = "\"" + std::to_string(i) + "\"";
        size_t pos = jsonText.find(prefix);
        if (pos == std::string::npos) {
            std::cerr << "Missing share " << i << "\n";
            return 1;
        }
        size_t basePos = jsonText.find("\"base\"", pos);
        int base = std::stoi(findValue(jsonText.substr(basePos), "\"base\""));
        size_t valPos = jsonText.find("\"value\"", pos);
        std::string val = findValue(jsonText.substr(valPos), "\"value\"");

        unsigned long long y = parseBase(val, base);
        shares.push_back({i, y});
    }

    // Lagrange interpolation at 0 for P(0)
    unsigned long long C = 0;
    for (int i = 0; i < k; ++i) {
        long long num = shares[i].y;
        long long den = 1;
        for (int j = 0; j < k; ++j) {
            if (j == i) continue;
            num *= (-shares[j].x);
            den *= (shares[i].x - shares[j].x);
        }
        C += num / den; // integer division assumes exact
    }

    std::cout << C << std::endl; // Output only the value of C

    return 0;
}
