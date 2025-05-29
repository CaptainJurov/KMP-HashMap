#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Node {
    std::string key;
    std::vector<int> positions;
    Node* next;
    Node(std::string key, int firstPos) {
        this->key = key;
        this->positions.push_back(firstPos);
        this->next = nullptr;
    }
};

class LinkedList {
public:
    Node* head;
    LinkedList() { head = nullptr; }

    ~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    std::vector<int> get(std::string key) {
        Node* current = head;
        while (current != nullptr) {
            if (current->key == key) return current->positions;
            current = current->next;
        }
        return {};
    }

    void insert(std::string key, int pos) {
        Node* current = head;
        while (current != nullptr) {
            if (current->key == key) {
                current->positions.push_back(pos);
                return;
            }
            current = current->next;
        }
        Node* newNode = new Node(key, pos);
        newNode->next = head;
        head = newNode;
    }

    void remove(std::string key) {
        if (head == nullptr) return;
        if (head->key == key) {
            Node* temp = head;
            head = head->next;
            delete temp;
            return;
        }
        Node* current = head;
        while (current->next != nullptr && current->next->key != key) {
            current = current->next;
        }
        if (current->next != nullptr) {
            Node* temp = current->next;
            current->next = current->next->next;
            delete temp;
        }
    }
};

class HashTable {
protected:
    int size;
    LinkedList* table;

    int hashFunc(std::string key) {
        int hash = 0;
        for (char c : key) hash += (int)c;
        return hash % size;
    }

public:
    HashTable(int size) {
        this->size = size;
        table = new LinkedList[size];
    }

    ~HashTable() {
        delete[] table;
    }

    void insert(std::string key, int pos) {
        table[hashFunc(key)].insert(key, pos);
    }

    void remove(std::string key) {
        table[hashFunc(key)].remove(key);
    }

    std::vector<int> get(std::string key) {
        return table[hashFunc(key)].get(key);
    }
};

std::vector<int> computeLPS(std::string pattern) {
    int n = pattern.size();
    std::vector<int> lps(n, 0);
    int len = 0;
    for (int i = 1; i < n; ) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) len = lps[len - 1];
            else lps[i] = 0, i++;
        }
    }
    return lps;
}

std::vector<int> KMP(std::string text, std::string word) {
    std::vector<int> positions;
    if (word.empty()) return positions;

    std::vector<int> lps = computeLPS(word);
    int i = 0, j = 0;
    while (i < text.size()) {
        if (text[i] == word[j]) {
            i++, j++;
            if (j == word.size()) {
                positions.push_back(i - j);
                j = lps[j - 1];
            }
        } else {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
    return positions;
}
class FileProcessor : public HashTable {
public:
    FileProcessor(int size) : HashTable(size) {}
    void loadWords(std::string filename) {
        std::ifstream file(filename);
        std::string word;
        while (file >> word) {
            insert(word, -1);
        }
        file.close();
    }

    void processText(std::string textFilename) {
        std::ifstream file(textFilename);
        std::string text((std::istreambuf_iterator<char>(file)), 
                    std::istreambuf_iterator<char>());
        file.close();

        for (int i = 0; i < size; ++i) {
            Node* current = table[i].head;
            while (current != nullptr) {
                std::string word = current->key;
                std::vector<int> positions = KMP(text, word);
                current->positions = positions;
                current = current->next;
            }
        }
    }

    void searchWord(std::string word) {
        std::vector<int> positions = get(word);
        if (positions.empty()) {
            std::cout << -1 << std::endl;
        } else {
            for (int pos : positions) std::cout << pos << " ";
            std::cout << std::endl;
        }
    }
};
int main() {
    FileProcessor fp(100);
    fp.loadWords("words.txt");
    fp.processText("text.txt");
    std::string input;
    while (true) {
        std::cout << "Чё за слово ('exit'): ";
        std::cin >> input;
        if (input == "exit") break;
        fp.searchWord(input);
    }

    return 0;
}
