#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <initializer_list>
template <typename T>
struct Operator
{
    char symbol;
    int precedence;
    std::function<T(T, T)> func;
    bool isUnary = false;
    bool isLeftHand = false;
};

template <typename T, typename U>
struct ExpressionSpecs
{
    std::vector<Operator<T> >sOperators;
    std::function<T(U)> sConvertionFunc;
};

#include "utils.h"

template<typename T, typename U>
class ExpressionEvaluator
{
    public:
        ExpressionEvaluator(const ExpressionSpecs<T,U>& specs);

        void addOperator(const Operator<T>& op);
        void setSpecs(const ExpressionSpecs<T,U>& specs);
        T evaluate(std::string& expression);
        template<typename L, typename M>
        friend std::ostream& operator<<(std::ostream& out, ExpressionEvaluator<L, M>& exp);

        std::vector<Operator<T> > getOperators() const;

    private:
        void insert(T& val, int& opTop, int& valTop, std::vector<Operator<T> >& op, std::vector<T>& vals);
        void calculate(int& opTop, int& valTop, std::vector<T>& values, std::vector<Operator<T> >& op);
        void checkConsecutives(int i, std::string& exp, int& opTop, int& valTop, std::vector<T>& values, std::vector<Operator<T>>& op);
        T emptyFunc(T arg1, T arg2) {return arg1;}

    private:
        Operator<T> mConsecutiveOperator;
        std::vector<Operator<T> > mOperators;
        std::function<T(U)> mConversionFunc;
};

#include "ExpressionEvaluatorDefinition.h"
