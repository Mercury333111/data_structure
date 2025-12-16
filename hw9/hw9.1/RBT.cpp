#include <iostream>
#include <utility>
#include <vector>
using namespace std;

// 抽象基类：字典（bsTree的父类）
template<class K, class E>
class dictionary {
public:
    virtual E* find(const K& k) = 0;
    virtual void insert(const pair<K, E>& p) = 0;
    virtual void erase(const K& k) = 0;
    virtual void clear() = 0;
    virtual ~dictionary() = default;
};

// 抽象类bsTree（继承自dictionary）
template<class K, class E>
class bsTree : public dictionary<K, E> {
public:
    virtual void ascend() = 0;
    virtual int height() = 0;
    virtual ~bsTree() = default;
};

// 红黑树节点定义
template<class K, class E>
struct RBNode {
    pair<K, E> data;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    bool isRed;  // true: 红, false: 黑
    RBNode(const pair<K, E>& p) : data(p), left(nullptr), right(nullptr), parent(nullptr), isRed(true) {}
};

// 红黑树实现（继承bsTree）
template<class K, class E>
class redBlackTree : public bsTree<K, E> {
private:
    RBNode<K, E>* root;
    int nodeCount;  // 节点总数

    // 左旋
    void leftRotate(RBNode<K, E>* x) {
        RBNode<K, E>* y = x->right;
        x->right = y->left;
        if (y->left != nullptr) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    // 右旋
    void rightRotate(RBNode<K, E>* y) {
        RBNode<K, E>* x = y->left;
        y->left = x->right;
        if (x->right != nullptr) x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == nullptr) root = x;
        else if (y == y->parent->right) y->parent->right = x;
        else y->parent->left = x;
        x->right = y;
        y->parent = x;
    }

    // 插入后修复红黑树
    void insertFixup(RBNode<K, E>* z) {
        while (z->parent != nullptr && z->parent->isRed) {
            if (z->parent == z->parent->parent->left) {  // 父是左孩子
                RBNode<K, E>* y = z->parent->parent->right;  // 叔叔
                if (y != nullptr && y->isRed) {  // 叔叔红：变色
                    z->parent->isRed = false;
                    y->isRed = false;
                    z->parent->parent->isRed = true;
                    z = z->parent->parent;
                } else {  // 叔叔黑：旋转+变色
                    if (z == z->parent->right) {  // z是右孩子：先左旋
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->isRed = false;
                    z->parent->parent->isRed = true;
                    rightRotate(z->parent->parent);
                }
            } else {  // 父是右孩子（镜像操作）
                RBNode<K, E>* y = z->parent->parent->left;
                if (y != nullptr && y->isRed) {
                    z->parent->isRed = false;
                    y->isRed = false;
                    z->parent->parent->isRed = true;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->isRed = false;
                    z->parent->parent->isRed = true;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->isRed = false;  // 根始终为黑
    }

    // 查找节点（辅助函数）
    RBNode<K, E>* findNode(const K& k) const {
        RBNode<K, E>* cur = root;
        while (cur != nullptr) {
            if (k == cur->data.first) return cur;
            else if (k < cur->data.first) cur = cur->left;
            else cur = cur->right;
        }
        return nullptr;
    }

    // 中序遍历（辅助ascend）
    void inOrder(RBNode<K, E>* node, vector<K>& keys) const {
        if (node == nullptr) return;
        inOrder(node->left, keys);
        keys.push_back(node->data.first);
        inOrder(node->right, keys);
    }

    // 计算树高（辅助height）
    int calcHeight(RBNode<K, E>* node) const {
        if (node == nullptr) return 0;
        int leftH = calcHeight(node->left);
        int rightH = calcHeight(node->right);
        return max(leftH, rightH) + 1;
    }

    // 删除节点后修复红黑树
    void deleteFixup(RBNode<K, E>* x) {
        while (x != root && (x == nullptr || !x->isRed)) {
            if (x == x->parent->left) {  // x是左孩子
                RBNode<K, E>* w = x->parent->right;  // 兄弟
                if (w->isRed) {  // 兄弟红：变色+左旋
                    w->isRed = false;
                    x->parent->isRed = true;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                // 兄弟的两个孩子都黑
                if ((w->left == nullptr || !w->left->isRed) && (w->right == nullptr || !w->right->isRed)) {
                    w->isRed = true;
                    x = x->parent;
                } else {
                    if (w->right == nullptr || !w->right->isRed) {  // 兄弟右孩子黑：左孩子红
                        w->left->isRed = false;
                        w->isRed = true;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    // 兄弟右孩子红
                    w->isRed = x->parent->isRed;
                    x->parent->isRed = false;
                    w->right->isRed = false;
                    leftRotate(x->parent);
                    x = root;  // 退出循环
                }
            } else {  // x是右孩子（镜像操作）
                RBNode<K, E>* w = x->parent->left;
                if (w->isRed) {
                    w->isRed = false;
                    x->parent->isRed = true;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if ((w->right == nullptr || !w->right->isRed) && (w->left == nullptr || !w->left->isRed)) {
                    w->isRed = true;
                    x = x->parent;
                } else {
                    if (w->left == nullptr || !w->left->isRed) {
                        w->right->isRed = false;
                        w->isRed = true;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->isRed = x->parent->isRed;
                    x->parent->isRed = false;
                    w->left->isRed = false;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        if (x != nullptr) x->isRed = false;
    }

    // 释放节点（辅助clear）
    void destroy(RBNode<K, E>* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    redBlackTree() : root(nullptr), nodeCount(0) {}
    ~redBlackTree() { clear(); }

    // 查找：返回键k对应的值，找不到返回null
    E* find(const K& k) override {
        RBNode<K, E>* node = findNode(k);
        return (node != nullptr) ? &(node->data.second) : nullptr;
    }

    // 插入：不允许重复键
    void insert(const pair<K, E>& p) override {
        if (find(p.first) != nullptr) return;  // 键已存在，不插入

        RBNode<K, E>* z = new RBNode<K, E>(p);
        RBNode<K, E>* y = nullptr;
        RBNode<K, E>* x = root;

        // BST插入
        while (x != nullptr) {
            y = x;
            if (z->data.first < x->data.first) x = x->left;
            else x = x->right;
        }
        z->parent = y;
        if (y == nullptr) root = z;  // 空树
        else if (z->data.first < y->data.first) y->left = z;
        else y->right = z;

        nodeCount++;
        insertFixup(z);  // 修复红黑树
    }

    // 删除：删除键k对应的节点
    void erase(const K& k) override {
        RBNode<K, E>* z = findNode(k);
        if (z == nullptr) return;  // 键不存在

        RBNode<K, E>* y = z;
        RBNode<K, E>* x = nullptr;
        bool yOriginalColor = y->isRed;

        // 情况1：z只有右孩子
        if (z->left == nullptr) {
            x = z->right;
            transplant(z, z->right);
        }
        // 情况2：z只有左孩子
        else if (z->right == nullptr) {
            x = z->left;
            transplant(z, z->left);
        }
        // 情况3：z有两个孩子
        else {
            y = minimum(z->right);  // 后继节点
            yOriginalColor = y->isRed;
            x = y->right;
            if (y->parent == z) {
                if (x != nullptr) x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->isRed = z->isRed;
        }
        delete z;
        nodeCount--;
        if (!yOriginalColor) {  // 若删除的是黑节点，修复红黑树
            if (x != nullptr) deleteFixup(x);
            else if (root != nullptr) deleteFixup(root->left);  // 处理x为null的情况
        }
    }

    // 按升序输出所有键
    void ascend() override {
        vector<K> keys;
        inOrder(root, keys);
        cout << "(";
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << keys[i];
        }
        cout << ")" << endl;
    }

    // 清空树
    void clear() override {
        destroy(root);
        root = nullptr;
        nodeCount = 0;
    }

    // 返回树的高度（空树为0）
    int height() override {
        return calcHeight(root);
    }

    // 辅助函数：替换子树（红黑树删除用）
    void transplant(RBNode<K, E>* u, RBNode<K, E>* v) {
        if (u->parent == nullptr) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v != nullptr) v->parent = u->parent;
    }

    // 辅助函数：找子树的最小节点（红黑树删除用）
    RBNode<K, E>* minimum(RBNode<K, E>* node) const {
        while (node->left != nullptr) node = node->left;
        return node;
    }
};


// 测试用例
int main() {
    redBlackTree<int, string> T;

    // 1. 空树遍历
    cout << "1. ascend(T) -> ";
    T.ascend();

    // 2. 空树查找
    cout << "2. find(T, 10) -> ";
    if (T.find(10) == nullptr) cout << "null" << endl;
    else cout << "(" << 10 << ", " << *T.find(10) << ")" << endl;

    // 3. 插入30后遍历
    T.insert({30, "v30"});
    cout << "3. ascend(T) -> ";
    T.ascend();

    // 4. 插入15、40后遍历
    T.insert({15, "v15"});
    T.insert({40, "v40"});
    cout << "4. ascend(T) -> ";
    T.ascend();

    // 5. 插入10、20、35、50后遍历
    T.insert({10, "v10"});
    T.insert({20, "v20"});
    T.insert({35, "v35"});
    T.insert({50, "v50"});
    cout << "5. ascend(T) -> ";
    T.ascend();

    // 6. 查找存在的键20
    cout << "6. find(T, 20) -> ";
    if (T.find(20) != nullptr) cout << "(20, " << *T.find(20) << ")" << endl;
    else cout << "null" << endl;

    // 7. 查找不存在的键17
    cout << "7. find(T, 17) -> ";
    if (T.find(17) == nullptr) cout << "null" << endl;
    else cout << "(17, " << *T.find(17) << ")" << endl;

    // 8. 删除叶子10后遍历
    T.erase(10);
    cout << "8. ascend(T) -> ";
    T.ascend();

    // 9. 删除叶子50后遍历
    T.erase(50);
    cout << "9. ascend(T) -> ";
    T.ascend();

    // 10. 删除有两个子女的30后遍历+查找
    T.erase(30);
    cout << "10. ascend(T) -> ";
    T.ascend();
    cout << "    find(T, 30) -> ";
    if (T.find(30) == nullptr) cout << "null" << endl;
    else cout << "(30, " << *T.find(30) << ")" << endl;

    // 11. 插入18、37后遍历
    T.insert({18, "v18"});
    T.insert({37, "v37"});
    cout << "11. ascend(T) -> ";
    T.ascend();

    // 12. 删除18、37后遍历
    T.erase(18);
    T.erase(37);
    cout << "12. ascend(T) -> ";
    T.ascend();

    // 13. 清空后插入1-6再遍历
    T.clear();
    T.insert({1, "v1"});
    T.insert({2, "v2"});
    T.insert({3, "v3"});
    T.insert({4, "v4"});
    T.insert({5, "v5"});
    T.insert({6, "v6"});
    cout << "13. ascend(T) -> ";
    T.ascend();

    // 14. 输出树高
    cout << "14. height(T) -> " << T.height() << endl;

    return 0;
}