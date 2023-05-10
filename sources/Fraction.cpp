#include "Fraction.hpp"
#include <iostream>
#include <limits>

using namespace std;

namespace ariel
{
    //Constructor
    Fraction::Fraction()
    {
        numerator = 1;
        denominator = 1;
    }

    Fraction::Fraction(int num, int den)
    {
        if(den == 0)
        {
            throw invalid_argument("divide by zero");
        }
        numerator = num;
        denominator = den;
        reduce();
    }

    Fraction::Fraction(float flo) : Fraction((int)(flo*1000), 1000)
    {
        reduce();
    }

    //Unary operators, pre- and post- fix
    //prefix
    Fraction& Fraction::operator++()
    {
        this->numerator += this->denominator;
        reduce();

        return *this;
    }

    //postfix
    /*const*/ Fraction Fraction::operator++(int flag)
    {
        Fraction res(numerator, denominator);
        this->numerator += this->denominator;
        reduce();
        return res;
    }

    //prefix
    Fraction& Fraction::operator-- ()
    {
        this->numerator -= this->denominator;
        reduce();

        return *this;
    }

    //postfix
    /*const*/ Fraction Fraction::operator--(int flag)
    {
        Fraction res(numerator, denominator);
        this->numerator -= this->denominator;
        reduce();
        return res;
    }

    //Binary operators
    Fraction Fraction::operator+ (const Fraction& other)
    {
        int newNum, newDen;
        //the denominator is the LCM of both denominators
        newDen = lcm(this->denominator, other.denominator);
        //multiply each numerator by the lcm divided by the original denominator, and add them together
        if(__builtin_sadd_overflow(this->numerator*(newDen/this->denominator), other.numerator*(newDen/other.denominator), &newNum))
        {
            throw std::overflow_error("Overflow in addition");
        }
        //newNum = this->numerator * (newDen/this->denominator) + other.numerator * (newDen/other.denominator);
        Fraction res(newNum, newDen);
        res.reduce();

        return res;
    }
    Fraction Fraction::operator- (const Fraction& other)
    {
        int newNum, newDen;
        //the denominator is the LCM of both denominators
        newDen = lcm(this->denominator, other.denominator);
        //multiply each numerator by the lcm divided by the original denominator, and subtract the second from the first
        if(__builtin_ssub_overflow(this->numerator*(newDen/this->denominator), other.numerator*(newDen/other.denominator), &newNum))
        {
            throw std::overflow_error("Overflow in subtraction");
        }
        //newNum = this->numerator * (newDen/this->denominator) - other.numerator * (newDen/other.denominator);
        Fraction res(newNum, newDen);
        res.reduce();
        return res;
    }
    Fraction Fraction::operator* (const Fraction& other)
    {
        int newNum, newDen;
        //multiply numerators and denominators
        if(__builtin_smul_overflow(this->numerator, other.numerator, &newNum) || __builtin_smul_overflow(this->denominator, other.denominator, &newDen))
        {
            throw std::overflow_error("Overflow in multiplication");
        }
        Fraction res(newNum, newDen);
        //Fraction res(this->numerator * other.numerator, this->denominator * other.denominator);
        res.reduce();
        return res;
    }
    Fraction Fraction::operator/ (const Fraction& other)
    {
        if(other.numerator == 0)
        {
            throw runtime_error("divide by zero");
        }

        int newNum, newDen;
        //cross-multiply between numerators and denominators
        if(__builtin_smul_overflow(this->numerator, other.denominator, &newNum) || __builtin_smul_overflow(this->denominator, other.numerator, &newDen))
        {
            throw std::overflow_error("Overflow in division");
        }
        Fraction res(newNum, newDen);

        //Fraction res (this->numerator * other.denominator, this->denominator * other.numerator);
        res.reduce();
        return res;
    }

    //Binary operators with float on right
    Fraction Fraction::operator+ (const float& other)
    {
        //convert float to fraction, and return result of fraction addition
        Fraction temp(other);
        return (*this + temp);
    }
    Fraction Fraction::operator- (const float& other)
    {
        //convert float to fraction, and return result of fraction subtraction
        Fraction temp(other);
        return (*this - temp);
    }
    Fraction Fraction::operator* (const float& other)
    {
        //convert float to fraction, and return result of fraction multiplication
        Fraction temp(other);
        return (*this * temp);
    }
    Fraction Fraction::operator/ (const float& other)
    {
        if(other == 0)
        {
            throw runtime_error("divide by zero");
        }
        //convert float to fraction, and return result of fraction division
        Fraction temp(other);
        return (*this/temp);
    }

    //Binary operators with float on left
    Fraction operator+ (const float& rhs, const Fraction& lhs)
    {
        //convert float to fraction, and return result of fraction addition
        Fraction temp(rhs);
        return (temp + lhs);
    }
    Fraction operator- (const float& rhs, const Fraction& lhs)
    {
        //convert float to fraction, and return result of fraction subtraction
        Fraction temp(rhs);
        return temp - lhs;
    }
    Fraction operator* (const float& rhs, const Fraction& lhs)
    {
        //convert float to fraction, and return result of fraction multiplication
        Fraction temp(rhs);
        return (temp*lhs);
    }
    Fraction operator/ (const float& rhs, const Fraction& lhs)
    {
        if(lhs.numerator == 0)
        {
            throw runtime_error("divide by zero");
        }
        //convert float to fraction, and return result of fraction division
        Fraction temp(rhs);
        return (temp/lhs);
    }

    //Boolean operators
    bool Fraction::operator== (const Fraction& other) const
    {
        //Find a common denominator, and then compare numerators
        int common = lcm(this->denominator, other.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = other.numerator * (common / other.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(!(other.numerator < 0) != !(other.denominator < 0))
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        return (lcmThis == lcmOther);
    }
    bool Fraction::operator>= (const Fraction& other) const
    {
        //Find a common denominator, and then compare numerators
        int common = lcm(this->denominator, other.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = other.numerator * (common / other.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(!(other.numerator < 0) != !(other.denominator < 0))
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        return (lcmThis >= lcmOther);
    }
    bool Fraction::operator<= (const Fraction& other) const
    {
        //Find a common denominator, and then compare numerators
        int common = lcm(this->denominator, other.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = other.numerator * (common / other.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(!(other.numerator < 0) != !(other.denominator < 0))
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        return (lcmThis <= lcmOther);
    }
    bool Fraction::operator< (const Fraction& other) const
    {
        //Find a common denominator, and then compare numerators
        int common = lcm(this->denominator, other.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = other.numerator * (common / other.denominator);
        
        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(!(other.numerator < 0) != !(other.denominator < 0))
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        //cout << lcmThis << "/" << common << " and " << lcmOther << "/" << common << endl;

        return (lcmThis < lcmOther);
    }
    bool Fraction::operator> (const Fraction& other) const
    {
        //Find a common denominator, and then compare numerators
        int common = lcm(this->denominator, other.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = other.numerator * (common / other.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(!(other.numerator < 0) != !(other.denominator < 0))
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        //cout << lcmThis << "/" << common << " and " << lcmOther << "/" << common << endl;

        return (lcmThis > lcmOther);
    }

    //Boolean operators with float on right
    bool Fraction::operator== (const float& other) const
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(other);
        int common = lcm(this->denominator, temp.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = temp.numerator * (common / temp.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(other < 0.0)
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        return (lcmThis == lcmOther);
    }
    bool Fraction::operator>= (const float& other) const
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(other);
        int common = lcm(this->denominator, temp.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = temp.numerator * (common / temp.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(other < 0.0)
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        return (lcmThis >= lcmOther);
    }
    bool Fraction::operator<= (const float& other) const
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(other);
        int common = lcm(this->denominator, temp.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = temp.numerator * (common / temp.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(other < 0.0)
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        return (lcmThis <= lcmOther);
    }
    bool Fraction::operator< (const float& other) const
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(other);
        int common = lcm(this->denominator, temp.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = temp.numerator * (common / temp.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(other < 0.0)
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        return (lcmThis < lcmOther);
    }
    bool Fraction::operator> (const float& other) const
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(other);
        int common = lcm(this->denominator, temp.denominator);
        int lcmThis = this->numerator * (common / this->denominator);
        int lcmOther = temp.numerator * (common / temp.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(!(this->numerator < 0) != !(this->denominator < 0))
        {
            if(lcmThis > 0)
                lcmThis *= (-1);
        }
        if(other < 0.0)
        {
            if(lcmOther > 0)
                lcmOther *= (-1);
        }

        return (lcmThis > lcmOther);
    }

    //Boolean operators: left float
    bool operator== (const float& lhs, const Fraction& rhs)
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(lhs);
        int common = temp.lcm(rhs.denominator, temp.denominator);
        int lcmlhs = temp.numerator * (common / temp.denominator);
        int lcmrhs = rhs.numerator * (common / rhs.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(lhs < 0.0)
        {
            if(lcmlhs > 0)
                lcmlhs *= (-1);
        }
        if(!(rhs.numerator < 0) != !(rhs.denominator < 0))
        {
            if(lcmrhs > 0)
                lcmrhs *= (-1);
        }

        return (lcmlhs == lcmrhs);
    }
    bool operator>= (const float& lhs, const Fraction& rhs)
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(lhs);
        int common = temp.lcm(rhs.denominator, temp.denominator);
        int lcmlhs = temp.numerator * (common / temp.denominator);
        int lcmrhs = rhs.numerator * (common / rhs.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(lhs < 0.0)
        {
            if(lcmlhs > 0)
                lcmlhs *= (-1);
        }
        if(!(rhs.numerator < 0) != !(rhs.denominator < 0))
        {
            if(lcmrhs > 0)
                lcmrhs *= (-1);
        }

        return (lcmlhs >= lcmrhs);
    }
    bool operator<= (const float& lhs, const Fraction& rhs)
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(lhs);
        int common = temp.lcm(rhs.denominator, temp.denominator);
        int lcmlhs = temp.numerator * (common / temp.denominator);
        int lcmrhs = rhs.numerator * (common / rhs.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(lhs < 0.0)
        {
            if(lcmlhs > 0)
                lcmlhs *= (-1);
        }
        if(!(rhs.numerator < 0) != !(rhs.denominator < 0))
        {
            if(lcmrhs > 0)
                lcmrhs *= (-1);
        }

        return (lcmlhs <= lcmrhs);
    }
    bool operator< (const float& lhs, const Fraction& rhs)
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(lhs);
        int common = temp.lcm(rhs.denominator, temp.denominator);
        int lcmlhs = temp.numerator * (common / temp.denominator);
        int lcmrhs = rhs.numerator * (common / rhs.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(lhs < 0.0)
        {
            if(lcmlhs > 0)
                lcmlhs *= (-1);
        }
        if(!(rhs.numerator < 0) != !(rhs.denominator < 0))
        {
            if(lcmrhs > 0)
                lcmrhs *= (-1);
        }

        return (lcmlhs < lcmrhs);
    }
    bool operator> (const float& lhs, const Fraction& rhs)
    {
        //Convert float to fraction, then find a common denominator, and then compare numerators
        Fraction temp(lhs);
        int common = temp.lcm(rhs.denominator, temp.denominator);
        int lcmlhs = temp.numerator * (common / temp.denominator);
        int lcmrhs = rhs.numerator * (common / rhs.denominator);

        //Adjust for negative fractions based on xor check for negatives on numerator and denominator
        if(lhs < 0.0)
        {
            if(lcmlhs > 0)
                lcmlhs *= (-1);
        }
        if(!(rhs.numerator < 0) != !(rhs.denominator < 0))
        {
            if(lcmrhs > 0)
                lcmrhs *= (-1);
        }

        return (lcmlhs > lcmrhs);
    }

    //Input/output operators
    istream& operator>> (istream &instream, Fraction &frac)
    {
        //throw runtime error if we get a 0 denominator or a non-int argument
        char divider;
        int newMerator, newMinator;

        if(!(instream >> newMerator) || !(instream >> newMinator))
        {
            throw runtime_error("not in fraction form");
        }

        if(newMinator == 0)
        {
            throw runtime_error("divide by zero");
        }
        
        frac.numerator = newMerator;
        frac.denominator = newMinator;

        return instream;
    }
    ostream& operator<< (ostream &out, const Fraction &frac)
    {
        Fraction temp(frac.numerator, frac.denominator);
        temp.reduce();
        return (out << temp.numerator << '/' << temp.denominator);
    }

    //getters
    int Fraction::getNumerator()
    {
        return numerator;
    }

    int Fraction::getDenominator()
    {
        return denominator;
    }

    void Fraction::reduce()
    {
        int common = gcd(numerator, denominator);
        numerator /= common;
        denominator /= common;

        if(numerator > 0 && denominator < 0)
        {
            numerator *= (-1);
            denominator *= (-1);
        }
        else if (numerator < 0 && denominator < 0)
        {
            numerator *= (-1);
            denominator *= (-1);
        }
        
    }

    //internal helper functions

    //Euclid's algorithm for GCD of two numbers
    int Fraction::gcd(int num1, int num2) const
    {
        //if the fraction is a 0 fraction, this will allow us to reduce it to a 0/1 when calling reduce()
        if(num1 == 0)
        {
            return num2;
        }
        if(num2 == 0)
        {
            return num1;
        }

        return gcd(num2, (num1 % num2));
    }

    //Find lowest common denominator using gcd
    int Fraction::lcm(int num1, int num2) const
    {
        int gcdval = gcd(num1, num2);

        if(gcdval == 1)
        {
            return num1*num2;
        }
        else if(num1 > num2)
        {
            return (num1/gcdval) * num2;
        }
        else
        {
            return (num2/gcdval) * num1;
        }
    }

    /*//Find lowest common denominator using gcd
    int Fraction::lcm(int num1, int num2) const
    {
        int gcdval = gcd(num1, num2), product;

        if(gcdval == 1)
        {   
            //Check for overflow in multiplication, if no overflow, result is saved in 'product'
            if(__builtin_smul_overflow(num1, num2, &product))
            {
                throw std::overflow_error("Overflow in the lcm");
            }
            return product;
        }
        else if(num1 > num2)
        {
            //Check for overflow in multiplication, if no overflow, result is saved in 'product'
            if(__builtin_smul_overflow(num1/gcdval, num2, &product))
            {
                throw std::overflow_error("Overflow in the lcm");
            }
            return product;
        }
        else
        {
            //Check for overflow in multiplication, if no overflow, result is saved in 'product'
             if(__builtin_smul_overflow(num2/gcdval, num1, &product))
             {
                throw std::overflow_error("Overflow in the lcm");
             }
            return product;
        }
    }*/
}