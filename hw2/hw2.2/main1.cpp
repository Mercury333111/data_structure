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

    // 成员方法 split
    void split(ExtendedChain& a, ExtendedChain& b) {
        Node* current = head->next; // 当前节点
        Node* lastA = nullptr;      // a链表的最后一个节点
        Node* lastB = nullptr;      // b链表的最后一个节点
        int index = 0;

        while (current != head) {
            Node* next = current->next; // 保存下一个节点

            if (index % 2 == 0) {
                // 将当前节点加入到a链表
                if (lastA == nullptr) {
                    a.head->next = current;
                    a.head->prev = current;
                    current->next = a.head;
                    current->prev = a.head;
                } else {
                    lastA->next = current;
                    current->prev = lastA;
                    current->next = a.head;
                    a.head->prev = current;
                }
                lastA = current;
            } else {
                // 将当前节点加入到b链表
                if (lastB == nullptr) {
                    b.head->next = current;
                    b.head->prev = current;
                    current->next = b.head;
                    current->prev = b.head;
                } else {
                    lastB->next = current;
                    current->prev = lastB;
                    current->next = b.head;
                    b.head->prev = current;
                }
                lastB = current;
            }

            current = next; // 移动到下一个节点
            index++;
        }

        // 清空原链表
        head->next = head;
        head->prev = head;
    }
};

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
        c.split(a, b);

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