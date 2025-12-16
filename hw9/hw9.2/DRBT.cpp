#include <iostream>
#include <utility>
#include <vector>
using namespace std;

// 抽象基类：字典（dBSTree的父类）
template<class K, class E>
class dictionary {
public:
    virtual E* find(const K& k) = 0;
    virtual void insert(const pair<K, E>& p) = 0;
    virtual void erase(const K& k) = 0;
    virtual void clear() = 0;
    virtual ~dictionary() = default;
};

// 抽象类dBSTree（支持重复键的二叉搜索树）
template<class K, class E>
class dBSTree : public dictionary<K, E> {
public:
    virtual void ascend() = 0;
    virtual int height() = 0;
    virtual ~dBSTree() = default;
};

// 红黑树节点定义（同普通红黑树，无修改）
template<class K, class E>
struct RBNode {
    pair<K, E> data;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    bool isRed;  // true:红，false:黑
    RBNode(const pair<K, E>& p) : data(p), left(nullptr), right(nullptr), parent(nullptr), isRed(true) {}
};

// 带重复键的红黑树实现
template<class K, class E>
class dRedBlackTree : public dBSTree<K, E> {
private:
    RBNode<K, E>* root;
    int nodeCount;

    // 左旋（复用普通红黑树逻辑）
    void leftRotate(RBNode<K, E>* x) {
        RBNode<K, E>* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    // 右旋（复用普通红黑树逻辑）
    void rightRotate(RBNode<K, E>* y) {
        RBNode<K, E>* x = y->left;
        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        if (!y->parent) root = x;
        else if (y == y->parent->right) y->parent->right = x;
        else y->parent->left = x;
        x->right = y;
        y->parent = x;
    }

    // 插入后修复（复用普通红黑树逻辑）
    void insertFixup(RBNode<K, E>* z) {
        while (z->parent && z->parent->isRed) {
            if (z->parent == z->parent->parent->left) {
                RBNode<K, E>* y = z->parent->parent->right;
                if (y && y->isRed) {
                    z->parent->isRed = false;
                    y->isRed = false;
                    z->parent->parent->isRed = true;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->isRed = false;
                    z->parent->parent->isRed = true;
                    rightRotate(z->parent->parent);
                }
            } else {
                RBNode<K, E>* y = z->parent->parent->left;
                if (y && y->isRed) {
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
        root->isRed = false;
    }

    // 查找节点（返回任意一个键为k的节点，支持重复键）
    RBNode<K, E>* findNode(const K& k) const {
        RBNode<K, E>* cur = root;
        while (cur) {
            if (k == cur->data.first) return cur;  // 找到任意一个即返回
            else if (k < cur->data.first) cur = cur->left;
            else cur = cur->right;
        }
        return nullptr;
    }

    // 查找待删除的节点（找最左侧的k节点，确保删除一个且不破坏有序性）
    RBNode<K, E>* findEraseNode(const K& k) const {
        RBNode<K, E>* cur = root;
        RBNode<K, E>* target = nullptr;
        while (cur) {
            if (k == cur->data.first) {
                target = cur;  // 记录第一个找到的节点（最左侧）
                cur = cur->left;  // 继续向左找，确保删除最左侧的k（不影响其他重复键）
            } else if (k < cur->data.first) {
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        return target;
    }

    // 中序遍历（输出所有键，包括重复键）
    void inOrder(RBNode<K, E>* node, vector<K>& keys) const {
        if (!node) return;
        inOrder(node->left, keys);
        keys.push_back(node->data.first);
        inOrder(node->right, keys);
    }

    // 计算树高
    int calcHeight(RBNode<K, E>* node) const {
        if (!node) return 0;
        return max(calcHeight(node->left), calcHeight(node->right)) + 1;
    }

    // 删除后修复（复用普通红黑树逻辑）
    void deleteFixup(RBNode<K, E>* x) {
        while (x != root && (!x || !x->isRed)) {
            if (x == x->parent->left) {
                RBNode<K, E>* w = x->parent->right;
                if (w->isRed) {
                    w->isRed = false;
                    x->parent->isRed = true;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if ((!w->left || !w->left->isRed) && (!w->right || !w->right->isRed)) {
                    w->isRed = true;
                    x = x->parent;
                } else {
                    if (!w->right || !w->right->isRed) {
                        w->left->isRed = false;
                        w->isRed = true;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->isRed = x->parent->isRed;
                    x->parent->isRed = false;
                    w->right->isRed = false;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                RBNode<K, E>* w = x->parent->left;
                if (w->isRed) {
                    w->isRed = false;
                    x->parent->isRed = true;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if ((!w->right || !w->right->isRed) && (!w->left || !w->left->isRed)) {
                    w->isRed = true;
                    x = x->parent;
                } else {
                    if (!w->left || !w->left->isRed) {
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
        if (x) x->isRed = false;
    }

    // 释放节点（清空树）
    void destroy(RBNode<K, E>* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    dRedBlackTree() : root(nullptr), nodeCount(0) {}
    ~dRedBlackTree() { clear(); }

    // 查找：返回任意一个键为k的值，找不到返回null
    E* find(const K& k) override {
        RBNode<K, E>* node = findNode(k);
        return node ? &(node->data.second) : nullptr;
    }

    // 插入：支持重复键（重复键插入到右子树）
    void insert(const pair<K, E>& p) override {
        RBNode<K, E>* z = new RBNode<K, E>(p);
        RBNode<K, E>* y = nullptr;
        RBNode<K, E>* x = root;

        // BST插入：k <= 当前节点键时，继续查找左子树；k > 时查找右子树（重复键插入右子树）
        while (x) {
            y = x;
            if (z->data.first < x->data.first) {
                x = x->left;
            } else {  // 关键修改：重复键（==）插入到右子树
                x = x->right;
            }
        }
        z->parent = y;
        if (!y) root = z;
        else if (z->data.first < y->data.first) y->left = z;
        else y->right = z;

        nodeCount++;
        insertFixup(z);  // 修复红黑树平衡
    }

    // 删除：删除任意一个键为k的节点（优先删最左侧的k）
    void erase(const K& k) override {
        RBNode<K, E>* z = findEraseNode(k);
        if (!z) return;  // 无该键，直接返回

        RBNode<K, E>* y = z;
        RBNode<K, E>* x = nullptr;
        bool yOriginalColor = y->isRed;

        // 按子女数量分情况替换（同普通红黑树）
        if (!z->left) {
            x = z->right;
            transplant(z, z->right);
        } else if (!z->right) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->isRed;
            x = y->right;
            if (y->parent == z) {
                if (x) x->parent = y;
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
        if (!yOriginalColor) {
            if (x != nullptr) deleteFixup(x);
            else if (root) deleteFixup(root);  // 处理x为null的情况
        }
    }

    // 升序输出所有键（包括重复键）
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

    // 返回树高（空树为0）
    int height() override {
        return calcHeight(root);
    }

    // 辅助函数：替换子树
    void transplant(RBNode<K, E>* u, RBNode<K, E>* v) {
        if (!u->parent) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v) v->parent = u->parent;
    }

    // 辅助函数：找子树最小节点（后继节点）
    RBNode<K, E>* minimum(RBNode<K, E>* node) const {
        while (node->left) node = node->left;
        return node;
    }
};


// 测试用例
int main() {
    dRedBlackTree<int, string> T;

    // 1. 空树遍历
    cout << "1. ascend(T) -> ";
    T.ascend();

    // 2. 空树查找
    cout << "2. find(T, 10) -> ";
    cout << (T.find(10) ? "非null" : "null") << endl;

    // 3. 插入20、10、30、两个10（共3个10）
    T.insert({20, "v20"});
    T.insert({10, "v10"});
    T.insert({30, "v30"});
    T.insert({10, "v10_2"});
    T.insert({10, "v10_3"});
    cout << "3. ascend(T) -> ";
    T.ascend();

    // 4. 查找重复键10
    cout << "4. find(T, 10) -> ";
    string* val = T.find(10);
    if (val) cout << "(10, " << *val << ")" << endl;
    else cout << "null" << endl;

    // 5. 删除一个10（剩两个10）
    T.erase(10);
    cout << "5. ascend(T) -> ";
    T.ascend();

    // 6. 再删除两个10（删光）
    T.erase(10);
    T.erase(10);
    cout << "6. ascend(T) -> ";
    T.ascend();

    // 7. 验证10已删除
    cout << "7. find(T, 10) -> ";
    cout << (T.find(10) ? "非null" : "null") << endl;

    // 8. 混合场景：插入3、三个5、8，删除一个5
    T.clear();
    T.insert({5, "A"});
    T.insert({5, "B"});
    T.insert({5, "C"});
    T.insert({3, "D"});
    T.insert({8, "E"});
    cout << "8. after insert ascend(T) -> ";
    T.ascend();
    T.erase(5);
    cout << "   after erase ascend(T) -> ";
    T.ascend();

    // 9. 插入1-6（无重复），输出并查高
    T.clear();
    T.insert({1, "v1"});
    T.insert({2, "v2"});
    T.insert({3, "v3"});
    T.insert({4, "v4"});
    T.insert({5, "v5"});
    T.insert({6, "v6"});
    cout << "9. ascend(T) -> ";
    T.ascend();
    cout << "   height(T) -> " << T.height() << endl;

    // 10. 逆序插入10-50（无重复），输出并查高
    T.clear();
    T.insert({50, "v50"});
    T.insert({40, "v40"});
    T.insert({30, "v30"});
    T.insert({20, "v20"});
    T.insert({10, "v10"});
    cout << "10. ascend(T) -> ";
    T.ascend();
    cout << "    height(T) -> " << T.height() << endl;

    return 0;
}