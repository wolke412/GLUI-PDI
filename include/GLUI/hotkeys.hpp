#include <GLFW/glfw3.h>
#include <functional>
#include <map>

struct Hotkey {
    char key;
    int modifiers;
    std::function<void()> callback;

    bool matches(char key, int mods) const {
        return (this->key == key) && ((mods & this->modifiers) == this->modifiers);
    }

    int rank() const {
        int count = 0;
        if (modifiers & GLFW_MOD_CONTROL) count++;
        if (modifiers & GLFW_MOD_SHIFT) count++;
        if (modifiers & GLFW_MOD_ALT) count++;
        return count;
    }
};

class Hotkeys {
public:
    void registerhk(int key, int modifiers, std::function<void()> callback) {
        Hotkey hk{key, modifiers, callback};
        hotkeys[{key, modifiers}] = hk;
    }

    void registerhk(char key, int modifiers, std::function<void()> callback) {
        Hotkey hk{key, modifiers, callback};
        hotkeys[{key, modifiers}] = hk;
    }

    void handle(char key, int mods) {
        std::cout << "Checking hotkeys for " << key << " with " << mods << std::endl;
        Hotkey* best = nullptr;

        for (auto& [_, hk] : hotkeys) {
            if (hk.matches(key, mods)) {
                if (!best || hk.rank() > best->rank())
                    best = &hk;
            }
        }

        if (best && best->callback)
            best->callback();
    }

private:
    std::map<std::pair<char, int>, Hotkey> hotkeys;
};
