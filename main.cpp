#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

using namespace std;

/**
 * @brief Generates an ANSI escape code for RGB color.
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @return ANSI escape code string for the specified RGB color.
 */
string rgb(int r, int g, int b) {
	return "\033[38;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m";
}

const string reset = "\033[0m";

/**
 * @brief Represents a syntax highlighting rule with a regex pattern and color code.
 * @param pattern The regex pattern to match.
 * @param color_code The ANSI escape code for the color to apply.
 */
struct HighlightRule {
	regex pattern;
	string color_code;
};

/**
 * @brief Returns a vector of syntax highlighting rules based on the file extension.
 * @param ext The file extension (e.g., "cpp", "py").
 * @return A vector of HighlightRule objects containing regex patterns and color codes.
 */
vector<HighlightRule> getHighlightRules(const string& ext) {
	if (ext == "cpp" || ext == "hpp" || ext == "c" || ext == "h") {
		return {
			// C++ keywords (blue)
			{regex(R"(\b(alignas|alignof|and|and_eq|asm|auto|bool|break|case|catch|char|class|const|constexpr|const_cast|continue|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|nullptr|operator|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq)\b)"), rgb(0x00, 0x88, 0xff)},
			// Strings (orange)
			{regex(R"("[^"\\]*(\\.[^"\\]*)*")"), rgb(0xff, 0x88, 0x00)},
			{regex(R"('[^'\\]*(\\.[^'\\]*)*')"), rgb(0xff, 0x88, 0x00)},
			// Single line comment (gray)
			{regex(R"(//.*$)"), rgb(0x88, 0x88, 0x88)},
			// Numbers (magenta)
			{regex(R"(\b\d+(\.\d+)?\b)"), rgb(0xff, 0x00, 0xff)}
		};
	} else if (ext == "py") {
		return {
			// Python keywords (blue)
			{regex(R"(\b(False|None|True|and|as|assert|async|await|break|class|continue|def|del|elif|else|except|finally|for|from|global|if|import|in|is|lambda|nonlocal|not|or|pass|raise|return|try|while|with|yield)\b)"), rgb(0x00, 0x88, 0xff)},
			// Strings (orange)
			{regex(R"("([^"\\]|\\.)*")"), rgb(0xff, 0x88, 0x00)},
			{regex(R"('([^'\\]|\\.)*')"), rgb(0xff, 0x88, 0x00)},
			// Comments (gray)
			{regex(R"(#.*$)"), rgb(0x88, 0x88, 0x88)},
			// Numbers (magenta)
			{regex(R"(\b\d+(\.\d+)?\b)"), rgb(0xff, 0x00, 0xff)}
		};
	} else {
		return {};
	}
}

/**
 * @brief Extracts the file extension from a filename.
 * @param filename The name of the file.
 * @return The file extension (e.g., "cpp", "py") or an empty string if no extension is found.
 */
string getFileExtension(const string& filename) {
	auto pos = filename.find_last_of('.');
	if (pos == string::npos) return "";
	return filename.substr(pos + 1);
}

/**
 * @brief Highlights and prints a line of code based on the provided syntax highlighting rules.
 * @param line The line of code to highlight.
 * @param rules The vector of HighlightRule objects containing regex patterns and color codes.
 * @return void
 */
void highlightAndPrintLine(const string& line, const vector<HighlightRule>& rules) {
	string remaining = line;
	size_t pos = 0;

	while (!remaining.empty()) {
		size_t earliest_pos = string::npos;
		size_t earliest_len = 0;
		string color_code;
		smatch earliest_match;

		// Find earliest match among all rules
		for (const auto& rule : rules) {
			smatch match;
			if (regex_search(remaining, match, rule.pattern)) {
				size_t match_pos = match.position(0);
				if (match_pos < earliest_pos) {
					earliest_pos = match_pos;
					earliest_len = match.length(0);
					color_code = rule.color_code;
					earliest_match = match;
				}
			}
		}

		if (earliest_pos == string::npos) {
			// No more matches, print rest normal
			cout << remaining;
			break;
		}

		// Print text before match normally
		if (earliest_pos > 0) {
			cout << remaining.substr(0, earliest_pos);
		}

		// Print matched text colored
		cout << color_code << earliest_match.str(0) << reset;

		// Move forward
		remaining = remaining.substr(earliest_pos + earliest_len);
	}

	cout << "\n";
}

/**
 * @brief Main function to read a file and apply syntax highlighting based on its extension.
 * @param argc Argument count.
 * @param argv Argument vector containing the filename as the first argument.
 * @return Exit status code.
 */
int main(int argc, char* argv[]) {
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " filename\n";
		return 1;
	}

	string filename = argv[1];
	ifstream file(filename);
	if (!file) {
		cerr << "Error: Cannot open file " << filename << "\n";
		return 1;
	}

	string ext = getFileExtension(filename);
	auto rules = getHighlightRules(ext);

	string line;
	while (getline(file, line)) {
		highlightAndPrintLine(line, rules);
	}

	return 0;
}
