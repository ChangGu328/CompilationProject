//
// Created by Chang Gu on 24-5-31.
//
#include "mpg_analysis.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

PerPcode::PerPcode(int f, int l, int a) : F(f), L(l), A(a) {}

AllPcode::AllPcode() : pcodeArray(new PerPcode[MAX_PCODE]) {
    for (int i = 0; i < MAX_PCODE; i++) {
        pcodeArray[i] = PerPcode(-1, -1, -1);
    }
}

void AllPcode::gen(int f, int l, int a) {
    pcodeArray[codePtr] = PerPcode(f, l, a);
    codePtr++;
}

TableRow::TableRow(int type, int value, int level, int address, int size, std::string name) :
        type(type), value(value), level(level), address(address), size(size), name(name) {}

SymbolTable::SymbolTable() : table(new TableRow[rowMax]) {
    for (int i = 0; i < rowMax; i++)
        table[i] = TableRow();
}

void SymbolTable::enterConst(std::string name, int level, int value, int address) {
    table[tablePtr] = TableRow(myconst, value, level, address, 4, name);
    tablePtr++;
    length++;
}

void SymbolTable::enterVar(std::string name, int level, int address) {
    int value = table[tablePtr].value;
    table[tablePtr] = TableRow(var, value, level, address, 0, name);
    tablePtr++;
    length++;
}

void SymbolTable::enterProc(std::string name, int level, int address) {
    int value = table[tablePtr].value;
    table[tablePtr] = TableRow(proc, value, level, address, 0, name);
    tablePtr++;
    length++;
}

bool SymbolTable::isPreExistSTable(std::string name, int lev) {
    for (int i = 0; i < length; i++) {
        if (table[i].name == name && table[i].level <= lev)
            return true;
    }
    return false;
}

bool SymbolTable::isNowExistSTable(std::string name, int lev) {
    for (int i = 0; i < length; i++) {
        if (table[i].name == name && table[i].level == lev)
            return true;
    }
    return false;
}

int SymbolTable::getNameRow(std::string name) {
    for (int i = length - 1; i >= 0; i--) {
        if (table[i].name == name)
            return i;
    }
    return -1;
}

int SymbolTable::getLevelPorc() {
    for (int i = length - 1; i >= 0; i--) {
        if (table[i].type == proc)
            return i;
    }
    return -1;
}

MpgAnalysis::MpgAnalysis(std::string filename) : RVLENGTH(1000), rv(new RValue[RVLENGTH]), lex(filename) {
    for (int i = 0; i < RVLENGTH; i++) {
        rv[i].id = -2;
        rv[i].value = "-2";
    }
}


bool MpgAnalysis::mgpAnalysis() {
    lex.bAnalysis();
    readLex();
    prog();
    return errorHapphen;
}

void MpgAnalysis::showPcodeInStack() {
    for (int i = 0; i < Pcode.codePtr; i++) {
        switch (Pcode.pcodeArray[i].F) {
            case 0:
                std::cout << "LIT  ";
                break;
            case 1:
                std::cout << "OPR  ";
                break;
            case 2:
                std::cout << "LOD  ";
                break;
            case 3:
                std::cout << "STO  ";
                break;
            case 4:
                std::cout << "CAL  ";
                break;
            case 5:
                std::cout << "INT  ";
                break;
            case 6:
                std::cout << "JMP  ";
                break;
            case 7:
                std::cout << "JPC  ";
                break;
            case 8:
                std::cout << "RED  ";
                break;
            case 9:
                std::cout << "WRI  ";
                break;

        }
        std::cout << Pcode.pcodeArray[i].L << " " << Pcode.pcodeArray[i].A << "\n";
    }
}

void MpgAnalysis::readLex() {
    std::string filename = "lex.txt";
    std::ifstream file(filename);
    std::string tempLex;

    if (!file.is_open()) {
        std::cerr << "File not found: " << filename << std::endl;
        return;
    }

    int i = 0;
    while (std::getline(file, tempLex)) {
        std::istringstream iss(tempLex);
        std::string temp;
        int id, line;
        std::string value;

        iss >> id >> value >> line;
        rv[i].id = id;
        rv[i].value = value;
        rv[i].line = line;
        i++;
    }

    file.close();
}

void MpgAnalysis::prog() {
    if (rv[terPtr].id == PROG) {
        terPtr++;
        if (rv[terPtr].id != SYM) {
            errorHapphen = true;
            showError(1, "");
        } else {
            terPtr++;
            if (rv[terPtr].id != SEMIC) {
                errorHapphen = true;
                showError(0, "");
            } else {
                terPtr++;
                block();
            }
        }
    } else {
        errorHapphen = true;
        showError(2, "");
    }
}

void MpgAnalysis::showError(int i, std::string name) {
    switch (i) {
        case -1:
            std::cout << "ERROR " << i << " in line " << rv[terPtr].line << ":";
            std::cout << "wrong token" << std::endl;        //常量定义不是const开头,变量定义不是var 开头
            break;
        case 0:
            std::cout << "ERROR " << i << " in line " << (rv[terPtr].line - 1) << ":" << std::endl;
            std::cout << "Missing semicolon" << std::endl;        //缺少分号
            break;
        case 1:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":" << std::endl;
            std::cout << "Identifier illegal" << std::endl;       //标识符不合法
            break;
        case 2:
            std::cout << "ERROR " << i << " in line " << rv[terPtr].line << ":" << std::endl;
            std::cout << "The beginning of program must be 'program'" << std::endl;       //程序开始第一个字符必须是program
            break;
        case 3:
            std::cout << "ERROR " << i << " in line " << rv[terPtr].line << ":" << std::endl;
            std::cout << "Assign must be ':='" << std::endl;       //赋值没用：=
            break;
        case 4:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":" << std::endl;
            std::cout << "Missing '('" << std::endl;       //缺少左括号
            break;
        case 5:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "Missing ')'" << std::endl;       //缺少右括号
            break;
        case 6:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "Missing 'begin'" << std::endl;       //缺少begin
            break;
        case 7:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "Missing 'end'" << std::endl;       //缺少end
            break;
        case 8:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "Missing 'then'" << std::endl;       //缺少then
            break;
        case 9:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "Missing 'do'" << std::endl;       //缺少do
            break;
        case 10:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "Not exist " << "'" << rv[terPtr].value << "'" << std::endl;       //call，write，read语句中，不存在标识符
            break;
        case 11:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "'" << rv[terPtr].value << "'" << "is not a procedure" << std::endl;       //该标识符不是proc类型
            break;
        case 12:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout <<
                      "'" << rv[terPtr].value << "'" << "is not a variable"
                      << std::endl;       //read，write语句中，该标识符不是var类型
            break;
        case 13:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "'" << name << "'" << "is not a variable" << std::endl;       //赋值语句中，该标识符不是var类型
            break;
        case 14:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "Not exist" << "'" << name << "'" << std::endl;       //赋值语句中，该标识符不存在
            break;
        case 15:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout << "Already exist" << "'" << name << "'" << std::endl;       //该标识符已经存在
            break;
        case 16:
            std::cout << "ERROR " << i << " " << "in line " << rv[terPtr].line << ":";
            std::cout <<
                      "Number of parameters of procedure " << "'" << name << "'" << "is incorrect"
                      << std::endl;       //该标识符已经存在
            break;
    }
}


void MpgAnalysis::block() {
    int addr0 = address;      //记录本层之前的数据量，以便恢复时返回
    int tx0 = STable.tablePtr;       //记录本层名字的初始位置
    int cx0;
    int propos = 0;
    if (tx0 > 0) {
        propos = STable.getLevelPorc();
        tx0 = tx0 - STable.table[propos].size;   //记录本本层变量的开始位置
    }
    if (tx0 == 0) {
        address = 3;      //每一层最开始位置的三个空间用来存放静态连SL、动态连DL、和返回地址RA
    } else {
        //每一层最开始位置的三个空间用来存放静态连SL、动态连DL、和返回地址RA
        //紧接着放形参的个数
        address = 3 + STable.table[propos].size;
    }


    //暂存当前Pcode.codePtr的值，即jmp,0,0在codePtr中的位置，用来一会回填
    int tempCodePtr = Pcode.codePtr;
    Pcode.gen(AllPcode::JMP, 0, 0);


    if (rv[terPtr].id == CON) {//此处没有terPtr++
        condecl();
    }
    if (rv[terPtr].id == VAR) {
        vardecl();
    }
    if (rv[terPtr].id == PROC) {
        proc();
        level--;
    }
    /*
     * 声明部分完成，进入语句处理部分，之前生成的jmp，0，0应当跳转到这个位置
     *
     * */
    //回填jmp，0，0的跳转地址
    if (tx0 > 0) {
        for (int i = 0; i < STable.table[propos].size; i++) {
            Pcode.gen(AllPcode::STO, 0, STable.table[propos].size + 3 - 1 - i);
        }
    }
    Pcode.pcodeArray[tempCodePtr].A = Pcode.codePtr;
    Pcode.gen(AllPcode::INT, 0, address);        //生成分配内存的代码
    if (tx0 == 0) {
        // STable.getRow(tx0).setValue(Pcode.getCodePtr());     //将本过程在符号表中的值设为本过程执行语句开始的位置
    } else {
        STable.table[propos].value =
                Pcode.codePtr - 1 - STable.table[propos].size;     //将本过程在符号表中的值设为本过程执行语句开始的位置
    }

    body();
    Pcode.gen(AllPcode::OPR, 0, 0);      //生成退出过程的代码，若是主程序，则直接退出程序

    address = addr0;      //分程序结束，恢复相关值
    STable.tablePtr = tx0;

}

void MpgAnalysis::condecl() {
    if (rv[terPtr].id == CON) {
        terPtr++;
        myconst();
        while (rv[terPtr].id == COMMA) {
            terPtr++;
            myconst();
        }
        if (rv[terPtr].id != SEMIC) {
            errorHapphen = true;
            showError(0, "");
        } else {
            terPtr++;
        }
    } else {
        errorHapphen = true;
        showError(-1, "");
    }
}

void MpgAnalysis::myconst() {
    std::string name;
    int value;
    if (rv[terPtr].id == SYM) {
        name = rv[terPtr].value;
        terPtr++;
        if (rv[terPtr].id == CEQU) {
            terPtr++;
            if (rv[terPtr].id == CONST) {
                value = std::stoi(rv[terPtr].value);
                if (STable.isNowExistSTable(name, level)) {
                    errorHapphen = true;
                    showError(15, name);
                }
                STable.enterConst(name, level, value, address);
//                   address+=addrIncrement;             //登录符号表后地址加1指向下一个
                terPtr++;
            }
        } else {
            errorHapphen = true;
            showError(3, "");
        }
    } else {
        errorHapphen = true;
        showError(1, "");
    }
}

void MpgAnalysis::vardecl() {
    std::string name;
    int value;
    if (rv[terPtr].id == VAR) {
        terPtr++;
        if (rv[terPtr].id == SYM) {
            name = rv[terPtr].value;
            if (STable.isNowExistSTable(name, level)) {
                errorHapphen = true;
                showError(15, name);
            }
            STable.enterVar(name, level, address);
            address++;
            terPtr++;
            while (rv[terPtr].id == COMMA) {
                terPtr++;
                if (rv[terPtr].id == SYM) {
                    name = rv[terPtr].value;
                    if (STable.isNowExistSTable(name, level)) {
                        errorHapphen = true;
                        showError(15, name);
                    }
                    STable.enterVar(name, level, address);
                    address++;     //地址加1登录符号表
                    terPtr++;
                } else {
                    errorHapphen = true;
                    showError(1, "");
                    return;
                }
            }
            if (rv[terPtr].id != SEMIC) {
                errorHapphen = true;
                showError(0, "");
            } else {
                terPtr++;
            }
        } else {
            errorHapphen = true;
            showError(1, "");
        }

    } else {
        errorHapphen = true;
        showError(-1, "");
    }
}

void MpgAnalysis::proc() {
    if (rv[terPtr].id == PROC) {
        terPtr++;
        //id();
        int count = 0;//用来记录proc中形参的个数
        int propos;// 记录本proc在符号表中的位置
        if (rv[terPtr].id == SYM) {
            std::string name = rv[terPtr].value;
            if (STable.isNowExistSTable(name, level)) {
                errorHapphen = true;
                showError(15, name);
            }
            propos = STable.tablePtr;
            STable.enterProc(rv[terPtr].value, level, address);
            level++;                //level值加一，因为其后的所有定义均在该新的proc中完成
            terPtr++;
            if (rv[terPtr].id == LBR) {
                terPtr++;
                //id();
                if (rv[terPtr].id == SYM) {
                    STable.enterVar(rv[terPtr].value, level, 3 + count);      //3+count+1为形参在存储空间中的位置
                    count++;
                    STable.table[propos].size = count;        //用本过程在符号表中的size域记录形参的个数
                    terPtr++;
                    while (rv[terPtr].id == COMMA) {
                        terPtr++;
                        if (rv[terPtr].id == SYM) {
                            STable.enterVar(rv[terPtr].value, level, 3 + count);      //3+count+1为形参在存储空间中的位置
                            count++;
                            STable.table[propos].size = count;        //用本过程在符号表中的size域记录形参的个数
                            terPtr++;
                        } else {
                            errorHapphen = true;
                            showError(1, "");
                            return;
                        }
                    }
                }
                if (rv[terPtr].id == RBR) {
                    terPtr++;
                    if (rv[terPtr].id != SEMIC) {
                        errorHapphen = true;
                        showError(0, "");
                    } else {
                        terPtr++;
                        block();
                        while (rv[terPtr].id == SEMIC) {
                            terPtr++;
                            proc();
                        }
                    }
                } else {
                    errorHapphen = true;
                    showError(5, "");
                }

            } else {
                errorHapphen = true;
                showError(4, "");
            }
        } else {
            errorHapphen = true;
            showError(1, "");
        }

    } else {
        errorHapphen = true;
        showError(-1, "");
    }

}

void MpgAnalysis::body() {
    if (rv[terPtr].id == BEG) {
        terPtr++;
        statement();
        while (rv[terPtr].id == SEMIC) {
            terPtr++;
            statement();
        }
        if (rv[terPtr].id == END) {
            terPtr++;
        } else {
            errorHapphen = true;
            showError(7, "");
        }
    } else {
        errorHapphen = true;
        showError(6, "");
    }
}

void MpgAnalysis::statement() {
    if (rv[terPtr].id == IF) {
        int cx1;
        terPtr++;
        lexp();
        if (rv[terPtr].id == THEN) {
            cx1 = Pcode.codePtr;             //用cx1记录jpc ，0，0（就是下面这一条语句产生的目标代码）在Pcode中的地址，用来一会回填
            Pcode.gen(AllPcode::JPC, 0, 0);  //产生条件转移指令，条件的bool值为0时跳转，跳转的目的地址暂时填为0
            terPtr++;
            statement();
            int cx2 = Pcode.codePtr;
            Pcode.gen(AllPcode::JMP, 0, 0);
            Pcode.pcodeArray[cx1].A = Pcode.codePtr;        //地址回填，将jpc，0，0中的A回填
            Pcode.pcodeArray[cx2].A = Pcode.codePtr;
            if (rv[terPtr].id == ELS) {
                terPtr++;
                statement();
                Pcode.pcodeArray[cx2].A = Pcode.codePtr;
            }
        } else {
            errorHapphen = true;
            showError(8, "");
        }
    } else if (rv[terPtr].id == WHI) {
        int cx1 = Pcode.codePtr;     //保存条件表达式在Pcode中的地址
        terPtr++;
        lexp();
        if (rv[terPtr].id == DO) {
            int cx2 = Pcode.codePtr;     //保存条件跳转指令的地址，在回填时使用，仍是条件不符合是跳转
            Pcode.gen(AllPcode::JPC, 0, 0);
            terPtr++;
            statement();
            Pcode.gen(AllPcode::JMP, 0, cx1);    //完成DO后的相关语句后，需要跳转至条件表达式处，检查是否符合条件，即是否继续循环
            Pcode.pcodeArray[cx2].A = Pcode.codePtr;        //回填条件转移指令
        } else {
            errorHapphen = true;
            showError(9, "");
        }
    } else if (rv[terPtr].id == CAL) {
        terPtr++;
        //id();
        int count = 0;//用来检验传入的参数和设定的参数是否相等
        TableRow tempRow;
        if (rv[terPtr].id == SYM) {
            if (STable.isPreExistSTable(rv[terPtr].value, level)) {        //符号表中存在该标识符
                tempRow = STable.table[STable.getNameRow(rv[terPtr].value)];    //获取该标识符所在行的所有信息，保存在tempRow中
                if (tempRow.type != STable.proc)
                    //判断该标识符类型是否为procedure，SymTable中procdure类型用proc变量来表示，
                    //if类型为proc
                {       //cal类型不一致的错误
                    errorHapphen = true;
                    showError(11, "");
                    return;
                }
            }       //if符号表中存在标识符
            else {           //cal 未定义变量的错误
                errorHapphen = true;
                showError(10, "");
                return;
            }
            terPtr++;
            if (rv[terPtr].id == LBR) {
                terPtr++;
                if (rv[terPtr].id == RBR) {
                    terPtr++;
                    Pcode.gen(AllPcode::CAL, level - tempRow.level,
                              tempRow.value);        //调用过程中的保存现场由解释程序完成，这里只产生目标代码,+3需详细说明
                } else {
                    exp();
                    count++;
                    while (rv[terPtr].id == COMMA) {
                        terPtr++;
                        exp();
                        count++;
                    }
                    if (count != tempRow.size) {
                        errorHapphen = true;
                        showError(16, tempRow.name);
                        return;
                    }
                    Pcode.gen(AllPcode::CAL, level - tempRow.level,
                              tempRow.value);        //调用过程中的保存现场由解释程序完成，这里只产生目标代码,+3需详细说明
                    if (rv[terPtr].id == RBR) {
                        terPtr++;
                    } else {
                        errorHapphen = true;
                        showError(5, "");
                    }
                }
            } else {
                errorHapphen = true;
                showError(4, "");
            }
        } else {
            errorHapphen = true;
            showError(1, "");
        }

    } else if (rv[terPtr].id == REA) {
        terPtr++;
        if (rv[terPtr].id == LBR) {
            terPtr++;
            //      id();
            if (rv[terPtr].id == SYM) {
                if (!STable.isPreExistSTable((rv[terPtr].value), level)) {      //首先判断在符号表中在本层或本层之前是否有此变量
                    errorHapphen = true;
                    showError(10, "");
                    return;

                }//if判断在符号表中是否有此变量
                else {           //sto未定义变量的错误
                    TableRow tempTable = STable.table[STable.getNameRow(rv[terPtr].value)];
                    if (tempTable.type == STable.var) {       //该标识符是否为变量类型
                        Pcode.gen(AllPcode::OPR, 0,
                                  16);         //OPR 0 16	从命令行读入一个输入置于栈顶
                        Pcode.gen(AllPcode::STO, level - tempTable.level,
                                  tempTable.address);  //STO L ，a 将数据栈栈顶的内容存入变量（相对地址为a，层次差为L）
                    }//if标识符是否为变量类型
                    else {       //sto类型不一致的错误
                        errorHapphen = true;
                        showError(12, "");
                        return;
                    }
                }
                terPtr++;
                while (rv[terPtr].id == COMMA) {
                    terPtr++;
                    if (rv[terPtr].id == SYM) {
                        if (!STable.isPreExistSTable((rv[terPtr].value), level)) {      //首先判断在符号表中是否有此变量
                            errorHapphen = true;
                            showError(10, "");
                            return;
                        }//if判断在符号表中是否有此变量
                        else {           //sto未定义变量的错误
                            TableRow tempTable = STable.table[STable.getNameRow(rv[terPtr].value)];
                            if (tempTable.type == STable.var) {       //该标识符是否为变量类型
                                Pcode.gen(AllPcode::OPR, 0,
                                          16);         //OPR 0 16	从命令行读入一个输入置于栈顶   //层差的含义所在？？直接用嵌套的层次数作为参数不可以吗？
                                Pcode.gen(AllPcode::STO, level - tempTable.level,
                                          tempTable.address);  //STO L ，a 将数据栈栈顶的内容存入变量（相对地址为a，层次差为L）
                            }//if标识符是否为变量类型
                            else {       //sto类型不一致的错误
                                errorHapphen = true;
                                showError(12, "");
                                return;
                            }
                        }
                        terPtr++;
                    } else {
                        errorHapphen = true;
                        showError(1, "");
                        return;
                    }
                }
                if (rv[terPtr].id == RBR) {
                    terPtr++;
                } else {
                    errorHapphen = true;
                    showError(25, "");
                }
            } else {
                errorHapphen = true;
                showError(26, "");
            }
        } else {
            errorHapphen = true;
            showError(4, "");
        }
    } else if (rv[terPtr].id == WRI) {
        terPtr++;
        if (rv[terPtr].id == LBR) {
            terPtr++;
            exp();
            Pcode.gen(AllPcode::OPR, 0, 14);         //输出栈顶的值到屏幕
            while (rv[terPtr].id == COMMA) {
                terPtr++;
                exp();
                Pcode.gen(AllPcode::OPR, 0, 14);         //输出栈顶的值到屏幕
            }

            Pcode.gen(AllPcode::OPR, 0, 15);         //输出换行
            if (rv[terPtr].id == RBR) {
                terPtr++;
            } else {
                errorHapphen = true;
                showError(5, "");
            }
        } else {
            errorHapphen = true;
            showError(4, "");
        }
    } else if (rv[terPtr].id == BEG) {//这里也没有terPtr++;          //body不生成目标代码
        body();
    } else if (rv[terPtr].id == SYM) {      //赋值语句
        std::string name = rv[terPtr].value;
        terPtr++;
        if (rv[terPtr].id == CEQU) {
            terPtr++;
            exp();
            if (!STable.isPreExistSTable(name, level)) {        //检查标识符是否在符号表中存在
                errorHapphen = true;
                showError(14, name);
            }//if判断在符号表中是否有此变量
            else {           //sto未定义变量的错误
                TableRow tempTable = STable.table[STable.getNameRow(name)];
                if (tempTable.type == STable.var) {           //检查标识符是否为变量类型
                    Pcode.gen(AllPcode::STO, level - tempTable.level,
                              tempTable.address);  //STO L ，a 将数据栈栈顶的内容存入变量
                }////检查标识符是否为变量类型
                else {       //类型不一致的错误
                    errorHapphen = true;
                    showError(13, name);
                }
            }
        } else {
            errorHapphen = true;
            showError(3, "");
        }
    } else {
        errorHapphen = true;
        showError(1, "");
    }
}

void MpgAnalysis::lexp() {
    if (rv[terPtr].id == ODD) {
        terPtr++;
        exp();
        Pcode.gen(AllPcode::OPR, 0, 6);  //OPR 0 6	栈顶元素的奇偶判断，结果值在栈顶
    } else {
        exp();
        int loperator = lop();        //返回值用来产生目标代码，如下
        exp();
        if (loperator == EQU) {
            Pcode.gen(AllPcode::OPR, 0, 8);      //OPR 0 8	次栈顶与栈顶是否相等，退两个栈元素，结果值进栈
        } else if (loperator == NEQE) {
            Pcode.gen(AllPcode::OPR, 0, 9);      //OPR 0 9	次栈顶与栈顶是否不等，退两个栈元素，结果值进栈
        } else if (loperator == LES) {
            Pcode.gen(AllPcode::OPR, 0, 10);     //OPR 0 10	次栈顶是否小于栈顶，退两个栈元素，结果值进栈
        } else if (loperator == LESE) {
            Pcode.gen(AllPcode::OPR, 0, 13);     // OPR 0 13	次栈顶是否小于等于栈顶，退两个栈元素，结果值进栈
        } else if (loperator == LAR) {
            Pcode.gen(AllPcode::OPR, 0, 12);     //OPR 0 12	次栈顶是否大于栈顶，退两个栈元素，结果值进栈
        } else if (loperator == LARE) {
            Pcode.gen(AllPcode::OPR, 0, 11);     //OPR 0 11	次栈顶是否大于等于栈顶，退两个栈元素，结果值进栈
        }
    }
}

void MpgAnalysis::exp() {
    int tempId = rv[terPtr].id;
    if (rv[terPtr].id == ADD) {
        terPtr++;
    } else if (rv[terPtr].id == SUB) {
        terPtr++;
    }
    term();
    if (tempId == SUB) {
        Pcode.gen(AllPcode::OPR, 0, 1);      //  OPR 0 1	栈顶元素取反
    }
    while (rv[terPtr].id == ADD || rv[terPtr].id == SUB) {
        tempId = rv[terPtr].id;
        terPtr++;
        term();
        if (tempId == ADD) {
            Pcode.gen(AllPcode::OPR, 0, 2);       //OPR 0 2	次栈顶与栈顶相加，退两个栈元素，结果值进栈
        } else if (tempId == SUB) {
            Pcode.gen(AllPcode::OPR, 0, 3);      //OPR 0 3	次栈顶减去栈顶，退两个栈元素，结果值进栈
        }
    }
}

void MpgAnalysis::term() {
    factor();
    while (rv[terPtr].id == MUL || rv[terPtr].id == DIV) {
        int tempId = rv[terPtr].id;
        terPtr++;
        factor();
        if (tempId == MUL) {
            Pcode.gen(AllPcode::OPR, 0, 4);       //OPR 0 4	次栈顶乘以栈顶，退两个栈元素，结果值进栈
        } else if (tempId == DIV) {
            Pcode.gen(AllPcode::OPR, 0, 5);      // OPR 0 5	次栈顶除以栈顶，退两个栈元素，结果值进栈
        }
    }
}

void MpgAnalysis::factor() {
    if (rv[terPtr].id == CONST) {
        Pcode.gen(AllPcode::LIT, 0, std::stoi(rv[terPtr].value));    //是个数字,  LIT 0 a 取常量a放入数据栈栈顶
        terPtr++;
    } else if (rv[terPtr].id == LBR) {
        terPtr++;
        exp();
        if (rv[terPtr].id == RBR) {
            terPtr++;
        } else {
            errorHapphen = true;
            showError(5, "");
        }
    } else if (rv[terPtr].id == SYM) {
        std::string name = rv[terPtr].value;
        if (!STable.isPreExistSTable(name, level)) {     //判断标识符在符号表中是否存在
            errorHapphen = true;
            showError(10, "");
            return;
        }//if判断在符号表中是否有此变量
        else {           //未定义变量的错误
            TableRow tempRow = STable.table[STable.getNameRow(name)];
            if (tempRow.type == STable.var) { //标识符是变量类型
                Pcode.gen(AllPcode::LOD, level - tempRow.level,
                          tempRow.address);    //变量，LOD L  取变量（相对地址为a，层差为L）放到数据栈的栈顶
            } else if (tempRow.type == STable.myconst) {
                Pcode.gen(AllPcode::LIT, 0, tempRow.value);         //常量，LIT 0 a 取常量a放入数据栈栈顶
            } else {       //类型不一致的错误
                errorHapphen = true;
                showError(12, "");
                return;
            }
        }
        terPtr++;
    } else {
        errorHapphen = true;
        showError(1, "");
    }
}

int MpgAnalysis::lop() {
    std::string loperator;
    if (rv[terPtr].id == EQU) {
        terPtr++;
        return EQU;
    } else if (rv[terPtr].id == NEQE) {
        terPtr++;
        return NEQE;
    } else if (rv[terPtr].id == LES) {
        terPtr++;
        return LES;
    } else if (rv[terPtr].id == LESE) {
        terPtr++;
        return LESE;
    } else if (rv[terPtr].id == LAR) {
        terPtr++;
        return LAR;
    } else if (rv[terPtr].id == LARE) {
        terPtr++;
        return LARE;
    }
    return -1;
}
