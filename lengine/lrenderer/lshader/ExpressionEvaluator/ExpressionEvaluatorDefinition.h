template <typename T, typename U>
ExpressionEvaluator<T, U>::ExpressionEvaluator(const ExpressionSpecs<T, U>& specs)
    :mOperators(specs.sOperators),
    mConversionFunc(specs.sConvertionFunc)
{
    Operator<T> open = Global::openPara<T>;
    Operator<T> close = Global::closePara<T>;
    close.precedence = mOperators.size();
    mOperators.emplace_back(open);
    mOperators.emplace_back(close);
    for (auto i : mOperators)
        if (i.symbol == '\0')
        {
            mConsecutiveOperator = i;
        }
}

    template <typename T, typename U>
void ExpressionEvaluator<T, U>::addOperator(const Operator<T>& op)
{
    mOperators.emplace_back(op);
    if (op.symbol == '\0')
        mConsecutiveOperator = op;
}

    template <typename T, typename U>
void ExpressionEvaluator<T, U>::setSpecs(const ExpressionSpecs<T,U>& specs)
{
    mOperators = specs.sOperators;
    mConversionFunc = specs.sConverstionFunc;
}

    template <typename T, typename U>
T ExpressionEvaluator<T, U>::evaluate(std::string& expression)
{
    std::vector<Operator<T> > op;
    std::vector<T> values;
    int leftHandCounts = 0;
    values.resize(expression.size());
    op.resize(expression.size());
    int opTop, valTop;
    opTop = 0;
    valTop = 0;
    bool isOperator = false;
    for (int j = 0; j < expression.size(); j++)
    {
        char c = expression[j];
        if (c == ' ' || c == '\t' || c == '\n')
            continue;
        if (c == '(')
        {
            op[opTop++] = Global::openPara<T>;
            isOperator = true;
        }

        else if (c == ')')
        {
            isOperator = true;
            while (opTop && op[opTop - 1].precedence != 0)
                calculate(opTop, valTop, values, op);
            --opTop;
            --valTop;
            insert(values[valTop], opTop, valTop, op, values);
            checkConsecutives(j, expression, opTop, valTop, values, op);
        }
        else
        {
            for (auto i : mOperators)
            {
                if (c == i.symbol && !i.isLeftHand)
                {
                    isOperator = true;
                    while (opTop && op[opTop - 1].precedence > i.precedence - 1)
                        calculate(opTop, valTop, values, op);
                    op[opTop++] = i;
                    if (i.isUnary)
                        checkConsecutives(j, expression, opTop, valTop, values, op);
                    break;
                }
                else if (c == i.symbol && i.isLeftHand)
                {
                    isOperator = true;
                    op[opTop++] = i;
                }
            }
        }
        if (!isOperator)
        {
            T val = mConversionFunc(c);
            insert(val, opTop, valTop, op, values);
            checkConsecutives(j, expression, opTop, valTop, values, op);
        }
        isOperator = false;
    }
    while (opTop)
        calculate(opTop, valTop, values, op);
    return values[0];
}

    template <typename T, typename U>
void ExpressionEvaluator<T, U>::checkConsecutives(int i, std::string& exp
        , int& opTop, int& valTop, std::vector<T>& values
        , std::vector<Operator<T>>& op)
{
    bool foundSymb = true;
    if (( i < exp.size() - 1))
    {
        foundSymb = false;
        for (auto k : mOperators)
        {
            foundSymb |= (exp[i + 1] !='(' && (exp[i + 1] == k.symbol
                            || exp[i + 1] == '\t' || exp[i+1] == '\n' || exp[i + 1] == ' '));
        }
    }
    if (!foundSymb)
    {
        while (opTop && op[opTop - 1].precedence > mConsecutiveOperator.precedence - 1)
            calculate(opTop, valTop, values, op);
        op[opTop++] = mConsecutiveOperator;
    }
}

    template <typename T, typename U>
void ExpressionEvaluator<T, U>::insert(T& val, int& opTop, int& valTop, std::vector<Operator<T> >& op, std::vector<T>& values)
{
    while (opTop && op[opTop - 1].isLeftHand)
    {
        val = op[opTop - 1].func(val, val);
        opTop--;
    }
    values[valTop++] = val;
}

    template <typename T, typename U>
void ExpressionEvaluator<T, U>::calculate(int& opTop, int& valTop, std::vector<T>& values, std::vector<Operator<T> >& op)
{
    T ans;
    Operator<T> opr = op[--opTop];
    for (auto& i : mOperators)
    {
        if (opr.symbol == i.symbol)
        {
            T a = values[--valTop];
            if (!i.isUnary)
            {
                T b = values[--valTop];
                ans = i.func(b, a);
            }
            else
            {
                ans = i.func(a,a);
            }
            break;
        }
    }
    insert(ans, opTop, valTop, op, values);
}

    template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, ExpressionEvaluator<T, U>& exp)
{
    os << "operators: {";
    for (auto& i : exp.mOperators)
    {
        os << "(" << i.symbol << ", " << i.precedence << "), ";
    }
    os << "}";
    return os;
}

template <typename T, typename U>
std::vector<Operator<T>> ExpressionEvaluator<T, U>::getOperators() const
{
    return mOperators;
}
