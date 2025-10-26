#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
using namespace std;

// 定义稀疏矩阵结构
struct SparseMatrix {
    int rows;
    int cols;
    vector<tuple<int, int, int>> triples;

    SparseMatrix(int r, int c, vector<tuple<int, int, int>> t)
        : rows(r), cols(c), triples(t) {}
};

// 稀疏矩阵乘法
SparseMatrix multiplySparseMatrices(const SparseMatrix& A, const SparseMatrix& B) {
    // 检查维度是否匹配
    if (A.cols != B.rows) {
        throw invalid_argument("Dimension mismatch: A.cols != B.rows");
    }

    int C_rows = A.rows;
    int C_cols = B.cols;
    vector<tuple<int, int, int>> C_triples;

    // 将 B 的三元组按列存储到哈希表中，便于快速查找
    vector<vector<pair<int, int>>> B_cols(B.rows);
    for (const auto& [r, c, val] : B.triples) {
        B_cols[r].emplace_back(c, val);
    }

    // 遍历 A 的每个非零元素
    for (const auto& [i, k, A_val] : A.triples) {
        // 遍历 B 的第 k 行的所有非零元素
        for (const auto& [j, B_val] : B_cols[k]) {
            int C_val = A_val * B_val;

            // 检查是否已经存在 C[i][j]，如果存在则累加
            bool found = false;
            for (auto& [C_i, C_j, C_val_existing] : C_triples) {
                if (C_i == i && C_j == j) {
                    C_val_existing += C_val;
                    found = true;
                    break;
                }
            }

            // 如果不存在，则添加新的三元组
            if (!found) {
                C_triples.emplace_back(i, j, C_val);
            }
        }
    }

    // 移除结果中的零元素
    C_triples.erase(remove_if(C_triples.begin(), C_triples.end(),
                                   [](const tuple<int, int, int>& t) { return get<2>(t) == 0; }),
                    C_triples.end());

    return SparseMatrix(C_rows, C_cols, C_triples);
}

// 打印稀疏矩阵
void printSparseMatrix(const SparseMatrix& mat) {
    cout << "(" << mat.rows << ", " << mat.cols << ", triples=[";
    for (const auto& [r, c, val] : mat.triples) {
        cout << "(" << r << ", " << c << ", " << val << "), ";
    }
    cout << "])" << endl;
}

// 测试用例
int main() {
    try {
        // 测试用例 1
        SparseMatrix A1(1, 1, {{0, 0, 5}});
        SparseMatrix B1(1, 1, {{0, 0, 7}});
        SparseMatrix C1 = multiplySparseMatrices(A1, B1);
        printSparseMatrix(C1);

        // 测试用例 2
        SparseMatrix A2(2, 3, {{0, 0, 1}, {1, 2, 2}});
        SparseMatrix B2(3, 2, {{0, 1, 3}, {2, 0, 4}});
        SparseMatrix C2 = multiplySparseMatrices(A2, B2);
        printSparseMatrix(C2);

        // 测试用例 3
        SparseMatrix A3(3, 3, {{0, 2, 7}, {1, 1, 5}});
        SparseMatrix B3(3, 3, {{0, 0, 1}, {1, 1, 1}, {2, 2, 1}});
        SparseMatrix C3 = multiplySparseMatrices(A3, B3);
        printSparseMatrix(C3);

        // 测试用例 4
        SparseMatrix A4(2, 2, {{0, 1, 5}});
        SparseMatrix B4(2, 2, {{0, 0, 9}});
        SparseMatrix C4 = multiplySparseMatrices(A4, B4);
        printSparseMatrix(C4);

        // 测试用例 5
        SparseMatrix A5(2, 3, {{0, 0, 1}});
        SparseMatrix B5(4, 2, {{0, 0, 1}});
        SparseMatrix C5 = multiplySparseMatrices(A5, B5); // 这里应该抛出异常
    } catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}