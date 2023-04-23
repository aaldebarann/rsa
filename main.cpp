#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <chrono>
#include <cmath>
#include <vector>

using namespace std;

const size_t N = 210;

__uint128_t modPow(__uint128_t base, __uint128_t exp, __uint128_t m) {
    if(exp == 0) {
        return 1;
    } else  {
        __uint128_t tmp = modPow(base, exp/2, m);
        if(exp % 2 == 0) {
            return (tmp * tmp % m ) ;
        } else {
            return (tmp * tmp * base % m );
        }
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
    for(int i = 0; i < t; i++) {
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

char encrypt(char x, uint64_t N, uint64_t e) {
    return modPow((uint64_t)x, e, N);
}
string encrypt(const string& s, uint64_t N, uint64_t e) {
    string result;
    result.resize(s.length());
    for(int i = 0; i < s.length(); i++)
        result[i] = encrypt(s[i], N, e);
    return result;
}
char decrypt(char x, uint64_t N, uint64_t d) {
    return modPow((uint64_t)x, d, N);
}
string decrypt(const string& s, uint64_t N, uint64_t d) {
    string result;
    result.resize(s.length());
    for(int i = 0; i < s.length(); i++)
        result[i] = decrypt(s[i], N, d);
    return result;
}

int main() {
    uint64_t N, p, q, e, d;
    p = 12004991;
    q = 12004999;
    N = p*q;
    e = 7;
    d = (uint64_t)inverse((int64_t)e, (int64_t)(p - 1)*(q-1));
    cout << d << endl;
    cout << N << endl;

    char c = 'q';
    char cc = encrypt(c, N, e);
    char ccc = decrypt(c, N ,d);
    cout << c << endl << cc << endl << ccc << endl;


    string str = "Don't panic!";
    cout << str << endl;
    string y = encrypt(str, N, e);
    cout<<"y='" << y<<"'" << endl;
    cout <<"x[0]='" <<(int)decrypt(y[0], N, d)<<"'" << endl;
    cout<<"x='" << encrypt(y, N, d)<<"'" << endl;

}
