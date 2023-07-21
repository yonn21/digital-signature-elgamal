#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <string.h>
#include "./lib/bigint.h"

using namespace std;

// random[a, b]
bigint random(bigint a, bigint b)
{
    if (a > b)
        return random(b, a);
    srand((int)time(0));
    bigint rd = rand();
    return a + rd % (b - a + 1);
}

// a^b mod n
bigint power(bigint base, bigint exponent, bigint mod)
{
    bigint x = 1;
    bigint y = base;
    while (exponent > 0)
    {
        if (exponent % 2 == 1)
            x = (x * y) % mod;

        y = (y * y) % mod;
        exponent = exponent / 2;
    }
    return x % mod;
}

// a^(-1)
bigint modInverse(bigint a, bigint m)
{
    bigint m0 = m;
    bigint y = 0, x = 1;

    if (m == 1)
        return 0;

    while (a > 1)
    {
        bigint q = a / m;
        bigint t = m;

        m = a % m, a = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < 0)
        x += m0;

    return x;
}

bigint hexadecimalToDecimal(string hexVal)
{
    bigint base = 1;
    bigint dec_val = 0;

    int len = hexVal.size();
    for (int i = len - 1; i >= 0; i--)
    {
        if (hexVal[i] >= '0' && hexVal[i] <= '9')
        {
            dec_val += (bigint(hexVal[i]) - 48) * base;
            base = base * 16;
        }
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F')
        {
            dec_val += (bigint(hexVal[i]) - 55) * base;
            base = base * 16;
        }
    }
    return dec_val;
}

void input(bigint &p, bigint &alp, bigint &a, bigint &bet)
{
    cout << "Enter prime p: ";
    cin >> p;
    cout << "Enter alpha: ";
    cin >> alp;
    cout << "Enter a: ";
    cin >> a;
    
    bet = power(alp, a, p);
}

void printKey(bigint p, bigint alp, bigint a, bigint bet)
{
    cout << "== Public key: {" << p << ", " << alp << ", " << bet << "}\n";
    cout << "== Private key: {" << a << "}\n";
}

// sign
void signMessage(bigint p, bigint alp, bigint a)
{
    cout << "\n..........Sign the message............................................................\n";

    // read m
    bool flag = false;
    string nameMsgFile, hash_msg;
    do
    {
        cout << "\n Input the message file hash: ";
        cin >> nameMsgFile;
        ifstream inf(nameMsgFile.c_str());
        if (!inf)
            cout << "    Error: This file does not exist!\n";
        else
        {
            inf >> hash_msg;
            inf.close();
            flag = true;
        }
    } while (!flag);

    bigint m = hexadecimalToDecimal(hash_msg);

    // create key
    bigint r, s;
    short select;
    do
    {
        bigint k = random(1, p);
        while (gcd(k, p - 1) != 1)
            k = random(1, p);

        bigint rev_k = modInverse(k, p - 1);
        r = power(alp, k, p);
        s = (rev_k * (m - a * r)) % (p - 1);
        if (s < 0)
            s += (p - 1);

        cout << "\n k = " << k << endl;
        cout << "\n The digital signature of message: {" << r << ", " << s << "}\n";

        cout << "\n 1 - Next\n 0 - Re-random k\n";
        do
        {
            cout << " Select: ";
            cin >> select;
            if (select < 0 || select > 1)
                cout << " Invalid select!\n";
        } while (select < 0 || select > 1);
    } while (select == 0);

    // save key
    string nameSigFile;
    cout << "\n Save signature to: ";
    cin >> nameSigFile;

    ofstream outf(nameSigFile);
    outf << r << "\n"
         << s;
    cout << "\n Successfully save to " << nameSigFile << "\n";
    outf.close();

    getchar();
    cout << "\n Return to menu";
    getchar();
}

// verify
void verifyMessage(bigint p, bigint alp, bigint bet)
{
    cout << "\n..........Verify the message.............................................................\n";

    // read m
    bool flag = false;
    string nameMsgFile, hash_msg;
    do
    {
        cout << "\n Input the message file hash: ";
        cin >> nameMsgFile;
        ifstream inf(nameMsgFile.c_str());
        if (!inf)
            cout << "    Error: This file does not exist!\n";
        else
        {
            inf >> hash_msg;
            inf.close();
            flag = true;
        }
    } while (!flag);

    bigint m = hexadecimalToDecimal(hash_msg);

    // read key
    bigint v1;
    bigint v2;
    string nameSigFile;
    flag = false;
    do
    {
        cout << "\n Input the signature file to be verify: ";
        cin >> nameSigFile;
        ifstream inf(nameSigFile.c_str());
        if (!inf)
            cout << "    Error: This file does not exist!\n";
        else
        {
            bigint temp, r, s;
            bool i = 1;
            while (inf >> temp)
            {
                if (i == 1)
                    r = temp;
                else
                    s = temp;
                i = 0;
            }

            v1 = (power(bet, r, p) * power(r, s, p)) % p;
            v2 = power(alp, m, p);

            inf.close();
            flag = true;
        }
    } while (!flag);

    // check
    if (v1 != v2)
        cout << "\n False verification!";
    else
        cout << "\n True verification!";

    getchar();
    cout << "\n\n Return to menu";
    getchar();
}

main()
{
    bigint p, alp, a, bet;
    input(p, alp, a, bet);

    short select;
    do
    {
        system("cls");
        printKey(p, alp, a, bet);
        cout << "\n..... MENU ...........\n";
        cout << "1 - Sign the message\n2 - Verify the message\n";
        cout << "3 - Re-enter paraeters\n0 - Exit\n";
        do
        {
            cout << "Select: ";
            cin >> select;
            if (select < 0 || select > 3)
                cout << "Invalid select!\n";
        } while (select < 0 || select > 3);

        switch (select)
        {
        case 1:
            system("cls");
            printKey(p, alp, a, bet);
            signMessage(p, alp, a);
            break;
        case 2:
            system("cls");
            printKey(p, alp, a, bet);
            verifyMessage(p, alp, bet);
            break;
        case 3:
            system("cls");
            input(p, alp, a, bet);
            break;
        }
    } while (select != 0);
}