#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "toyTokenizer.cpp"
using namespace std;

// =====================================================================
// PARSER  — recursive descent following the BNF
// =====================================================================
struct Parser {
    Token*    tokens;
    int       count;
    int       pos;
};

const Token* prCurrent(const Parser &p) { return &p.tokens[p.pos]; }
bool prCheck(const Parser &p, TokenType t) { return prCurrent(p)->tokenType == t; }

void prAdvance(Parser &p) {
    p.pos++;
}

bool prMatch(Parser &p, TokenType t) {
    if (prCheck(p, t)) { prAdvance(p); return true; }
    return false;
}

void prError(const Parser &p, string msg) {
    const Token* t = prCurrent(p);
	cerr << "Parser error at line " << t->line << ": column " << t->column << ": " << msg << " (got " << tokenTypeName(t->tokenType) << ")\n";
    exit(1);
}

const Token* prExpect(Parser &p, TokenType t, string what) {
    if (!prCheck(p, t)) {
		prError(p, "expected " + what + " (" + tokenTypeName(t) + ")");
    }
    const Token* tok = prCurrent(p);
    return tok;
}

// -------- Forward decl --------
void parseS(Parser &p);
void parseA(Parser &p);
void parseB(Parser &p);
void parseC(Parser &p);

// -------- <C> -> c --------
void parseC(Parser &p) {
    if (prCheck(p, TOKEN_C)) {
        prAdvance(p);
        return;
    }
    prError(p, "expected 'c'");
}

// -------- <B> -> b<B> | b --------
void parseB(Parser &p) {
    if (prCheck(p, TOKEN_B)) {
        prAdvance(p);
        if (prCheck(p, TOKEN_B)) {
            parseB(p);
        }
        return;
    }
    prError(p, "expected 'b'");
}

// -------- <A> -> a<A> | a --------
void parseA(Parser &p) {
    if (prCheck(p, TOKEN_A)) {
        prAdvance(p);
        if (prCheck(p, TOKEN_A)) {
            parseA(p);
        }
        return;
    }
    prError(p, "expected 'a'");
}

// -------- S -> <A><C><B> | <A> --------
void parseS(Parser &p) {
    parseA(p);
    if (prCheck(p, TOKEN_C)) {
        parseC(p);
        parseB(p);
    }
}

// -------- <program> -> parses the entire token stream as S --------
void parseProgram(Parser &p) {
	while (1) {
		parseS(p);
		if (prCheck(p, ENDFILE)) break;
	}
}

// =====================================================================
// MAIN
// =====================================================================
int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return 1;
    }
    // ---- Tokenize ----
    Tokenizer tk;
    tkInit(&tk, fp);

    Token tokens[MAX_TOKENS];
    int tokenCount = tokenize(&tk, tokens, MAX_TOKENS);

    fclose(fp);

	Parser parser;
	parser.tokens = tokens;
    parser.count  = tokenCount;
	parser.pos    = 0;

	parseProgram(parser);
	cout << "No errors found\n";

    return 0;
}