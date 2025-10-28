#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

template <class T>
class Stack {
public:
    void push(const T& value) {
        elements.push(value);
    }

    void pop() {
        if (!elements.empty()) {
            elements.pop();
        }
    }

    T top() const {
        if (!elements.empty()) {
            return elements.top();
        }
        throw out_of_range("Stack<>::top(): empty stack");
    }

    bool empty() const {
        return elements.empty();
    }

private:
    stack<T> elements;
};

template <class T>
void delete_all(Stack<T> &s, const T &x) {
    vector<T> temp;
    while (!s.empty()) {
        T value = s.top();
        s.pop();
        if (value != x) {
            temp.push_back(value);
        }
    }

    while (!temp.empty()) {
        s.push(temp.back());
        temp.pop_back();
    }
}

int main() {
    ifstream infile("input.txt");
    if (!infile.is_open()) {
        cout << "File cannot be opened" << endl;
        return 1;
    }

    char x;
    infile >> x;
    infile.ignore(); // Ignore the newline character

    Stack<char> s;
    char ch;
    while (infile >> ch) {
        s.push(ch);
        infile.ignore(); // Ignore the space, newline or tab character
    }

    delete_all(s, x);

    if (s.empty()) {
        cout << endl << endl; // 输出两个换行符
    } else {
        while (!s.empty()) {
            cout << s.top();
            s.pop();
            if (!s.empty()) {
                cout << " ";
            }
        }
        cout << endl << endl; // 输出两个换行符
    }

    return 0;
}