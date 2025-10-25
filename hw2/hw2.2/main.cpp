#include <iostream>
#include <vector>
using namespace std;

// 定义双向循环链表节点
struct Node {
    int data;
    Node* prev;
    Node* next;

    Node(int val) : data(val), prev(nullptr), next(nullptr) {}
};

// 扩展链表类
class ExtendedChain {
private:
    Node* head; // 头节点

public:
    ExtendedChain() {
        head = new Node(0); // 创建头节点
        head->prev = head;
        head->next = head;
    }

    // 析构函数，释放链表内存
    ~ExtendedChain() {
        if (head) {
            Node* current = head->next;
            while (current != head) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
            delete head;
        }
    }

    // 插入元素到链表尾部
    void insert(int val) {
        Node* newNode = new Node(val);
        newNode->prev = head->prev;
        newNode->next = head;
        head->prev->next = newNode;
        head->prev = newNode;
    }

    // 打印链表
    void print() const {
        Node* current = head->next;
        while (current != head) {
            cout << current->data << " ";
            current = current->next;
        }
    }

    // 获取头节点的指针
    Node* getHead() const {
        return head;
    }
};

// 非成员方法 split
void split(const ExtendedChain& c, ExtendedChain& a, ExtendedChain& b) {
    int index = 0;
    Node* current = c.getHead()->next; // 使用公共方法获取头节点

    while (current != c.getHead()) {
        if (index % 2 == 0) {
            a.insert(current->data);
        } else {
            b.insert(current->data);
        }
        current = current->next;
        index++;
    }
}

// 测试代码
int main() {
    // 测试用例
    vector<vector<int>> testCases = {
        {}, {7}, {1, 2}, {1, 2, 3, 4, 5}, {10, 20, 30, 40}, {-1, -1, 0, 7}
    };

    for (const auto& testCase : testCases) {
        ExtendedChain c;
        for (int val : testCase) {
            c.insert(val);
        }

        ExtendedChain a, b;
        split(c, a, b);

        cout << "c = [";
        c.print();
        cout << "] → a = [";
        a.print();
        cout << "], b = [";
        b.print();
        cout << "]" << endl;
    }

    return 0;
}