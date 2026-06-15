#include <iostream>
#include <set>
#include <string>
#include <vector>

const int EMPTY = -1;
const int ROOT_CHECK = -2;

class DoubleArrayTrie {
private:
    int alpha;
    std::vector<int> base;
    std::vector<int> check;
    std::vector<char> terminal;
    std::set<int> freePos;
    int wordCount;

    // Pretvaranje znaka u indeks
    int token(char ch) const {
        if (ch >= 'A' && ch <= 'Z') ch = (char)(ch - 'A' + 'a');
        if (ch < 'a' || ch > 'z') return -1;
        return ch - 'a';
    }

    // Prosirenje nizova do trazenog indeksa
    void ensureSize(int pos) {
        while ((int)base.size() <= pos) {
            int idx = (int)base.size();
            base.push_back(EMPTY);
            check.push_back(EMPTY);
            terminal.push_back(0);
            if (idx != 0) freePos.insert(idx);
        }
    }

    // Da li postoji prelaz iz stanja sa znakom
    int transition(int state, int c) const {
        if (state < 0 || state >= (int)base.size()) return -1;
        int b = base[state];
        if (b <= 0) return -1;
        int next = b + c;
        if (next < 0 || next >= (int)check.size()) return -1;
        if (check[next] != state) return -1;
        return next;
    }

    // Sva djeca stanja
    std::vector<int> childrenOf(int state) const {
        std::vector<int> out;
        if (state < 0 || state >= (int)base.size()) return out;
        int b = base[state];
        if (b <= 0) return out;
        for (int c = 0; c < alpha; ++c) {
            int pos = b + c;
            if (pos >= 0 && pos < (int)check.size() && check[pos] == state) out.push_back(c);
        }
        return out;
    }

    // Provjera da li stanje ima djecu
    bool hasChild(int state) const {
        if (state < 0 || state >= (int)base.size()) return false;
        int b = base[state];
        if (b <= 0) return false;
        for (int c = 0; c < alpha; ++c) {
            int pos = b + c;
            if (pos >= 0 && pos < (int)check.size() && check[pos] == state) return true;
        }
        return false;
    }

    // Nadji bazu bez konflikta za skup djece
    int findBaseFor(const std::set<int>& values) {
        if (values.empty()) return 1;

        int minv = *values.begin();
        ensureSize((int)check.size() + alpha + 64);

        while (true) {
            auto it = freePos.lower_bound(minv);
            for (; it != freePos.end(); ++it) {
                int candidateBase = *it - minv;
                if (candidateBase <= 0) continue;

                bool ok = true;
                for (int v : values) {
                    int pos = candidateBase + v;
                    ensureSize(pos);
                    if (check[pos] != EMPTY) {
                        ok = false;
                        break;
                    }
                }

                if (ok) return candidateBase;
            }

            ensureSize((int)check.size() + alpha + 64);
        }
    }

    // Premjesti svu djecu u novu bazu
    void relocateChildren(int state, int newBase) {
        int oldBase = base[state];
        std::vector<int> kids = childrenOf(state);

        for (int c : kids) {
            int oldPos = oldBase + c;
            int newPos = newBase + c;

            ensureSize(newPos);

            check[newPos] = state;
            base[newPos] = base[oldPos];
            terminal[newPos] = terminal[oldPos];
            freePos.erase(newPos);

            if (base[newPos] > 0) {
                for (int d = 0; d < alpha; ++d) {
                    int g = base[newPos] + d;
                    if (g >= 0 && g < (int)check.size() && check[g] == oldPos) {
                        check[g] = newPos;
                    }
                }
            }
        }

        for (int c : kids) {
            int oldPos = oldBase + c;
            int newPos = newBase + c;
            if (oldPos == newPos) continue;
            base[oldPos] = EMPTY;
            check[oldPos] = EMPTY;
            terminal[oldPos] = 0;
            if (oldPos != 0) freePos.insert(oldPos);
        }

        base[state] = newBase;
    }

public:
    // Konstruktor
    DoubleArrayTrie(int alphabet = 26) : alpha(alphabet), wordCount(0) {
        base.push_back(1);
        check.push_back(ROOT_CHECK);
        terminal.push_back(0);
    }

    // Dodavanje rijeci
    bool add(const std::string& word) {
        if (word.empty()) {
            if (terminal[0]) return false;
            terminal[0] = 1;
            ++wordCount;
            return true;
        }

        int state = 0;
        int i = 0;

        while (i < (int)word.size()) {
            int c = token(word[i]);
            if (c < 0 || c >= alpha) return false;

            int next = transition(state, c);
            if (next != -1) {
                state = next;
                ++i;
                continue;
            }

            if (base[state] <= 0) base[state] = 1;
            int target = base[state] + c;
            ensureSize(target);

            if (check[target] == EMPTY) {
                check[target] = state;
                base[target] = 1;
                terminal[target] = 0;
                freePos.erase(target);
                state = target;
                ++i;
                continue;
            }

            if (check[target] != state) {
                std::vector<int> exist = childrenOf(state);
                std::set<int> values(exist.begin(), exist.end());
                values.insert(c);
                int newBase = findBaseFor(values);
                relocateChildren(state, newBase);
                continue;
            }
        }

        if (terminal[state]) return false;
        terminal[state] = 1;
        ++wordCount;
        return true;
    }

    // Provjera tacnog pogotka
    bool contains(const std::string& word) const {
        int state = 0;
        for (char ch : word) {
            int c = token(ch);
            if (c < 0 || c >= alpha) return false;
            state = transition(state, c);
            if (state == -1) return false;
        }
        return terminal[state] != 0;
    }

    // Provjera prefiksa
    bool startsWith(const std::string& pref) const {
        int state = 0;
        for (char ch : pref) {
            int c = token(ch);
            if (c < 0 || c >= alpha) return false;
            state = transition(state, c);
            if (state == -1) return false;
        }
        return true;
    }

    // Brisanje rijeci
    bool remove(const std::string& word) {
        int state = 0;
        std::vector<int> path;
        path.push_back(0);

        for (char ch : word) {
            int c = token(ch);
            if (c < 0 || c >= alpha) return false;
            state = transition(state, c);
            if (state == -1) return false;
            path.push_back(state);
        }

        if (!terminal[state]) return false;
        terminal[state] = 0;
        --wordCount;

        for (int i = (int)path.size() - 1; i >= 1; --i) {
            int node = path[i];
            if (terminal[node]) break;
            if (hasChild(node)) break;
            base[node] = EMPTY;
            check[node] = EMPTY;
            terminal[node] = 0;
            freePos.insert(node);
        }

        return true;
    }

    // Broj rijeci
    int size() const {
        return wordCount;
    }

    // Velicina abecede
    int alphabetSize() const {
        return alpha;
    }
};

int main() {
    DoubleArrayTrie dat(26);

    dat.add("cat");
    dat.add("car");
    dat.add("cargo");
    dat.add("cabin");
    dat.add("metal");
    dat.add("mental");
    dat.add("metallica");
    dat.add("mask");
    dat.add("tree");
    dat.add("train");
    dat.add("dispatcher");

    std::cout<<std::boolalpha;

    std::cout << "abeceda: " << dat.alphabetSize() << "\n";
    std::cout << "contains metal: " << dat.contains("metal") << "\n";
    std::cout << "contains meta: " << dat.contains("meta") << "\n";
    std::cout << "startsWith met: " << dat.startsWith("met") << "\n";
    std::cout << "size: " << dat.size() << "\n";

    dat.remove("metal");
    std::cout << "contains metal poslije remove: " << dat.contains("metal") << "\n";
    std::cout << "size poslije remove: " << dat.size() << "\n";

    return 0;
}
