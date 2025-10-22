#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// 递归函数生成所有子集
void generateSubsets(int index, int n, vector<bool>& subset, vector<char>& elements) {
    if (index == n) {
        // 输出子集
        bool isEmpty = true;
        for (int i = 0; i < n; ++i) {
            if (subset[i]) {
                cout << elements[i];
                isEmpty = false;
            }
            if (i < n - 1 && subset[i]) {
                cout << " ";
            }
        }
        if (isEmpty) {
            cout << endl; // 空集输出一行空行
        } else {
            cout << endl;
        }
        return;
    }
    subset[index] = false;
    generateSubsets(index + 1, n, subset, elements);

    subset[index] = true;
    generateSubsets(index + 1, n, subset, elements);
}

int main() {
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    int n;
    if (!(inputFile >> n) || n < 1 || n > 26) {
        cout << "WRONG" << endl;
    } else {
        vector<char> elements(n);
        for (int i = 0; i < n; ++i) {
            elements[i] = 'a' + i;
        }
        vector<bool> subset(n, false);
        generateSubsets(0, n, subset, elements);
    }
    inputFile.close();
    return 0;
}