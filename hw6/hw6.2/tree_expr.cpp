#include <bits/stdc++.h>
using namespace std;

/* ---------- 二叉树节点 ---------- */
struct Node {
    char op;                 // 运算符或操作数
    Node *left = nullptr;
    Node *right = nullptr;
    Node(char c): op(c) {}
};

/* ---------- 中缀转后缀（调度场） ---------- */
int prec(char c) {
    if (c == '+' || c == '-') return 1;
    if (c == '*' || c == '/') return 2;
    return 0;
}
vector<char> infixToPostfix(const string& s) {
    vector<char> out;
    stack<char> st;
    for (char c : s) {
        if (isspace(c)) continue;
        if (isalpha(c)) { out.push_back(c); continue; }
        if (c == '(') { st.push(c); continue; }
        if (c == ')') {
            while (!st.empty() && st.top() != '(') {
                out.push_back(st.top()); st.pop();
            }
            if (!st.empty()) st.pop(); // 弹出 '('
            continue;
        }
        // 运算符
        while (!st.empty() && prec(st.top()) >= prec(c)) {
            out.push_back(st.top()); st.pop();
        }
        st.push(c);
    }
    while (!st.empty()) { out.push_back(st.top()); st.pop(); }
    return out;
}

/* ---------- 后缀建树 ---------- */
Node* buildTree(const vector<char>& post) {
    stack<Node*> st;
    for (char c : post) {
        if (isalpha(c)) {
            st.push(new Node(c));
        } else {                           // 运算符
            Node* R = st.top(); st.pop();  // 先右操作数
            Node* L = st.top(); st.pop();  // 再左操作数
            Node* p = new Node(c);
            p->left  = L;
            p->right = R;
            st.push(p);
        }
    }
    return st.empty() ? nullptr : st.top();
}


/* ---------- 缩进打印 ---------- */
const int IND = 4; // 每层缩进空格数
void printTree(Node* root, int depth = 0) {
    if (!root) return;
    // 先右子树（在上）
    printTree(root->right, depth + 1);
    // 当前节点
    cout << string(depth * IND, ' ') << root->op << '\n';
    // 后左子树（在下）
    printTree(root->left,  depth + 1);
}

/* ---------- 主流程 ---------- */
int main() {
    ifstream fin("input.txt");
    if (!fin) { cerr << "无法打开 input.txt\n"; return 0; }
    string line;
    getline(fin, line);
    fin.close();

    auto post = infixToPostfix(line);
    Node* root = buildTree(post);
    if (!root) { cout << "空表达式\n"; return 0; }

    printTree(root);

    // 简单内存释放（可选）
    function<void(Node*)> del = [&](Node* p){
        if (!p) return;
        del(p->left);
        del(p->right);
        delete p;
    };
    del(root);
    return 0;
}