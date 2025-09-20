# C++ Search Engine Makefile
# Compiler and flags
CC = g++
CPPFLAGS = -Wall -Wextra -std=c++11 -O2

# Executable name
TARGET = search_engine

# Source files
SOURCES = main.cpp SearchEngine.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Header files
HEADERS = SearchEngine.h

# Default target
default: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CPPFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files to object files
%.o: %.cpp $(HEADERS)
	$(CC) $(CPPFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install (copy to /usr/local/bin - requires sudo)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall (remove from /usr/local/bin - requires sudo)
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Create sample dataset
sample_data:
	@echo "Creating sample dataset..."
	@echo -e "0\tWelcome to the world of programming and software development" > sample_dataset.txt
	@echo -e "1\tC++ is a powerful programming language for system development" >> sample_dataset.txt
	@echo -e "2\tSearch engines use data structures like trie and heap for efficiency" >> sample_dataset.txt
	@echo -e "3\tInformation retrieval systems rank documents by relevance using algorithms" >> sample_dataset.txt
	@echo -e "4\tTF-IDF term frequency inverse document frequency is used for scoring" >> sample_dataset.txt
	@echo -e "5\tProgramming languages include Python Java JavaScript and C++" >> sample_dataset.txt
	@echo -e "6\tData structures and algorithms are fundamental to computer science" >> sample_dataset.txt
	@echo -e "7\tSoftware engineering requires careful design and implementation" >> sample_dataset.txt
	@echo -e "8\tMachine learning and artificial intelligence are growing fields" >> sample_dataset.txt
	@echo -e "9\tWeb development involves frontend and backend programming technologies" >> sample_dataset.txt
	@echo "Sample dataset created: sample_dataset.txt"

# Test the search engine with sample data
test: $(TARGET) sample_data
	@echo "Testing search engine with sample data..."
	@echo "Running: ./$(TARGET) -d sample_dataset.txt -k 5"
	@echo "Note: After initialization, try searching for terms like 'programming', 'data structures', or 'C++'"

# Run with Valgrind for memory leak detection
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) -d sample_dataset.txt -k 5

# Debug build (with debug symbols)
debug: CPPFLAGS += -g -DDEBUG
debug: clean $(TARGET)

# Release build (optimized)
release: CPPFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  default      - Build the search engine (same as 'make')"
	@echo "  clean        - Remove object files and executable"
	@echo "  install      - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall    - Remove from /usr/local/bin (requires sudo)"
	@echo "  sample_data  - Create sample dataset file"
	@echo "  test         - Build and prepare for testing with sample data"
	@echo "  memcheck     - Run with Valgrind to check for memory leaks"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage after building:"
	@echo "  ./$(TARGET) -d <dataset_file> -k <num_results>"
	@echo ""
	@echo "Example:"
	@echo "  make sample_data"
	@echo "  make"
	@echo "  ./$(TARGET) -d sample_dataset.txt -k 5"

# Declare phony targets
.PHONY: default clean install uninstall sample_data test memcheck debug release help