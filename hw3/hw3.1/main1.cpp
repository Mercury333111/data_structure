#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

// 定义上三角矩阵类
class upperTriangularMatrix {
private:
    vector<vector<int>> matrix;

public:
    upperTriangularMatrix(const vector<vector<int>>& mat) {
        // 确保矩阵是上三角矩阵
        for (size_t i = 0; i < mat.size(); ++i) {
            for (size_t j = 0; j < mat[i].size(); ++j) {
                if (i > j && mat[i][j] != 0) {
                    throw invalid_argument("Matrix is not upper triangular");
                }
            }
        }
        matrix = mat;
    }

    vector<vector<int>> getMatrix() const {
        return matrix;
    }

    void print() const {
        for (const auto& row : matrix) {
            for (int val : row) {
                cout << val << " ";
            }
            cout << endl;
        }
    }
};

// 定义下三角矩阵类
class lowerTriangularMatrix {
private:
    vector<vector<int>> matrix;

public:
    lowerTriangularMatrix(const vector<vector<int>>& mat) {
        // 确保矩阵是下三角矩阵
        for (size_t i = 0; i < mat.size(); ++i) {
            for (size_t j = 0; j < mat[i].size(); ++j) {
                if (i < j && mat[i][j] != 0) {
                    throw invalid_argument("Matrix is not lower triangular");
                }
            }
        }
        matrix = mat;
    }

    upperTriangularMatrix transpose() const {
        size_t n = matrix.size();
        vector<vector<int>> transposed(n, vector<int>(n, 0));

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                transposed[j][i] = matrix[i][j];
            }
        }

        return upperTriangularMatrix(transposed);
    }

    vector<vector<int>> getMatrix() const {
        return matrix;
    }

    void print() const {
        for (const auto& row : matrix) {
            for (int val : row) {
                cout << val << " ";
            }
            cout << endl;
        }
    }
};

// 测试用例
int main() {
    try {
        // 测试用例 1
        vector<vector<int>> L1 = {{5}};
        lowerTriangularMatrix lower1(L1);
        upperTriangularMatrix upper1 = lower1.transpose();
        cout << "Test case 1:" << endl;
        upper1.print();

        // 测试用例 2
        vector<vector<int>> L2 = {{1, 0, 0}, {4, 2, 0}, {7, 5, 3}};
        lowerTriangularMatrix lower2(L2);
        upperTriangularMatrix upper2 = lower2.transpose();
        cout << "Test case 2:" << endl;
        upper2.print();

        // 测试用例 3
        vector<vector<int>> L3 = {{0, 0}, {0, 0}};
        lowerTriangularMatrix lower3(L3);
        upperTriangularMatrix upper3 = lower3.transpose();
        cout << "Test case 3:" << endl;
        upper3.print();

        // 测试用例 4
        vector<vector<int>> L4 = {{1, 9, 0}, {4, 2, 0}, {7, 5, 3}};
        lowerTriangularMatrix lower4(L4); // 这里应该抛出异常
    } catch (const invalid_argument& e) {
        cout << "Test case 4: " << e.what() << endl;
    }

    // 测试用例 5
    try {
        vector<vector<int>> L5 = {{1, 0, 0}, {4, 2, 0}, {7, 5, 3}};
        lowerTriangularMatrix lower5(L5);
        upperTriangularMatrix upper5 = lower5.transpose();
        lowerTriangularMatrix lower5_transposed(upper5.getMatrix());
        if (lower5_transposed.getMatrix() == lower5.getMatrix()) {
            cout << "Test case 5: Transpose of transpose is equal to original matrix." << endl;
        } else {
            cout << "Test case 5: Transpose of transpose is not equal to original matrix." << endl;
        }
    } catch (const invalid_argument& e) {
        cout << "Test case 5: " << e.what() << endl;
    }

    return 0;
}