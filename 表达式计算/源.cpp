#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;
#define MAX_SIZE 100

class Expression // 0-9 + - * / % ^ F ( )   ��F(x)Ϊ������������ѧ������
{
public:
	Expression(string expression,double x);
	~Expression() {};
	double calculator();
private:
	string expression;
	double x;
	string op[MAX_SIZE];//������ջ
	string output[MAX_SIZE];//��׺���ʽ����
	double answer[MAX_SIZE] = { 0 };//������ջ
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
	bool xNegFlag = false; //xǰ���Ƿ���-�ż�����Ϊ��(-x)
	int lenNum = 0; //���ֵĳ��ȣ�λ����
	int lenAlpha = 0; //�����ĳ���
	for (int i = 0; i < expression.length(); i++)
	{
		if (isNum(expression[i]) || expression[i] == '.' 
			|| (i > 0 && expression[i-1] == '(' && expression[i] == '-'
				&& !(expression[i+1] == 'x' || expression[i+1] == 'X'))) //����(С�����߸���)
		{

			lenNum++;

			//˵��ǰ������ĸ�����������ĸ���Ͱ��������������output���飬����len��Ϊ0
			//���Ϊ0��˵��ǰ����Ѿ�������ĸ�ˣ��Ͳ��ô棬����һ����˼
			if (lenAlpha)
				op[++top] = expression.substr(i - lenAlpha, lenAlpha);
			lenAlpha = 0;
		}
		else if (int now = operatorType(expression[i])) //������
		{
			if (lenNum)
				output[pos++] = expression.substr(i - lenNum, lenNum);
			lenNum = 0;

			if (lenAlpha)
				op[++top] = expression.substr(i - lenAlpha, lenAlpha);
			lenAlpha = 0;

			if (expression[i] == '-' && i > 0 && expression[i - 1] == '(' 
				&& (expression[i + 1] == 'x' || expression[i + 1] == 'X')) //xǰ��ĸ���
			{
				xNegFlag = true;
				continue; //ֱ��continue����Ϊ-��������ĩβ����Ķ���else if���߼�
			}

			if (top > -1)
			{
				int opTop = operatorType(op[top][0]); //����Ǻ����������ţ����᷵��0
				if (isAlpha(op[top][0])) //����Ǻ��������ȼ��ᵽ���
					opTop = 3;
				while (opTop >= now) //������ջ��Ԫ�����ȼ����ڵ��ڵ�ǰ������
				{
					output[pos++] = op[top--];
					if (top == -1 || op[top][0] == '(') //ջ�ջ�����������������
						break;
					opTop = operatorType(op[top][0]);
					if (isAlpha(op[top][0]))
						opTop = 3;
				}
			}
			
			op[++top] = expression.substr(i, 1); //��ǰ��������ջ

		}
		else if (expression[i] == 'x' || expression[i] == 'X') //����x
		{
			if (lenNum)
				output[pos++] = expression.substr(i - lenNum, lenNum);
			lenNum = 0;

			if (lenAlpha)
				op[++top] = expression.substr(i - lenAlpha, lenAlpha);
			lenAlpha = 0;

			double d = x; //�������ֱ��x*=-1���޸�ԭ�������룬����Ū��d
			if (xNegFlag) //���xǰ���и���
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
		else if (isAlpha(expression[i])) //��ĸ������������sin�����ģ�
		{
			lenAlpha++;

			if (lenNum) //���ǰ�治�����ֵĻ��϶�֮ǰlen�Ѿ���Ϊ0�ˡ�
				output[pos++] = expression.substr(i - lenNum, lenNum);
			lenNum = 0;
		}
		else if (expression[i] == '(') //������ֱ����ջ
		{
			if (lenAlpha) //���ж��ַ�����Ϊ��������ߵ���Ч����ֻ���Ǻ���
				op[++top] = expression.substr(i - lenAlpha, lenAlpha);
			lenAlpha = 0;
			op[++top] = "(";
		}
		else if (expression[i] == ')') //���������ţ�ջ���Ԫ�����γ�ջ��������
		{
			if (lenNum) //���ж����֣���Ϊ��������ߵ���Ч����ֻ��������
				output[pos++] = expression.substr(i - lenNum, lenNum);
			lenNum = 0;
			while (op[top] != "(")
			{
				output[pos++] = op[top--];
			}
			--top; //�����ų�ջ
		}
		if (i == expression.length() - 1) //����ĩβ
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
		if (isNum(output[i][0]) || (output[i].length() > 1 && isNum(output[i][1]))) //���x�Ǹ����Ļ�Ҫ�ж���һ����
		{
			stringstream ss;
			ss << output[i];
			double d;
			ss >> d;
			answer[++top] = d;
		}
		else if (operatorType(output[i][0]))
		{
			char ch = output[i][0]; //�õ�������
			double a, b;
			b = answer[top--];
			a = answer[top--]; //������������ջ��ע��˳��
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
			string function = output[i]; //ȡ�ú�����
			double d = answer[top--]; //ȡ��һ��������
			if(function == "ln")
				answer[++top] = log(d);//��ѧ������log��eΪ��log10����10Ϊ��
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
	cout << "���ʽ�������,֧��0-9 + - * / % ^ F(x),F(x)Ϊ���õ���������(�������Ǻ���)" << endl;
	cout << "������һ������,����������Ϊx��ע���޿ո������ű���ΪӢ��,����Ҫ������������" << endl << endl;

	cout << "��������ʽ��";
	cin >> input;

	double x;
	cout << "������x��ֵ: ";
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
