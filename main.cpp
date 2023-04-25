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
uint64_t decrypt(uint64_t x, uint64_t d, uint64_t N) {
    return modPow(x, d, N);
}

string encrypt(const string& s, uint64_t e, uint64_t N) {
    size_t len = s.length();
    uint64_t y[len];
    for(size_t i = 0; i < len; i++) {
        y[i] = encrypt((uint64_t)s[i], e, N);
    }
    string res;
    res.resize(len * 8);
    memcpy(&res[0], y, len * 8);
    return res;
}
string decrypt(const string& s, uint64_t d, uint64_t N) {
    size_t len = s.length();
    
    uint64_t y[len / 8];
    memcpy(y, &s[0], len);
        
    string res;
    res.resize(len / 8);

    for(size_t i = 0; i < len / 8; i++) {
        res[i] = (char)decrypt(y[i], d, N);
    }
    return res;
}
vector <uint64_t> factorize(uint64_t n) {
    uint64_t up = (uint64_t)sqrt(n) + 1; // upper bound
    vector<uint64_t> v;
    for(uint64_t i = 2; i < up; i++) {
        if(n % i == 0) {
            v.push_back(i);
            v.push_back(n / i);
        }
    }
    return v;
    
} // returns vector od divisors
string hack(const string& s, uint64_t e, uint64_t N) {
    // calculating private key
    vector <uint64_t> v = factorize(N);
    uint64_t p = v[0], q = v[1];
    uint64_t d = (uint64_t)inverse((int64_t)e, (int64_t)(p - 1)*(q-1));
    
    return decrypt(s, d, N);
} // decrypt using only public key

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
    
    // reading from file
    ifstream fin("input.txt");
    ofstream fenc("encrypt.txt");
    ofstream fdec("decrypt.txt");
    ofstream fhack("hack.txt");
    
    chrono::steady_clock::time_point begin, end; 
    
    // reading input text
    string tmp, text = "";
    while(getline(fin, tmp)) {
        text += tmp;
    };
    // encrypting
    begin = std::chrono::steady_clock::now();
    string encrypted = encrypt(text, e, N);
    end = std::chrono::steady_clock::now();
    // write in file
    fenc << encrypted << endl;
    fenc << "total elapsed time: ";
    fenc << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    fenc << " microseconds";
    // decrypting
    begin = std::chrono::steady_clock::now();
    string decrypted = decrypt(encrypted, d, N);
    end = std::chrono::steady_clock::now();
    // write in file
    fdec << decrypted << endl;
    fdec << "total elapsed time: ";
    fdec << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    fdec << " microseconds";
    // hacking
    begin = std::chrono::steady_clock::now();
    string hacked = hack(encrypted, e, N);
    end = std::chrono::steady_clock::now();
    // write in file
    fhack << hacked << endl;
    fhack << "total elapsed time: ";
    fhack << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    fhack << " microseconds";
    
}
