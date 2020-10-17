#ifndef __RUNTIME_ERROR_H
#define __RUNTIME_ERROR_H

class Runtime_error : public std::runtime_error {
    std::shared_ptr<Token> token;
public:
    Runtime_error(std::string msg, std::shared_ptr<Token> token)
        : std::runtime_error(msg), token(token) {}
    Runtime_error(const Runtime_error&) = default;
    Runtime_error(Runtime_error&&) = default;
    ~Runtime_error() = default;
    Runtime_error& operator=(Runtime_error&) = default;
    Runtime_error& operator=(Runtime_error&&) = default;

    std::shared_ptr<Token> get_token() { return token; }
};

#endif
