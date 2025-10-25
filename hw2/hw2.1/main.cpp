#include <iostream>
#include <string>
using namespace std;

// 定义链表节点结构
struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

// 定义扩展链表类
class ExtendedChain {
private:
    Node* head;
    Node* tail;

public:
    ExtendedChain() : head(nullptr), tail(nullptr) {}

    // 添加元素到链表尾部
    void append(int val) {
        Node* newNode = new Node(val);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    // 获取链表头节点
    Node* getHead() const {
        return head;
    }

    // 打印链表
    void print() const {
        if (head == nullptr) {
            cout << "[]" << endl;
            return;
        }

        cout << "[";
        Node* current = head;
        while (current) {
            cout << current->data;
            if (current->next) {
                cout << ", ";
            }
            current = current->next;
        }
        cout << "]" << endl;
    }
};

// meld
ExtendedChain meld(const ExtendedChain& a, const ExtendedChain& b) {
    ExtendedChain c;
    Node* aNode = a.getHead();
    Node* bNode = b.getHead();

    while (aNode && bNode) {
        c.append(aNode->data);
        aNode = aNode->next;
        c.append(bNode->data);
        bNode = bNode->next;
    }

    // 添加剩余的节点
    while (aNode) {
        c.append(aNode->data);
        aNode = aNode->next;
    }
    while (bNode) {
        c.append(bNode->data);
        bNode = bNode->next;
    }

    return c;
}

// 测试
void testMeld() {
    ExtendedChain a, b, c;

    // 测试用例1
    a = ExtendedChain();
    b = ExtendedChain();
    c = meld(a, b);
    cout << "a = [], b = [] -> c = ";
    c.print();

    // 测试用例2
    a = ExtendedChain();
    b = ExtendedChain();
    b.append(1); b.append(2); b.append(3);
    c = meld(a, b);
    cout << "a = [], b = [1,2,3] -> c = ";
    c.print();

    // 测试用例3
    a = ExtendedChain();
    a.append(7); a.append(8);
    b = ExtendedChain();
    c = meld(a, b);
    cout << "a = [7,8], b = [] -> c = ";
    c.print();

    // 测试用例4
    a = ExtendedChain();
    a.append(1); a.append(3); a.append(5);
    b = ExtendedChain();
    b.append(2); b.append(4); b.append(6);
    c = meld(a, b);
    cout << "a = [1,3,5], b = [2,4,6] -> c = ";
    c.print();

    // 测试用例5
    a = ExtendedChain();
    a.append(10); a.append(20); a.append(30); a.append(40);
    b = ExtendedChain();
    b.append(11);
    c = meld(a, b);
    cout << "a = [10,20,30,40], b = [11] -> c = ";
    c.print();

    // 测试用例6
    a = ExtendedChain();
    a.append(-3); a.append(0); a.append(5);
    b = ExtendedChain();
    b.append(-2); b.append(-1); b.append(6); b.append(7);
    c = meld(a, b);
    cout << "a = [-3,0,5], b = [-2,-1,6,7] -> c = ";
    c.print();
}

int main() {
    testMeld();
    return 0;
}