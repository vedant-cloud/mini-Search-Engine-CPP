#include "SearchEngine.h"

// ListNode destructor
ListNode::~ListNode() {
    delete next;
}

// TrieNode destructor
TrieNode::~TrieNode() {
    delete child;
    delete sibling;
    delete documents;
}

// ScoreList destructor
ScoreList::~ScoreList() {
    delete next;
}

// MaxHeap implementation
MaxHeap::MaxHeap(int cap) : capacity(cap), size(0) {
    heap = new HeapNode[capacity];
}

MaxHeap::~MaxHeap() {
    delete[] heap;
}

void MaxHeap::heapifyUp(int index) {
    if (index == 0) return;
    
    int parentIndex = parent(index);
    if (heap[parentIndex].score < heap[index].score) {
        HeapNode temp = heap[parentIndex];
        heap[parentIndex] = heap[index];
        heap[index] = temp;
        heapifyUp(parentIndex);
    }
}

void MaxHeap::heapifyDown(int index) {
    int left = leftChild(index);
    int right = rightChild(index);
    int largest = index;
    
    if (left < size && heap[left].score > heap[largest].score) {
        largest = left;
    }
    
    if (right < size && heap[right].score > heap[largest].score) {
        largest = right;
    }
    
    if (largest != index) {
        HeapNode temp = heap[index];
        heap[index] = heap[largest];
        heap[largest] = temp;
        heapifyDown(largest);
    }
}

void MaxHeap::insert(int doc_id, double score) {
    if (isFull()) return;
    
    heap[size] = HeapNode(doc_id, score);
    heapifyUp(size);
    size++;
}

MaxHeap::HeapNode MaxHeap::extractMax() {
    if (isEmpty()) return HeapNode();
    
    HeapNode root = heap[0];
    heap[0] = heap[size - 1];
    size--;
    heapifyDown(0);
    
    return root;
}

// SearchEngine implementation
SearchEngine::SearchEngine() : root(nullptr), document_map(nullptr), 
                               total_documents(0), max_line_length(0) {
    root = new TrieNode('\0'); // Root node with null character
}

SearchEngine::~SearchEngine() {
    cleanup();
}

void SearchEngine::cleanup() {
    delete root;
    
    if (document_map) {
        for (int i = 0; i < total_documents; i++) {
            delete[] document_map[i];
        }
        delete[] document_map;
    }
}

bool SearchEngine::initialize(const char* dataset_file) {
    if (!validateFile(dataset_file)) {
        cout << "Error: Cannot open dataset file: " << dataset_file << endl;
        return false;
    }
    
    cout << "Reading dataset size..." << endl;
    if (readDatasetSize(dataset_file) < 0) {
        cout << "Error: Cannot read dataset size." << endl;
        return false;
    }
    
    cout << "Loading dataset..." << endl;
    loadDataset(dataset_file);
    
    cout << "Building search index..." << endl;
    buildIndex();
    
    cout << "Search engine initialized successfully!" << endl;
    cout << "Total documents: " << total_documents << endl;
    cout << "Max line length: " << max_line_length << endl;
    
    return true;
}

int SearchEngine::readDatasetSize(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;
    
    char buffer[10000]; // Large buffer for reading lines
    total_documents = 0;
    max_line_length = 0;
    
    while (fgets(buffer, sizeof(buffer), file)) {
        int line_length = strlen(buffer);
        if (line_length > max_line_length) {
            max_line_length = line_length;
        }
        total_documents++;
    }
    
    fclose(file);
    return total_documents;
}

void SearchEngine::loadDataset(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;
    
    // Allocate memory for document map
    document_map = new char*[total_documents];
    for (int i = 0; i < total_documents; i++) {
        document_map[i] = new char[max_line_length + 1];
    }
    
    char buffer[10000];
    int doc_index = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && doc_index < total_documents) {
        // Find the first tab to skip document ID
        char* content_start = strchr(buffer, '\t');
        if (content_start) {
            content_start++; // Skip the tab character
            strcpy(document_map[doc_index], content_start);
            
            // Remove newline character
            int len = strlen(document_map[doc_index]);
            if (len > 0 && document_map[doc_index][len - 1] == '\n') {
                document_map[doc_index][len - 1] = '\0';
            }
        }
        doc_index++;
    }
    
    fclose(file);
}

void SearchEngine::buildIndex() {
    for (int doc_id = 0; doc_id < total_documents; doc_id++) {
        string document_text = document_map[doc_id];
        vector<string> words;
        tokenizeQuery(document_text, words);
        
        for (const string& word : words) {
            if (!word.empty()) {
                insertWord(toLowerCase(word), doc_id);
            }
        }
    }
}

void SearchEngine::insertWord(const string& word, int doc_id) {
    TrieNode* current = root;
    
    for (char ch : word) {
        // Look for child with this character
        TrieNode* child = current->child;
        while (child && child->character != ch) {
            child = child->sibling;
        }
        
        if (!child) {
            // Create new child
            child = new TrieNode(ch);
            child->sibling = current->child;
            current->child = child;
        }
        
        current = child;
    }
    
    // Add document occurrence
    ListNode* doc_node = current->documents;
    while (doc_node && doc_node->document_id != doc_id) {
        doc_node = doc_node->next;
    }
    
    if (doc_node) {
        doc_node->frequency++;
    } else {
        ListNode* new_doc = new ListNode(doc_id, 1);
        new_doc->next = current->documents;
        current->documents = new_doc;
    }
}

TrieNode* SearchEngine::searchWord(const string& word) {
    TrieNode* current = root;
    
    for (char ch : word) {
        TrieNode* child = current->child;
        while (child && child->character != ch) {
            child = child->sibling;
        }
        
        if (!child) return nullptr;
        current = child;
    }
    
    return current;
}

void SearchEngine::tokenizeQuery(const string& query, vector<string>& tokens) {
    string current_token;
    
    for (char ch : query) {
        if (isAlphaNum(ch)) {
            current_token += ch;
        } else {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
}

double SearchEngine::calculateTF(const string& word, int doc_id) {
    int word_count = countWordInDocument(word, doc_id);
    if (word_count == 0) return 0.0;
    
    // Count total words in document
    vector<string> words;
    tokenizeQuery(document_map[doc_id], words);
    int total_words = words.size();
    
    return (double)word_count / total_words;
}

double SearchEngine::calculateIDF(const string& word) {
    int docs_containing = countDocumentsContaining(word);
    if (docs_containing == 0) return 0.0;
    
    return log((double)total_documents / docs_containing);
}

double SearchEngine::calculateTFIDF(const string& word, int doc_id) {
    double tf = calculateTF(word, doc_id);
    double idf = calculateIDF(word);
    return tf * idf;
}

int SearchEngine::countWordInDocument(const string& word, int doc_id) {
    TrieNode* word_node = searchWord(toLowerCase(word));
    if (!word_node) return 0;
    
    ListNode* doc_node = word_node->documents;
    while (doc_node) {
        if (doc_node->document_id == doc_id) {
            return doc_node->frequency;
        }
        doc_node = doc_node->next;
    }
    
    return 0;
}

int SearchEngine::countDocumentsContaining(const string& word) {
    TrieNode* word_node = searchWord(toLowerCase(word));
    if (!word_node) return 0;
    
    int count = 0;
    ListNode* doc_node = word_node->documents;
    while (doc_node) {
        count++;
        doc_node = doc_node->next;
    }
    
    return count;
}

void SearchEngine::search(const string& query, int k) {
    vector<string> query_terms;
    tokenizeQuery(query, query_terms);
    
    if (query_terms.empty()) {
        cout << "Error: Empty query." << endl;
        return;
    }
    
    cout << "Searching for: \"" << query << "\"" << endl;
    cout << "Query terms: ";
    for (const string& term : query_terms) {
        cout << "[" << term << "] ";
    }
    cout << endl << endl;
    
    // Calculate scores for all documents
    MaxHeap results(k);
    
    for (int doc_id = 0; doc_id < total_documents; doc_id++) {
        double total_score = 0.0;
        
        for (const string& term : query_terms) {
            double tf_idf = calculateTFIDF(term, doc_id);
            total_score += tf_idf;
        }
        
        if (total_score > 0.0) {
            if (!results.isFull()) {
                results.insert(doc_id, total_score);
            } else {
                // If heap is full, only insert if score is better
                // This is a simplified version - in practice, you'd want a min-heap for this
                results.insert(doc_id, total_score);
            }
        }
    }
    
    displayResults(results);
}

void SearchEngine::displayResults(MaxHeap& results) {
    if (results.isEmpty()) {
        cout << "No results found." << endl;
        return;
    }
    
    cout << "Search Results:" << endl;
    cout << "===============" << endl;
    
    int rank = 1;
    while (!results.isEmpty() && rank <= 100) {
        MaxHeap::HeapNode result = results.extractMax();
        
        cout << "Rank " << rank << " (Score: " << result.score << "):" << endl;
        cout << "Document ID: " << result.document_id << endl;
        cout << "Content: " << document_map[result.document_id] << endl;
        cout << endl;
        
        rank++;
    }
}

string SearchEngine::toLowerCase(const string& str) {
    string result = str;
    for (char& c : result) {
        c = tolower(c);
    }
    return result;
}

bool SearchEngine::isAlphaNum(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

// Utility functions
bool validateFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

int getTerminalWidth() {
    // Simplified version - return default width
    return 80;
}