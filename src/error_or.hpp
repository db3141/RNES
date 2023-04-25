#ifndef ERROR_OR_INCLUDED
#define ERROR_OR_INCLUDED

// Thanks SerenityOS for the idea!
#include <optional>
#include <variant>

class ErrorCode {
public:
    explicit ErrorCode() : m_errorCode(0) {
        ;
    }

    explicit ErrorCode(int t_errorCode) : m_errorCode(t_errorCode) {
        ;
    }

    [[nodiscard]] int getErrorCode() const {
        return m_errorCode;
    }

private:
    int m_errorCode;
};

template<typename Ty>
class ErrorOr {
public:
    ErrorOr(Ty t_value) : m_value(t_value) {
        ;
    }

    ErrorOr(ErrorCode t_error) : m_value(t_error) {
        ;
    }

    [[nodiscard]] bool is_error() const {
        return std::holds_alternative<ErrorCode>(m_value);
    }

    [[nodiscard]] Ty get_value() const {
        return std::get<Ty>(m_value);
    }

    [[nodiscard]] ErrorCode get_error() const {
        return std::get<ErrorCode>(m_value);
    }

private:
    std::variant<Ty, ErrorCode> m_value;
};

template <>
class ErrorOr<void> {
public:
    ErrorOr() : m_error(std::nullopt) {
        ;
    }

    ErrorOr(ErrorCode t_error) : m_error(t_error) {
        ;
    }

    [[nodiscard]] bool is_error() const {
        return m_error.has_value();
    }

    void get_value() const {
        ;
    }

    [[nodiscard]] ErrorCode get_error() const {
        return m_error.value();
    }

private:
    std::optional<ErrorCode> m_error;
};

// This uses a GCC extension!
#define TRY(x) ({\
    const auto _errorOrX = (x);\
    if (_errorOrX.is_error()) {\
        return _errorOrX.get_error();\
    }\
    _errorOrX.get_value();\
})

#define REQUIRE(x, e) do {\
    if (!(x)) {\
        return ErrorCode(e);\
    }\
} while(false)


#endif
