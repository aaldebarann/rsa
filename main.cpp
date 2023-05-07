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
vector<uint8_t> encrypt(vector<uint8_t> str, uint64_t e, uint64_t N) {
    // encrypt text by 4-byte blocks

    uint64_t original_size = str.size(); // original text size
    // make sure vector size divides by 4
    while(str.size() % 4 != 0)
        str.push_back(' ');
    // push back original size
    for(int i = 0; i < 8; i++) {
        str.push_back(' ');
    }
    memcpy(&str[str.size()], &original_size, 8);

    vector<uint8_t> encrypted(str.size() * 2);
    uint32_t x;
    uint64_t y;
    for(size_t i = 0; i < str.size() / 4; i++) {
        memcpy(&x, &str[i * 4], 4);
        // encrypt x -> y
        y = encrypt((uint64_t)x, e, N);
        memcpy(&encrypted[i * 8], &y, 8);
    }
    return encrypted;
}
vector<uint8_t> decrypt(const vector<uint8_t>& str, uint64_t d, uint64_t N) {

    vector<uint8_t> decrypted(str.size() / 2);

    uint64_t y;
    uint32_t x;
    for(size_t i = 0; i < str.size() / 8; i++) {
        memcpy(&y, &str[i * 8], 8);
        // decrypt y -> x
        x = (uint32_t)decrypt(y, d, N);
        memcpy(&decrypted[i * 4], &x, 4);
    }
    // last 8 bytes - original text size
    uint64_t original_size;
    memcpy(&original_size, &decrypted[decrypted.size() - 8], 8);
    // delete useless bytes
    while(decrypted.size() > original_size)
        decrypted.pop_back();
    return decrypted;
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
    p = 12004999;
    q = 12004991;
    e = 7;
    N = p*q;
    d = (uint64_t)inverse((int64_t)e, (int64_t)(p - 1)*(q-1));

    // reading from file
    ifstream fin("input.txt", std::ios::binary);
    ofstream fenc("encrypt.txt", std::ios::binary);
    ofstream fdec("decrypt.txt", std::ios::binary);
    ofstream fhack("hack.txt", std::ios::binary);

    chrono::steady_clock::time_point begin, end, begin1, end1;

    // reading input text
    std::vector<uint8_t> text(std::istreambuf_iterator<char>(fin), {});

    begin = std::chrono::steady_clock::now();
    vector<uint8_t> encrypted = encrypt(text, e, N);
    end = std::chrono::steady_clock::now();
    // write in file
    for(uint8_t c: encrypted)
        fenc << c;
    fenc << endl;
    fenc << "total elapsed time: ";
    fenc << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    fenc << " microseconds";
    // decrypting
    begin = std::chrono::steady_clock::now();
    vector<uint8_t> decrypted = decrypt(encrypted, d, N);
    end = std::chrono::steady_clock::now();
    // write in file
    for(uint8_t c: decrypted)
        fdec << c;
    fdec << endl;
    fdec << "total elapsed time: ";
    fdec << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    fdec << " microseconds";
    // hacking
    begin = std::chrono::steady_clock::now();
    uint64_t dHack = hack(e, N);
    end1 = std::chrono::steady_clock::now();
    vector<uint8_t> hacked = decrypt(encrypted, dHack, N);
    end = std::chrono::steady_clock::now();
    // write in file
    for(uint8_t c: hacked)
        fhack << c;
    fhack << endl;
    fhack << "hacking time: ";
    fhack << chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();
    fhack << " microseconds" << endl;
    fhack << "total elapsed time: ";
    fhack << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    fhack << " microseconds";
}
