#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cmath>
#include <vector>
#include <cstring>

using namespace std;

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

// RSA
uint64_t encrypt(uint64_t x, uint64_t e, uint64_t N) {
    return modPow(x, e, N);
}
uint64_t decrypt(uint64_t x, uint64_t d, uint64_t N) {
    return modPow(x, d, N);
}
string encrypt(const string& s, uint64_t e, uint64_t N) {
    // char is encrypted with a uint64_t
    // 1 byte -> 8 bytes
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
    // 8 chars represent 1 source char
    size_t len = s.length();
    uint64_t y[len / 8];
    memcpy(y, &s[0], len);
    // now 1 uint64_t -> 1 char
    string res;
    res.resize(len / 8);
    for(size_t i = 0; i < len / 8; i++) {
        res[i] = (char)decrypt(y[i], d, N);
    }
    return res;
}
vector <uint64_t> factorize(uint64_t n) {
    uint64_t up = (uint64_t)sqrt(n) + 1; // upper bound
    vector<uint64_t> v; // vector of divisors
    for(uint64_t i = 2; i < up; i++) {
        if(n % i == 0) {
            v.push_back(i);
            v.push_back(n / i);
        }
    }
    return v;

} // returns vector of divisors
uint64_t hack(uint64_t e, uint64_t N) {
    // calculating private key
    vector <uint64_t> v = factorize(N);
    uint64_t p = v[0], q = v[1];
    uint64_t d = (uint64_t)inverse((int64_t)e, (int64_t)(p - 1)*(q-1));

    return d;
} // calculating private key "d"

int main() {
    // keys
    uint64_t N, p, q, e, d;
    p = 2002726669;
    q = 2002726723;
    e = 5;
    N = p*q;
    d = (uint64_t)inverse((int64_t)e, (int64_t)(p - 1)*(q-1));

    // reading from file
    ifstream fin("input.txt");
    ofstream fenc("encrypt.txt");
    ofstream fdec("decrypt.txt");
    ofstream fhack("hack.txt");

    chrono::steady_clock::time_point begin, end, begin1, end1;

    // reading input text
    string tmp, text = "";
    while(getline(fin, tmp)) {
        text += tmp;
        text += '\n';
    };
    text.pop_back(); // delete extra '\n'
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
    uint64_t dHack = hack(e, N);
    end1 = std::chrono::steady_clock::now();
    string hacked = decrypt(encrypted, dHack, N);
    end = std::chrono::steady_clock::now();
    // write in file
    fhack << hacked << endl;
    fhack << "hacking time: ";
    fhack << chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();
    fhack << " microseconds" << endl;
    fhack << "total elapsed time: ";
    fhack << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    fhack << " microseconds";

}
