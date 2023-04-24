#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <chrono>
#include <cmath>
#include <vector>
#include <cstring>

using namespace std;

const size_t N = 210;

__uint128_t modPow(__uint128_t base, __uint128_t exp, __uint128_t m) {
    if(exp == 0) {
        return 1;
    }
    __uint128_t tmp = modPow(base, exp/2, m);
    if(exp % 2 == 0) {
        return (tmp * tmp) % m  ;
    } else {
        return ((tmp * tmp) % m ) * base % m;
    }
} // returns (base ^ exp) mod m

// primality testing functions
// return true, if n is prime
bool bruteForce(uint64_t n) {
    uint64_t up = (uint64_t)sqrt(n) + 1; // upper bound
    for(uint64_t i = 2; i < up; i++) {
        if(n % i == 0)
            return false;
    }
    return true;
}
bool fermat(uint64_t n) {
    // fermat test for a = 2, a = 3
    if(modPow(2, n - 1, n) == 1 && modPow(3, n - 1, n) == 1)
        return true;
    return false;
}
bool rabinMiller(uint64_t n) {
    if(n % 2 == 0)
        return false;
    if(!fermat(n))
        return false;
    // n - 1 = u * 2^t
    int t = 0;
    int64_t u = n - 1;
    while(u % 2 == 0) {
        t++;
        u /= 2;
    }
    uint64_t a = 2;
    a = modPow(a, u, n);
    if(a == 1)
        return true;
    for(uint64_t i = 0; i < t; i++) {
        uint64_t last = a;
        a = modPow(a, 2, n);
        if(a == 1) {
            if(last == n - 1)
                return true;
            else {
                return false;
            }
        }
    }
    return true;
}

int64_t inverse(int64_t a, int64_t m) {
    // m > 1
    int64_t a1, a2, tmp, r, q, b = m;
    a1 =  1;
    a2 =  0;
    r = a % b;
    q = a / b;
    while(r != 0) {
        a = b;
        b = r;
        r = a % b;
        tmp = a1 - a2 * q;
        a1 = a2;
        a2 = tmp;
        q = a / b;
    }
    if(a2 < 0)
        a2 += m;
    return a2;
} // returns modular inverse

uint64_t encrypt(uint64_t x, uint64_t e, uint64_t N) {
    return modPow((uint64_t)x, e, N);
}
string encrypt(const string& s, uint64_t e, uint64_t N) {
    uint64_t y[s.length()];
    for(size_t i = 0; i < s.length(); i++) {
        y[i] = encrypt((uint64_t)s[i], e, N);
    }
    char cStr[s.length() * 8];
    memcpy(cStr, y, s.length()*8);
    string cppStr{};
    cppStr.resize(s.length()*8);
    for(int i = 0; i < s.length()*8; i++)
        cppStr[i] = cStr[i];
    cout << cppStr<< endl;
    return cppStr;
}
uint64_t decrypt(uint64_t x, uint64_t d, uint64_t N) {
    return modPow(x, d, N);
}
string decrypt(const string& s, uint64_t d, uint64_t N) {
    char result[s.length() / 8];
    const char *cStr = s.c_str();
    for(size_t i = 0; i < s.length() / 8; i++) {
        uint64_t x;
        memcpy(&x, cStr + i*8, 8);
        result[i] = (char)decrypt(x, d, N);
        cout << result[i];
    }
    cout <<"'"<< endl;
    string cppString(result);
    cout << cppString<<endl;
    return cppString;
}

int main() {
    uint64_t N, p, q, e, d;
    p = 12004991;
    q = 12004999;
    e = 7;
    /*
    p = 5;
    q = 7;
    e = 5;
    */
    N = p*q;
    d = (uint64_t)inverse((int64_t)e, (int64_t)(p - 1)*(q-1));
    cout << "d = "<< d << endl;
    cout << "N = " << N << endl;

    uint64_t x = 97;
    uint64_t y = encrypt(x, e, N);
    uint64_t xx = decrypt(y, d, N);
    cout << (uint64_t)x << endl << (uint64_t)y << endl << (uint64_t)xx << endl;

    string s = "aaabbcb";
    cout << "s='" << s << "'" << endl;
    string ss = encrypt(s, e, N);
    cout << "ss='" << ss << "'" << endl;
    string sss = decrypt(ss, e, N);
    cout << "sss='" << sss << "'" << endl;
}


/*

string encrypt(const string& s, uint64_t e, uint64_t N) {
    string result;
    result.resize(s.length() * 8);
    for(size_t i = 0; i < s.length(); i++) {
        // char -> char x 8
        cout << "original char ="<<s[i] << "="<<(uint32_t)s[i] << endl;;
        uint64_t y = encrypt((uint64_t)s[i], e, N);
        cout << "encrypt ="<<y<<endl;
        for(int j = 0; j < 8; j++) {
            cout <<"="<< ((uint32_t)((y >> (7-j)*8) & 255))<<"="<< ((uint8_t)((y >> (7-j)*8) & 255)) << endl;
            result[i*8 + j] = (y >> (7-j)*8) & 255;
        }
    }
    return result;
}
uint64_t decrypt(uint64_t x, uint64_t d, uint64_t N) {
    return modPow(x, d, N);
}
string decrypt(const string& s, uint64_t d, uint64_t N) {
    string result;
    result.resize(s.length() / 8);
    cout << "decr string="<<s<< endl;
    for(size_t i = 0; i < s.length() / 8; i++) {
        uint64_t x = 0;
        for(int j = 0; j < 8; j++) {
            cout<<"x=" << x << endl;
            cout << "byte="<<(uint64_t)s[i*8 + j]<<endl;
            x |= (uint64_t)s[i*8 + j] << (7 - j)*8;
        }
        cout << "was encrypted="<<x<<"="<<(uint8_t)x<<endl;
        result[i] = decrypt(x, d, N);
        cout << "wich is="<<result[i]<<endl;
    }
    return result;
}
*/
