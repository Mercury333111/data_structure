#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// 定义一个足够大的数组来保存Fibonacci数
const int MAX_N = 90;
vector<long long> fib(MAX_N + 1, -1);

// 递归函数计算Fibonacci数
long long fibonacci(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    if (fib[n] != -1) return fib[n]; // 如果已经计算过，直接返回结果
    fib[n] = fibonacci(n - 1) + fibonacci(n - 2); // 计算并保存结果
    return fib[n];
}

int main() {
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    int n;
    if (!(inputFile >> n) || n < 0 || n > 90) {
        cout << "WRONG" << endl;
    } else {
        cout << fibonacci(n) << endl;
    }
    cout << endl; // 输出一个空行
    inputFile.close();
    return 0;
}