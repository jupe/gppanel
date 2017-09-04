/**
 *  @file   gpFormula.h
 *	@author Jussi Vatjus-Anttila (jussiva@gmail.com)
 *	@date	23.04.2012
 *
 *  licence:	wxWindows Library License v3.1
 *
 *  Copyright 2009- Jussi Vatjus-Anttila
 *
 *  This code may be reused without my permission as long as credits are given to
 *  the original author.  If you find bugs, please send me a note...
 *
 *
 *	@note
 *		Former use "reverse polis notation" -method:
 *		http://en.wikipedia.org/wiki/Reverse_Polish_notation
 *
 *      Easy to implement new operators to former
 *
 *   Mathematic operators:
 *   -*+/^ sin,cos,tan,arcsin,arccos,arctan, min, max
 *
 */

 #include "gpFormula.h"
#include <math.h>
/*std::string d2string(double d)
{
    std::stringstream ostr;
    ostr << d;
    return ostr.str();
}*/
gpOper::gpOper()
{
	m_opcode        =-1;
	m_precedence    = 0;
	m_argument_count= 0;
	m_oplen         = 0;
	m_value         = 0;
}

gpOper::gpOper(string op, int code, int prec, int c)
{
	m_op            =op;
	m_opcode        =code;
	m_precedence    =prec;
	m_argument_count=c;
	m_oplen         = op.length();
	m_value         = 0;
}

gpOper::gpOper(double value)
{
	m_op            = "";
	m_oplen         = 0;
	m_opcode        = -1;
	m_argument_count= 0;
	m_value         = value;
	m_precedence    = 0;
}

gpOper::gpOper(const gpOper& op)
{
	this->m_op = op.m_op;
	this->m_oplen = op.m_oplen;
	this->m_opcode = op.m_opcode;
	this->m_argument_count = op.m_argument_count;
	this->m_value = op.m_value;
	this->m_precedence = op.m_precedence;
}
gpOper::~gpOper(){}
gpOper::operator int()
{return m_opcode;}



gpFormulaParser::gpFormulaParser(string formula)
{
	int i=0;
								//operator, code, precedence, operator argument count
	m_operatorList.push_back(new gpOper("(",    i++, 0, 0));		//op 0
	m_operatorList.push_back(new gpOper(")",    i++, 0, 0));		//op 1
	m_operatorList.push_back(new gpOper("+",    i++, 0, 2));		//op 2
	m_operatorList.push_back(new gpOper("-",    i++, 0, 2));		//op 3
	m_operatorList.push_back(new gpOper("*",    i++, 1, 2));		//op 4
	m_operatorList.push_back(new gpOper("/",    i++, 1, 2));		//op 5
	m_operatorList.push_back(new gpOper("^",    i++, 2, 2));		//op 6
	m_operatorList.push_back(new gpOper("%",    i++, 0, 2));		//op 7
	m_operatorList.push_back(new gpOper("sin",  i++, 0, 1));	//op 8
	m_operatorList.push_back(new gpOper("cos",  i++, 0, 1));	//op 9
	m_operatorList.push_back(new gpOper("tan",  i++, 0, 1));	//op 10
	m_operatorList.push_back(new gpOper("arcsin", i++, 0, 1));	//op 11
	m_operatorList.push_back(new gpOper("arcsos", i++, 0, 1));	//op 12
	m_operatorList.push_back(new gpOper("arctan", i++, 0, 1));	//op 13
	m_operatorList.push_back(new gpOper("!",      i++, 1, 1));	//op 14
	m_operatorList.push_back(new gpOper("min",    i++, 0, 2));	//op 15
	m_operatorList.push_back(new gpOper("max",    i++, 0, 2));	//op 15
	parseInfixNotations(formula);
}
gpFormulaParser::~gpFormulaParser()
{
	operatorpList_t::iterator it;
	for(it = m_operatorList.begin(); it!= m_operatorList.end(); ++it)
		delete *it;
}


operatorList_t gpFormulaParser::GetResultStack(){return m_outputStack;}

/**
 *  Shunting-yard_algorithm
 *  http://en.wikipedia.org/wiki/Shunting-yard_algorithm
 */
void gpFormulaParser::parseInfixNotations(string formula)
{
	double result;
	gpOper* oper, tmpoper;
	char *ptr = (char*)formula.c_str(), *tmp;
	char *ptrend = (char*)formula.c_str()+formula.length();
	m_operatorStack.clear();
	m_outputStack.clear();

	while(ptr<ptrend)
	{
		result=strtod(ptr, &tmp);
		oper=operator_valid(ptr);
		if(  oper != 0 //&&
			/// @bug if 5--5 case or 5*-5, there is bug!
			// special operator '-', if there is digit over that, must convert to negative value but only when before that found operator!
			// !( *(ptr[0]=='-' && isdigit(ptr[1])) )
			)
		{

			if(oper->m_op == ")")
			{
				while(m_operatorStack.front().m_op!="(")
				{
					m_outputStack.push_back(m_operatorStack.front());
					m_operatorStack.pop_front();
				}
				m_operatorStack.pop_front();
			}
			else
			{
				if(oper->m_precedence > 0)
				{

					/*if(m_operatorStack.size() == 0 )
					{
						//wxLogError(_("Operator stack is null!"));
						break; //while
					}

					tmpoper = m_operatorStack.front();
					if(tmpoper.m_precedence == oper->m_precedence && oper->m_op!="^")
					{
						m_outputStack.push_back( m_operatorStack.front() );
						m_operatorStack.pop_front();        //delete first operator
					}
					else if(tmpoper.m_precedence > oper->m_precedence)
					{
						//
					}*/


					if(m_operatorStack.size() > 0 )
					{
						//check previous operator
						tmpoper = m_operatorStack.front();
						if(tmpoper.m_precedence == oper->m_precedence && oper->m_op!="^")
						{
							m_outputStack.push_back( m_operatorStack.front() );
							m_operatorStack.pop_front();        //delete first operator
						}
						else if(tmpoper.m_precedence > oper->m_precedence)
						{
							//
						}
					}


				}
				m_operatorStack.push_front(*oper);

			}
			if(oper->m_oplen==0)tmp=ptr+1;
			else tmp = ptr + oper->m_oplen;

		}
		else
		{
			//value
			m_outputStack.push_back(gpOper(result));
		}

		/*
		//debug prints
		printf("%c: ", *ptr);
		operatorList_t::iterator it;
		for(it=m_outputStack.begin(); it!=m_outputStack.end(); it++)
		{
			if(it->m_opcode==-1)printf("%.0f ", it->m_value);
			else std::cout << it->m_op;
		}
		printf("\t|\t");
		for(it=m_operatorStack.begin(); it!=m_operatorStack.end(); it++)
		{
			if(it->m_opcode==-1)printf("%.0f ", it->m_value);
			else std::cout << it->m_op;
		}
		std::cout << std::endl << "-----------------------------"<< std::endl;*/

		ptr = tmp;
	}
	m_outputStack.insert(m_outputStack.end(), m_operatorStack.begin(), m_operatorStack.end());


	//debug print:
	/*
	operatorList_t::iterator it;
	for(it=m_outputStack.begin(); it!=m_outputStack.end(); it++)
	{
		if(it->m_opcode==-1)printf("%.3f ", it->m_value);
		else std::cout << it->m_op;
	}
	std::cout << std::endl;
	*/

}

/**
 *  Test if token begins valid operator
 *  @param token    token to be test
 *  @param [out] oplen   operator length
 *  @return operator. -1 is invalid, >-1 valid operator
 */
gpOper* gpFormulaParser::operator_valid(string token)
{
	int index=0;
	operatorpList_t::iterator it;
	for(it=m_operatorList.begin(); it!=m_operatorList.end(); it++, index++)
	{
		if( token.compare(0, (*it)->m_oplen, (*it)->m_op) == 0 )
		{
			return *it;
		}
	}
	return 0;
}

/* gpFormula */

gpFormula::gpFormula(){}
# define M_PI           3.14159265358979323846  /* pi */
gpFormula::gpFormula(string formula)
{
	m_strformula = formula;
	m_variable["PI"] = M_PI;
}

gpFormula::~gpFormula(){}

void gpFormula::SetFormula(string formula)
{ m_strformula = formula; }


double gpFormula::calculate()
{
	double result=0;
	string str = m_strformula;
	remove(str, " \t\x0a\x0d");
	replaceWhiteMull(str);
	insertVariables(str);
	calculate(str, result);
	return result;
}


gpFormula::operator double()
{ return calculate(); }


void gpFormula::AddVariable(string key, double value)
{ m_variable[key]=value;}


/** PRIVATE */

void gpFormula::replaceWhiteMull(string &content)
{
	size_t found=0;
	std::map<std::string, double>::iterator it;
	for(it=m_variable.begin(); it!=m_variable.end();it++)
	{
		if( (found = content.find(it->first)) != std::string::npos )
		{
			char t = content[found+it->first.length()];
			switch( t )
			{
				case('0'):case('1'):case('2'):case('3'):case('4'):case('5'):case('6'):
				case('7'):case('8'):case('9'):case('-'):case('+'):case('.'):
					content.insert(found+it->first.length(), "*" );
				default:
					break;
			}
			t = content[found-1];
			switch( t )
			{
				case('0'):case('1'):case('2'):case('3'):case('4'):case('5'):case('6'):
				case('7'):case('8'):case('9'):case('-'):case('+'):case('.'):
					content.insert(found, "*" );
				default:
					break;
			}
		}
	}
}

void gpFormula::insertVariables(string &content)
{
	std::map<std::string, double>::iterator it;
	for(it=m_variable.begin(); it!=m_variable.end();it++)
		replace(content, it->first, it->second);
}
void gpFormula::replace(string& content, string key, double value)
{
	size_t i;
	std::stringstream ostr;

	while( (i=content.find(key)) != string::npos)
	{
		content.erase(i, key.length());
		ostr << value;
		content.insert(i, ostr.str() );
	}
}
void gpFormula::remove(string& str, std::string rem)
{
	std::string temp;
	for (unsigned int i = 0; i < str.length(); i++){
		if( rem.find(str[i], 0) == std::string::npos )
			temp += str[i]; }
	str = temp;
}

void gpFormula::toUpperCase(string& str)
{
	// explicit cast needed to resolve ambiguity
	std::transform(str.begin(), str.end(), str.begin(),
			   (int(*)(int)) ::toupper);

}


bool gpFormula::calculate(string str, double& out)
{
	double a,b, result = 0;
	gpOper oper;
	gpFormulaParser parser(str);
	operatorList_t operators = parser.GetResultStack();

	while(m_resultStack.size())m_resultStack.pop();

	while(operators.size())
	{

		oper = operators.front();
		if(oper.m_opcode == -1)
		{
			result =  oper.m_value;
		}
		else
		{
			a=b=0;
			if(oper.m_argument_count == 0)
			{
				//wxLogMessage(wxT("NO OPER WHERE ARGUMENT COUNT IS 0"));
			}
			if(oper.m_argument_count > 0 && m_resultStack.size()>0)
				{b=m_resultStack.top();m_resultStack.pop();}
			if(oper.m_argument_count == 2&& m_resultStack.size()>0)
				{a=m_resultStack.top();m_resultStack.pop();}

			operator_select(a,b, oper.m_opcode, result);

			//printf("%.0f (%i)%s %.0f = %f\n", a, oper.m_opcode, oper.m_op.c_str(), b, result);


		}
		m_resultStack.push(result); //push result to result stack

		operators.pop_front();      //delete handled operator.
	}

	if(m_resultStack.size()==0)
	{
		//wxLogError( _("Exception while calculate formula: ") + wxString(str.c_str(), wxConvUTF8) + _(" .") );
		return false;
	}
	else
	{
		out = m_resultStack.top();
		return true;
	}
}

bool gpFormula::operator_select(double a, double b, int opcode, double& result)
{
	bool ok=true;
	switch(opcode)
	{
		case(-1):
		default:    //wxLogError(_("Not undestand the operator"));
					ok=false;                   break;
		case(2):	result = operator_add(a,b); break;
		case(3):	result = operator_sub(a,b); break;
		case(4):	result = operator_mul(a,b); break;
		case(5):	result = operator_div(a,b); break;
		case(6):	result = operator_pow(a,b); break;
		case(7):	result = operator_mod(a,b); break;
		case(8):	result = operator_sin(b);   break;
		case(9):	result = operator_cos(b);   break;
		case(10):	result = operator_tan(b);   break;
		case(11):	result = operator_arcsin(b); break;
		case(12):	result = operator_arccos(b); break;
		case(13):	result = operator_arctan(b); break;
		case(14):	result = operator_factorial(b); break;
		case(15):	result = operator_min(a,b); break;
		case(16):	result = operator_max(a,b); break;
	};
	return ok;
}

/* OPERATOR FUNCTIONS */

double gpFormula::operator_add(double a, double b){return a+b;}
double gpFormula::operator_sub(double a, double b){return a-b;}
double gpFormula::operator_mul(double a, double b){return a*b;}
double gpFormula::operator_div(double a, double b){return a/b;}
double gpFormula::operator_pow(double a, double b){return pow(a,b);}
double gpFormula::operator_mod(double a, double b){return (int)a%(int)b;}

double gpFormula::operator_sin(double a){return sin(a);}
double gpFormula::operator_cos(double a){return cos(a);}
double gpFormula::operator_tan(double a){return tan(a);}
double gpFormula::operator_arcsin(double a){return asin(a);}
double gpFormula::operator_arccos(double a){return acos(a);}
double gpFormula::operator_arctan(double a){return atan(a);}

double gpFormula::operator_factorial(double a)
{
	double result=1;
	for(int i=0;i<(int)a;i++)
	{
		result *= i;
	}
	return result;
}

double gpFormula::operator_min(double a, double b){return a<b?a:b;}
double gpFormula::operator_max(double a, double b){return a>b?a:b;}
