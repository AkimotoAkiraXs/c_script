#include <iostream>
#include <string>
#include <vector>

using namespace std;

// function statement
string parse(const string &line);

enum AsrStateType {
    STATE_NULL,
    START,
    ARRAY_START,
    STRING_START,
    ESCAPE,
    STRING_END,
    ARRAY_NEXT,
    ARRAY_END,
    END,
    S_COUNT  // cnt

};

enum AsrCharType {
    CHAR_NULL,
    SP,      // space
    LB,      // left bracket
    RB,      // right bracket
    DQ,      // double quotation
    C,       // character except escape and single quotation
    E,       // escape
    CMA,     // comma
    EF,      // end of file
    C_COUNT  // cnt
};

static const int stateTypeSize = AsrStateType::S_COUNT;
static const int charTypeSize = AsrCharType::C_COUNT;
static const string errorResult = "[NULL]";
static const string emptyArray = "[]";
static AsrStateType matrix[stateTypeSize][charTypeSize];

void init() {
    matrix[AsrStateType::START][AsrCharType::LB] = AsrStateType::ARRAY_START;
    matrix[AsrStateType::START][AsrCharType::SP] = AsrStateType::START;
    matrix[AsrStateType::START][AsrCharType::EF] = AsrStateType::END;
    matrix[AsrStateType::ARRAY_START][AsrCharType::RB] = AsrStateType::ARRAY_END;
    matrix[AsrStateType::ARRAY_START][AsrCharType::DQ] =
            AsrStateType::STRING_START;
    matrix[AsrStateType::ARRAY_START][AsrCharType::SP] =
            AsrStateType::ARRAY_START;
    matrix[AsrStateType::STRING_START][AsrCharType::DQ] =
            AsrStateType::STRING_END;
    matrix[AsrStateType::STRING_START][AsrCharType::E] = AsrStateType::ESCAPE;
    matrix[AsrStateType::STRING_START][AsrCharType::C] =
            AsrStateType::STRING_START;
    matrix[AsrStateType::ESCAPE][AsrCharType::DQ] = AsrStateType::STRING_START;
    matrix[AsrStateType::ESCAPE][AsrCharType::E] = AsrStateType::STRING_START;
    matrix[AsrStateType::ESCAPE][AsrCharType::C] = AsrStateType::STRING_START;
    matrix[AsrStateType::STRING_END][AsrCharType::RB] = AsrStateType::ARRAY_END;
    matrix[AsrStateType::STRING_END][AsrCharType::CMA] = AsrStateType::ARRAY_NEXT;
    matrix[AsrStateType::STRING_END][AsrCharType::SP] = AsrStateType::STRING_END;
    matrix[AsrStateType::ARRAY_NEXT][AsrCharType::DQ] =
            AsrStateType::STRING_START;
    matrix[AsrStateType::ARRAY_NEXT][AsrCharType::SP] = AsrStateType::ARRAY_NEXT;
    matrix[AsrStateType::ARRAY_END][AsrCharType::EF] = AsrStateType::END;
    matrix[AsrStateType::ARRAY_END][AsrCharType::SP] = AsrStateType::ARRAY_END;
}

int main() {
    init();
    string line;
    while (getline(cin, line))cout << parse(line) << endl;
    return 0;
}

string parse(const string &line) {
    if (line.empty()) return emptyArray;
    string result;
    AsrStateType state = START;
    for (char c: line) {
        AsrCharType charType = AsrCharType::CHAR_NULL;
        switch (state) {
            case STRING_START:
            case ESCAPE:
                switch (c) {
                    case '"':
                        charType = AsrCharType::DQ;
                        break;
                    case '\\':
                        charType = AsrCharType::E;
                        break;
                    default:
                        charType = AsrCharType::C;
                        break;
                };
                break;
            default:
                switch (c) {
                    case ' ':
                        charType = AsrCharType::SP;
                        break;
                    case '[':
                        charType = AsrCharType::LB;
                        break;
                    case ']':
                        charType = AsrCharType::RB;
                        break;
                    case '"':
                        charType = AsrCharType::DQ;
                        break;
                    case '\\':
                        charType = AsrCharType::E;
                        break;
                    case ',':
                        charType = AsrCharType::CMA;
                    default:;
                }
                break;
        }
        if (charType == AsrCharType::CHAR_NULL) {
            return errorResult;
        }
        state = matrix[state][charType];
        if (state == AsrStateType::STATE_NULL) {
            return errorResult;
        }
        if (c == '"') c = '\'';
        result += c;
    }
    if (AsrStateType::END != matrix[state][AsrCharType::EF]) {
        return errorResult;
    }
    return result.empty() ? errorResult : result;
}
