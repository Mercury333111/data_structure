#include <iostream>
#include <algorithm>

// AVL树的节点结构
struct AVLNode {
    int key; // 节点的键值
    int height; // 节点的高度
    AVLNode* left; // 左子节点指针
    AVLNode* right; // 右子节点指针

    AVLNode(int k) : key(k), height(1), left(nullptr), right(nullptr) {} // 构造函数
};

// 辅助函数：计算节点高度
int getHeight(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

// 辅助函数：更新节点高度
void updateHeight(AVLNode* node) {
    if (node != nullptr) {
        node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
    }
}

// 辅助函数：计算平衡因子
int getBalanceFactor(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

// 辅助函数：右旋操作
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    y->left = x->right;
    x->right = y;

    updateHeight(y);
    updateHeight(x);

    return x;
}

// 辅助函数：左旋操作
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    x->right = y->left;
    y->left = x;

    updateHeight(x);
    updateHeight(y);

    return y;
}

// 插入操作
AVLNode* insert(AVLNode* node, int key) {
    if (node == nullptr) {
        return new AVLNode(key);
    }

    if (key < node->key) {
        node->left = insert(node->left, key);
    } else if (key > node->key) {
        node->right = insert(node->right, key);
    } else {
        return node; // 不允许插入重复键值
    }

    updateHeight(node);

    int balance = getBalanceFactor(node);

    // 左左情况
    if (balance > 1 && key < node->left->key) {
        return rotateRight(node);
    }

    // 右右情况
    if (balance < -1 && key > node->right->key) {
        return rotateLeft(node);
    }

    // 左右情况
    if (balance > 1 && key > node->left->key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // 右左情况
    if (balance < -1 && key < node->right->key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// 辅助函数：找到最小值节点
AVLNode* findMin(AVLNode* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

// 删除操作
AVLNode* erase(AVLNode* node, int key) {
    if (node == nullptr) {
        return node;
    }

    if (key < node->key) {
        node->left = erase(node->left, key);
    } else if (key > node->key) {
        node->right = erase(node->right, key);
    } else {
        // 节点有左子树和右子树
        if (node->left != nullptr && node->right != nullptr) {
            AVLNode* temp = findMin(node->right);
            node->key = temp->key;
            node->right = erase(node->right, temp->key);
        } else {
            // 节点只有一个子树或没有子树
            AVLNode* temp = node->left ? node->left : node->right;
            delete node;
            return temp;
        }
    }

    updateHeight(node);

    int balance = getBalanceFactor(node);

    // 左左情况
    if (balance > 1 && getBalanceFactor(node->left) >= 0) {
        return rotateRight(node);
    }

    // 左右情况
    if (balance > 1 && getBalanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // 右右情况
    if (balance < -1 && getBalanceFactor(node->right) <= 0) {
        return rotateLeft(node);
    }

    // 右左情况
    if (balance < -1 && getBalanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// 主函数：测试用例
int main() {
    AVLNode* T = nullptr; // 初始化空树

    // 插入节点
    T = insert(T, 30);
    std::cout << "Height after inserting 30: " << getHeight(T) << std::endl; // 高度为1

    T = insert(T, 40);
    T = insert(T, 50);
    std::cout << "Height after inserting 40 and 50: " << getHeight(T) << std::endl; // 高度为2

    T = insert(T, 20);
    T = insert(T, 10);
    std::cout << "Height after inserting 20 and 10: " << getHeight(T) << std::endl; // 高度为3

    T = insert(T, 25);
    std::cout << "Height after inserting 25: " << getHeight(T) << std::endl; // 高度为3

    T = insert(T, 60);
    T = insert(T, 70);
    std::cout << "Height after inserting 60 and 70: " << getHeight(T) << std::endl; // 高度为4

    // 删除节点
    T = erase(T, 10);
    T = erase(T, 25);
    std::cout << "Height after erasing 10 and 25: " << getHeight(T) << std::endl; // 高度为3

    return 0;
}