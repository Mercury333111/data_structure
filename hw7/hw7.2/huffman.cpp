#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <string>
#include <utility>
#include <cassert>

using namespace std;

// 哈夫曼树节点结构
struct HuffmanNode {
    uint8_t byte;
    uint32_t freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(uint8_t b, uint32_t f) : byte(b), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(uint32_t f, HuffmanNode* l, HuffmanNode* r) : byte(0), freq(f), left(l), right(r) {}
    ~HuffmanNode() {
        delete left;
        delete right;
    }
};

struct NodeCompare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) const {
        return a->freq > b->freq;
    }
};

HuffmanNode* buildHuffmanTree(const vector<uint32_t>& freq) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, NodeCompare> pq;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(new HuffmanNode(static_cast<uint8_t>(i), freq[i]));
        }
    }
    if (pq.empty()) return nullptr;
    if (pq.size() == 1) return pq.top();
    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        pq.push(new HuffmanNode(left->freq + right->freq, left, right));
    }
    return pq.top();
}

void generateHuffmanCodes(HuffmanNode* root, string code, unordered_map<uint8_t, string>& code_map) {
    if (!root) return;
    if (!root->left && !root->right) {
        code_map[root->byte] = code.empty() ? "0" : code;
        return;
    }
    generateHuffmanCodes(root->left, code + "0", code_map);
    generateHuffmanCodes(root->right, code + "1", code_map);
}

pair<string, pair<uint64_t, uint64_t>> compress(const string& src, const string& dst) {
    ifstream in_file(src, ios::binary | ios::ate);
    if (!in_file.is_open()) return {"error", {0, 0}};

    uint64_t in_bytes = in_file.tellg();
    vector<uint8_t> data;
    if (in_bytes > 0) {
        in_file.seekg(0, ios::beg);
        data.resize(in_bytes);
        in_file.read(reinterpret_cast<char*>(data.data()), in_bytes);
        if (!in_file) { in_file.close(); return {"error", {in_bytes, 0}}; }
    }
    in_file.close();

    if (in_bytes == 0) {
        ofstream out_file(dst, ios::binary);
        out_file.close();
        return {"ok", {0, 0}};
    }

    vector<uint32_t> freq(256, 0);
    for (uint8_t byte : data) freq[byte]++;

    HuffmanNode* root = buildHuffmanTree(freq);
    unordered_map<uint8_t, string> code_map;
    generateHuffmanCodes(root, "", code_map);
    delete root;

    uint64_t total_bits = 0;
    for (uint8_t byte : data) total_bits += code_map[byte].size();
    uint64_t code_bytes = (total_bits + 7) / 8;

    vector<pair<uint8_t, uint16_t>> non_zero_freq;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) non_zero_freq.emplace_back(static_cast<uint8_t>(i), static_cast<uint16_t>(freq[i]));
    }
    uint8_t non_zero_count = static_cast<uint8_t>(non_zero_freq.size());

    uint64_t header_bytes = 1 + non_zero_count * (1 + 2);
    uint64_t total_compress_size = header_bytes + code_bytes + 1;

    if (total_compress_size >= in_bytes) {
        ofstream out_file(dst, ios::binary);
        if (!out_file.is_open()) return {"error", {in_bytes, 0}};
        out_file.put(0);
        out_file.write(reinterpret_cast<const char*>(data.data()), in_bytes);
        out_file.close();
        return {"ok", {in_bytes, in_bytes + 1}};
    }

    ofstream out_file(dst, ios::binary);
    if (!out_file.is_open()) return {"error", {in_bytes, 0}};

    out_file.put(1);
    out_file.write(reinterpret_cast<const char*>(&non_zero_count), 1);
    for (const auto& p : non_zero_freq) {
        out_file.write(reinterpret_cast<const char*>(&p.first), 1);
        out_file.write(reinterpret_cast<const char*>(&p.second), 2);
    }

    uint8_t current_byte = 0;
    int bit_count = 0;
    for (uint8_t byte : data) {
        const string& code = code_map[byte];
        for (char c : code) {
            current_byte = (current_byte << 1) | (c == '1' ? 1 : 0);
            bit_count++;
            if (bit_count == 8) {
                out_file.write(reinterpret_cast<const char*>(&current_byte), 1);
                current_byte = 0;
                bit_count = 0;
            }
        }
    }

    uint8_t padding_bits = 0;
    if (bit_count > 0) {
        padding_bits = 8 - bit_count;
        current_byte <<= padding_bits;
        out_file.write(reinterpret_cast<const char*>(&current_byte), 1);
    }
    out_file.put(padding_bits);
    out_file.close();

    return {"ok", {in_bytes, total_compress_size}};
}

// 解压函数核心修改：从频率表计算原文件大小，限制输出字节数
pair<string, uint64_t> decompress(const string& src, const string& dst) {
    ifstream in_file(src, ios::binary | ios::ate);
    if (!in_file.is_open()) return {"error", 0};

    uint64_t huf_file_size = in_file.tellg();
    if (huf_file_size == 0) {
        ofstream out_file(dst, ios::binary);
        out_file.close();
        in_file.close();
        return {"ok", 0};
    }

    in_file.seekg(0, ios::beg);
    uint8_t flag;
    if (!in_file.read(reinterpret_cast<char*>(&flag), 1)) {
        in_file.close();
        return {"error", 0};
    }

    if (flag == 0) {
        uint64_t original_size = huf_file_size - 1;
        vector<uint8_t> data(original_size);
        if (!in_file.read(reinterpret_cast<char*>(data.data()), original_size)) {
            in_file.close();
            return {"error", 0};
        }
        ofstream out_file(dst, ios::binary);
        if (!out_file.is_open()) {
            in_file.close();
            return {"error", 0};
        }
        out_file.write(reinterpret_cast<const char*>(data.data()), original_size);
        out_file.close();
        in_file.close();
        return {"ok", original_size};
    }

    uint8_t non_zero_count;
    if (!in_file.read(reinterpret_cast<char*>(&non_zero_count), 1)) {
        in_file.close();
        return {"error", 0};
    }

    vector<uint32_t> freq(256, 0);
    uint64_t original_size = 0; // 新增：从频率表计算原文件总字节数
    for (int i = 0; i < non_zero_count; ++i) {
        uint8_t byte;
        uint16_t f;
        if (!in_file.read(reinterpret_cast<char*>(&byte), 1) ||
            !in_file.read(reinterpret_cast<char*>(&f), 2)) {
            in_file.close();
            return {"error", 0};
        }
        freq[byte] = f;
        original_size += f; // 累加频率得到原文件大小
    }

    HuffmanNode* root = buildHuffmanTree(freq);
    if (!root) {
        in_file.close();
        ofstream out_file(dst, ios::binary);
        out_file.close();
        return {"ok", 0};
    }

    in_file.seekg(huf_file_size - 1, ios::beg);
    uint8_t padding_bits;
    if (!in_file.read(reinterpret_cast<char*>(&padding_bits), 1)) {
        delete root;
        in_file.close();
        return {"error", 0};
    }

    uint64_t code_data_start = 1 + 1 + non_zero_count * 3;
    in_file.seekg(code_data_start, ios::beg);
    if (in_file.tellg() != code_data_start) {
        delete root;
        in_file.close();
        return {"error", 0};
    }

    ofstream out_file(dst, ios::binary);
    if (!out_file.is_open()) {
        delete root;
        in_file.close();
        return {"error", 0};
    }

    HuffmanNode* current_node = root;
    uint64_t out_bytes = 0;
    uint8_t byte;
    const uint64_t code_data_end = huf_file_size - 1;

    // 解码时增加“输出字节数达到原文件大小则终止”的判断
    while (in_file.tellg() < code_data_end && out_bytes < original_size) {
        if (!in_file.read(reinterpret_cast<char*>(&byte), 1)) break;

        for (int i = 7; i >= 0 && out_bytes < original_size; --i) {
            if (!current_node->left && !current_node->right) {
                out_file.write(reinterpret_cast<const char*>(&current_node->byte), 1);
                out_bytes++;
                continue;
            }

            bool bit = (byte >> i) & 1;
            current_node = bit ? current_node->right : current_node->left;

            if (!current_node->left && !current_node->right) {
                out_file.write(reinterpret_cast<const char*>(&current_node->byte), 1);
                out_bytes++;
                current_node = root;
            }
        }
    }

    // 处理最后一个字节时同样限制输出字节数
    if (in_file.tellg() == code_data_end && padding_bits < 8 && out_bytes < original_size) {
        if (in_file.read(reinterpret_cast<char*>(&byte), 1)) {
            for (int i = 7; i >= padding_bits && out_bytes < original_size; --i) {
                if (!current_node->left && !current_node->right) {
                    out_file.write(reinterpret_cast<const char*>(&current_node->byte), 1);
                    out_bytes++;
                    continue;
                }

                bool bit = (byte >> i) & 1;
                current_node = bit ? current_node->right : current_node->left;

                if (!current_node->left && !current_node->right) {
                    out_file.write(reinterpret_cast<const char*>(&current_node->byte), 1);
                    out_bytes++;
                    current_node = root;
                }
            }
        }
    }

    delete root;
    in_file.close();
    out_file.close();

    return {"ok", out_bytes};
}

pair<string, bool> compare(const string& file1, const string& file2) {
    ifstream f1(file1, ios::binary | ios::ate);
    ifstream f2(file2, ios::binary | ios::ate);
    if (!f1.is_open() || !f2.is_open()) return {"error", false};
    if (f1.tellg() != f2.tellg()) { f1.close(); f2.close(); return {"ok", false}; }
    f1.seekg(0, ios::beg); f2.seekg(0, ios::beg);
    uint8_t b1, b2;
    while (f1.read(reinterpret_cast<char*>(&b1), 1) && f2.read(reinterpret_cast<char*>(&b2), 1)) {
        if (b1 != b2) { f1.close(); f2.close(); return {"ok", false}; }
    }
    f1.close(); f2.close();
    return {"ok", true};
}

void runTests() {
    cout << "=== Testing mixed.txt ===" << endl;
    auto compress_res1 = compress("mixed.txt", "mixed.huf");
    cout << "compress result: " << compress_res1.first 
         << ", in_bytes: " << compress_res1.second.first 
         << ", out_bytes: " << compress_res1.second.second << endl;
    assert(compress_res1.first == "ok");
    assert(compress_res1.second.second < compress_res1.second.first);

    auto decompress_res1 = decompress("mixed.huf", "mixed.out");
    cout << "decompress result: " << decompress_res1.first 
         << ", out_bytes: " << decompress_res1.second << endl;
    assert(decompress_res1.first == "ok");
    assert(decompress_res1.second == compress_res1.second.first);

    auto compare_res1 = compare("mixed.txt", "mixed.out");
    cout << "compare result: " << compare_res1.first 
         << ", equal: " << (compare_res1.second ? "true" : "false") << endl;
    assert(compare_res1.first == "ok" && compare_res1.second);

    cout << "\n=== Testing one_char.txt ===" << endl;
    auto compress_res2 = compress("one_char.txt", "one_char.huf");
    cout << "compress result: " << compress_res2.first 
         << ", in_bytes: " << compress_res2.second.first 
         << ", out_bytes: " << compress_res2.second.second << endl;
    assert(compress_res2.first == "ok");
    assert(compress_res2.second.first == 238);
    assert(compress_res2.second.second < 238);

    auto decompress_res2 = decompress("one_char.huf", "one_char.out");
    cout << "decompress result: " << decompress_res2.first 
         << ", out_bytes: " << decompress_res2.second << endl;
    assert(decompress_res2.first == "ok");
    assert(decompress_res2.second == 238);

    auto compare_res2 = compare("one_char.txt", "one_char.out");
    cout << "compare result: " << compare_res2.first 
         << ", equal: " << (compare_res2.second ? "true" : "false") << endl;
    assert(compare_res2.first == "ok" && compare_res2.second);

    cout << "\n=== Testing empty.txt ===" << endl;
    auto compress_res3 = compress("empty.txt", "empty.huf");
    cout << "compress result: " << compress_res3.first 
         << ", in_bytes: " << compress_res3.second.first 
         << ", out_bytes: " << compress_res3.second.second << endl;
    assert(compress_res3.first == "ok");
    assert(compress_res3.second.first == 0 && compress_res3.second.second == 0);

    auto decompress_res3 = decompress("empty.huf", "empty.out");
    cout << "decompress result: " << decompress_res3.first 
         << ", out_bytes: " << decompress_res3.second << endl;
    assert(decompress_res3.first == "ok" && decompress_res3.second == 0);

    auto compare_res3 = compare("empty.txt", "empty.out");
    cout << "compare result: " << compare_res3.first 
         << ", equal: " << (compare_res3.second ? "true" : "false") << endl;
    assert(compare_res3.first == "ok" && compare_res3.second);

    cout << "\nAll tests passed!" << endl;
}

int main() {
    runTests();
    return 0;
}