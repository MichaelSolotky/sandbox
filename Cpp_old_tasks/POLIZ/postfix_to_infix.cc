#include <iostream>
#include <string>
#include <stack>

bool IsLetter(char c) {
    if (c >= 'a' && c <= 'z') {
        return true;
    } else {
        return false;
    }
}

bool IsOperation(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/') {
        return true;
    } else {
        return false;
    }
}

int main() {
    std::stack<std::string> st;
    char c;
    while (std::cin >> c) {
        if (IsLetter(c)) {
            std::string str(1, c);
            st.push(std::move(str));
        } else if (IsOperation(c)) {
            std::string tmp_str_2 = st.top();
            st.pop();
            std::string tmp_str_1 = st.top();
            st.pop();
            std::string new_str(1, '(');
            new_str += tmp_str_1;
            new_str.push_back(c);
            new_str += tmp_str_2;
            new_str.push_back(')');
            st.push(std::move(new_str));
        }
    }
    std::cout << st.top() << std::endl;
}