#include <iostream>
#include <fstream>
using namespace std;

#define MAX_TOKENS   8192
#define MAX_IDENT    64

// Tokens names: dictionary of all legal symbols/words in our mini-language
enum TokenType { 
	TOKEN_A, 
	TOKEN_B, 
	TOKEN_C, 
	ENDFILE, 
	TOK_UNKNOWN 
};

// declare a token structure
struct Token {
	TokenType tokenType;
	string lexeme;
	int line;
	int column;
};

/******************************************************/
/*   Helping function to display the token as a string */
const char* tokenTypeName(TokenType t) {
	switch (t) {
		case TOKEN_A: return "TOKEN_A"; break;
		case TOKEN_B: return "TOKEN_B"; break;
		case TOKEN_C: return "TOKEN_C"; break;
		case ENDFILE: return "EOF"; break;
	}
	return "Unknown";
}

// Tokenizer structure 
struct Tokenizer {
	FILE *infp;
	int line;
	int column;
	int cur;
	int hasCur;
};

// --- Basic char reading with position tracking ---

void tkInit(Tokenizer *tk, FILE * infp) {
	tk->infp = infp;
	tk->line = 1;
	tk->column = 1;
	tk->cur = 0;
	tk->hasCur = 0;
}

/********************************************/
/* errMsg - function to display the error message with the line number of the error detected. */
void errMsg (Tokenizer *tk, string msg) {
	cout << "Tokenizer error at line " << tk->line << " column " << tk->column << ": "<< msg << endl;
	exit(1);
	
}

int getChar(Tokenizer *tk) {
	int c = fgetc(tk->infp);
	return c;
}

int peekChar(Tokenizer *tk) {
	if (!tk->hasCur) {
		tk->cur = getChar(tk);
		tk->hasCur = 1; // peeked
	}
	return tk->cur;
}

void consumeChar(Tokenizer *tk) {
	if (!tk->hasCur) { // char was peeked but not consumed
		tk->cur = getChar(tk);
		tk->hasCur = 1; // peeked
	}
	if (tk->cur == '\n') {
		tk->line++;
		tk->column = 1;
	} else if (tk->cur != EOF) {
		tk->column++;
	}
	tk->hasCur = 0; // so next peekChar will read fresh new char
}

/*****************************************************/
/* skipWhiteSpaces - a function to call peekChar until it
 returns a non-whitespace character */
void skipWhiteSpaces(Tokenizer *tk) {
	while (1) {
		int c = peekChar(tk);
		if (c == EOF) return;
		if (isspace(c)) { consumeChar(tk); continue; }
		return;
	}
}

/*******************************************************************
LookupKeyword - a simple lookup code for keywords in the language: */
TokenType lookupKeywords (int c) {
	// return keyword token
	TokenType token = TOK_UNKNOWN;
	switch (c) {
		case 'a': token = TOKEN_A; break;
		case 'b': token = TOKEN_B; break;
		case 'c': token = TOKEN_C; break;
	}
	return token;
}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic 
 expressions */
int tokenize(Tokenizer *tk, Token *tokens, int maxTokens) {
	int count = 0;
	while (1) {
		skipWhiteSpaces(tk);

		if (count >= maxTokens) errMsg(tk, "too many tokens");

		int c = peekChar(tk);
		int ln = tk->line;
		int col = tk->column;
		
		if (c == EOF) {
			tokens[count].tokenType = ENDFILE;
			tokens[count].lexeme = "\0";
			tokens[count].line = ln;
			tokens[count].column = col;
			return count+1;
		}

		// Single symbol: operator or puctuation
		tokens[count].tokenType = lookupKeywords(c);
		tokens[count].lexeme += char(c);
		tokens[count].line = ln;
		tokens[count].column = col;
		if (tokens[count].tokenType == TOK_UNKNOWN) errMsg(tk, "unknown token");
		count++;
		consumeChar(tk);
	}
}