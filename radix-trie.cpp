#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

struct RadixNode {
    // Mapa koja povezuje prvi karakter grane sa ostatkom stringa i djetetom
    // Kljuc: prvi karakter, Vrijednost: par (ostatak stringa na grani, pokazivac na dijete)
    std::unordered_map<char, std::pair<std::string, RadixNode*>> edges;
    bool is_terminal;

    RadixNode() : is_terminal(false) {}
    
    ~RadixNode() {
        for (auto& edge : edges) {
            delete edge.second.second;
        }
    }
};

class RadixTrie {
private:
    RadixNode* root;

public:
    RadixTrie() { root = new RadixNode(); }
    ~RadixTrie() { delete root; }

    // Pojednostavljena pretraga koja demonstrira skakanje po stringovima
    bool search(const std::string& word) {
        RadixNode* current = root;
        int i = 0;

        while (i < word.length()) {
            char c = word[i];
            
            if (current->edges.find(c) == current->edges.end()) {
                return false;
            }

            std::string edge_label = current->edges[c].first;
            RadixNode* next_node = current->edges[c].second;

            int j = 0;
            while (j < edge_label.length() && i < word.length() && edge_label[j] == word[i]) {
                i++;
                j++;
            }

            if (j < edge_label.length()) {
                return false;
            }

            current = next_node;
        }

        return current->is_terminal;
    }

    // Umetanje rijeci
    void insert(const std::string& word) {
        RadixNode* current = root;
        int i = 0;

        while (i < (int)word.size()) {
            char c = word[i];
            auto it = current->edges.find(c);

            if (it == current->edges.end()) {
                RadixNode* node = new RadixNode();
                current->edges[c] = {word.substr(i), node};
                node->is_terminal = true;
                return;
            }

            std::string label = it->second.first;
            RadixNode* child = it->second.second;

            int j = 0;
            int remain = (int)word.size() - i;
            while (j < (int)label.size() && j < remain && label[j] == word[i + j]) j++;

            if (j == (int)label.size()) {
                i += j;
                current = child;
                continue;
            }

            std::string common = label.substr(0, j);
            std::string label_rest = label.substr(j);
            std::string word_rest = word.substr(i + j);

            RadixNode* mid = new RadixNode();

            mid->edges[label_rest[0]] = {label_rest, child};

            current->edges[c] = {common, mid};

            if (word_rest.empty()) {
                mid->is_terminal = true;
            } else {
                RadixNode* newNode = new RadixNode();
                newNode->is_terminal = true;
                mid->edges[word_rest[0]] = {word_rest, newNode};
            }

            return;
        }

        current->is_terminal = true;
    }

    // Pomocna funkcija za pronalazak svih rijeci unutar cvora
    void collect(RadixNode* node, const std::string& prefix, std::vector<std::string>& out) {
        if (!node) return;
        if (node->is_terminal) out.push_back(prefix);
        for (auto& e : node->edges) {
            const std::string& label = e.second.first;
            RadixNode* child = e.second.second;
            collect(child, prefix + label, out);
        }
    }

    // Vracanje svih rijeci
    std::vector<std::string> listAll() {
        std::vector<std::string> res;
        collect(root, "", res);
        return res;
    }

    // Provjera prefiksa
    bool startsWith(const std::string& prefix) {
        RadixNode* current = root;
        int i = 0;

        while (i < (int)prefix.size()) {
            char c = prefix[i];
            auto it = current->edges.find(c);
            if (it == current->edges.end()) return false;
            std::string label = it->second.first;
            RadixNode* child = it->second.second;

            int j = 0;
            while (j < (int)label.size() && i < (int)prefix.size() && label[j] == prefix[i]) { j++; i++; }

            if (j < (int)label.size() && i >= (int)prefix.size()) {
                return true;
            }

            if (j < (int)label.size()) return false;

            current = child;
        }

        return true;
    }

    // Broj ukupnih rijeci
    int countWords() {
        return (int)listAll().size();
    }

    // Brisanje rijeci
    bool erase(const std::string& word) {
        struct Frame { RadixNode* parent; char edge_c; std::string edge_label; RadixNode* node; };
        std::vector<Frame> stack;

        RadixNode* current = root;
        int i = 0;

        while (i < (int)word.size()) {
            char c = word[i];
            auto it = current->edges.find(c);
            if (it == current->edges.end()) return false;
            std::string label = it->second.first;
            RadixNode* child = it->second.second;

            int j = 0;
            while (j < (int)label.size() && i < (int)word.size() && label[j] == word[i]) { j++; i++; }

            if (j < (int)label.size()) return false; // mismatch

            stack.push_back({current, c, label, child});
            current = child;
        }

        if (!current->is_terminal) return false; // not present
        current->is_terminal = false;

        for (int k = (int)stack.size() - 1; k >= 0; --k) {
            RadixNode* parent = stack[k].parent;
            char edge_c = stack[k].edge_c;
            RadixNode* node = stack[k].node;

            if (node->is_terminal) break;
            if (node->edges.size() > 1) break;

            if (node->edges.empty()) {
                parent->edges.erase(edge_c);
                delete node;
            } else {
                auto only = *node->edges.begin();
                std::string rest_label = only.second.first;
                RadixNode* childNode = only.second.second;
                std::string new_label = stack[k].edge_label + rest_label;
                parent->edges.erase(edge_c);
                parent->edges[new_label[0]] = {new_label, childNode};
                node->edges.clear();
                delete node;
            }
        }

        return true;
    }
};