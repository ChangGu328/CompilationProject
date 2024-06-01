//
// Created by Chang Gu on 24-5-31.
//

#ifndef COMPILATIONPROJECT_MPG_ANALYSIS_HPP
#define COMPILATIONPROJECT_MPG_ANALYSIS_HPP

#include <string>
#include <vector>
#include "lex_analysis.hpp"

/**
 * PerPcode类表示一条Pcode指令。
 * 每条指令包含操作码、层次差和操作数。
 */
struct PerPcode {
    int F, L, A;

    PerPcode(int f = -1, int l = -1, int a = -1);
};

/**
 * 管理所有Pcode指令
 * pcodeArray存储所有perCode
 * 用于存放目标代码
 */
struct AllPcode {
    /*
     *    代码的具体形式：
     *    FLA
     *    其中：F段代表伪操作码
     *    L段代表调用层与说明层的层差值
     *    A段代表位移量（相对地址）
     *    进一步说明：
     *    INT：为被调用的过程（包括主过程）在运行栈S中开辟数据区，这时A段为所需数据单元个数（包括三个连接数据）；L段恒为0。
     *    CAL：调用过程，这时A段为被调用过程的过程体（过程体之前一条指令）在目标程序区的入口地址。
     *    LIT：将常量送到运行栈S的栈顶，这时A段为常量值。
     *    LOD：将变量送到运行栈S的栈顶，这时A段为变量所在说明层中的相对位置。
     *    STO：将运行栈S的栈顶内容送入某个变量单元中，A段为变量所在说明层中的相对位置。
     *    JMP：无条件转移，这时A段为转向地址（目标程序）。
     *    JPC：条件转移，当运行栈S的栈顶的布尔值为假（0）时，则转向A段所指目标程序地址；否则顺序执行。
     *    OPR：关系或算术运算，A段指明具体运算，例如A=2代表算术运算“＋”；A＝12代表关系运算“>”等等。运算对象取自运行栈S的栈顶及次栈顶。
     *
     *    OPR 0 0	过程调用结束后,返回调用点并退栈
     *    OPR 0 1	栈顶元素取反
     *    OPR 0 2	次栈顶与栈顶相加，退两个栈元素，结果值进栈
     *    OPR 0 3	次栈顶减去栈顶，退两个栈元素，结果值进栈
     *    OPR 0 4	次栈顶乘以栈顶，退两个栈元素，结果值进栈
     *    OPR 0 5	次栈顶除以栈顶，退两个栈元素，结果值进栈
     *    OPR 0 6	栈顶元素的奇偶判断，结果值在栈顶
     *    OPR 0 7
     *    OPR 0 8	次栈顶与栈顶是否相等，退两个栈元素，结果值进栈
     *    OPR 0 9	次栈顶与栈顶是否不等，退两个栈元素，结果值进栈
     *    OPR 0 10	次栈顶是否小于栈顶，退两个栈元素，结果值进栈
     *    OPR 0 11	次栈顶是否大于等于栈顶，退两个栈元素，结果值进栈
     *    OPR 0 12	次栈顶是否大于栈顶，退两个栈元素，结果值进栈
     *    OPR 0 13	次栈顶是否小于等于栈顶，退两个栈元素，结果值进栈
     *    OPR 0 14	栈顶值输出至屏幕
     *    OPR 0 15	屏幕输出换行
     *    OPR 0 16	从命令行读入一个输入置于栈顶
     */
private:
    const int MAX_PCODE = 10000;

public:
    static const int LIT = 0;        //LIT 0 ，a 取常量a放入数据栈栈顶
    static const int OPR = 1;        //OPR 0 ，a 执行运算，a表示执行某种运算，具体是何种运算见上面的注释
    static const int LOD = 2;        //LOD L ，a 取变量（相对地址为a，层差为L）放到数据栈的栈顶
    static const int STO = 3;        //STO L ，a 将数据栈栈顶的内容存入变量（相对地址为a，层次差为L）
    static const int CAL = 4;        //CAL L ，a 调用过程（转子指令）（入口地址为a，层次差为L）
    static const int INT = 5;         //INT 0 ，a 数据栈栈顶指针增加a
    static const int JMP = 6;       //JMP 0 ，a无条件转移到地址为a的指令
    static const int JPC = 7;        //JPC 0 ，a 条件转移指令，转移到地址为a的指令
    static const int RED = 8;       //RED L ，a 读数据并存入变量（相对地址为a，层次差为L）
    static const int WRT = 9;       //WRT 0 ，0 将栈顶内容输出
    int codePtr = 0;
    PerPcode *const pcodeArray;

    AllPcode();

    void gen(int f, int l, int a);
};

/**
 * TableRow是符号表中的每一行
 */
struct TableRow {
    int type;           //表示常量、变量或过程
    int value;          //表示常量或变量的值
    int level;          //嵌套层次，最大应为3，不在这里检查其是否小于等于，在SymbolTable中检查
    int address;      //相对于所在嵌套过程基地址的地址
    int size;         //表示常量，变量，过程所占的大小，此变量和具体硬件有关，实际上在本编译器中为了方便，统一设为4了,设置过程在SymTable中的三个enter函数中
    std::string name;   //变量、常量或过程名
    TableRow(int type = 0, int value = 0, int level = 0, int address = 0, int size = 0, std::string name = "");
};

/**
 * SymTable符号表，每一列的具体含义见TableRow
 */
struct SymbolTable {
    const int rowMax = 10000;           //最大表长
    const int valueMax = 100000;        //最大常量或变量值
    const int levelMax = 3;                 //最深嵌套层次
    const int addressMax = 10000;       //最大地址数


    const int myconst = 1;                  //常量类型用1表示
    const int var = 2;                      //变量类型用2表示
    const int proc = 3;             //过程类型用3表示

    //TableRow是符号表中的每一行
    //tablePtr指向符号表中已经填入值最后一项的下一项
    //length表示符号表中填入；了多少行数据，实际上可以用tablePtr来表示
    TableRow *const table;

    int tablePtr = 0;
    int length = 0;

    SymbolTable();

    /*
     *登录常量进符号表
     * 参数：
     * name：常量名
     * level：所在层次
     * value：值
     * address：相对于所在层次基地址的地址
     */
    void enterConst(std::string name, int level, int value, int address);

    /*
     *    登录变量进符号表
     *  参数同上
     *  说明：由于登录符号表操作都是在变量声明或常量声明或过程声明中调用，而PL/0不支持变量声明时赋值，所以不传入参数value
     *
     */
    void enterVar(std::string name, int level, int address);

    //登录过程进符号表，参数同上
    void enterProc(std::string name, int level, int address);

    //在lev层之前，包括lev层，名字为name的变量、常量、或过程是否被定义，
    //使用变量、常量、或过程时调用该函数
    bool isPreExistSTable(std::string name, int lev);

    //在lev层，名字为name的变量、常量、或过程是否被定义，
    //定义变量、常量、或过程时调用该函数
    bool isNowExistSTable(std::string name, int lev);

    //返回符号表中名字为name的行的行号
    int getNameRow(std::string name);

    //查找本层的过程在符号表中的位置
    int getLevelPorc();
};

class MpgAnalysis {
public:

    explicit MpgAnalysis(std::string filename);

    bool mgpAnalysis();

    void showPcodeInStack();

private:
    const int RVLENGTH;
    RValue *const rv;
    LexAnalysis lex;
    int terPtr = 0;
    bool errorHapphen = false;
    int address = 0;
    int level = 0;
    AllPcode Pcode;
    SymbolTable STable;

    void readLex();

    void prog();

    void showError(int i, std::string name);

    void block();

    void condecl();

    void myconst();

    void vardecl();

    //递归语法分析
    void proc();

    void body();

    void statement();

    void lexp();

    void exp();

    void term();

    void factor();

    int lop();
};

#endif //COMPILATIONPROJECT_MPG_ANALYSIS_HPP
