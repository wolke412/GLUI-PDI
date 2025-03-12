#include <GLUI/input.hpp>

bool is_valid_tex_input(unsigned int codepoint) {
    // Define a set of special characters you want to allow
    std::unordered_set<unsigned int> special_chars = {
        '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '-', '=', '{', '}', '[', ']', '|', '\\', ';', ':', '\'', '\"', '<', '>', '.', ',', '/', '?', '~'};

    // Check if the codepoint is alphanumeric or a special character
    return ((codepoint >= 'A' && codepoint <= 'Z') ||
            (codepoint >= 'a' && codepoint <= 'z') ||
            (codepoint >= '0' && codepoint <= '9') ||
            (special_chars.find(codepoint) != special_chars.end())); // Check if it's a special character
}