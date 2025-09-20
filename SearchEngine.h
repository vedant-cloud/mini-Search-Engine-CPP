#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cmath>

using namespace std;

// Forward declarations
class TrieNode;
class ListNode;
class ScoreList;
class MaxHeap;

// ListNode class for storing document occurrences
class ListNode {
public:
    int document_id;
    int frequency;
    ListNode* next;
    
    ListNode(int doc_id, int freq) : document_id(doc_id), frequency(freq), next(nullptr) {}
    ~ListNode();
};

// TrieNode class for the Trie data structure
class TrieNode {
public:
    char character;
    TrieNode* child;
    TrieNode* sibling;
    ListNode* documents;
    
    TrieNode(char ch) : character(ch), child(nullptr), sibling(nullptr), documents(nullptr) {}
    ~TrieNode();
};

// ScoreList class for storing document scores
class ScoreList {
public:
    int document_id;
    double score;
    ScoreList* next;
    
    ScoreList(int doc_id, double sc) : document_id(doc_id), score(sc), next(nullptr) {}
    ~ScoreList();
};

// MaxHeap class for priority queue of search results
class MaxHeap {
public:
    struct HeapNode {
        int document_id;
        double score;
        
        HeapNode() : document_id(-1), score(0.0) {}
        HeapNode(int doc_id, double sc) : document_id(doc_id), score(sc) {}
    };
    
    HeapNode* heap;
    int capacity;
    int size;
    
    void heapifyUp(int index);
    void heapifyDown(int index);
    int parent(int index) { return (index - 1) / 2; }
    int leftChild(int index) { return 2 * index + 1; }
    int rightChild(int index) { return 2 * index + 2; }
    
public:
    MaxHeap(int cap);
    ~MaxHeap();
    
    void insert(int doc_id, double score);
    HeapNode extractMax();
    bool isEmpty() { return size == 0; }
    bool isFull() { return size == capacity; }
    int getSize() { return size; }
};

// Main SearchEngine class
class SearchEngine {
private:
    TrieNode* root;
    char** document_map;
    int total_documents;
    int max_line_length;
    
    // Helper functions
    void insertWord(const string& word, int doc_id);
    TrieNode* searchWord(const string& word);
    double calculateTF(const string& word, int doc_id);
    double calculateIDF(const string& word);
    double calculateTFIDF(const string& word, int doc_id);
    int countWordInDocument(const string& word, int doc_id);
    int countDocumentsContaining(const string& word);
    void tokenizeQuery(const string& query, vector<string>& tokens);
    bool isAlphaNum(char c);
    string toLowerCase(const string& str);
    
    // File processing functions
    int readDatasetSize(const char* filename);
    void loadDataset(const char* filename);
    void buildIndex();
    
public:
    SearchEngine();
    ~SearchEngine();
    
    bool initialize(const char* dataset_file);
    void search(const string& query, int k);
    void displayResults(MaxHeap& results);
    void cleanup();
};

// Utility functions
int getTerminalWidth();
void printHighlighted(const string& text, const vector<string>& query_terms);
bool validateFile(const char* filename);

#endif // SEARCH_ENGINE_H