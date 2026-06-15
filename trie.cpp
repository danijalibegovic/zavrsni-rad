#include <iostream>
#include <string>
#include <vector>

// Konstanta koja definise velicinu azbuke (prosireni ASCII)
const int ALPHABET_SIZE = 256;

// Struktura koja predstavlja jedan cvor u prefiksnom stablu
struct TrieNode {
    TrieNode* children[ALPHABET_SIZE];
    bool is_terminal;

    TrieNode() {
        is_terminal = false;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            children[i] = nullptr;
        }
    }
};

class Trie {
private:
    TrieNode* root;

    // Pomocna rekurzivna funkcija za oslobadjanje memorije
    void destroyTrie(TrieNode* node) {
        if (node == nullptr) return;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i] != nullptr) {
                destroyTrie(node->children[i]);
            }
        }
        delete node;
    }

    // Pomocna metoda koja provjerava da li cvor ima djece
    bool isEmpty(TrieNode* node) const {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i] != nullptr) {
                return false;
            }
        }
        return true;
    }

    // Rekurzivna funkcija za stvarno brisanje cvorova (pruning)
    bool removeHelper(TrieNode* current, const std::string& word, int depth) {
        if (current == nullptr) {
            return false;
        }

        if (depth == word.length()) {
            if (!current->is_terminal) {
                return false;
            }
            current->is_terminal = false;
            return isEmpty(current);
        }

        unsigned char index = static_cast<unsigned char>(word[depth]);
        
        if (removeHelper(current->children[index], word, depth + 1)) {
            delete current->children[index];
            current->children[index] = nullptr;

            return !current->is_terminal && isEmpty(current);
        }

        return false;
    }

public:
    // Konstruktor
    Trie() {
        root = new TrieNode();
    }

    // Destruktor
    ~Trie() {
        destroyTrie(root);
    }

    // Metoda za umetanje nove rijeci u stablo
    void insert(const std::string& word) {
        TrieNode* current = root;
        for (char c : word) {
            unsigned char index = static_cast<unsigned char>(c);
            
            if (current->children[index] == nullptr) {
                current->children[index] = new TrieNode();
            }
            current = current->children[index];
        }
        current->is_terminal = true;
    }

    // Metoda za pretragu tacnog pogotka u rjecniku
    bool search(const std::string& word) const {
        TrieNode* current = root;
        for (char c : word) {
            unsigned char index = static_cast<unsigned char>(c);
            if (current->children[index] == nullptr) {
                return false;
            }
            current = current->children[index];
        }
        return current->is_terminal;
    }

    // Metoda koja provjerava da li postoji rijec sa datim prefiksom
    bool startsWith(const std::string& prefix) const {
        TrieNode* current = root;
        for (char c : prefix) {
            unsigned char index = static_cast<unsigned char>(c);
            if (current->children[index] == nullptr) {
                return false;
            }
            current = current->children[index];
        }
        return true;
    }

    // Javna metoda za brisanje rijeci
    void remove(const std::string& word) {
        removeHelper(root, word, 0);
    }
};

// Primjer upotrebe
int main() {
    Trie t;
    t.insert("pas");
    t.insert("pauk");
    t.insert("sir");
    t.insert("sirup");

    std::cout<<std::boolalpha;

    std::cout << "Trie sadrzi 'pas'? " << t.search("pas") << std::endl;
    std::cout << "Trie sadrzi 'sir'? " << t.search("sir") << std::endl;
    std::cout << "Trie sadrzi 'pauk'? " << t.search("pauk") << std::endl;
    std::cout << "Trie sadrzi 'sirasto'? " << t.search("sirasto") << std::endl;
    
    std::cout << "Postoji li prefiks 'pa'? " << t.startsWith("pa") << std::endl;
    std::cout << "Postoji li prefiks 'si'? " << t.startsWith("si") << std::endl;
    std::cout << "Postoji li prefiks 'ka'? " << t.startsWith("ka") << std::endl;

    return 0;
}