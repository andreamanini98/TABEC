#ifndef UTOTPARSER_EXCEPTIONS_H
#define UTOTPARSER_EXCEPTIONS_H

class NotXMLFormatException : public std::exception {
public:
    explicit NotXMLFormatException(const char *message) : message_(message)
    {}


    [[nodiscard]] const char *what() const noexcept override
    {
        return message_.c_str();
    }


private:
    std::string message_;
};

#endif //UTOTPARSER_EXCEPTIONS_H
