//
// Created by aram on 5/13/23.
//

#ifndef APOLLO_SERVER_RSA_H
#define APOLLO_SERVER_RSA_H

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstring>
#include <numeric>
#include <random>

using std::vector;
using std::string;

struct PrivateKey {
    int n;
    int d;
};

struct PublicKey {
    int n;
    int e;
};

struct Keypair {
    PublicKey pub;
    PrivateKey pri;
};

bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int generatePrimeNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100, 200);
    int n = dis(gen);

    while (!isPrime(n)) {
        n = dis(gen);
    }

    return n;
}

int modInverse(int a, int m) {
    int m0 = m;
    int y = 0, x = 1;

    while (a > 1) {
        int q = a / m;
        int t = m;

        m = a % m, a = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < 0) {
        x += m0;
    }

    return x;
}

int powerMod(int a, int b, int m) {
    int result = 1;
    a %= m;
    while (b > 0) {
        if (b & 1) {
            result = (result * a) % m;
        }
        a = (a * a) % m;
        b >>= 1;
    }
    return result;
}

void generateKeys(Keypair &keys) {
    int p, q;
    do {
        p = generatePrimeNumber();
        q = generatePrimeNumber();
    } while (p == q);

    int n = p * q;
    int phi = (p - 1) * (q - 1);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(2, phi);
    int e, d;

    do {
        e = dis(gen);
    } while (std::gcd(e, phi) != 1);

    do {
        d = modInverse(e, phi);
    } while (e == d);

    keys.pri.d = d;
    keys.pri.n = n;
    keys.pub.e = e;
    keys.pub.n = n;
}

vector<int> encrypt(PublicKey pub, string text) {
    unsigned int len = text.size();
    vector<int> cypher(len);

    for (int i = 0; i < len; i++) {
        cypher[i] = powerMod((int) text[i], pub.e, pub.n);
    }

    return cypher;
}

string decrypt(PrivateKey pri, vector<int> cypher) {
    unsigned int len = cypher.size();
    string decrypt = "";

    for (int i = 0; i < len; i++) {
        decrypt += (char) powerMod(cypher[i], pri.d, pri.n);
    }

    return decrypt;
}

#endif //APOLLO_SERVER_RSA_H
