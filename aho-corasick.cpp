#include <iostream>
#include <vector>
#include <queue>
#include <string>

const int ALPHABET_SIZE = 256;

// Struktura koja predstavlja cvor u Aho-Corasick automatu
struct ACNode {
    ACNode* children[ALPHABET_SIZE];
    ACNode* fail;
    std::vector<std::string> output;

    ACNode() {
        fail = nullptr;
        for (int i = 0; i < ALPHABET_SIZE; i++) children[i] = nullptr;
    }
};

// Klasa za Aho-Corasick automat
class AhoCorasick {
private:
    ACNode* root;

public:
    // Konstruktor
    AhoCorasick() { 
        root = new ACNode(); 
    }

    // Umetanje rijeci
    void insert(const std::string& word) {
        ACNode* current = root;
        for (char c : word) {
            int index = static_cast<unsigned char>(c);
            if (!current->children[index]) {
                current->children[index] = new ACNode();
            }
            current = current->children[index];
        }
        current->output.push_back(word);
    }

    // Izgradnja failure linkova
    void buildAutomaton() {
        std::queue<ACNode*> q;
        root->fail = root;

        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (root->children[i]) {
                root->children[i]->fail = root;
                q.push(root->children[i]);
            }
        }

        while (!q.empty()) {
            ACNode* current = q.front();
            q.pop();

            for (int i = 0; i < ALPHABET_SIZE; i++) {
                if (current->children[i]) {
                    ACNode* child = current->children[i];
                    ACNode* failNode = current->fail;

                    while (failNode != root && failNode->children[i] == nullptr) {
                        failNode = failNode->fail;
                    }

                    if (failNode->children[i]) {
                        child->fail = failNode->children[i];
                    } else {
                        child->fail = root;
                    }

                    child->output.insert(child->output.end(),
                                         child->fail->output.begin(),
                                         child->fail->output.end());
                    q.push(child);
                }
            }
        }
    }

    // Pronadji sve pojave uzoraka u tekstu
    std::vector<std::pair<int, std::string>> search(const std::string& text) {
        std::vector<std::pair<int, std::string>> results;
        ACNode* current = root;

        for (int i = 0; i < (int)text.size(); ++i) {
            int index = static_cast<unsigned char>(text[i]);

            while (current != root && !current->children[index]) {
                current = current->fail;
            }

            if (current->children[index]) current = current->children[index];

            if (!current->output.empty()) {
                for (const auto& pat : current->output) {
                    results.emplace_back(i - (int)pat.size() + 1, pat);
                }
            }
        }

        return results;
    }

    // Oslobadjanje cvorova
    void freeNodes(ACNode* node) {
        if (!node) return;
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                ACNode* child = node->children[i];
                node->children[i] = nullptr;
                freeNodes(child);
            }
        }
        delete node;
    }

    // Destruktor
    ~AhoCorasick() {
        freeNodes(root);
    }
};