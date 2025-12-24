#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
using namespace std;

// 图的类封装
class Graph {
private:
    vector<vector<int>> adj;  // 邻接表（顶点从1开始）
    int vertexCount;          // 顶点总数

public:
    // 构造函数：初始化顶点数
    Graph(int n) : vertexCount(n) {
        adj.resize(n + 1);  // 索引0不用，1~n对应顶点
    }

    // 添加无向边（题目中边默认无向，若有向则只加u→v）
    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    // BFS实现findPath，返回路径数组（动态分配）
    int* findPath(int theSource, int theDestination) {
        // 特殊情况：起点=终点
        if (theSource == theDestination) {
            int* path = new int[2];  // path[0]=长度，path[1]=顶点
            path[0] = 0;
            path[1] = theSource;
            return path;
        }

        // 初始化辅助数组
        int* predecessor = new int[vertexCount + 1];  // 前驱顶点
        int* distance = new int[vertexCount + 1];     // 距离（边数）
        bool* visited = new bool[vertexCount + 1];    // 访问标记
        queue<int> q;

        for (int i = 1; i <= vertexCount; ++i) {
            predecessor[i] = -1;
            distance[i] = -1;
            visited[i] = false;
        }

        // 起点初始化
        distance[theSource] = 0;
        visited[theSource] = true;
        q.push(theSource);

        bool found = false;
        while (!q.empty() && !found) {
            int u = q.front();
            q.pop();

            // 遍历u的所有邻接顶点
            for (int v : adj[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    predecessor[v] = u;
                    distance[v] = distance[u] + 1;
                    q.push(v);

                    // 找到终点，终止BFS
                    if (v == theDestination) {
                        found = true;
                        break;
                    }
                }
            }
        }

        // 终点不可达，释放资源并返回NULL
        if (!found) {
            delete[] predecessor;
            delete[] distance;
            delete[] visited;
            return nullptr;
        }

        // 回溯路径：从终点到起点，再逆序
        int pathLen = distance[theDestination];
        int* path = new int[pathLen + 2];  // path[0] + 路径顶点（1~pathLen+1）
        path[0] = pathLen;
        int current = theDestination;
        for (int i = pathLen + 1; i >= 1; --i) {
            path[i] = current;
            current = predecessor[current];
        }

        // 释放辅助数组
        delete[] predecessor;
        delete[] distance;
        delete[] visited;
        return path;
    }

    // 释放图资源（可选）
    ~Graph() {
        adj.clear();
    }
};

// 打印路径结果
void printPathResult(int* path, const string& testCaseName) {
    cout << "【测试用例" << testCaseName << "】" << endl;
    if (path == nullptr) {
        cout << "结果：NULL" << endl;
    } else {
        int len = path[0];
        cout << "路径长度（边数）：" << len << endl;
        cout << "路径顶点：";
        for (int i = 1; i <= len + 1; ++i) {
            cout << path[i] << (i == len + 1 ? "" : " -> ");
        }
        cout << endl;
        delete[] path;  // 释放动态分配的路径数组
    }
    cout << "-------------------------" << endl;
}

// 主函数：测试用例
int main() {
    // ========== 测试用例1：顶点{1}，边集{} ==========
    Graph G1(1);
    int* path1 = G1.findPath(1, 1);
    printPathResult(path1, "1");

    // ========== 测试用例2：顶点{1,2,3}，边{(1,2)} ==========
    Graph G2(3);
    G2.addEdge(1, 2);
    int* path2 = G2.findPath(1, 3);
    printPathResult(path2, "2");

    // ========== 测试用例3：顶点{1,2,3,4,5}，边{(1,2),(2,3),(3,4),(4,5)} ==========
    Graph G3(5);
    G3.addEdge(1, 2);
    G3.addEdge(2, 3);
    G3.addEdge(3, 4);
    G3.addEdge(4, 5);
    int* path3 = G3.findPath(1, 5);
    printPathResult(path3, "3");

    // ========== 测试用例4：顶点{1,2,3,4,5}，边{(1,2),(2,5),(1,3),(3,4),(4,5)} ==========
    Graph G4(5);
    G4.addEdge(1, 2);
    G4.addEdge(2, 5);
    G4.addEdge(1, 3);
    G4.addEdge(3, 4);
    G4.addEdge(4, 5);
    int* path4 = G4.findPath(1, 5);
    printPathResult(path4, "4");

    // ========== 测试用例5：顶点{1,2,3,4}，边{(1,2),(2,4),(1,3),(3,4)} ==========
    Graph G5(4);
    G5.addEdge(1, 2);
    G5.addEdge(2, 4);
    G5.addEdge(1, 3);
    G5.addEdge(3, 4);
    int* path5 = G5.findPath(1, 4);
    printPathResult(path5, "5");

    return 0;
}