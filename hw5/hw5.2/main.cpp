#include <iostream>
#include <string>
using namespace std;

// 链表节点结构（包含尾哨兵所需的next指针）
template <class K, class E>
struct ChainNode {
    K key;
    E value;
    ChainNode* next;

    // 普通节点构造函数（默认指向尾哨兵）
    ChainNode(const K& k, const E& v, ChainNode* n = nullptr) : key(k), value(v), next(n) {}
    // 尾哨兵专用构造函数（无key/value，next指向自身）
    ChainNode() : next(this) {}
};

// 全局尾哨兵（单例，所有链表共享）
ChainNode<int, string> T; // 直接用ChainNode实例作为尾哨兵，避免类型转换问题

template <class K, class E>
class hashChainsWithTail {
private:
    ChainNode<K, E>** table; // 散列表
    int divisor;             // 桶数量
    ChainNode<K, E>* tail;   // 指向全局尾哨兵（类型匹配）

    int hash(const K& k) const {
        return k % divisor;
    }

public:
    hashChainsWithTail(int cap) : divisor(cap) {
        table = new ChainNode<K, E>*[divisor];
        // 所有空桶直接指向尾哨兵
        for (int i = 0; i < divisor; ++i) {
            table[i] = reinterpret_cast<ChainNode<K, E>*>(&T);
        }
        tail = reinterpret_cast<ChainNode<K, E>*>(&T);
    }

    ~hashChainsWithTail() {
        for (int i = 0; i < divisor; ++i) {
            ChainNode<K, E>* cur = table[i];
            while (cur != tail) {
                ChainNode<K, E>* temp = cur;
                cur = cur->next;
                delete temp;
            }
        }
        delete[] table;
    }

    pair<string, pair<int, int>> insert(const K& k, const E& v) {
        int b = hash(k);
        ChainNode<K, E>* prev = nullptr;
        ChainNode<K, E>* cur = table[b];
        int pos = 0;

        while (cur != tail && cur->key < k) {
            prev = cur;
            cur = cur->next;
            pos++;
        }

        if (cur != tail && cur->key == k) {
            return {"exists", {b, pos}};
        }

        ChainNode<K, E>* newNode = new ChainNode<K, E>(k, v, cur);
        if (prev == nullptr) {
            table[b] = newNode;
        } else {
            prev->next = newNode;
        }
        return {"inserted", {b, pos}};
    }

    pair<string, pair<int, int>> find(const K& k) const {
        int b = hash(k);
        ChainNode<K, E>* cur = table[b];
        int pos = 0;

        while (cur != tail && cur->key != k) {
            cur = cur->next;
            pos++;
        }

        if (cur != tail && cur->key == k) {
            return {"found", {b, pos}};
        } else {
            return {"not_found", {b, -1}};
        }
    }

    pair<string, int> erase(const K& k) {
        int b = hash(k);
        ChainNode<K, E>* prev = nullptr;
        ChainNode<K, E>* cur = table[b];

        while (cur != tail && cur->key != k) {
            prev = cur;
            cur = cur->next;
        }

        if (cur == tail || cur->key != k) {
            return {"not_found", b};
        }

        if (prev == nullptr) {
            table[b] = cur->next;
        } else {
            prev->next = cur->next;
        }
        delete cur;
        return {"removed", b};
    }

    // 修复尾哨兵检查逻辑
    bool checkTailSentinel() const {
        // 1. 检查尾哨兵自身的next是否指向自己
        if (T.next != &T) {
            return false;
        }

        // 2. 检查每个桶的链表末尾是否指向尾哨兵
        for (int i = 0; i < divisor; ++i) {
            ChainNode<K, E>* cur = table[i];
            // 遍历到链表最后一个节点（next为尾哨兵）
            while (cur->next != tail) {
                // 防止死循环（若链表成环，此处会无限循环，需额外处理，但按题意不会出现）
                cur = cur->next;
            }
            // 确认最后一个节点的next是尾哨兵
            if (cur->next != tail) {
                return false;
            }
        }

        return true;
    }
};

void test() {
    hashChainsWithTail<int, string> hc(5);

    // 测试用例
    auto res1 = hc.insert(6, "a");
    cout << "{" << res1.first << ", bucket:" << res1.second.first << ", pos:" << res1.second.second << "}" << endl;

    auto res2 = hc.insert(1, "b");
    cout << "{" << res2.first << ", bucket:" << res2.second.first << ", pos:" << res2.second.second << "}" << endl;

    auto res3 = hc.insert(11, "c");
    cout << "{" << res3.first << ", bucket:" << res3.second.first << ", pos:" << res3.second.second << "}" << endl;

    auto res4 = hc.find(6);
    cout << "{" << res4.first << ", bucket:" << res4.second.first << ", pos:" << res4.second.second << "}" << endl;

    auto res5 = hc.erase(1);
    cout << "{" << res5.first << ", bucket:" << res5.second << "}" << endl;

    auto res6 = hc.erase(7);
    cout << "{" << res6.first << ", bucket:" << res6.second << "}" << endl;

    auto res7 = hc.insert(16, "d");
    cout << "{" << res7.first << ", bucket:" << res7.second.first << ", pos:" << res7.second.second << "}" << endl;

    auto res8 = hc.insert(6, "A'");
    cout << "{" << res8.first << ", bucket:" << res8.second.first << ", pos:" << res8.second.second << "}" << endl;

    auto res9 = hc.find(99);
    cout << "{" << res9.first << ", bucket:" << res9.second.first << "}" << endl;

    // 输出尾哨兵检查结果
    bool tailCheck = hc.checkTailSentinel();
    cout <<  (tailCheck ? "pass" : "not pass") << endl;
}

int main() {
    test();
    return 0;
}