#pragma once
namespace Common{

    namespace Algebra{

        template<typename T>
        T AddOp(T a, T b) { return a + b;}

        template<typename T>
        T NegOp(T a, T b) { return -a;}

        template<typename T>
         T MultOp(T a, T b) { return a * b;}

        template<typename T>
        T DivOp(T a, T b) { return a / b; }

        template<typename T>
        T getValFromChar(char c) { return (T)(c - '0'); }

        template<typename T>
        T DigitOp(T a, T b)
        {
            return (10 * a + b) * (a >= 0) + (10 * a - b) * (a < 0);
        }

        template<typename T>
        Operator<T> Add{.symbol= '+', .precedence = 1, .func = AddOp<T>,};

        template<typename T>
        Operator<T> Negate{.symbol= '-', .precedence = 3, .func = NegOp<T>, .isUnary = true, .isLeftHand = true};

        template<typename T>
        Operator<T> Multiply{.symbol= '*', .precedence = 2, .func = MultOp<T>,};

        template<typename T>
        Operator<T> addDigit{.symbol= '\0', .precedence = 4, .func = DigitOp<T>,};

        template<typename T>
        Operator<T> Divide{.symbol= '/', .precedence = 2, .func = DivOp<T>,};

        template<typename T>
        std::vector<Operator<T> > operators{Add<T>, Negate<T>, Multiply<T>,addDigit<T>, Divide<T>};
    }

    namespace Boolean{
        template<typename T>
            T NotOp(T a, T b) { return !a;}
        template<typename T>
            T AndOp(T a, T b) { return a & b;}
        template<typename T>
            T OrOp(T a, T b) { return a | b;}
        template <typename T>
            T getBoolFromChar(char c) { return c == 'T' || c == '1' || c == 't'
                || c == 'V' || c == 'v'; }
        template<typename T>
            Operator<T> Not{.symbol= '!', .precedence = 3, .func = NotOp<T>, .isUnary = true, .isLeftHand = true};
        template<typename T>
            Operator<T> And{.symbol= '&', .precedence = 2, .func = AndOp<T>,};
        template<typename T>
            Operator<T> Or{.symbol= '|', .precedence = 1, .func = OrOp<T>,};
        template<typename T>
            std::vector<Operator<T> > operators{Not<T>, And<T>, Or<T>};
    }
}

namespace Global{
    template <typename T>
        Operator<T> openPara{.symbol = '(', .precedence = 0,
            .func =[](T a, T b){return a;}, .isUnary = false};
    template <typename T>
        Operator<T> closePara{.symbol = ')', .precedence = 9,
            .func =[](T a, T b){return a;}, .isUnary = false};
}
