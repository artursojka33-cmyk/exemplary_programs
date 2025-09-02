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
string entropyArray[64];
string hash_string;
string tmp33;
int counter = 0;
ofstream myFileIn;
int iteration = 0;
int which_iteration = 0;
string PC_1_Array[56];
int hexCounter = 0;
string currentHex = "";
string original = "";
string leftArray[28];
string rightArray[28];
string PC_2_Array[16];
string keyArray[16];
string key = "";
int whichKey = 0;

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

string HexToBin(string hex) {

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

string BinToHex(string bin) {

    long int i = 0;
    string tmp = "";
    string bin_tmp = "";
    int counter = 0;
   
    for (int i = 0; i < bin.length(); i++) {

        bin_tmp += bin[i];
        counter += 1;

        if (counter == 4) {

        if (bin_tmp == "0000") {

            tmp += "0";

        }

        else if (bin_tmp == "0001") {

            tmp += "1";

        }

        else if (bin_tmp == "0010") {

            tmp += "2";

        }
        else if (bin_tmp == "0011") {

            tmp += "3";

        }
        else if (bin_tmp == "0100") {

            tmp += "4";

        }
        else if (bin_tmp == "0101") {

            tmp += "5";

        }
        else if (bin_tmp == "0110") {

            tmp += "6";

        }
        else if (bin_tmp == "0111") {

            tmp += "7";

        }

        else if (bin_tmp == "1000") {

            tmp += "8";

        }
        else if (bin_tmp == "1001") {

            tmp += "9";

        }
        else if (bin_tmp == "1010") {

            tmp += "A";

        }
        else if (bin_tmp == "1011") {

            tmp += "B";

        }
        else if (bin_tmp == "1100") {

            tmp += "C";

        }
        else if (bin_tmp == "1101") {

            tmp += "D";

        }
        else if (bin_tmp == "1110") {

            tmp += "E";

        }
        else if (bin_tmp == "1111") {

            tmp += "F";

        }
            counter = 0;
            bin_tmp = "";

        }

    }

    return tmp;
}

void get64bits() {

    auto start = high_resolution_clock::now();

    for (int i = 0; i < 64; i++) {

        //timestamp obtained from memory access and hashing operations

        jitter(i);

        //hashing previous entropy pool state, time delta and iteration

        string bit_iteration = toBit(i); //iteration to bit

        //check whether its the first loop, mash the strings together

        if (i == 0) {

            hash_string = createString("0", "0", bit_iteration);

        }

        else {

            hash_string = createString(entropyArray[i - 1], timeDelta_array[i-1], bit_iteration);

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
        cout << ".";

    }

    //extra randomization
    which_iteration = rand() % 64 + 1;
}

void PC_1() {
    //one less (array iteration)
    int first = 56;

    for (int i = 0; i < 28; i++) {

        if (first < 0) {

            first += 65;

        }

        for (int j = 0; j < entropyArray[which_iteration-1].length(); j++) {

            if (j == first) {

                PC_1_Array[i] = (entropyArray[which_iteration-1])[j];
                break;

            }

        }

        first -= 8;

    }
    //one less (array iteration)
    first = 62;

    for (int i = 28; i < 56; i++) {

        if (first < 0) {

            if (i == 51) {

                first = 27;

            }

            else {

                first += 63;
                
            }

        }

        for (int j = 0; j < entropyArray[which_iteration-1].length(); j++) {

            if (j == first) {

                PC_1_Array[i] = (entropyArray[which_iteration-1])[j];
                break;

            }

        }

        first -= 8;

    }

}

void decToHex(int number) {

    int tmp = 0;
    string hex = "";

    while (number > 0) {

        tmp = number % 16;

        if (tmp > 9) {

            switch (tmp) {

            case 10: hex = "A" + hex; break;
            case 11: hex = "B" + hex; break;
            case 12: hex = "C" + hex; break;
            case 13: hex = "D" + hex; break;
            case 14: hex = "E" + hex; break;
            case 15: hex = "F" + hex; break;


            }

        }

        else {

            hex = char(tmp + 48) + hex;

        }

        number /= 16;

    }

    if (hex == "") {

        currentHex = "0";

    }

    else {

        currentHex = hex;

    }

}

void stringToHex(string input, string hexArray[]) {

    string tmp = "";
    int count = 0;

    for (int i = 0; i < input.length(); i++) {

        decToHex(int(input[i]));
        tmp += currentHex;

        for (int j = 0; j < tmp.length(); j++) {

            count += 1;

        }

        if (count == 16) {

            hexArray[hexCounter] = tmp;
            tmp = "";
            hexCounter += 1;

        }

        count = 0;

    }

    for (int i = 0; i < tmp.length(); i++) {

        count += 1;

    }

    if (count < 16) {

        for (int j = count; j < 16; j++) {

            tmp += "0";

        }

        hexArray[hexCounter] = tmp;
        hexCounter += 1;

    }

}

void hexToString(string hexArray[]) {

    string hex = "";
    int count = 0;

    for (int i = 0; i < hexCounter; i++) {

        for (int j = 0; j < hexArray[i].length(); j++) {

            hex += (hexArray[i])[j];

            count += 1;

            if (count == 2) {
            
                unsigned int number = stoul(hex, nullptr, 16);
                original += char(number);
                hex = "";
                count = 0;

            }

        }

    }

}

void splitIntoHalfs() {

    int j = 0;

    for (int i = 0; i < 28; i++) {

        leftArray[i] = PC_1_Array[j];
        j += 1;

    }

    for (int i = 0; i < 28; i++) {

        rightArray[i] = PC_1_Array[j];
        j += 1;

    }

}

void joinBack() {

    for (int i = 0; i < 28; i++) {

        key += leftArray[i];

    }

    for (int i = 0; i < 28; i++) {

        key += rightArray[i];

    }

}

void bitShift() {

        string tmp = "";
        string tmp_ = "";
        string tmp2 = "";
        string tmp2_ = "";
        int counter = 0;
        int doubleShift = 0;
        
        for (int i = 0; i < 16; i++) {

            if (doubleShift == 6) {

                doubleShift = 0;
                counter = 1;

            }

            switch (counter) {

                case 0:

                    tmp = leftArray[0];
                    tmp2 = rightArray[0];


                    for (int j = 0; j < 27; j++) {

                        leftArray[j] = leftArray[j + 1];
                        rightArray[j] = rightArray[j + 1];

                    }

                    leftArray[27] = tmp;
                    rightArray[27] = tmp2;

                    joinBack();
                    keyArray[i] = key;
                    key = "";

                    counter += 1;
                    break;

                case 1:

                    tmp = leftArray[0];
                    tmp2 = rightArray[0];


                    for (int j = 0; j < 27; j++) {

                        leftArray[j] = leftArray[j + 1];
                        rightArray[j] = rightArray[j + 1];

                    }

                    leftArray[27] = tmp;
                    rightArray[27] = tmp2;

                    joinBack();
                    keyArray[i] = key;
                    key = "";

                    counter += 1;
                    break;

                case 2:

                    tmp = leftArray[0];
                    tmp_ = leftArray[1];
                    tmp2 = rightArray[0];
                    tmp2_ = rightArray[1];


                    for (int j = 0; j < 26; j++) {

                        leftArray[j] = leftArray[j + 1];
                        rightArray[j] = rightArray[j + 1];

                    }

                    leftArray[26] = tmp;
                    rightArray[26] = tmp2;
                    leftArray[27] = tmp_;
                    rightArray[27] = tmp2_;

                    joinBack();
                    keyArray[i] = key;
                    key = "";

                    doubleShift += 1;
                    break;

                default:
                    break;

            }


        }
        

}


void PC_2() {

    for (int i = 0; i < 16; i++) {
        //one less (array iteration)
        key += (keyArray[i])[13];
        key += (keyArray[i])[16];
        key += (keyArray[i])[10];
        key += (keyArray[i])[23];
        key += (keyArray[i])[0];
        key += (keyArray[i])[4];
        key += (keyArray[i])[2];
        key += (keyArray[i])[27];
        key += (keyArray[i])[14];
        key += (keyArray[i])[5];
        key += (keyArray[i])[20];
        key += (keyArray[i])[9];
        key += (keyArray[i])[22];
        key += (keyArray[i])[18];
        key += (keyArray[i])[11];
        key += (keyArray[i])[3];
        key += (keyArray[i])[25];
        key += (keyArray[i])[7];
        key += (keyArray[i])[15];
        key += (keyArray[i])[6];
        key += (keyArray[i])[26];
        key += (keyArray[i])[19];
        key += (keyArray[i])[12];
        key += (keyArray[i])[1];
        key += (keyArray[i])[40];
        key += (keyArray[i])[51];
        key += (keyArray[i])[30];
        key += (keyArray[i])[36];
        key += (keyArray[i])[46];
        key += (keyArray[i])[54];
        key += (keyArray[i])[29];
        key += (keyArray[i])[39];
        key += (keyArray[i])[50];
        key += (keyArray[i])[44];
        key += (keyArray[i])[32];
        key += (keyArray[i])[47];
        key += (keyArray[i])[43];
        key += (keyArray[i])[48];
        key += (keyArray[i])[38];
        key += (keyArray[i])[55];
        key += (keyArray[i])[33];
        key += (keyArray[i])[52];
        key += (keyArray[i])[45];
        key += (keyArray[i])[41];
        key += (keyArray[i])[49];
        key += (keyArray[i])[35];
        key += (keyArray[i])[28];
        key += (keyArray[i])[31];

        PC_2_Array[i] = key;
        key = "";

    }

   

}

void initialPermutation(string binArray[], string permArray[]) {

    //one less (array iteration)
    int first = 57;

    for (int i = 0; i < hexCounter; i++) {

        permArray[i] = "";
        first = 57;

        for (int j = 0; j < binArray[i].length(); j++) {

            if (first < 0) {

                first += 66;

            }

            if (first == 65) {

                first = 56;

            }
 
            permArray[i] += (binArray[i])[first];

            first -= 8;

        }

    }


}

void createBinArray(string arrayBin[], string arrayHex[]) {

    for (int i = 0; i < hexCounter; i++) {

        arrayBin[i] = HexToBin(arrayHex[i]);

    }

}

void splitPerm(string leftA[], string rightA[], string permArray[]) {

    for (int l = 0; l < hexCounter; l++) {

        leftA[l] = "";
        rightA[l] = "";

        for (int i = 0; i < 32; i++) {
          
            leftA[l] += (permArray[l])[i];

        }

        for (int i = 32; i < 64; i++) {

            rightA[l] += (permArray[l])[i];

        }

    }

    

}

void expandRight(string rightA[], string expanded[]) {

    string expansion = "";
    int first = 31;

    for (int j = 0; j < hexCounter; j++) {

        first = 31;

        for (int i = 0; i < 48; i++) {

            if (first == 32) {

                first = 0;

            }

            expansion += (rightA[j])[first];
            first++;

        }

        expanded[j] = expansion;
        expansion = "";

    }

}

void keyAndRightXor(string rightArr[], string xoredArr[], int whichKey) {

    string xored_string = "";
    int rightNumber = 0;
    int keyNumber = 0;
    int xored_int = 0;
    string tmp;

    for (int j = 0; j < hexCounter; j++) {

        xored_string = "";

        for (int i = 0; i < PC_2_Array[whichKey].length(); i++) {

            tmp = (rightArr[j])[i];
            if (tmp == "0") {

                rightNumber = 0;

            }

            else if(tmp == "1"){

                rightNumber= 1;

            }

            tmp = (PC_2_Array[whichKey])[i];

            if (tmp == "0") {

                keyNumber = 0;

            }

            else if (tmp == "1") {

                keyNumber = 1;

            }

            xored_int = rightNumber xor keyNumber;

            xored_string += to_string(xored_int);

        }

        xoredArr[j] = xored_string;

    }
        
}

int stringBinToInt(string bin) {

    int tmp = 0;

    if (bin == "00") {

        tmp = 0;
        return tmp;

    }

    else if (bin == "01") {

        tmp = 1;
        return tmp;

    }

    else if (bin == "10") {

        tmp = 2;
        return tmp;

    }

    else if (bin == "11") {

        tmp = 3;
        return tmp;

    }

    else if (bin == "0000") {

        tmp = 0;
        return tmp;

    }

    else if (bin == "0001") {

        tmp = 1;
        return tmp;

    }

    else if (bin == "0010") {

        tmp = 2;
        return tmp;

    }
    else if (bin == "0011") {

        tmp = 3;
        return tmp;

    }
    else if (bin == "0100") {

        tmp = 4;
        return tmp;

    }
    else if (bin == "0101") {

        tmp = 5;
        return tmp;

    }
    else if (bin == "0110") {

        tmp = 6;
        return tmp;

    }
    else if (bin == "0111") {

        tmp = 7;
        return tmp;

    }

    else if (bin == "1000") {

        tmp = 8;
        return tmp;

    }
    else if (bin == "1001") {

        tmp = 9;
        return tmp;

    }
    else if (bin == "1010") {

        tmp = 10;
        return tmp;

    }
    else if (bin == "1011") {

        tmp = 11;
        return tmp;

    }
    else if (bin == "1100") {

    tmp = 12;
    return tmp;

    }
    else if (bin == "1101") {

    tmp = 13;
    return tmp;

    }
    else if (bin == "1110") {

    tmp = 14;
    return tmp;

    }
    else if (bin == "1111") {

    tmp = 15;
    return tmp;

    }

}

string intToBin(int number) {

    string tmp;

    if (number == 0) {

        tmp = "0000";
        return tmp;

    }

    else if (number == 1) {

        tmp = "0001";
        return tmp;

    }

    else if (number == 2) {

        tmp = "0010";
        return tmp;

    }
    else if (number == 3) {

        tmp = "0011";
        return tmp;

    }
    else if (number == 4) {

        tmp = "0100";
        return tmp;

    }
    else if (number == 5) {

        tmp = "0101";
        return tmp;

    }
    else if (number == 6) {

        tmp = "0110";
        return tmp;

    }
    else if (number == 7) {

        tmp = "0111";
        return tmp;

    }
    else if (number == 8) {

        tmp = "1000";
        return tmp;

    }
    else if (number == 9) {

        tmp = "1001";
        return tmp;

    }
    else if (number == 10) {

        tmp = "1010";
        return tmp;
    }
    else if (number == 11) {

        tmp = "1011";
        return tmp;

    }
    else if (number == 12) {

        tmp = "1100";
        return tmp;

    }
    else if (number == 13) {

        tmp = "1101";
        return tmp;

    }
    else if (number == 14) {

        tmp = "1110";
        return tmp;

    }
    else if (number == 15) {

        tmp = "1111";
        return tmp;

    }
}

void SBlockCoding(string xored[], string rightA[]) {

    string keyedMessage = "";
    string firstAndLast = "";
    string middle = "";
    int counter = 0;
    int whichS = 1;
    int middleInt = 0;
    int firstAndLastInt = 0;

    int S1[4][16] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7
        , 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8
        , 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0
        , 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },

        S2[4][16] = { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10
        , 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5
        , 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15
        , 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },

        S3[4][16] = { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8
        , 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1
        , 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7
        , 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },

        S4[4][16] = { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15
        , 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9
        , 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4
        , 3, 15, 0 ,6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },

        S5[4][16] = { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9
        , 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6
        , 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14
        , 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },

        S6[4][16] = { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11
        , 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8
        , 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6
        , 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },

        S7[4][16] = { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1
        , 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6
        , 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2
        , 6 ,11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },

        S8[4][16] = { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12,7
        , 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2
        , 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8
        , 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 };


    for (int j = 0; j < hexCounter; j++) {

        keyedMessage = "";
        whichS = 1;
        counter = 0;
        firstAndLast = "";
        middle = "";
        
        for (int i = 0; i < xored[j].length(); i++) {

            if (counter == 6) {

                firstAndLastInt = stringBinToInt(firstAndLast);
                middleInt = stringBinToInt(middle);
                string tmp;

                switch(whichS) {

                case 1:

                    tmp = intToBin(S1[firstAndLastInt][middleInt]);
                    keyedMessage += tmp;
                    break;

                case 2:

                    tmp = intToBin(S2[firstAndLastInt][middleInt]);
                    keyedMessage += tmp;
                    break;

                case 3:

                    tmp = intToBin(S3[firstAndLastInt][middleInt]);
                    keyedMessage += tmp;
                    break;

                case 4:

                    tmp = intToBin(S4[firstAndLastInt][middleInt]);
                    keyedMessage += tmp;
                    break;

                case 5:

                    tmp = intToBin(S5[firstAndLastInt][middleInt]);
                    keyedMessage += tmp;
                    break;

                case 6:

                    tmp = intToBin(S6[firstAndLastInt][middleInt]);
                    keyedMessage += tmp;
                    break;

                case 7:

                    tmp = intToBin(S7[firstAndLastInt][middleInt]);
                    keyedMessage += tmp;
                    break;

                default:
                    break;

                }

                counter = 0;
                whichS += 1;
                firstAndLast = "";
                middle = "";
            }

            if (counter == 0 || counter == 5) {

                firstAndLast += (xored[j])[i];

            }

            else if (counter > 0 && counter < 5) {

                middle += (xored[j])[i];

            }

            counter += 1;
        }

        firstAndLastInt = stringBinToInt(firstAndLast);
        middleInt = stringBinToInt(middle);
        string tmp;
        tmp = intToBin(S8[firstAndLastInt][middleInt]);
        keyedMessage += tmp;
        rightA[j] = keyedMessage;

    }



}

void permutationP(string rightA[], string permArr[]) {

    string tmp = "";

    for (int i = 0; i < hexCounter; i++) {

        tmp = "";

        tmp += (rightA[i])[15];
        tmp += (rightA[i])[6];
        tmp += (rightA[i])[19];
        tmp += (rightA[i])[20];
        tmp += (rightA[i])[28];
        tmp += (rightA[i])[11];
        tmp += (rightA[i])[27];
        tmp += (rightA[i])[16];
        tmp += (rightA[i])[0];
        tmp += (rightA[i])[14];
        tmp += (rightA[i])[22];
        tmp += (rightA[i])[25];
        tmp += (rightA[i])[4];
        tmp += (rightA[i])[17];
        tmp += (rightA[i])[30];
        tmp += (rightA[i])[9];
        tmp += (rightA[i])[1];
        tmp += (rightA[i])[7];
        tmp += (rightA[i])[23];
        tmp += (rightA[i])[13];
        tmp += (rightA[i])[31];
        tmp += (rightA[i])[26];
        tmp += (rightA[i])[2];
        tmp += (rightA[i])[8];
        tmp += (rightA[i])[18];
        tmp += (rightA[i])[12];
        tmp += (rightA[i])[29];
        tmp += (rightA[i])[5];
        tmp += (rightA[i])[21];
        tmp += (rightA[i])[10];
        tmp += (rightA[i])[3];
        tmp += (rightA[i])[24];

        permArr[i] = tmp;

    }

}

void xorLeftAndF(string leftArr[], string fArr[], string rArr[]) {

    string xored = "";
    string tmp;
    int functionNumber;
    int leftNumber;
    int xored_int;

    for (int i = 0; i < hexCounter; i++) {

        xored = "";

        for (int j = 0; j < 32; j++) {

            tmp = (leftArr[i])[j];
            if (tmp == "0") {

                leftNumber = 0;

            }

            else if (tmp == "1") {

               leftNumber = 1;

            }

            tmp = (fArr[i])[j];

            if (tmp == "0") {

                functionNumber = 0;

            }

            else if (tmp == "1") {

                functionNumber = 1;

            }

            xored_int = leftNumber xor functionNumber;
            xored += to_string(xored_int);

        }

        leftArr[i] = rArr[i];
        rArr[i] = xored;
    }
    
    
}

void finalPermutation(string lArr[], string rArr[], string finalArr[]) {

    string tmp = "";
    bool substract = 1;
    int first = 39; //one less (iteration)
    int count = 0;

    for (int i = 0; i < hexCounter; i++) {

        tmp += rArr[i];
        tmp += lArr[i];
        finalArr[i] = tmp;

        tmp = "";
        first = 39;
        count = 0;

        for (int j = 0; j < 64; j++) {

            tmp += (finalArr[i])[first];

            if (count == 7) {

                first += 7;
                count = 0;
                substract = 1;

            }

            else if (substract) {

                first -= 32;
                substract = 0;
                count += 1;

            }

            else {

                first += 40;
                substract = 1;
                count += 1;

            }
        }

        finalArr[i] = tmp;
        tmp = "";

    }

}

int main() {

    string hexArray[1000];
    string binArray[1000];
    string initialPermArray[1000];
    string leftArray[1000];
    string rightArray[1000];
    string expandedRight[1000];
    string xoredRight[1000];
    string sBoxedRight[1000];
    string permP[1000];
    string finalPerm[1000];
    string decodedBin[1000];
    string message;

    cout << "Please input a message you'd like to encode." << endl;
    getline(cin, message);

    stringToHex(message, hexArray);

    cout << "Message converted into hex: " << endl;

    for (int i = 0; i < hexCounter; i++) {

        cout << hexArray[i] << " ";

    }
   
    cout << endl << "Generating bits please wait..." << endl;

    get64bits();
    PC_1();

    cout << endl;

    splitIntoHalfs();
    bitShift();

    PC_2();

    createBinArray(binArray, hexArray);
    initialPermutation(binArray, initialPermArray);
    splitPerm(leftArray, rightArray, initialPermArray);

    for (int i = 0; i < 16; i++) {

        expandRight(rightArray, expandedRight);
        keyAndRightXor(expandedRight, xoredRight, i); //16 different keys
        SBlockCoding(xoredRight, sBoxedRight);
        permutationP(sBoxedRight, permP);
        xorLeftAndF(leftArray, permP, rightArray);

    }

    finalPermutation(leftArray, rightArray, finalPerm);
    cout << "Encoded hex: ";

    for (int i = 0; i < hexCounter; i++) {

        cout << BinToHex(finalPerm[i]) << " ";

    }

    cout << endl;
   
    //decoding

    initialPermutation(finalPerm, initialPermArray);
    splitPerm(leftArray, rightArray, initialPermArray);

    for (int i = 15; i >= 0; i--) {

        expandRight(rightArray, expandedRight);
        keyAndRightXor(expandedRight, xoredRight, i); //16 different keys
        SBlockCoding(xoredRight, sBoxedRight);
        permutationP(sBoxedRight, permP);
        xorLeftAndF(leftArray, permP, rightArray);

    }

    finalPermutation(leftArray, rightArray, finalPerm);

    cout << "Decoded hex: ";

    for (int i = 0; i < hexCounter; i++) {

        decodedBin[i] = BinToHex(finalPerm[i]);
        cout << BinToHex(finalPerm[i]) << " ";

    }

    cout << endl;
    
    hexToString(decodedBin);

    cout << "Decoded message: "<< original << endl;

    return 0;
}