//
// Created by Chang Gu on 24-5-31.
//
#include <fstream>
#include <iostream>
#include <cstring>
#include "lex_analysis.hpp"

LexAnalysis::LexAnalysis(std::string _filename) : filename(_filename) {
    for (int i = 0; i < symLength; i++) {
        symTable[i] = "";
    }
    for (int j = 0; j < conLength; j++) {
        constTable[j] = "";
    }
}

void LexAnalysis::bAnalysis() {
    preManage();
    RValue temp;
    std::string str = "lex.txt";
    std::ofstream myout(str);
    if (!myout.is_open()) {
        std::cerr << "Failed to open file: " << str << std::endl;
        exit(-1);
    }

    while (searchPtr < buffer.length() && !errorHappen) {
        temp = analysis();

        std::string tempId = std::to_string(temp.id);
        std::string tempLine = std::to_string(temp.line);
        std::string bname = temp.value;

        myout << tempId << ' ' << bname << ' ' << tempLine << '\n';
    }

    myout.close();

    if (errorHappen) {
        showError();
    }
}

void LexAnalysis::preManage() {
    std::ifstream infile("../testPL0/" + filename);
    if (!infile.is_open()) {
        std::cerr << "Error: File not found or could not be opened" << std::endl;
    }
    std::string temp;
    while (getline(infile, temp)) {
        buffer = buffer + temp + "\n";
    }
    infile.close();
}

void LexAnalysis::getChar() {
    if (searchPtr < buffer.length()) {
        ch = buffer[searchPtr];
        searchPtr++;
    }
}

void LexAnalysis::getBC() {
    while ((ch == ' ' || ch == '	' || ch == '\n') && (searchPtr < buffer.length())) {
        if (ch == '\n') {
            line++;
        }
        getChar();
    }
}

bool LexAnalysis::isLetter() {
    return isalpha(ch);
}

bool LexAnalysis::isDigit() {
    return isnumber(ch);
}

void LexAnalysis::concat() {
    strToken = strToken + ch;
}

void LexAnalysis::retract() {
    searchPtr--;
    ch = ' ';
}

int LexAnalysis::reserve() {
    for (int i = 0; i < keyWord.size(); i++) {
        if (keyWord[i] == strToken) {
            return i + 1;
        }
    }
    return 0;
}

int LexAnalysis::insertId() {
    for (int i = 0; i < symLength; i++) {
        if (symTable[i].empty()) {
            symTable[i] = strToken;
            return i;
        }
    }
    return -1;//表示symTable已经满了
}

int LexAnalysis::insertConst() {
    for (int i = 0; i < conLength; i++) {
        if (constTable[i].empty()) {
            constTable[i] = strToken;
            return i;
        }
    }
    return -1;//constTable已经满了
}

RValue LexAnalysis::analysis() {
    int code, value;
    strToken = "";
    getChar();
    getBC();//经过此步，如果ch=='\n'，则肯定是到达了文件末尾
    if (ch == '\n') {
        rv.id = -1;
        rv.value = "-1";
        rv.line = line;
        return rv;
    }
    if (isLetter()) {
        while ((isLetter() || isDigit())) {
            concat();
            getChar();
        }
        retract();
        code = reserve();
        if (code == 0) {
            value = insertId();
            rv.id = SYM;
            rv.value = symTable[value];
            rv.line = line;
            return rv;
        } else {
            rv.id = code;
            rv.value = "-";
            rv.line = line;
            return rv;
        }
    } else if (isDigit()) {
        while (isDigit()) {
            concat();
            getChar();
        }
        retract();
        value = insertConst();
        rv.id = CONST;
        rv.value = constTable[value];
        rv.line = line;
        return rv;
    } else if (ch == '=') {
        rv.id = EQU;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == '+') {
        rv.id = ADD;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == '-') {
        rv.id = SUB;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == '*') {
        rv.id = MUL;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == '/') {
        rv.id = DIV;
        rv.value = "/";
        rv.line = line;
        return rv;
    } else if (ch == '<') {
        getChar();
        if (ch == '=') {
            rv.id = LESE;
            rv.value = "-";
            rv.line = line;
            return rv;
        } else if (ch == '>') {
            rv.id = NEQE;
            rv.value = "-";
            rv.line = line;
            return rv;
        } else {
            retract();
            rv.id = LES;
            rv.value = "-";
            rv.line = line;
            return rv;
        }
    } else if (ch == '>') {
        getChar();
        if (ch == '=') {
            rv.id = LARE;
            rv.value = "-";
            rv.line = line;
            return rv;
        } else {
            retract();
            rv.id = LAR;
            rv.value = "-";
            rv.line = line;
            return rv;
        }
    } else if (ch == ',') {
        rv.id = COMMA;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == ';') {
        rv.id = SEMIC;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == '.') {
        rv.id = POI;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == '(') {
        rv.id = LBR;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == ')') {
        rv.id = RBR;
        rv.value = "-";
        rv.line = line;
        return rv;
    } else if (ch == ':') {
        getChar();
        if (ch == '=') {
            rv.id = CEQU;
            rv.value = "-";
            rv.line = line;
            return rv;
        } else {
            retract();
        }
    }
    errorHappen = true;
    return rv;

}

void LexAnalysis::showError() {
    std::cout << "\n";
    std::cout << "ERROR: cannot recognize the word in line " << line;
    std::cout << "\n";
}
