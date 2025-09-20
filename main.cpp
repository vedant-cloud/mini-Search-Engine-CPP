#include "SearchEngine.h"

void printUsage(const char* program_name) {
    cout << "Usage: " << program_name << " -d <dataset_file> -k <num_results>" << endl;
    cout << "Options:" << endl;
    cout << "  -d <file>    Specify the dataset file path" << endl;
    cout << "  -k <number>  Number of top results to display (default: 5)" << endl;
    cout << endl;
    cout << "Dataset Format:" << endl;
    cout << "  Each line should contain: <doc_id><TAB><document_text>" << endl;
    cout << "  Example: 0<TAB>This is the first document about programming" << endl;
}

bool parseArguments(int argc, char* argv[], string& dataset_file, int& k) {
    if (argc != 5) {
        return false;
    }
    
    dataset_file = "";
    k = 5; // default value
    
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) return false;
        
        if (strcmp(argv[i], "-d") == 0) {
            dataset_file = argv[i + 1];
        } else if (strcmp(argv[i], "-k") == 0) {
            k = atoi(argv[i + 1]);
            if (k <= 0) {
                cout << "Error: Number of results must be positive." << endl;
                return false;
            }
        } else {
            return false;
        }
    }
    
    if (dataset_file.empty()) {
        cout << "Error: Dataset file not specified." << endl;
        return false;
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    cout << "==================================" << endl;
    cout << "    C++ Search Engine v1.0        " << endl;
    cout << "==================================" << endl;
    cout << endl;
    
    string dataset_file;
    int k;
    
    if (!parseArguments(argc, argv, dataset_file, k)) {
        printUsage(argv[0]);
        return -1;
    }
    
    cout << "Initializing search engine..." << endl;
    cout << "Dataset file: " << dataset_file << endl;
    cout << "Max results: " << k << endl;
    cout << endl;
    
    SearchEngine engine;
    
    if (!engine.initialize(dataset_file.c_str())) {
        cout << "Failed to initialize search engine." << endl;
        return -1;
    }
    
    cout << endl;
    cout << "Search engine ready! Enter your queries (type 'quit' to exit):" << endl;
    cout << "=================================================================" << endl;
    
    string query;
    while (true) {
        cout << endl << "Search> ";
        getline(cin, query);
        
        if (query == "quit" || query == "exit" || query == "q") {
            cout << "Goodbye!" << endl;
            break;
        }
        
        if (query.empty()) {
            continue;
        }
        
        cout << endl;
        engine.search(query, k);
        cout << "=================================================================" << endl;
    }
    
    return 0;
}