#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>
#include <unordered_map>
using namespace std;


class SymbolTable {
public:
  
    void declareVariable(const string &name, const string &type) {
        if (symbolTable.find(name) != symbolTable.end()) {
            throw runtime_error("Semantic error: Variable '" + name + "' is already declared.");
        }
        symbolTable[name] = type;
    }

    string getVariableType(const string &name) {
        if (symbolTable.find(name) == symbolTable.end()) {
            throw runtime_error("Semantic error: Variable '" + name + "' is not declared.");
        }
        return symbolTable[name];
    }

    vector<string> getAllVariables() const {
        vector<string> variables;
        for (const auto &entry : symbolTable) {
            variables.push_back(entry.first);
        }
        return variables;
    }

private:
    unordered_map<string, string> symbolTable;  
};

class IntermediateCodeGnerator {
public:
    vector<string> instructions;
    int tempCount = 0;

    string newTemp() {
        return "t" + to_string(tempCount++);
    }
    vector<string> getInstructions() const {
        return instructions; 
    }


    void addInstruction(const string &instr) {
        instructions.push_back(instr);
    }

    void printInstructions() {
        for (const auto &instr : instructions) {
            cout << instr << endl;
        }
    }
};

class AssemblyCodeGenerator {
public:
    void generateAssembly(const vector<string>& intermediateCode) {
        for (const auto& instr : intermediateCode) {
            if (instr.substr(0, 6) == "DECLARE") {
                string var = instr.substr(8);
                cout << "PUSH R0        ; Reserve space for " << var << endl;
            }
            else if (instr.substr(0, 6) == "ASSIGN") {
                size_t pos = instr.find("=");
                string var = instr.substr(7, pos - 7);
                string temp = instr.substr(pos + 2);
                cout << "MOV " << var << ", " << temp << "   ; Assign " << temp << " to " << var << endl;
            }
            else if (instr == "Executing loop while loop.") {
                cout << "Start of while loop condition check" << endl;
            }
            else if (instr == "IF condition is met, execute block.") {
                cout << "Start of if block execution" << endl;
            }
            else if (instr == "Return expression value.") {
                cout << " Return from function" << endl;
            }
            else {
                cout << "; " << instr << " ; Unknown instruction" << endl;
            }
        }
    }
};





enum TokenType{
    T_INT,T_ID,T_NUM,T_IF,T_ELSE,T_RETURN,T_ASSIGN,T_PLUS,T_MINUS,T_MUL,
    T_DIV,T_LPAREN,T_RPAREN,T_LBRACE,T_RBRACE,T_SEMICOLON,T_GT,T_EOF,
    T_FLOAT,T_WHILE
};
struct Token{
    TokenType type;
    string value;
    int line;
};
class Lexer{
    private:
    string src;
    size_t pos;
    int line;
    public:
    Lexer(const string &src){
    this->src=src;
    this->pos=0;
    this->line = 1;
}
string consumeNumber(){
    size_t start=pos;
    while(pos<src.size() && isdigit(src[pos]))pos++;
    return src.substr(start,pos-start);
}
string consueWord(){
    size_t start=pos;
    while(pos<src.size() && isalnum(src[pos]))pos++;
    return src.substr(start,pos-start);
    }
    vector<Token> tokenizer(SymbolTable &symbolTable){
    vector<Token>tokens;
    while(pos<src.size()){
    char current=src[pos];
if (isspace(current)) {
    if (current == '\n') {
        line++;
    }
    pos++;
    continue;
}

// comments ignore
if(src.substr(pos,2)=="//"){
    while(pos<src.size() && src[pos]!='\n')
    {
        pos++;  
    }
    continue;  
}

if(isdigit(current)){     
    tokens.push_back(Token{T_NUM, consumeNumber(), line});
continue;
}

if(isalpha(current)){
    string word=consueWord();
    if(word == "int") tokens.push_back(Token{T_INT,word,line});
    else if (word == "agar") tokens.push_back(Token{T_IF,word,line});
    else if (word == "else") tokens.push_back(Token{T_ELSE,word,line});
    else if (word == "while") tokens.push_back(Token{T_WHILE,word,line}); // for while loop
    else if (word == "return")

    tokens.push_back(Token{T_RETURN,word,line});

    else tokens.push_back(Token{T_ID,word,line});
continue;
}
switch ((current))
{
    case '=': tokens.push_back(Token{T_ASSIGN,"=",line});break;
    case '+': tokens.push_back(Token{T_PLUS,"+",line});break;
    case '-': tokens.push_back(Token{T_MINUS,"-",line});break;
    case '*': tokens.push_back(Token{T_MUL,"*",line});break;
    case '/': tokens.push_back(Token{T_DIV,"/",line});break;
    case '(': tokens.push_back(Token{T_LPAREN,"(",line});break;
    case ')': tokens.push_back(Token{T_RPAREN,")",line});break;
    case '{': tokens.push_back(Token{T_LBRACE,"{",line});break;
    case '}': tokens.push_back(Token{T_RBRACE,"}",line});break;
    case ';': tokens.push_back(Token{T_SEMICOLON,";",line});break;
    case '>': tokens.push_back(Token{T_GT,">",line});break;
    default:
    cout<< "unexpected character: " << current <<" " <<"at line: "<<line<< endl;exit(1);
    }
    pos++;
    }
    tokens.push_back(Token{T_EOF,"",line});
    return tokens;
}
};
class Parser {
private:
    vector<Token> tokens;
    size_t pos;
    IntermediateCodeGnerator &icg; 
    SymbolTable &symbolTable; 
public:
    Parser(const vector<Token> &tokens,IntermediateCodeGnerator &icg, SymbolTable &symbolTable)
        : tokens(tokens), pos(0), icg(icg), symbolTable(symbolTable){
        this->tokens = tokens;
        this->pos = 0;
        this->icg = icg;  
    }

void parseProgram(){

while(tokens[pos].type != T_EOF){
    parseStatement();
    }
    cout<< "parsing completed successfully !!! NO syntax error"<<endl;
    }
    void parseStatement(){
    if(tokens[pos].type == T_INT){
    parseDeclaration();
    }
    else if(tokens[pos].type == T_ID){
    parseAssignment();
    }
    else if(tokens[pos].type == T_IF){
    parseifStatement();
    }
    else if(tokens[pos].type == T_WHILE){  
    parseWhileStatement();
    }
    else if(tokens[pos].type == T_RETURN){
    parseReturnStatement();
    }
    else if(tokens[pos].type == T_LBRACE){
    parseBlock();
}

else{{
cout << "SYNTAX ERROR: unexpected token at line " << tokens[pos].line << endl;
exit(1); 
}}
}
void parseBlock(){
    expect(T_LBRACE);
    while(tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF){
    parseStatement();
    }
    expect(T_RBRACE);
}

void parseDeclaration() {
    expect(T_INT);
    string varName = tokens[pos].value; 
    expect(T_ID);  
    expect(T_SEMICOLON);

    try {
        symbolTable.declareVariable(varName, "int");  
    } catch (runtime_error &e) {
        cout << e.what() << " at line " << tokens[pos - 1].line << endl;
        exit(1);
    }

    icg.addInstruction("DECLARE " + varName);
}

void parseAssignment() {
    string varName = tokens[pos].value;  
    expect(T_ID);

    try {
        symbolTable.getVariableType(varName);  
    } catch (runtime_error &e) {
        cout << e.what() << " at line " << tokens[pos - 1].line << endl;
        exit(1);
    }

    expect(T_ASSIGN);  
        parseExpression(); 
    expect(T_SEMICOLON);

    string temp = icg.newTemp();  
    icg.addInstruction("ASSIGN " + varName + " = " + temp);
}


void parseifStatement(){
    expect(T_IF); 
    expect(T_LPAREN);
    parseExpression();
    expect(T_RPAREN);
    parseStatement();
    if (tokens[pos].type == T_ELSE) {
        expect(T_ELSE);  
        parseStatement(); 
    }
    icg.addInstruction("IF condition is encountered.");
}

// while statement
void parseWhileStatement() {    
    expect(T_WHILE);           
    expect(T_LPAREN);            
    parseExpression();         
    expect(T_RPAREN);          
    parseStatement();          
    icg.addInstruction("While loop condition checked, executing loop.");  
}
void parseReturnStatement(){
    expect(T_RETURN);
    parseExpression();
    expect(T_SEMICOLON);
    icg.addInstruction("Return expression value.");
}
    void parseExpression(){
    parseTerm();
    while(tokens[pos].type==T_PLUS || tokens[pos].type==T_MINUS){
    pos++;
    parseTerm();
    }
    if(tokens[pos].type == T_GT){
    pos++;
    parseExpression();
}
}
void parseTerm(){
    parseFactor();
    while(tokens[pos].type==T_MUL || tokens[pos].type==T_DIV){
    pos++;
    parseFactor();
}
}
void parseFactor(){
 if(tokens[pos].type == T_ID || tokens[pos].type == T_NUM) {
        pos++;
    }
    else if(tokens[pos].type == T_LPAREN){      
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
    } 
    else{
        cout<<"syntax error:unexpected token "<<tokens[pos].value<<" at line "<<tokens[pos].line<<endl;
        exit(1);
}
}

void expect(TokenType type){
    if(tokens[pos].type == type){
    pos++;
    }
    else{
    cout<<"syntax error"<<type <<"but found"<<tokens[pos].value<<endl;
    exit(1);
    }
    }
};





int main(int argc, char* argv[])
{
    AssemblyCodeGenerator assemblyCodeGen;
    string line;
    ifstream file(argv[1]);
     if(!file.is_open()){
        cout<<"File not found"<<endl;
    }
    SymbolTable symbolTable;
    IntermediateCodeGnerator icg;
    string programContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    Lexer lexer(programContent);
    vector<Token> tokens = lexer.tokenizer(symbolTable);
    Parser parser(tokens, icg, symbolTable);
    parser.parseProgram();  

      cout << "\nINTERMEDIATE CODE: " << endl;
    icg.printInstructions();  
        cout << "\nASSEMBLY CODE: " << endl;
    assemblyCodeGen.generateAssembly(icg.getInstructions()); 
     cout << "\nSYMBOL TABLE: " << endl;
    for (const auto &var : symbolTable.getAllVariables()) {
        cout << var << ": " << symbolTable.getVariableType(var) << endl;
    }

    file.close(); 
    return 0;
}
