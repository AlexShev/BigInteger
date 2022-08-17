#include "BigInteger.h"
#include <stdexcept>
#include <numeric>


#pragma region Constructors

BigInteger::BigInteger()
{
	SetDefault();
}


BigInteger::BigInteger(const std::string& number)
{
	if (!IsNumber(number))
	{
		throw std::invalid_argument("it's not a number!");
	}

	int stop = 0;

	_isPositive = IsPositive(number);

	if (!_isPositive)
	{
		++stop;
	}

	for (int i = number.size() - 1; i >= stop; --i)
	{
		_number.push_back(number[i] - '0');
	}
}


BigInteger::BigInteger(Digits&& number, bool isPositive = true)
{
	if (number.empty())
	{
		SetDefault();
	}
	else
	{
		_number = std::move(number);

		_isPositive = isPositive;
	}

}


BigInteger::BigInteger(const BigInteger& number)
{
	number.CopyTo(*this);
}


BigInteger::BigInteger(BigInteger&& number)
{
	number.MoveTo(*this);
}

#pragma endregion


#pragma region Copy_functions

void BigInteger::SetDefault()
{
	_isPositive = true;

	_number.clear();

	_number.push_back(0);
}


void BigInteger::CopyTo(BigInteger& number) const
{
	number._isPositive = this->_isPositive;

	number._number = this->_number;
}


void BigInteger::MoveTo(BigInteger& number)
{
	number._isPositive = this->_isPositive;

	number._number = std::move(this->_number);

	this->SetDefault();
}


BigInteger& BigInteger::operator= (const BigInteger& left)
{
	if (this != &left)
	{
		left.CopyTo(*this);
	}

	return *this;
}

BigInteger& BigInteger::operator= (BigInteger&& left)
{
	if (this != &left)
	{
		left.MoveTo(*this);
	}

	return *this;
}

#pragma endregion


#pragma region non_math_functions

std::string BigInteger::ToString() const
{
	std::string res;

	res.reserve(_number.size() + 1);

	if (!_isPositive)
	{
		res.push_back('-');
	}

	for (int i = _number.size() - 1; i >= 0; --i)
	{
		res.push_back(_number[i] + '0');
	}

	return res;
}

#pragma endregion


#pragma region Public_math_function


BigInteger& BigInteger::Add(const BigInteger& number)
{
	return *this = std::move(BigInteger::Sum(*this, number));
}


BigInteger& BigInteger::Subtract(const BigInteger& number)
{
	return *this = std::move(BigInteger::Subtract(*this, number));
}

#pragma endregion


#pragma region Public_static_math_function

BigInteger BigInteger::Sum(const std::initializer_list<BigInteger*> numbers)
{
	std::vector<const Digits*> positives;
	std::vector<const Digits*> negatives;

	for (auto& number : numbers)
	{
		if (number->_isPositive)
		{
			positives.push_back(&(number->_number));
		}
		else
		{
			negatives.push_back(&(number->_number));
		}
	}

	BigInteger positive = std::move(BigInteger(Sum(positives), true));
	BigInteger negative = std::move(BigInteger(Sum(negatives), false));

	return SubtractOppositeSign(positive._number, negative._number);
}


BigInteger BigInteger::Sum(const BigInteger& first, const BigInteger& second)
{
	if (first._isPositive == second._isPositive)
	{
		return BigInteger(Sum(first._number, second._number), first._isPositive);
	}
	else
	{
		if (AbsLess(first._number, second._number))
		{
			return BigInteger(Subtract(second._number, first._number), false);
		}
		else
		{
			return BigInteger(Subtract(first._number, second._number), true);
		}
	}
}


BigInteger BigInteger::Subtract(const BigInteger& first, const BigInteger& second)
{
	if (first._isPositive == second._isPositive)
	{
		bool isLess = AbsLess(first._number, second._number);

		if (isLess)
		{
			return BigInteger(Subtract(second._number, first._number), isLess ^ second._isPositive);
		}
		else
		{
			return BigInteger(Subtract(first._number, second._number), isLess ^ second._isPositive);
		}
	}
	else
	{
		return BigInteger(Sum(first._number, second._number), first._isPositive);
	}
}

#pragma endregion


#pragma region Private_math_function

Digits BigInteger::Sum(const std::vector<const Digits*> numbers)
{
	size_t maxLenght = 0;

	for (auto& number : numbers)
	{
		maxLenght = std::max(maxLenght, number->size());
	}

	Digits res;
	res.reserve(maxLenght + numbers.size());

	Digit transferredUnit = 0;
	size_t i = 0;

	for (; i < maxLenght; i++)
	{
		Digits params;
		
		for (auto& number : numbers)
		{
			if (i < number->size())
			{
				params.push_back(number->at(i));
			}
		}

		res.push_back(Sum(params, transferredUnit));
	}

	while (transferredUnit > 0)
	{
		res.push_back(Sum(transferredUnit, transferredUnit));
	}

	return res;
}


Digits BigInteger::Sum(const Digits& first, const Digits& second)
{
	const Digits& shortNum = (first.size() < second.size() ? first : second);
	const Digits& longNum = (first.size() >= second.size() ? first : second);

	size_t minLenght = shortNum.size(); // std::min(first.size(), second.size());
	size_t maxLenght = longNum.size(); // std::max(first.size(), second.size());

	Digits res;
	res.reserve(maxLenght + 1);

	Digit transferredUnit = 0;
	size_t i = 0;

	for (; i < minLenght; i++)
	{
		res.push_back(Sum({ shortNum[i], longNum[i]}, transferredUnit));
	}

	for (; i < maxLenght; i++)
	{
		res.push_back(Sum(longNum[i], transferredUnit));
	}

	while (transferredUnit > 0)
	{
		res.push_back(Sum(transferredUnit , transferredUnit));
	}

	return res;
}


Digits BigInteger::Subtract(const Digits& longNum, const Digits& shortNum)
{
	size_t minLenght = shortNum.size();
	size_t maxLenght = longNum.size() - 1;

	Digits res;
	res.reserve(maxLenght);

	Digit debt = 0;
	size_t i = 0;

	Digit digit;
	int zeroCounter = 0;

	for (; i < minLenght; i++)
	{
		digit = Subtract(longNum[i], shortNum[i], debt);
		
		if (digit == 0)
		{
			++zeroCounter;
		}
		else
		{
			while (zeroCounter-- > 0)
			{
				res.push_back(0);
			}

			res.push_back(digit);
		}
	}

	for (; i < maxLenght; ++i)
	{
		res.push_back(Subtract(longNum[i], debt));

		if (debt == 0)
		{
			while (i < maxLenght)
			{
				res.push_back(longNum[++i]);
			}

			return res;
		}
	}

	//if (Digit digit = Subtract(longNum[i], debt))
	//{
	//	res.push_back(digit);
	//}

	return res;
}

#pragma endregion


#pragma region Sum_help_functions

Digit BigInteger::Sum(const std::initializer_list<Digit>& digits, Digit& transferredUnit)
{
	Digit digit = std::accumulate(digits.begin(), digits.end(), 0) + transferredUnit;

	return Sum(digit, transferredUnit);
}

Digit BigInteger::Sum(const Digits& digits, Digit& transferredUnit)
{
	Digit digit = std::accumulate(digits.begin(), digits.end(), 0) + transferredUnit;

	return Sum(digit, transferredUnit);
}

Digit BigInteger::Sum(Digit digit, Digit& transferredUnit)
{
	if (digit >= NUMBER_BASE)
	{
		transferredUnit = digit / NUMBER_BASE;
		digit %= NUMBER_BASE;
	}
	else
	{
		transferredUnit = 0;
	}

	return digit;
}

#pragma endregion


#pragma region Subtract_help_functions

Digit BigInteger::Subtract(Digit reduced, Digit subtracted, Digit& debt)
{
	reduced -= (debt + subtracted);

	if (reduced < 0)
	{
		debt = 1;
		
		return NUMBER_BASE + reduced;
	}
	else
	{
		debt = 0;

		return reduced;
	}
}

Digit BigInteger::Subtract(Digit reduced, Digit& debt)
{
	reduced -= debt;

	if (reduced < 0)
	{
		debt = 1;

		return NUMBER_BASE + reduced;
	}
	else
	{
		debt = 0;

		return reduced;
	}
}

BigInteger BigInteger::SubtractOppositeSign(const Digits& first, const Digits& second)
{
	if (AbsLess(first, second))
	{
		return BigInteger(Subtract(second, first), false);
	}
	else
	{
		return BigInteger(Subtract(first, second), true);
	}
}
#pragma endregion


#pragma region Compare_function

bool operator < (const BigInteger& first, const BigInteger& second)
{
	if (first._isPositive != second._isPositive)
	{
		return !first._isPositive;
	}

	// first._isPositive == second._isPositive
	if (first._isPositive)
	{
		return BigInteger::AbsLess(first._number, second._number);
	}
	else
	{
		return BigInteger::AbsLess(second._number, first._number);
	}
}


bool operator > (const BigInteger& first, const BigInteger& second)
{
	if (first._isPositive != second._isPositive)
	{
		return !first._isPositive;
	}

	// first._isPositive == second._isPositive
	if (first._isPositive)
	{
		return BigInteger::AbsGreat(first._number, second._number);
	}
	else
	{
		return BigInteger::AbsGreat(second._number, first._number);
	}
}


bool operator== (const BigInteger& first, const BigInteger& second)
{
	if (first._number.size() != second._number.size())
	{
		return first._number.size() < second._number.size();
	}

	// first._number.size() == second._number.size()
	for (int i = first._number.size() - 1; i >= 0; --i)
	{
		if (first._number[i] != second._number[i])
		{
			return false;
		}
	}

	return true;
}

bool operator!= (const BigInteger& first, const BigInteger& second)
{
	return !(first == second);
}

bool operator<= (const BigInteger& first, const BigInteger& second)
{
	return !(first > second);
}

bool operator>= (const BigInteger& first, const BigInteger& second)
{
	return !(first > second);
}



bool BigInteger::AbsLess(const Digits& first, const Digits& second)
{
	if (first.size() != second.size())
	{
		return first.size() < second.size();
	}

	// first._number.size() == second._number.size()
	for (int i = first.size() - 1; i >= 0; --i)
	{
		if (first[i] != second[i])
		{
			return first[i] < second[i];
		}
	}

	return false;
}


bool BigInteger::AbsGreat(const Digits& first, const Digits& second)
{
	if (first.size() != second.size())
	{
		return first.size() > second.size();
	}

	// first._number.size() == second._number.size()
	for (size_t i = 0; i < first.size(); i++)
	{
		if (first[i] != second[i])
		{
			return first[i] > second[i];
		}
	}

	return false;
}

#pragma endregion


#pragma region Parse_help_functions

inline bool BigInteger::IsPositive(const std::string& number)
{
	return number[0] != '-';
}

bool BigInteger::IsNumber(const std::string& number)
{
	if (number.empty())
	{
		return false;
	}

	int i = 0;

	if (!IsPositive(number))
	{
		++i;
	}

	for (; i < number.size(); i++)
	{
		if (!isdigit(number[i]))
		{
			return false;
		}
	}

	return true;
}

#pragma endregion