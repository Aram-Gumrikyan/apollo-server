//
// Created by aram on 5/15/23.
//

#ifndef APOLLO_SERVER_HUFFMAN_H
#define APOLLO_SERVER_HUFFMAN_H

#include <iostream>
#include <queue>
#include <unordered_map>
#include <bitset>

using std::unordered_map;
using std::string;
using std::vector;
using std::priority_queue;
using std::byte;
using std::bitset;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char ch, int freq, Node* left = nullptr, Node* right = nullptr)
            : ch(ch), freq(freq), left(left), right(right) {}
    ~Node() {
        delete left;
        delete right;
    }
};

struct NodeCmp {
    bool operator() (Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

Node* buildHuffmanTree(string input) {
    unordered_map<char, int> freqMap;
    for (char ch : input) {
        freqMap[ch]++;
    }

    priority_queue<Node*, vector<Node*>, NodeCmp> pq;
    for (auto pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();
        pq.push(new Node('\0', left->freq + right->freq, left, right));
    }

    return pq.top();
}

void generateHuffmanCodes(Node* root, unordered_map<char, string>& codeMap, string code = "") {
    if (root == nullptr) {
        return;
    }

    if (root->left == nullptr && root->right == nullptr) {
        codeMap[root->ch] = code;
    }

    generateHuffmanCodes(root->left, codeMap, code + "0");
    generateHuffmanCodes(root->right, codeMap, code + "1");
}

string compress(string input, unordered_map<char, string>& codeMap) {
    string output = "";
    for (char ch : input) {
        output += codeMap[ch];
    }

    while (output.size() % 8 != 0) {
        output += "0";
    }

    string compressed = "";
    for (int i = 0; i < output.size(); i += 8) {
        bitset<8> byte(output.substr(i, 8));
        compressed += (char)byte.to_ulong();
    }

    return compressed;
}

string decompress(string input, Node* root) {
    string output = "";
    Node* node = root;
    for (char ch : input) {
        bitset<8> byte(ch);
        for (int i = 7; i >= 0; i--) {
            if (node->left == nullptr && node->right == nullptr) {
                output += node->ch;
                node = root;
            }

            if (byte[i] == 0) {
                node = node->left;
            } else {
                node = node->right;
            }
        }
    }

    return output;
}

string encodeHuffmanTree(Node* root) {
    if (root == nullptr) {
        return "";
    } else if (root->left == nullptr && root->right == nullptr) {
        return "leaf " + string(1, root->ch) + " ";
    } else {
        return "internal " + std::to_string(root->freq) + " " + encodeHuffmanTree(root->left) + encodeHuffmanTree(root->right);
    }
}

Node* decodeHuffmanTree(string& s, int& index) {
    if (index >= s.length()) {
        return nullptr;
    }
    if (s.substr(index, 5) == "leaf ") {
        char ch = s[index + 5];
        index += 7;
        return new Node(ch, 0);
    } else {
        int freqStart = s.find_first_of("0123456789", index + 9);
        int freqEnd = s.find_first_not_of("0123456789", freqStart);
        int freq = stoi(s.substr(freqStart, freqEnd - freqStart));
        index += 9 + freqEnd - freqStart;
        Node* left = decodeHuffmanTree(s, index);
        Node* right = decodeHuffmanTree(s, index);
        return new Node('\0', freq, left, right);
    }
}

#endif //APOLLO_SERVER_HUFFMAN_H
