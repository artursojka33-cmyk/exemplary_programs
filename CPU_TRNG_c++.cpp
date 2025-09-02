#include "TCHAR.h"
#include "pdh.h"
#include <iostream>
#include <algorithm>
#include <windows.h>
#include "../digestpp-master/algorithm/sha3.hpp"
#include <chrono>
#include<string>
#include <sstream>
#include <bitset>
#include <fstream>

#pragma comment(lib,"pdh.lib")

using namespace std;
using namespace std::chrono;

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;

static double CalculateCPULoad(unsigned long long iTicks, unsigned long long tTicks) //idle ticks and total ticks
{
    static unsigned long long _pTTicks = 0;
    static unsigned long long _pITicks = 0;

    unsigned long long tTSinceLastTime = tTicks - _pTTicks;
    unsigned long long iTSinceLastTime = iTicks - _pITicks;

    float ret = 1.0f - ((tTSinceLastTime > 0) ? ((float)iTSinceLastTime) / tTSinceLastTime : 0);

    _pTTicks = tTicks;
    _pITicks = iTicks;
    return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME& ft) { return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); }

double GetCPULoad()
{
    FILETIME iTime, kTime, uTime; //idle time, kernel time, user time
    return GetSystemTimes(&iTime, &kTime, &uTime) ? CalculateCPULoad(FileTimeToInt64(iTime), FileTimeToInt64(kTime) + FileTimeToInt64(uTime)) : -1.0f;
}

long int timestamp = 0;
long int timestamp_array[100000];
string timeDelta_array[100000];
long double jitter_array[100000];
string entropyArray[100000];
string hash_string;
string tmp33;
int counter = 0;
ofstream myFileIn;

//int to bit

string toBit(int number) {

    string tmp;
    bool Pierw_Jedynka = false;
    const int a = 1;
    const int b = 0;

    for (unsigned long long int mask = 0x8000000000000000; mask > 0; mask = mask >> 1)
    {
        if ((number & mask) != 0)
        {
            Pierw_Jedynka = true;

            tmp = tmp + to_string(a);


        }
        else
        {
            if (Pierw_Jedynka == true)
            {
                if ((number & mask) != 0)
                {

                    tmp = tmp + to_string(a);
                }
                else
                    tmp = tmp + to_string(b);

            }
        }

    }

    return tmp;

}


void timeDelta(long int timestamp, int i) {

    //save a timestamp

    if (i == 0)
    {

        timestamp_array[i] = timestamp;

    }

    else
    {
        timestamp_array[i] = (timestamp + timestamp_array[i - 1]);
    }


    //count a time delta

    if (i == 0) {

        timeDelta_array[i] = toBit(timestamp);

    }

    else {

        double diff = timestamp_array[i] - timestamp_array[i - 1];

        timeDelta_array[i] = toBit(diff);

    }

}

void jitter(int i) {

    //get jitter

    Sleep(250);

    double currentCPU = GetCPULoad();

    currentCPU *= 100;

    //save

    jitter_array[i] = currentCPU;

}

//string masher

string createString(string entropy, string timeDelta, string iteration) {

    string tmp;

    tmp = tmp + entropy + timeDelta + iteration;

    return tmp;
}

//convert hex to bin

string HexToBin(string hex){

    long int i = 0;
    string tmp;

    while (hex[i]) {

        switch (hex[i]) {
        case '0':
            tmp += "0000";
            break;
        case '1':
            tmp += "0001";
            break;
        case '2':
            tmp += "0010";
            break;
        case '3':
            tmp += "0011";
            break;
        case '4':
            tmp += "0100";
            break;
        case '5':
            tmp += "0101";
            break;
        case '6':
            tmp += "0110";
            break;
        case '7':
            tmp += "0111";
            break;
        case '8':
            tmp += "1000";
            break;
        case '9':
            tmp += "1001";
            break;
        case 'A':
        case 'a':
            tmp += "1010";
            break;
        case 'B':
        case 'b':
            tmp += "1011";
            break;
        case 'C':
        case 'c':
            tmp += "1100";
            break;
        case 'D':
        case 'd':
            tmp += "1101";
            break;
        case 'E':
        case 'e':
            tmp += "1110";
            break;
        case 'F':
        case 'f':
            tmp += "1111";
            break;
        default:
            break;
        }
        i++;
    }

    return tmp;
}

int main() {

    //data in

    myFileIn.open("bitsout.txt");

    for (int i = 0; i < 10000; i++) {

        //timestamp obtained from memory access and hashing operations

        auto start = high_resolution_clock::now();

        jitter(i);

        //hashing previous entropy pool state, time delta and iteration

        string bit_iteration = toBit(i); //iteration to bit

        //check whether its the first loop, mash the strings together

        if (i == 0) {

            hash_string = createString("0", "0", bit_iteration);

        }

        else {

            hash_string = createString(entropyArray[i-1], timeDelta_array[i], bit_iteration);

        }

        //hashing operation

        digestpp::sha3 hasher(256);
        hasher.absorb(hash_string);
        string entropy = hasher.hexdigest();

        //end recording a timestamp

        auto end = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(end - start);

        timestamp = long int(duration.count());

        //count a timeDelta

        timeDelta(timestamp, i);

        //count bin number from hex

        entropyArray[i] = HexToBin(entropy);

        //check data

        /*cout << "Timestamp: " << timestamp_array[i] << " ";
        cout << " Time_Delta: " << timeDelta_array[i] << " ";
        cout << " CPU usage: " << jitter_array[i] << " ";
        cout << " Entropy_hex: " << entropy << " ";
        cout <<  " Entropy_bin: " << entropyArray[i] << endl;*/

        //out

        cout << entropyArray[i];


        //save to file
        string tmp = entropyArray[i];

        for (int j = 0; j < tmp.length(); j++) {

            myFileIn << tmp[j];

            counter++;

            if (counter == 8) {

                myFileIn << endl;
                counter = 0;

            }

        }

    }

    myFileIn.close();
    return 0;
}