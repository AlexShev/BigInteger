#pragma once
#include <vector>
#include <string>

#define Digit char
#define Digits std::vector<Digit>

class BigInteger
{
public:

	enum Type;

	BigInteger();
	BigInteger(const std::string& number);
	BigInteger(const BigInteger& number);
	BigInteger(BigInteger&& number);
	BigInteger(Digits&& number, BigInteger::Type isPositive);


	void CopyTo(BigInteger& number) const;
	void MoveTo(BigInteger& number);
	BigInteger& operator= (const BigInteger& left);
	BigInteger& operator= (BigInteger&& left);

	
	BigInteger& Add(const BigInteger& number);
	BigInteger& Subtract(const BigInteger& number);


	BigInteger static Sum(const BigInteger& first, const BigInteger& second);
	BigInteger static Sum(const std::initializer_list<BigInteger*> numbers);

	
	BigInteger static Subtract(const BigInteger& first, const BigInteger& second);


	friend bool operator < (const BigInteger& first, const BigInteger& second);
	friend bool operator > (const BigInteger& first, const BigInteger& second);

	friend bool operator == (const BigInteger& first, const BigInteger& second);
	friend bool operator != (const BigInteger& first, const BigInteger& second);

	friend bool operator <= (const BigInteger& first, const BigInteger& second);
	friend bool operator >= (const BigInteger& first, const BigInteger& second);


	std::string ToString() const;
	void ChangeSign();

private:

	inline static bool IsPositive(const std::string& number);
	bool static IsNumber(const std::string& number);
	bool static IsZero(const std::string& number);

	
	void SetDefault();
	void SetNaN();
	Type static GetOpositeType(const Type& type);


	Digits static Sum(const Digits& first, const Digits& second);
	Digits static Sum(const std::vector<const Digits*> numbers);

	Digit static Sum(const std::initializer_list<Digit>& digits, Digit& transferredUnit);
	Digit static Sum(const Digits& digits, Digit& transferredUnit);
	Digit static Sum(Digit digit, Digit& transferredUnit);


	Digits static Subtract(const Digits& longNum, const Digits& shortNum);
	
	Digit static Subtract(Digit reduced, Digit subtracted, Digit& debt);
	Digit static Subtract(Digit reduced, Digit& debt);
	
	BigInteger static SubtractOppositeSign(const Digits& first, const Digits& second);


	bool static AbsLess(const Digits& first, const Digits& second);
	bool static AbsGreat(const Digits& first, const Digits& second);



	Digits _number;
	Type _type;

	static const int NUMBER_BASE = 10;
};

enum BigInteger::Type
{
	NaN = -2,
	negative = -1,
	zero = 0,
	positive = 1,
};
