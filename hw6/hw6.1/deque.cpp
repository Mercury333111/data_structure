#include <bits/stdc++.h>
using namespace std;

const int MAXN = 3;          // 默认容量为 3

struct Deque {
    int a[MAXN];
    int L, R;                // L 指向真实左端，R 指向真实右端
    int cnt;                 // 当前元素个数

    Deque() { Create(); }

    void Create() {
        L = 0;
        R = -1;              // 初始置为空
        cnt = 0;
    }

    bool IsEmpty() const { return cnt == 0; }
    bool IsFull()  const { return cnt == MAXN; }

    int Left()  const { return IsEmpty() ? -1 : a[L]; }
    int Right() const { return IsEmpty() ? -1 : a[R]; }

    bool AddLeft(int x) {
        if (IsFull()) return false;
        L = (L - 1 + MAXN) % MAXN;
        a[L] = x;
        ++cnt;
        if (cnt == 1) R = L; // 第一个元素
        return true;
    }

    bool AddRight(int x) {
        if (IsFull()) return false;
        R = (R + 1) % MAXN;
        a[R] = x;
        ++cnt;
        if (cnt == 1) L = R;
        return true;
    }

    bool DeleteLeft() {
        if (IsEmpty()) return false;
        L = (L + 1) % MAXN;
        --cnt;
        return true;
    }

    bool DeleteRight() {
        if (IsEmpty()) return false;
        R = (R - 1 + MAXN) % MAXN;
        --cnt;
        return true;
    }

    // 按题意从左到右输出，元素间空格，行末无空格
    void Print() const {
        if (IsEmpty()) {
            cout << '\n';
            return;
        }
        for (int i = 0, p = L; i < cnt; ++i, p = (p + 1) % MAXN) {
            if (i) cout << ' ';
            cout << a[p];
        }
        cout << '\n';
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ifstream fin("input.txt");
    if (!fin) {
        cerr << "无法打开 input.txt\n";
        return 0;
    }

    Deque dq;
    string op;
    while (fin >> op) {
        if (op == "End") break;

        if (op == "Create") {
            dq.Create();
            continue;               // Create 无输出
        }

        if (op == "IsEmpty") {
            cout << (dq.IsEmpty() ? "Yes\n" : "No\n");
        }
        else if (op == "IsFull") {
            cout << (dq.IsFull() ? "Yes\n" : "No\n");
        }
        else if (op == "Left") {
            if (dq.IsEmpty()) cout << "EMPTY\n";
            else cout << dq.Left() << '\n';
        }
        else if (op == "Right") {
            if (dq.IsEmpty()) cout << "EMPTY\n";
            else cout << dq.Right() << '\n';
        }
        else if (op == "AddLeft") {
            int x; fin >> x;
            if (dq.IsFull()) cout << "FULL\n";
            else {
                dq.AddLeft(x);
                dq.Print();
            }
        }
        else if (op == "AddRight") {
            int x; fin >> x;
            if (dq.IsFull()) cout << "FULL\n";
            else {
                dq.AddRight(x);
                dq.Print();
            }
        }
        else if (op == "DeleteLeft") {
            if (dq.IsEmpty()) cout << "EMPTY\n";
            else {
                dq.DeleteLeft();
                dq.Print();
            }
        }
        else if (op == "DeleteRight") {
            if (dq.IsEmpty()) cout << "EMPTY\n";
            else {
                dq.DeleteRight();
                dq.Print();
            }
        }
        else if (op == "Print") {
            dq.Print();
        }
        else {                      // 非法指令
            cout << "WRONG\n";
        }
    }
    return 0;
}