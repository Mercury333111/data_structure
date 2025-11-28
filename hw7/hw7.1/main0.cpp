#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

// 定义 push 方法的返回值结构体
struct PushResult {
    string status;  // "ok" / "full" / "invalid"
    int size;       // 当前堆大小
    long long top;  // 当前堆顶（满堆时也返回）
};

// 定义 pop 方法的返回值结构体
struct PopResult {
    string status;  // "ok" / "empty"
    long long value;// 弹出的值（empty 时无意义）
    int size;       // 弹出后的堆大小
    long long top;  // 弹出后的堆顶（非空时有效）
};

template<class T>
class maxHeap {
private:
    T* heap;          // 堆数组，heap[0]存maxElement，heap[1:n]存元素，heap[n+1:2n+1]存minElement
    int heapSize;     // 当前堆中元素个数（n）
    int capacity;     // 堆的最大容量（即测试用例中的cap）
    T maxElement;     // 堆中元素的上限
    T minElement;     // 堆中元素的下限

public:
    // 构造函数：传入容量、maxElement、minElement
    maxHeap(int cap, const T& maxE, const T& minE) 
        : capacity(cap), maxElement(maxE), minElement(minE), heapSize(0) {
        // 数组大小为2*cap+1（对应heap[0:2cap+1]）
        heap = new T[2 * capacity + 1];
        heap[0] = maxElement;  // heap[0]存maxElement
        // heap[n+1:2n+1]存minElement（n最大为capacity，所以初始化heap[capacity+1 : 2*capacity+1]）
        for (int i = capacity + 1; i <= 2 * capacity + 1; ++i) {
            heap[i] = minElement;
        }
    }

    // 析构函数
    ~maxHeap() {
        delete[] heap;
    }

    // 判断堆是否为空
    bool empty() const {
        return heapSize == 0;
    }

    // 返回堆中元素个数
    int size() const {
        return heapSize;
    }

    // 返回堆顶元素（最大元素）
    pair<bool, T> top() const {
        if (empty()) {
            return {false, T()};
        }
        return {true, heap[1]};
    }

    // 插入元素：返回自定义结构体
    PushResult push(const T& theElement) {
        PushResult res;
        // 检查是否超过容量
        if (heapSize == capacity) {
            res.status = "full";
            res.size = heapSize;
            auto t = top();
            res.top = t.first ? t.second : T();
            return res;
        }
        // 检查元素是否在[minElement, maxElement]范围内
        if (theElement > maxElement || theElement < minElement) {
            res.status = "invalid";
            res.size = heapSize;
            res.top = T();
            return res;
        }

        // 元素个数+1
        ++heapSize;
        int currentNode = heapSize;
        // 上滤：当前节点的父节点值 < 当前元素，就交换
        while (currentNode != 1 && heap[currentNode / 2] < theElement) {
            heap[currentNode] = heap[currentNode / 2];
            currentNode /= 2;
        }
        // 插入元素
        heap[currentNode] = theElement;

        res.status = "ok";
        res.size = heapSize;
        auto t = top();
        res.top = t.first ? t.second : T();
        return res;
    }

    // 删除堆顶元素：返回自定义结构体
    PopResult pop() {
        PopResult res;
        if (empty()) {
            res.status = "empty";
            res.value = T();
            res.size = heapSize;
            res.top = T();
            return res;
        }

        // 保存堆顶元素（要返回的值）
        T maxVal = heap[1];
        // 取最后一个元素，元素个数-1
        T lastElement = heap[heapSize--];
        int currentNode = 1;
        int child = 2;  // 当前节点的左孩子

        // 下滤：将lastElement放到合适位置
        while (child <= heapSize) {
            // 选择较大的孩子
            if (child < heapSize && heap[child] < heap[child + 1]) {
                ++child;
            }
            // 若lastElement >= 较大的孩子，结束
            if (lastElement >= heap[child]) {
                break;
            }
            // 否则，孩子上移
            heap[currentNode] = heap[child];
            currentNode = child;
            child *= 2;
        }
        // 放置lastElement
        heap[currentNode] = lastElement;

        res.status = "ok";
        res.value = maxVal;
        res.size = heapSize;
        auto t = top();
        res.top = t.first ? t.second : T();
        return res;
    }
};

// 测试代码（匹配题目用例）
int main() {
    // 初始化堆：容量6，maxElement=1e9，minElement=-1e9
    maxHeap<long long> h(6, 1000000000LL, -1000000000LL);

    // 1. empty()
    cout << "1. empty() -> {" << (h.empty() ? "true" : "false") << "}\n";
    // 2. size()
    cout << "2. size() -> {" << h.size() << "}\n";
    // 3. push(50)
    auto res3 = h.push(50);
    cout << "3. push(50) -> {ok, size:" << res3.size << ", top:" << res3.top << "}\n";
    // 4. push(30)
    auto res4 = h.push(30);
    cout << "4. push(30) -> {ok, size:" << res4.size << ", top:" << res4.top << "}\n";
    // 5. push(999999999)
    auto res5 = h.push(999999999LL);
    cout << "5. push(999999999) -> {ok, size:" << res5.size << ", top:" << res5.top << "}\n";
    // 6. push(-999999999)
    auto res6 = h.push(-999999999LL);
    cout << "6. push(-999999999) -> {ok, size:" << res6.size << ", top:" << res6.top << "}\n";
    // 7. push(50)
    auto res7 = h.push(50);
    cout << "7. push(50) -> {ok, size:" << res7.size << ", top:" << res7.top << "}\n";
    // 8. push(10)
    auto res8 = h.push(10);
    cout << "8. push(10) -> {ok, size:" << res8.size << ", top:" << res8.top << "}\n";
    // 9. push(60)
    auto res9 = h.push(60);
    cout << "9. push(60) -> {full, size:" << res9.size << ", top:" << res9.top << "}\n";
    // 10. top()
    auto top10 = h.top();
    cout << "10. top() -> {ok, value:" << top10.second << "}\n";
    // 11. empty()
    cout << "11. empty() -> {" << (h.empty() ? "true" : "false") << "}\n";
    // 12. pop()
    auto pop12 = h.pop();
    cout << "12. pop() -> {ok, value:" << pop12.value << ", size:" << pop12.size << ", top:" << pop12.top << "}\n";
    // 13. pop()
    auto pop13 = h.pop();
    cout << "13. pop() -> {ok, value:" << pop13.value << ", size:" << pop13.size << ", top:" << pop13.top << "}\n";
    // 14. pop()
    auto pop14 = h.pop();
    cout << "14. pop() -> {ok, value:" << pop14.value << ", size:" << pop14.size << ", top:" << pop14.top << "}\n";
    // 15. pop()
    auto pop15 = h.pop();
    cout << "15. pop() -> {ok, value:" << pop15.value << ", size:" << pop15.size << ", top:" << pop15.top << "}\n";
    // 16. pop()
    auto pop16 = h.pop();
    cout << "16. pop() -> {ok, value:" << pop16.value << ", size:" << pop16.size << ", top:" << pop16.top << "}\n";
    // 17. pop()
    auto pop17 = h.pop();
    cout << "17. pop() -> {ok, value:" << pop17.value << ", size:" << pop17.size << "}\n";
    // 18. pop()
    auto pop18 = h.pop();
    cout << "18. pop() -> {empty, size:" << pop18.size << "}\n";

    return 0;
}