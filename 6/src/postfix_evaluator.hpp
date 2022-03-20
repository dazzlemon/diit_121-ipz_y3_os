#include <QStack>
#include <string>
#include <exception>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <iterator>
#include <map>
#include <functional>

std::vector<std::string> words(const std::string& str) {
	std::istringstream iss(str);
	return std::vector<std::string>(
		std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
}

//works only for ints
class PostfixEvaluator {
public:
	std::string eval(const std::string& postfix) {
		std::vector<std::string> postfix_tokenized = words(postfix);
		std::string res;
		try {
			res =  __eval(postfix_tokenized);
		} catch (const std::exception& e) {
			res = std::string("error: ") + std::string(e.what());
		}
		return res;
	}
private:

	std::string __eval(const std::vector<std::string>& postfix) {
		ops.clear();
		for (auto& token : postfix) {
			ops.push(try_stoi(token) ? __itoken : calc(token));
		}
		if (ops.size() > 1) {
			throw std::runtime_error("bad expression");
		}
		return std::to_string(ops.pop());
	}

	int calc(const std::string& op) {
		int res;
		try {
			res = operators[op](ops.pop(), ops.pop());
		} catch (const ZeroExcept& e) {
			throw std::runtime_error(e.what());
		} catch (...) {
			throw std::runtime_error("bad expression");
		}
		return res;
	}
	
	bool try_stoi(const std::string& token) {
		try {
			__itoken = stoi(token);
		} catch (const std::invalid_argument&) {
			return false;
		}
		return true;
	}

	int __itoken;
	QStack<int> ops;
	std::map<std::string, std::function<int(int, int)>> operators = {
		{"+", [](int a, int b) {return a + b;}},
		{"-", [](int a, int b) {return a - b;}},
		{"*", [](int a, int b) {return a * b;}},
		{"/", [](int a, int b) {
			if (!b) {
				throw ZeroExcept();
			}
			return a / b;
		}}
	};

	class ZeroExcept : std::exception {
	public:
		const char* what() const noexcept {return "division by zero";}
	};
};
