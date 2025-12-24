#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

// 无向图类
class Graph {
private:
    int n; // 顶点数（顶点编号从1开始）
    vector<vector<bool>> adj; // 邻接矩阵：adj[i][j] = true 表示存在边(i,j)
    unordered_map<int, int> component; // 顶点 -> 连通分量编号

    // 深度优先搜索标记连通分量（邻接矩阵版，O(n?)）
    void dfs(int u, int comp_id) {
        component[u] = comp_id;
        for (int v = 1; v <= n; ++v) {
            if (adj[u][v] && component.find(v) == component.end()) {
                dfs(v, comp_id);
            }
        }
    }

    // 标记所有连通分量
    void markComponents() {
        component.clear();
        int comp_id = 0;
        for (int u = 1; u <= n; ++u) {
            if (component.find(u) == component.end()) {
                dfs(u, comp_id++);
            }
        }
    }

    // 统计每个连通分量的顶点数
    unordered_map<int, int> countComponentSize() {
        unordered_map<int, int> size_map;
        for (auto& p : component) {
            size_map[p.second]++;
        }
        return size_map;
    }

public:
    // 构造函数：初始化顶点数（顶点编号1~n）
    Graph(int num_vertices) : n(num_vertices) {
        adj.resize(n + 1, vector<bool>(n + 1, false)); // 1-based索引
    }

    // 添加无向边
    void addEdge(int u, int v) {
        if (u >= 1 && u <= n && v >= 1 && v <= n) {
            adj[u][v] = true;
            adj[v][u] = true;
        }
    }

    // 计算传递闭包（核心方法）
    vector<vector<int>> undirectedTC() {
        // 初始化传递闭包矩阵（全0）
        vector<vector<int>> tc(n + 1, vector<int>(n + 1, 0));

        // 步骤1：标记连通分量
        markComponents();

        // 步骤2：统计每个连通分量的大小
        auto comp_size = countComponentSize();

        // 步骤3：遍历所有顶点对，计算tc[i][j]
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= n; ++j) {
                if (i == j) {
                    tc[i][j] = 0; // 对角线强制为0
                    continue;
                }

                // 不在同一连通分量：tc=0
                if (component[i] != component[j]) {
                    tc[i][j] = 0;
                    continue;
                }

                int c = component[i];
                int size = comp_size[c];

                // 情况1：连通分量大小 > 2 → 存在长度>1的路径，tc=1
                if (size > 2) {
                    tc[i][j] = 1;
                }
                // 情况2：连通分量大小 = 2 → 只有一条边，无长度>1的路径，tc=0
                else if (size == 2) {
                    tc[i][j] = 0;
                }
                // 情况3：连通分量大小 = 1 → 无路径，tc=0
                else {
                    tc[i][j] = 0;
                }

                // 特殊修正：若连通分量大小=2但存在直接边+间接边（如三角形的子图）
                // 补充判断：i和j是否有长度>1的路径（即除了直接边外，是否有其他路径）
                // 例如用例5：连通分量大小=3，且任意两点间既有直接边又有间接边，tc=1
                // 验证：i和j之间是否存在除直接边外的其他路径
                if (size == 2) {
                    // 大小为2的连通分量只有两个顶点，无其他路径，无需修正
                } else {
                    // 确保无向图的对称性
                    tc[j][i] = tc[i][j];
                }
            }
        }

        return tc;
    }

    // 打印传递闭包矩阵（仅打印1~n行/列）
    static void printTC(const vector<vector<int>>& tc) {
        int n = tc.size() - 1;
        for (int i = 1; i <= n; ++i) {
            cout << "i=" << i << ": (";
            for (int j = 1; j <= n; ++j) {
                cout << tc[i][j];
                if (j < n) cout << ",";
            }
            cout << ")" << endl;
        }
        cout << "-------------------------" << endl;
    }
};

// 测试用例1：空图
void testCase1() {
    cout << "测试用例1：空图 V={1,2,3}, E=?" << endl;
    Graph g(3);
    auto tc = g.undirectedTC();
    Graph::printTC(tc);
}

// 测试用例2：两个顶点一条边
void testCase2() {
    cout << "测试用例2：V={1,2}, E={(1,2)}" << endl;
    Graph g(2);
    g.addEdge(1, 2);
    auto tc = g.undirectedTC();
    Graph::printTC(tc);
}

// 测试用例3：三个顶点链式边
void testCase3() {
    cout << "测试用例3：V={1,2,3}, E={(1,2),(2,3)}" << endl;
    Graph g(3);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    auto tc = g.undirectedTC();
    Graph::printTC(tc);
}

// 测试用例4：四个顶点，1连接2、3、4
void testCase4() {
    cout << "测试用例4：V={1,2,3,4}, E={(1,2),(1,3),(1,4)}" << endl;
    Graph g(4);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    auto tc = g.undirectedTC();
    Graph::printTC(tc);
}

// 测试用例5：三个顶点构成三角形
void testCase5() {
    cout << "测试用例5：V={1,2,3}, E={(1,2),(2,3),(1,3)}" << endl;
    Graph g(3);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(1, 3);
    auto tc = g.undirectedTC();
    Graph::printTC(tc);
}

int main() {
    testCase1();
    testCase2();
    testCase3();
    testCase4();
    testCase5();
    return 0;
}