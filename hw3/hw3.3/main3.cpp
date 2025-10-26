#include <iostream>
#include <vector>
#include <stdexcept>

class linkedMatrix {
private:
    int rows, cols;
    std::vector<std::tuple<int, int, int>> elements;

public:
    linkedMatrix(int r, int c) : rows(r), cols(c) {}

    // 插入元素
    void insert(int row, int col, int val) {
        if (val == 0) return; // 不存储值为0的元素
        elements.push_back(std::make_tuple(row, col, val));
    }

    // 取出元素
    int get(int row, int col) {
        for (auto& elem : elements) {
            if (std::get<0>(elem) == row && std::get<1>(elem) == col) {
                return std::get<2>(elem);
            }
        }
        return 0; // 如果元素不存在，返回0
    }

    // 矩阵加法
    linkedMatrix add(const linkedMatrix& other) {
        linkedMatrix result(rows, cols);
        std::vector<std::tuple<int, int, int>> resultElements;

        for (auto& elem1 : elements) {
            for (auto& elem2 : other.elements) {
                if (std::get<0>(elem1) == std::get<0>(elem2) && std::get<1>(elem1) == std::get<1>(elem2)) {
                    resultElements.push_back(std::make_tuple(std::get<0>(elem1), std::get<1>(elem1), std::get<2>(elem1) + std::get<2>(elem2)));
                } else {
                    resultElements.push_back(elem1);
                    resultElements.push_back(elem2);
                }
            }
        }

        for (auto& elem : resultElements) {
            if (std::get<2>(elem) != 0) {
                result.insert(std::get<0>(elem), std::get<1>(elem), std::get<2>(elem));
            }
        }

        return result;
    }

    // 矩阵减法
    linkedMatrix subtract(const linkedMatrix& other) {
        linkedMatrix result(rows, cols);
        for (auto& elem1 : elements) {
            bool found = false;
            for (auto& elem2 : other.elements) {
                if (std::get<0>(elem1) == std::get<0>(elem2) && std::get<1>(elem1) == std::get<1>(elem2)) {
                    result.insert(std::get<0>(elem1), std::get<1>(elem1), std::get<2>(elem1) - std::get<2>(elem2));
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.insert(std::get<0>(elem1), std::get<1>(elem1), std::get<2>(elem1));
            }
        }
        return result;
    }

    // 矩阵乘法
    linkedMatrix multiply(const linkedMatrix& other) {
        if (cols != other.rows) {
            throw std::invalid_argument("Matrix dimensions do not match for multiplication");
        }
        linkedMatrix result(rows, other.cols);
        for (auto& elem1 : elements) {
            for (auto& elem2 : other.elements) {
                if (std::get<1>(elem1) == std::get<0>(elem2)) {
                    result.insert(std::get<0>(elem1), std::get<1>(elem2), std::get<2>(elem1) * std::get<2>(elem2));
                }
            }
        }
        return result;
    }

    // 打印矩阵
    void print() {
        std::cout << "(" << rows << ", " << cols << ", triples=[";
        for (size_t i = 0; i < elements.size(); ++i) {
            std::cout << "(" << std::get<0>(elements[i]) << ", " << std::get<1>(elements[i]) << ", " << std::get<2>(elements[i]) << ")";
            if (i < elements.size() - 1) std::cout << ", ";
        }
        std::cout << "])" << std::endl;
    }
};

// 测试用例
int main() {
    linkedMatrix A(3, 3);
    A.insert(0, 2, 7);
    A.insert(1, 1, 5);
    A.insert(0, 1, 3);
    A.print();

    int val = A.get(0, 1);
    std::cout << "Value at (0,1): " << val << std::endl;

    linkedMatrix B(3, 3);
    B.insert(0, 2, 7);
    B.insert(1, 1, 5);
    linkedMatrix C = A.add(B);
    C.print();

    linkedMatrix D = A.subtract(B);
    D.print();

    linkedMatrix E = A.multiply(B);
    E.print();

    return 0;
}