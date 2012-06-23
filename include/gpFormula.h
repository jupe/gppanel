/**
 *  @file   gpFormula.h
 *	@author Jussi Vatjus-Anttila (jussiva@gmail.com)
 *	@date	16.10.2009
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
#ifndef GPFORMER_H
#define GPFORMER_H

#include <math.h>
#include <algorithm>
#include <stack>
#include <deque>
#include <string>
#include <map>
#include <sstream>
using std::string;

/*std::string d2string(double d)
{
    std::stringstream ostr;
    ostr << d;
    return ostr.str();
}*/

/**
 *  Operator
 */
class gpOper{
    public:
        string  m_op;
        int     m_oplen;
        int     m_opcode;
        int     m_argument_count;
        double  m_value;
        int     m_precedence;

    public:
        gpOper();
        /** Function operator constructor */
        gpOper(string op, int code, int prec, int c);
        /** Double value constructor */
        gpOper(double value);
        /** copy constructor */
        gpOper(const gpOper& op);
        ~gpOper();
        operator int();
};

typedef std::deque< gpOper* >  operatorpList_t;
typedef std::deque< gpOper >  operatorList_t;
typedef std::stack< double >  doubleStack_t;

/**
 *   Class parse string with Shunting-yard_algorithm
 *   to operator list. List can be calculated by
 *   Reverse Polish notation algorithm.
 */
class gpFormulaParser
{
    public:
        operatorpList_t	    m_operatorList;

        operatorList_t	    m_outputStack;
        operatorList_t	    m_operatorStack;

    public:
        /**
         *  Constructor to present operators, and those parameters.
         */
        gpFormulaParser(string formula);
        ~gpFormulaParser();

        operatorList_t GetResultStack();

        /**
		 *  Shunting-yard_algorithm
		 *  http://en.wikipedia.org/wiki/Shunting-yard_algorithm
		 */
        void parseInfixNotations(string formula);

        /**
		 *  Test if token begins valid operator
		 *  @param token    token to be test
		 *  @param [out] oplen   operator length
		 *  @return operator. -1 is invalid, >-1 valid operator
		 */
		gpOper* operator_valid(string token);
};
/**
 *	Main class of formula.
 *	Simple formula parser for multifunctions.
 *
 *	Example:
 *	double y = gpFormula("5*4"); // y <= 20
 *
 */
class gpFormula
{
	private:

		doubleStack_t 	    m_resultStack;      //result stack
		string              m_strformula;       //original formula
		std::map<string, double>  m_variable;   //variable table

	public:
        /** Default constructor*/
		gpFormula();
		/** Constructor with formula*/
		gpFormula(string formula);
		/** Destructor */
		~gpFormula();
        /**
         * define formula
         * @param formula   formula to be calculated
         */
        void SetFormula(string formula);

		/**
		 *  Default calculate functions
		 */
		double calculate();
		/** Operator double()
		 *  std:cout << (double)gpFormula("2*PI") << std::endl;
		 */
		operator double();

        /**
        *   add/modify variable
        *   @param key  variable label
        *   @param value variable value
        */
		void AddVariable(string key, double value);

    private:
        /**
         */
        void replaceWhiteMull(string &content);
        /**
         *  Insert variables to content
         */
        void insertVariables(string &content);
        /** Replace string value to real value
         *  Used to replace variable labels with variable values
         *  @param content   content be search keys
         *  @param key       key to be erased
         *  @param value     value to be insert
         */
		void replace(string& content, string key, double value);
		/**
		 * remove characters
		 */
		void remove(string& str, std::string rem);
		/**
         *  Characters to uppercase
         *  @param str  string to transform
		 */
		void toUpperCase(string& str);
        /**
         *  Calculate all operator stack
         */
		bool calculate(string str, double& out);
        /**
         *  Select operator and calculate it
         *  @param a    left value
         *  @param b    right value
         *  @param opcode
         *  @param [out] result result of operator
         *  @return true if everythings goes right
         */
		bool operator_select(double a, double b, int opcode, double& result);

        /* OPERATOR FUNCTIONS */

		double operator_add(double a, double b);
		double operator_sub(double a, double b);
		double operator_mul(double a, double b);
		double operator_div(double a, double b);
		double operator_pow(double a, double b);
		double operator_mod(double a, double b);

		double operator_sin(double a);
		double operator_cos(double a);
		double operator_tan(double a);
		double operator_arcsin(double a);
		double operator_arccos(double a);
		double operator_arctan(double a);
		
		double operator_factorial(double a);

		double operator_min(double a, double b);
		double operator_max(double a, double b);

};
#endif
