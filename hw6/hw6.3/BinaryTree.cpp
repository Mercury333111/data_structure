#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
using namespace std;

// 二叉树节点结构（模板类，支持任意可打印类型）
template <typename T>
struct TreeNode {
    T data;
    TreeNode<T>* left;
    TreeNode<T>* right;
    TreeNode(T val) : data(val), left(nullptr), right(nullptr) {}
};

// 二叉树类
template <typename T>
class BinaryTree {
private:
    TreeNode<T>* root;  // 根节点

    // ① 递归统计叶节点数目（辅助函数）
    int countLeavesHelper(TreeNode<T>* node) {
        if (node == nullptr) return 0;
        // 叶节点判定：左右子树均为空
        if (node->left == nullptr && node->right == nullptr) return 1;
        return countLeavesHelper(node->left) + countLeavesHelper(node->right);
    }

    // ② 递归交换左右子树（辅助函数）
    void swapSubtreesHelper(TreeNode<T>* node) {
        if (node == nullptr) return;
        // 交换当前节点的左右子树
        swap(node->left, node->right);
        // 递归处理左右子树
        swapSubtreesHelper(node->left);
        swapSubtreesHelper(node->right);
    }

    // 释放二叉树内存（避免内存泄漏）
    void destroyTree(TreeNode<T>* node) {
        if (node == nullptr) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    // 构造函数（初始化空树）
    BinaryTree() : root(nullptr) {}

    // 析构函数（释放内存）
    ~BinaryTree() {
        destroyTree(root);
    }

    // 从文件读取二叉树（按层次顺序，'#'表示空节点）
    bool buildTreeFromFile(const string& filename) {
        ifstream infile(filename);
        if (!infile.is_open()) {
            cerr << "错误：无法打开文件 " << filename << endl;
            return false;
        }

        vector<T> nodes;
        T val;
        // 读取文件中所有节点（空格/换行分隔），'#'表示空节点
        while (infile >> val) {
            nodes.push_back(val);
        }
        infile.close();

        // 构建二叉树
        if (nodes.empty() || nodes[0] == '#') {
            root = nullptr;
            return true;
        }

        // 根节点初始化
        root = new TreeNode<T>(nodes[0]);
        queue<TreeNode<T>*> q;
        q.push(root);
        int idx = 1;  // 当前处理的节点索引

        while (!q.empty() && idx < nodes.size()) {
            TreeNode<T>* curr = q.front();
            q.pop();

            // 处理左子节点
            if (nodes[idx] != '#') {
                curr->left = new TreeNode<T>(nodes[idx]);
                q.push(curr->left);
            }
            idx++;

            // 处理右子节点（需判断是否还有剩余节点）
            if (idx < nodes.size() && nodes[idx] != '#') {
                curr->right = new TreeNode<T>(nodes[idx]);
                q.push(curr->right);
            }
            idx++;
        }

        return true;
    }

    // ① 统计叶节点数目（对外接口）
    int countLeaves() {
        return countLeavesHelper(root);
    }

    // ② 交换所有节点的左右子树（对外接口）
    void swapAllSubtrees() {
        swapSubtreesHelper(root);
    }

    // ③ 层次顺序遍历（返回遍历结果和各层节点数）
    pair<vector<T>, vector<int>> levelOrderTraversal() {
        vector<T> traversal;    // 层次遍历结果
        vector<int> levelSizes; // 各层节点数
        if (root == nullptr) return {traversal, levelSizes};

        queue<TreeNode<T>*> q;
        q.push(root);

        while (!q.empty()) {
            int levelSize = q.size();  // 当前层节点数
            levelSizes.push_back(levelSize);

            // 遍历当前层所有节点
            for (int i = 0; i < levelSize; i++) {
                TreeNode<T>* curr = q.front();
                q.pop();
                traversal.push_back(curr->data);

                // 左子节点入队
                if (curr->left != nullptr) q.push(curr->left);
                // 右子节点入队
                if (curr->right != nullptr) q.push(curr->right);
            }
        }
        return {traversal, levelSizes};
    }

    // ④ 求二叉树的宽度（同一层次最多节点数）
    int getTreeWidth() {
        if (root == nullptr) return 0;

        queue<TreeNode<T>*> q;
        q.push(root);
        int maxWidth = 0;

        while (!q.empty()) {
            int levelSize = q.size();
            maxWidth = max(maxWidth, levelSize);  // 更新最大宽度

            // 下一层节点入队
            for (int i = 0; i < levelSize; i++) {
                TreeNode<T>* curr = q.front();
                q.pop();
                if (curr->left != nullptr) q.push(curr->left);
                if (curr->right != nullptr) q.push(curr->right);
            }
        }
        return maxWidth;
    }

    // 交换后获取叶节点（从左到右，层次顺序）
    vector<T> getLeavesAfterSwap() {
        vector<T> leaves;
        if (root == nullptr) return leaves;

        queue<TreeNode<T>*> q;
        q.push(root);

        while (!q.empty()) {
            TreeNode<T>* curr = q.front();
            q.pop();

            // 叶节点判定
            if (curr->left == nullptr && curr->right == nullptr) {
                leaves.push_back(curr->data);
            } else {
                // 非叶节点，左右子节点入队（保证左到右顺序）
                if (curr->left != nullptr) q.push(curr->left);
                if (curr->right != nullptr) q.push(curr->right);
            }
        }
        return leaves;
    }

    // 辅助函数：打印向量（遍历结果、叶节点等）
    static void printVector(const vector<T>& vec) {
        for (size_t i = 0; i < vec.size(); i++) {
            if (i > 0) cout << " ";
            cout << vec[i];
        }
        cout << endl;
    }

    // 辅助函数：打印各层节点数
    static void printLevelSizes(const vector<int>& sizes) {
        cout << "各层节点数：";
        for (size_t i = 0; i < sizes.size(); i++) {
            if (i > 0) cout << " ";
            cout << sizes[i];
        }
        cout << endl;
    }
};

// 测试执行函数（统一处理输出格式）
template <typename T>
void runTestFromFile(const string& filename, const string& testName) {
    cout << "==================== " << testName << " ====================" << endl;
    BinaryTree<T> tree;

    // 从文件构建二叉树
    if (!tree.buildTreeFromFile(filename)) {
        cerr << "测试失败：无法构建二叉树" << endl;
        return;
    }

    // 1. 输出叶节点数目
    cout << "1. 叶节点数目：" << tree.countLeaves() << endl;

    // 2. 层次遍历 + 各层节点数
    auto [traversal, levelSizes] = tree.levelOrderTraversal();
    cout << "2. 层次遍历：";
    BinaryTree<T>::printVector(traversal);
    BinaryTree<T>::printLevelSizes(levelSizes);

    // 3. 树的宽度
    cout << "3. 树的宽度：" << tree.getTreeWidth() << endl;

    // 4. 交换左右子树
    tree.swapAllSubtrees();
    cout << "4. 交换左右子树后处理结果：" << endl;

    // 5. 交换后的层次遍历（即交换后的树结构）
    auto [swapTraversal, swapLevelSizes] = tree.levelOrderTraversal();
    cout << "   交换后的层次遍历（树结构）：";
    BinaryTree<T>::printVector(swapTraversal);

    // 6. 交换后的叶节点（从左到右）
    vector<T> swapLeaves = tree.getLeavesAfterSwap();
    cout << "   交换后的叶节点（从左到右）：";
    BinaryTree<T>::printVector(swapLeaves);

    // 7. 交换后的叶节点数目
    cout << "   交换后的叶节点数目：" << tree.countLeaves() << endl;

    cout << "==========================================================" << endl << endl;
}

int main() {
    // -------------------------- 测试用例1 --------------------------
    // 树结构：
    //        A
    //      /   \
    //     B     C
    //    / \     \
    //   D   E     F
    // input1.txt 内容：A B C D E # F # # # # #
    string file1 = "input1.txt";
    runTestFromFile<char>(file1, "测试用例1");

    // -------------------------- 测试用例2 --------------------------
    // 树结构：
    //    A
    //   /
    //  B
    // /
    // C
    // /
    // D
    // input2.txt 内容：A B # C # D # #
    string file2 = "input2.txt";
    runTestFromFile<char>(file2, "测试用例2");

    return 0;
}