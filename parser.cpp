#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>

using namespace std;

class Node {
    string identifier, scope, type;
    int lineNo;
    Node* next;

public:
    Node() { next = NULL; }

    Node(string key, string value, string type, int lineNo) {
        this->identifier = key;
        this->scope = value;
        this->type = type;
        this->lineNo = lineNo;
        next = NULL;
    }

    void print() {
        cout << "Identifier's Name: " << identifier
             << "\nType: " << type
             << "\nScope: " << scope
             << "\nLine Number: " << lineNo << endl;
    }

    friend class SymbolTable;
};
const int MAX = 100;

class SymbolTable {
    Node* head[MAX];

public:
    SymbolTable() {
        for (int i = 0; i < MAX; i++) head[i] = NULL;
    }

    int hashf(string id); 
    bool insert(string id, string scope, string Type, int lineno);
    string find(string id);
    bool deleteRecord(string id);
    bool modify(string id, string scope, string Type, int lineno);

    void printTable() {
        for (int i = 0; i < MAX; i++) {
            Node* start = head[i];
            while (start != NULL) {
                start->print();  
                start = start->next;
            }
        }
    }
};


int SymbolTable::hashf(string id) {
    int asciiSum = 0;
    for (int i = 0; i < id.length(); i++) {
        asciiSum = asciiSum + id[i];
    }
    return (asciiSum % 100); 
}

bool SymbolTable::insert(string id, string scope, string Type, int lineno) {
    int index = hashf(id);
    Node* p = new Node(id, scope, Type, lineno);

    if (head[index] == NULL) {
        head[index] = p;
        return true;
    } else {
        Node* start = head[index];
        while (start->next != NULL) start = start->next;
        start->next = p;
        return true;
    }
}


string SymbolTable::find(string id) {
    int index = hashf(id);
    Node* start = head[index];

    while (start != NULL) {
        if (start->identifier == id) {
            start->print();
            return start->scope;
        }
        start = start->next;
    }

    return "-1"; 
}


bool SymbolTable::deleteRecord(string id) {
    int index = hashf(id);
    Node* tmp = head[index];
    Node* par = head[index];

    if (tmp == NULL) return false;

    if (tmp->identifier == id && tmp->next == NULL) {
        tmp->next = NULL;
        delete tmp;
        return true;
    }

    while (tmp->identifier != id && tmp->next != NULL) {
        par = tmp;
        tmp = tmp->next;
    }

    if (tmp->identifier == id && tmp->next != NULL) {
        par->next = tmp->next;
        tmp->next = NULL;
        delete tmp;
        return true;
    }

    par->next = NULL;
    tmp->next = NULL;
    delete tmp;
    return true;
}

bool SymbolTable::modify(string id, string s, string t, int l) {
    int index = hashf(id);
    Node* start = head[index];

    while (start != NULL) {
        if (start->identifier == id) {
            start->scope = s;
            start->type = t;
            start->lineNo = l;
            return true;
        }
        start = start->next;
    }

    return false;
}


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
class Parser{
    private:
    vector <Token>tokens;
    size_t pos;
    public:
    Parser(const vector<Token> &tokens){
    this->tokens=tokens;
    this->pos=0;
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
void parseDeclaration(){
    expect(T_INT);
    expect(T_ID);
    expect(T_SEMICOLON);
}
void parseAssignment(){
    expect(T_ID);
    expect(T_ASSIGN);
    parseExpression();
    expect(T_SEMICOLON);
}

void parseifStatement(){
    expect(T_ID);
    expect(T_LPAREN);
    parseExpression();
    expect(T_RPAREN);
    parseStatement();
    if(tokens[pos].type == T_ELSE){
    expect(T_ELSE);
    parseStatement();
    }
}
// while statement
void parseWhileStatement() {    
    expect(T_WHILE);           
    expect(T_LPAREN);            
    parseExpression();         
    expect(T_RPAREN);          
    parseStatement();            
}
void parseReturnStatement(){
    expect(T_RETURN);
    parseExpression();
    expect(T_SEMICOLON);
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
    if(tokens[pos].type==T_MUL || tokens[pos].type==T_ID){
    pos++;
    }else if(tokens[pos].type==T_LPAREN){
    expect(T_LPAREN);
    parseExpression();
    expect(T_RPAREN);
    }else{
    cout<<"syntax error"<<tokens[pos].value<<endl;
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
    string line;
    ifstream file(argv[1]);
     if(!file.is_open()){
        cout<<"File not found"<<endl;
    }
    SymbolTable symbolTable;
    while(getline(file,line)){ 
        Lexer lexer(line);
        vector<Token> tokens = lexer.tokenizer(symbolTable);
        Parser parser(tokens);
     
        parser.parseProgram();  

    
    
    return 0;
    }
        cout << "SYMBOL TABLE " << endl;
       symbolTable.printTable();
    file.close();
}
