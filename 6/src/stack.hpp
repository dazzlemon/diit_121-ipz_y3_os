#include <initializer_list>
#include <cstddef>
#include <exception>
#include <stdexcept>

template<class T>
class Stack {
public:
	//constructor, destructor, assigment opetor
	Stack() {}
	Stack(const std::initializer_list<T>& il) {
		for (auto& i : il)
			push(i);
	}

	~Stack() {
		clear();
	}

	Stack& operator= (Stack other) {
		clear();
		while (!other.empty())
			push(other.pop());
		return *this;
	}

	//element access
	T& top() {
		if (empty())
			throw std::runtime_error("top on empty stack");
		return __top->val;
	}

	T& operator[](size_t i) {
		if (i < 0 || i > size())
			throw std::runtime_error("index out of range");
		auto ith = __top;
		for (; i >= 0; i--, ith = ith->prev) {}
		return ith->val;
	}

	//capacity
	bool empty() {
		return !__top;
	}

	size_t size() {
		size_t ret = 0;
		for (auto i = __top; i; i = i -> prev, ret++) {}
		return ret;
	}

	//modifies
	void push(const T& val) {
		__top = new Container(val, __top);
	}

	T pop() {
		if(empty())
			throw std::runtime_error("pop on empty stack");
		Container* old_top = __top;
		T ret = old_top->val;
		__top = old_top->prev;
		delete old_top;
		return ret;
	}

	void clear() {
		while (!empty())
			pop();
	}
private:
	class Container {
	public:
		T val;
		Container* prev;
		Container(const T& val, Container* prev) : val(val), prev(prev) {}
	};

	Container* __top = NULL;
};
