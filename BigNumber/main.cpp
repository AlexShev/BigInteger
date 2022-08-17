// BigNumber.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "BigInteger.h"


int main()
{
    BigInteger f("-197");
    BigInteger s("-197");

    BigInteger t("999");
    BigInteger fouth("-999");

    BigInteger fivth("-9999");
    BigInteger sixth("9999");

    //std::cout << BigInteger::Sum({ &f, &s, &t, &fouth, &fivth, &sixth }).ToString() << std::endl;

    std::cout << f.ToString() << std::endl;

    f.Subtract(s);

    f.Subtract(s);

    std::cout << f.ToString() << std::endl;


    //std::cout << f.Subtract(s).Subtract(s).ToString() << std::endl << f.ToString();


    // std::cout << (f >= s);


}