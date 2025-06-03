#include <iostream>
#include <string>

class Color {
public:
    static std::string Reset() {
        return "\033[0m";  // Reset color
    }

    static std::string Green(const std::string& text) {
        return "\033[32m" + text + Reset();
    }

    static std::string Red(const std::string& text) {
        return "\033[31m" + text + Reset();
    }

    static std::string Yellow(const std::string& text) {
        return "\033[33m" + text + Reset();
    }

    static std::string Blue(const std::string& text) {
        return "\033[34m" + text + Reset();
    }

    static std::string Magenta(const std::string& text) {
        return "\033[35m" + text + Reset();
    }

    static std::string Cyan(const std::string& text) {
        return "\033[36m" + text + Reset();
    }

    static std::string White(const std::string& text) {
        return "\033[37m" + text + Reset();
    }
};

class Logger {
public:
    void static Log(const std::string& message) {
        std::cout << message << std::endl;
    }
    void static Appendable(const std::string& message) {
        std::cout << message;
    }
};

