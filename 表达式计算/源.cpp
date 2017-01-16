#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;
#define MAX_SIZE 100

class Expression // 0-9 + - * / % ^ F ( )   （F(x)为单参数常用数学函数）
{
public:
	Expression(string expression,double x);
	~Expression() {};
	double calculator();
private:
	string expression;
	double x;
	string op[MAX_SIZE];//操作符栈
	string output[MAX_SIZE];//后缀表达式数组
	double answer[MAX_SIZE] = { 0 };//运算结果栈
	int top = -1;
	int pos = 0;
	void change();
	int operatorType(char ch);
	bool isAlpha(char ch);
	bool isNum(char ch);
};

Expression::Expression(string expression, double x)
{
	this->expression = expression;
	this->x = x;
}

int Expression::operatorType(char ch)
{
	if (ch == '+' || ch == '-')
		return 1;
	if (ch == '*' || ch == '/' || ch == '%')
		return 2;
	if (ch == '^')
		return 3;
	return 0;
}

bool Expression::isAlpha(char ch)
{
	if (ch >= 65 && ch <= 90 || 
		ch >= 97 && ch <=122 )
		return true;
	return false;
}

bool Expression::isNum(char ch)
{
	if (ch >= 48 && ch <= 57)
		return true;
	return false;
}

void Expression::change()
{
	bool xNegFlag = false; //x前面是否有-号即输入为：(-x)
	int lenNum = 0; //数字的长度（位数）
	int lenAlpha = 0; //函数的长度
	for (int i = 0; i < expression.length(); i++)
	{
		if (isNum(expression[i]) || expression[i] == '.' 
			|| (i > 0 && expression[i-1] == '(' && expression[i] == '-'
				&& !(expression[i+1] == 'x' || expression[i+1] == 'X'))) //数字(小数或者负数)
		{

			lenNum++;

			//说明前面是字母，这个不是字母，就把完整函数名存进output数组，并把len设为0
			//如果为0则说明前面就已经不是字母了，就不用存，下面一个意思
			if (lenAlpha)
				op[++top] = expression.substr(i - lenAlpha, lenAlpha);
			lenAlpha = 0;
		}
		else if (int now = operatorType(expression[i])) //操作符
		{
			if (lenNum)
				output[pos++] = expression.substr(i - lenNum, lenNum);
			lenNum = 0;

			if (lenAlpha)
				op[++top] = expression.substr(i - lenAlpha, lenAlpha);
			lenAlpha = 0;

			if (expression[i] == '-' && i > 0 && expression[i - 1] == '(' 
				&& (expression[i + 1] == 'x' || expression[i + 1] == 'X')) //x前面的负号
			{
				xNegFlag = true;
				continue; //直接continue。因为-不可能是末尾，别的都是else if的逻辑
			}

			if (top > -1)
			{
				int opTop = operatorType(op[top][0]); //如果是函数或者括号，都会返回0
				if (isAlpha(op[top][0])) //如果是函数，优先级提到最大
					opTop = 4;
				while (opTop >= now) //操作符栈顶元素优先级大于等于当前操作符
				{
					output[pos++] = op[top--];
					if (top == -1 || op[top][0] == '(') //栈空或者遇到左括号跳出
						break;
					opTop = operatorType(op[top][0]);
					if (isAlpha(op[top][0]))
						opTop = 4;
				}
			}
			
			op[++top] = expression.substr(i, 1); //当前操作符入栈

		}
		else if (expression[i] == 'x' || expression[i] == 'X') //变量x
		{
			if (lenNum)
				output[pos++] = expression.substr(i - lenNum, lenNum);
			lenNum = 0;

			if (lenAlpha)
				op[++top] = expression.substr(i - lenAlpha, lenAlpha);
			lenAlpha = 0;

			double d = x; //下面如果直接x*=-1会修改原本的输入，所以弄个d
			if (xNegFlag) //如果x前面有负号
			{
				xNegFlag = false;
				d *= -1;
			}
			stringstream ss;
			ss << d;
			string str;
			ss >> str;
			
			output[pos++] = str;
		}
		else if (isAlpha(expression[i])) //字母（函数名比如sin这样的）
		{
			lenAlpha++;

			if (lenNum) //如果前面不是数字的话肯定之前len已经置为0了。
				output[pos++] = expression.substr(i - lenNum, lenNum);
			lenNum = 0;
		}
		else if (expression[i] == '(') //左括号直接入栈
		{
			if (lenAlpha) //先判定字符，因为右括号左边的有效内容只能是函数
				op[++top] = expression.substr(i - lenAlpha, lenAlpha);
			lenAlpha = 0;
			op[++top] = "(";
		}
		else if (expression[i] == ')') //遇到右括号，栈里的元素依次出栈到左括号
		{
			if (lenNum) //先判定数字，因为右括号左边的有效内容只能是数字
				output[pos++] = expression.substr(i - lenNum, lenNum);
			lenNum = 0;
			while (op[top] != "(")
			{
				output[pos++] = op[top--];
			}
			--top; //左括号出栈
		}
		if (i == expression.length() - 1) //到达末尾
		{
			if (lenNum)
				output[pos++] = expression.substr(i - lenNum + 1, lenNum);
			lenNum = 0;
		}
	}
	while (top != -1)
	{
		output[pos++] = op[top--];
	}
}

double Expression::calculator()
{
	this->change();
	for (int i = 0; i < pos; i++)
	{
		if (isNum(output[i][0]) || (output[i].length() > 1 && isNum(output[i][1]))) //如果x是负数的话要判断下一个数
		{
			stringstream ss;
			ss << output[i];
			double d;
			ss >> d;
			answer[++top] = d;
		}
		else if (operatorType(output[i][0]))
		{
			char ch = output[i][0]; //得到操作符
			double a, b;
			b = answer[top--];
			a = answer[top--]; //两个操作数出栈（注意顺序）
			switch (ch)
			{
			case '+':
				answer[++top] = a + b;
				break;
			case '-':
				answer[++top] = a - b;
				break;
			case '*':
				answer[++top] = a * b;
				break;
			case '/':
				answer[++top] = a / b;
				break;
			case '%':
				answer[++top] = fmod(a, b);
				break;
			case '^':
				answer[++top] = pow(a, b);
				break;
			default:
				cout << "calculator ERROR!" << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if (isAlpha(output[i][0]))
		{
			string function = output[i]; //取得函数名
			double d = answer[top--]; //取得一个操作数
			if(function == "ln")
				answer[++top] = log(d);//数学库里面log是e为底log10才是10为底
			else if(function == "log")
				answer[++top] = log10(d);
			else if(function == "ceil")
				answer[++top] = ceil(d);
			else if(function == "floor")
				answer[++top] = floor(d);
			else if(function == "sqrt")
				answer[++top] = sqrt(d);
			else if(function == "abs")
				answer[++top] = abs(d);
			else if (function == "sin")
				answer[++top] = sin(d);
			else if (function == "sinh")
				answer[++top] = sinh(d);
			else if (function == "asin")
				answer[++top] = asin(d);
			else if (function == "cos")
				answer[++top] = cos(d);
			else if (function == "cosh")
				answer[++top] = cosh(d);
			else if (function == "acos")
				answer[++top] = acos(d);
			else if (function == "tan")
				answer[++top] = tan(d);
			else if (function == "tanh")
				answer[++top] = tanh(d);
			else if (function == "atan")
				answer[++top] = atan(d);
			else
				answer[++top] = 0;
		}
	}
	return answer[top];
}

int main()
{
	string input = "(-x)";
	cout << "表达式计算程序,支持0-9 + - * / % ^ F(x),F(x)为常用单参数函数(比如三角函数)" << endl;
	cout << "可以有一个变量,变量名必须为x，注意无空格且括号必须为英文,负数要用括号括起来" << endl << endl;

	cout << "请输入表达式：";
	cin >> input;

	double x;
	cout << "请输入x的值: ";
	cin >> x;
	Expression exp(input, x);
	cout << exp.calculator() << endl;

// 	int count = 0;
// 	for (double i = -392 * 0.02; i < 408 * 0.02; i += 0.02) {
// 		Expression exp(input, i);
// 		cout << i << ":" << exp.calculator() << endl;
// 		count++;
// 	}
// 	cout << count << endl;
	return 0;
}
