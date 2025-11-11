#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class LinearProbingHashTable {
private:
    struct Entry {
        int key;
        string value;
        bool inUse;     // 表示该桶当前是否有有效数据（未被删除）
        bool neverUsed; // 表示该桶是否从未被使用过（包括未插入和未删除）
    };

    vector<Entry> table;
    int capacity;
    int size; // 当前有效数据个数（inUse=true 的桶数）

    // 哈希函数
    int hash(int key) {
        return key % capacity;
    }

    // 计算空桶中 neverUsed=false 的比例（用于触发重组织）
    double getNeverUsedFalseRatio() {
        int emptyCount = 0;          // 空桶总数（inUse=false 的桶）
        int neverUsedFalseCount = 0; // 空桶中 neverUsed=false 的数量
        for (const auto& entry : table) {
            if (!entry.inUse) {
                emptyCount++;
                if (!entry.neverUsed) {
                    neverUsedFalseCount++;
                }
            }
        }
        return emptyCount == 0 ? 0.0 : (double)neverUsedFalseCount / emptyCount;
    }

    // 重组织散列表：重新插入所有有效数据，空桶 neverUsed 设为 true
    void reorganize() {
        vector<Entry> oldTable = table;
        // 重置新表：所有桶 neverUsed=true，inUse=false
        table.assign(capacity, {0, "", false, true});
        size = 0;

        // 重新插入旧表中所有有效数据（inUse=true）
        for (const auto& entry : oldTable) {
            if (entry.inUse) {
                insert(entry.key, entry.value, true); // 内部调用，跳过重组织检查
            }
        }
    }

    // 内部插入接口：skipReorganize=true 时跳过重组织检查（避免递归）
    string insert(int key, const string& value, bool skipReorganize) {
        if (!skipReorganize && getNeverUsedFalseRatio() >= 0.6) {
            reorganize();
        }

        int index = hash(key);
        int start = index;
        int firstDeletedIndex = -1; // 记录第一个遇到的已删除桶（inUse=false）

        do {
            // 情况1：桶从未使用过（neverUsed=true）—— 直接插入
            if (table[index].neverUsed) {
                // 优先使用之前找到的已删除桶（如果有）
                if (firstDeletedIndex != -1) {
                    index = firstDeletedIndex;
                }
                table[index].key = key;
                table[index].value = value;
                table[index].inUse = true;
                table[index].neverUsed = false;
                size++;
                return "{inserted, index:" + to_string(index) + "}";
            }
            // 情况2：桶已使用过（neverUsed=false）
            else {
                // 找到相同key，更新值
                if (table[index].key == key) {
                    table[index].value = value;
                    return "{updated, index:" + to_string(index) + "}";
                }
                // 遇到已删除的桶，记录第一个位置（用于后续插入）
                if (!table[index].inUse && firstDeletedIndex == -1) {
                    firstDeletedIndex = index;
                }
            }

            index = (index + 1) % capacity;
        } while (index != start);

        // 循环结束：表满（无从未使用的桶，且无已删除的桶）
        return "{full, index:" + to_string(start) + "}";
    }

public:
    LinearProbingHashTable(int cap) : capacity(cap), size(0) {
        table.assign(capacity, {0, "", false, true});
    }

    // 外部插入接口
    string insert(int key, const string& value) {
        return insert(key, value, false);
    }

    // 查找接口：返回探查终止时的索引（而非初始哈希索引）
    string find(int key) {
        int index = hash(key);
        int start = index;
        int finalIndex = start; // 记录探查终止时的索引

        do {
            finalIndex = index; // 更新当前探查索引为最终索引
            // 桶从未使用过，无需继续探查（后续桶也不可能有目标key）
            if (table[index].neverUsed) {
                break;
            }
            // 找到目标key，返回结果
            if (table[index].key == key && table[index].inUse) {
                return "{found, index:" + to_string(index) + ", value:" + table[index].value + "}";
            }

            index = (index + 1) % capacity;
        } while (index != start);

        // 未找到：返回探查终止时的索引
        return "{not_found, index:" + to_string(finalIndex) + "}";
    }

    // 删除接口：仅标记 inUse=false，不改变 neverUsed
    string erase(int key) {
        int index = hash(key);
        int start = index;

        do {
            // 桶从未使用过，无需继续探查
            if (table[index].neverUsed) {
                break;
            }
            // 找到目标key且有效，标记为删除
            if (table[index].key == key && table[index].inUse) {
                table[index].inUse = false;
                size--;
                return "{removed, index:" + to_string(index) + "}";
            }

            index = (index + 1) % capacity;
        } while (index != start);

        // 未找到
        return "{not_found, index:" + to_string(start) + "}";
    }
};

int main() {
    LinearProbingHashTable ht(7);

    cout << ht.insert(1, "a") << endl;    // {inserted, index:1}
    cout << ht.insert(8, "b") << endl;    // {inserted, index:2}
    cout << ht.insert(15, "c") << endl;   // {inserted, index:3}
    cout << ht.find(22) << endl;          // {not_found, index:4}
    cout << ht.erase(8) << endl;          // {removed, index:2}
    cout << ht.find(15) << endl;          // {found, index:3, value:"c"}
    cout << ht.insert(22, "d") << endl;   // {inserted, index:2}
    cout << ht.insert(0, "t0") << endl;   // {inserted, index:0}
    cout << ht.insert(2, "t2") << endl;   // {inserted, index:4}
    cout << ht.insert(4, "t4") << endl;   // {inserted, index:5}
    cout << ht.erase(0) << endl;          // {removed, index:0}
    cout << ht.erase(2) << endl;          // {removed, index:4}
    cout << ht.erase(4) << endl;          // {removed, index:5}
    cout << ht.insert(5, "e") << endl;    // 触发重组织后插入 → {inserted, index:5}
    cout << ht.find(1) << endl;           // {found, index:1, value:"a"}
    cout << ht.find(22) << endl;          // {found, index:2, value:"d"}
    cout << ht.find(0) << endl;           // {not_found, index:0}

    return 0;
}