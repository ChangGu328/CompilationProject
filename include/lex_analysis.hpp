//
// Created by Chang Gu on 24-5-31.
//

#ifndef COMPILATIONPROJECT_LEX_ANALYSIS_HPP
#define COMPILATIONPROJECT_LEX_ANALYSIS_HPP

#include <string>
#include <vector>

struct RValue {

    int id;         //属性
    int line;       //行号
    std::string value;   //值

};

enum Symbol {
    PROG = 1,   //program
    BEG,        //begin
    END,        //end
    IF,         //if
    THEN,       //then
    ELS,        //else
    CON,        //const
    PROC,       //procdure
    VAR,        //var
    DO,         //do
    WHI,        //while
    CAL,        //call
    REA,        //read
    WRI,        //write
    REP,        //repeate
    ODD,        //ODDl      和keyWord中每个字的序号是相等的
    EQU,        //"="
    LES,        //"<"
    LESE,       //"<="
    LARE,       //">="
    LAR,        //">"
    NEQE,       //"<>"
    ADD,        //"+"
    SUB,        //"-"
    MUL,        //"*"
    DIV,        //"/"
    SYM,        //标识符
    CONST,      //常量
    CEQU,       //":="
    COMMA,      //","
    SEMIC,      //","
    POI,        //"."
    LBR,        //"("
    RBR,        //")"
};

/**
 * 管理所有符号表
 */
struct ChTable {
private:
    const int symLength = 100;
    const int conLength = 100;
public:
    const std::vector<std::string> keyWord = {"program", "begin", "end", "if", "then", "else", "const", "procedure",
                                              "var", "do", "while", "call", "read", "write", "repeat", "odd"};

    std::vector<std::string> symTable;

    std::vector<std::string> constTable;

    ChTable() : symTable(symLength), constTable(conLength) {}

};

class LexAnalysis {
public:
    LexAnalysis(std::string _filename);

    /**
     * 循环识别出所有字符并输出到文件lex.txt中
     */
    void bAnalysis();

private:
    std::string filename;
    const ChTable ct;
    RValue rv;
    const std::vector<std::string> keyWord = ct.keyWord;
    std::vector<std::string> symTable = ct.symTable;
    const int symLength = symTable.size();
    std::vector<std::string> constTable = ct.constTable;
    const int conLength = constTable.size();
    char ch = ' ';
    std::string strToken;
    std::string buffer;
    int searchPtr = 0;
    int line = 1;
    bool errorHappen = false;

    void showError();

    /**
     * 预处理函数：
     * 功能：读取源文件内容到字符数组buffer中去，包括换行符
     */
    void preManage();

    /**
     * 字符识别函数
     * 功能：通过读取buffer数组中的单个字符进行识别源程序中的各个元素，每次识别一个元素
     *
     * @return 识别出的字符的属性：RV.id是属性，RV.value是值，RV.line是所在行号
     */
    RValue analysis();

    void getChar();

    void getBC();

    bool isLetter();

    bool isDigit();

    void concat();

    void retract();

    int reserve();

    int insertId();

    int insertConst();
};;

#endif //COMPILATIONPROJECT_LEX_ANALYSIS_HPP
