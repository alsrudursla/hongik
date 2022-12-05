#include <iostream>
#include <stack>
#include "post.h"
using namespace std;
bool Token::operator==(char b) { return len == 1 && str[0] == b; }
bool Token::operator!=(char b) { return len != 1 || str[0] != b; }
Token::Token() {}
Token::Token(char c) : len(1), type(c)
{
	str = new char[1]; str[0] = c;
} // default type = c itself
Token::Token(char c, char c2, int ty) : len(2), type(ty)
{
	str = new char[2]; str[0] = c; str[1] = c2;
}
Token::Token(char* arr, int l, int ty = ID) : len(l), type(ty) {
	str = new char[len + 1];
	for (int i = 0; i < len; i++) str[i] = arr[i];
	str[len] = '\0';
	if (type == NUM) {
		ival = arr[0] - '0';
		for (int i = 1; i < len; i++) ival = ival * 10 + arr[i] - '0';
	}
	else if (type == ID) ival = 0;
	else throw "must be ID or NUM";
}
bool Token::IsOperand() { return type == ID || type == NUM; }
ostream& operator<<(ostream& os, Token t) {
	if (t.type == UMINUS) os << "-u";
	else if (t.type == NUM) os << t.ival;
	else for (int i = 0; i < t.len; i++) os << t.str[i];
	os << " ";
	return os;
}
bool GetID(Expression& e, Token& tok)
{
	char arr[MAXLEN]; int idlen = 0;
	char c = e.str[e.pos];
	if (!(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')) return false;
	arr[idlen++] = c;
	e.pos++;
	while ((c = e.str[e.pos]) >= 'a' && c <= 'z'
		|| c >= 'A' && c <= 'Z'
		|| c >= '0' && c <= '9')
	{
		arr[idlen++] = c; e.pos++;
	}
	arr[idlen] = '\0';
	tok = Token(arr, idlen, ID); // return an ID
	return true;
}
bool GetInt(Expression& e, Token& tok)
{
	char arr[MAXLEN]; int idlen = 0;
	char c = e.str[e.pos];
	if (!(c >='0'&&c<='9')) return false;
	arr[idlen++] = c;
	e.pos++;
	while ((c = e.str[e.pos]) >= '0' && c <= '9')
	{
		arr[idlen++] = c; e.pos++;
	}
	arr[idlen] = '\0';
	tok = Token(arr, idlen, NUM); 
	return true;
}
void SkipBlanks(Expression& e)
{
	char c;
	while (e.pos < e.len && ((c = e.str[e.pos]) == ' ' || c == '\t'))
		e.pos++;
}
bool TwoCharOp(Expression& e, Token& tok)
{ //7가지 두글자 토큰들 // <= >= == != && || -u을 처리
	char c = e.str[e.pos]; char c2 = e.str[e.pos + 1];
	int op; // LE GE EQ NE AND OR UMINUS
	if (c == '<' && c2 == '=') op = LE;
	else if (c == '>' && c2 == '=') op = GE;  // 코드입력: 각 두 글자 토큰에 대해 알맞은 type값 op에 저장
	else if (c == '=' && c2 == '=') op = EQ;
	else if (c == '!' && c2 == '=') op = NE;
	else if (c == '&' && c2 == '&') op = AND;
	else if (c == '|' && c2 == '|') op = OR;
	else if (c == '-' && c2 == 'u') op = UMINUS;
	else
		return false; // 맞는 두 글자 토큰이 아니면 false를 return한다.
	tok = Token(c, c2, op); e.pos += 2;
	return true;
}
bool OneCharOp(Expression& e, Token& tok)
{
	char c = e.str[e.pos];
	if (c == '-' || c == '!' || c == '*' || c == '/' || c == '%' ||
		c == '+' || c == '<' || c == '>' || c == '(' || c == ')' || c == '=')
	{
		tok = Token(c); e.pos++; return true;
	}
	return false;
}
Token NextToken(Expression& e, bool INFIX = true)
{
	static bool oprrFound = true; // 종전에 연산자 발견되었다고 가정.
	Token tok;
	SkipBlanks(e); // skip blanks if any
	if (e.pos == e.len) // No more token left in this expression
	{
		if (INFIX) oprrFound = true; return Token('#');
	}
	if (GetID(e, tok) || GetInt(e, tok))
	{
		if (INFIX) oprrFound = false; return tok;
	}
	if (TwoCharOp(e, tok) || OneCharOp(e, tok)) {
		if (tok.type == '-' && INFIX && oprrFound) // operator 후 -발견
			tok = Token('-', 'u', UMINUS); // unary minus(-u)로 바꾸시오
		if (INFIX) oprrFound = true; return tok;
	}
	throw "Illegal Character Found";
}
int icp(Token& t) { // in-coming priority
	int ty = t.type;
	/* ty가 '('면 0, UMINUS나 '!'면 1,
	 '*'나 '/'나 '%'면 2, ‘+’나 '-'면 3,'<'나 '>'나 LE나 GE면 4,
	 EQ나 NE면 5, AND면 6, OR이면 7,'='이면 8, '#'면 9를 return한다.*/
	switch (ty)
	{
	case'(': return 0; break;
	case'!': return 1; break;
	case UMINUS: return 1; break;
	case '*': return 2; break;
	case '/': return 2; break;
	case '%': return 2; break;
	case '+': return 3; break;
	case '-': return 3; break;
	case '<': return 4; break;
	case '>': return 4; break;
	case LE: return 4; break;
	case GE: return 4; break;
	case EQ: return 5; break;
	case NE: return 5; break;
	case AND: return 6; break;
	case OR: return 7; break;
	case '=': return 8; break;
	case '#': return 9; break;
	}
}
int isp(Token& t) {
	//in-stack priority
   //코드 입력
	int ty = t.type;
	switch (ty)
	{
	case'(': return 9; break;
	case'!': return 1; break;
	case UMINUS: return 1; break;
	case '*': return 2; break;
	case '/': return 2; break;
	case '%': return 2; break;
	case '+': return 3; break;
	case '-': return 3; break;
	case '<': return 4; break;
	case '>': return 4; break;
	case LE: return 4; break;
	case GE: return 4; break;
	case EQ: return 5; break;
	case NE: return 5; break;
	case AND: return 6; break;
	case OR: return 7; break;
	case '=': return 8; break;
	case '#': return 10; break;
	}
}
void Postfix(Expression e) {
	// STL stack이용하고, 교재의 마지막 for문을 다음 같이 바꾼다 */
	// while (stack.top()!='#') { cout << stack.top(); stack.pop(); }
	// stack.pop();
	stack<Token> stack; // 스택의 초기화
	stack.push('#');
	for (Token x = NextToken(e); x != '#'; x = NextToken(e))
		if (x.IsOperand()) cout << x;
		else if (x == ')')
		{// unstack until ‘(‘
			for (; stack.top() != '('; stack.pop())
			cout << stack.top();
			stack.pop(); // unstack ‘(‘
		}
		else { // x is an operator
				for (; isp(stack.top()) <= icp(x); stack.pop()) {
					if (x == ' = ' && stack.top() == ' = ') break;
						cout << stack.top();
				}
				stack.push(x);
		}
	// end of expression; empty the stack
	for (; stack.top()!='#'; cout << stack.top(), stack.pop());
	stack.pop();
	cout << endl;

}