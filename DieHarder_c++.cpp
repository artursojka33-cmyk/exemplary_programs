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

static double CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
    static unsigned long long _previousTotalTicks = 0;
    static unsigned long long _previousIdleTicks = 0;

    unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
    unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

    float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

    _previousTotalTicks = totalTicks;
    _previousIdleTicks = idleTicks;
    return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME& ft) { return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); }

double GetCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
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

    Sleep(2);

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

////------DieHarder----\\\\\\

//Test4 - craps

const long int size_ = 9999999;
const long int size22 = 9999999;
int trow = 0; //rzut, docelowo 32 bity z pliku tekstowego
int score = 0; //wynik rzutu dwoma kośćmi 
long long int array_game_outcome[size_]; //wyznacza wynik rzutu win or lose
long long int array_bits[size22];
long long int array_counter[size22];
int i_4 = 0;
long long int number1;
long long int number2;
long double decimal1;
long double decimal2;
int throw1;
int throw2;
int k = 0;
long long int counter_4 = 0;
int g = 0;
long long int win_counter = 0;
long long int overall = 0;
bool just_won = 0;

void one_game(int score) //dwa rzuty kośćmi 
{
    if (counter_4 < 20) {

        counter_4++;
        overall++;

    }


    if (score == 7 or score == 11) { //wygrana

        array_game_outcome[k] = 1;

        array_counter[k] = counter_4;
        counter_4 = 0;

       // cout << "You got: " << throw1 + throw2 << " Game outcome: " << array_game_outcome[g] << endl;
       // cout << "Win! Times it took to win: " << array_counter[g] << endl;
        k++;
        g ++;
        win_counter++;
        overall++;
        just_won = 1;


    }

    else if (score == 2 or score == 3 or score == 12) { //przegrana

        array_game_outcome[k] = 0;
        array_counter[g] = counter_4;
       // cout << "You got: " << throw1 + throw2 << " Game outcome: " << array_game_outcome[k] << endl;
       // cout << "Lose! Number of tries: " << array_counter[g] << endl;
        k++;
        g ++;
        counter_4 = 0;
        overall++;

    }

    cout << ".";

}

void bit_to_decimal() {

    number1 = 0;
    number2 = 0;

    decimal1 = 0;
    decimal2 = 0;

    for (int j = 0; j < 32; j++) {

        number1 += array_bits[i_4 + j];
        number1 <<= 1;
        number2 += array_bits[i_4 + j + 32];
        number2 <<= 1;

    }

    i_4 += 64;

    number1 >>= 1;
    number2 >>= 1;
    
    long long int all = 4294967297; //2^32

    decimal1 = number1;
    decimal2 = number2;

    decimal1 /= all;
    decimal2 /= all;

    decimal1 *= 6;
    decimal2 *= 6;

    throw1 = int(decimal1);
    throw2 = int(decimal2);

    throw1 += 1;
    throw2 += 1;

    //cout << throw1 + throw2 << endl;
}

//Test 7 - Count-the-Ones

const int size_3 = 90000;
int ones_number = 0;
int array_basic[size22];
unsigned int mask = 1;
int i_7 = 0;
string letters_array[size_3];
string words_array_5[size_3];
string repetitions_5[size_3];
string words_array_4[size_3];
string repetitions_4[size_3];
int j_7 = 0; 
int repetitions_counter = 0;
int repetitions_counter_4 = 0;
int repetitions_counter_5 = 0;
int total_words_4 = 0;
int total_words_5 = 0;

//

void letters() {

    if (ones_number >= 0 and ones_number <= 2) {

        letters_array[j_7] = "A";
        ones_number = 0;

    }
    else if (ones_number == 3) {

        letters_array[j_7] = "B";
        ones_number = 0;

    }

    else if (ones_number == 4) {

        letters_array[j_7] = "C";
        ones_number = 0;

    }

    else if (ones_number == 5) {

        letters_array[j_7] = "D";
        ones_number = 0;

    }

    else if (ones_number >= 6 and ones_number <= 8) {

        letters_array[j_7] = "E";
        ones_number = 0;

    }

    j_7++;

}

void count_the_ones() {

    for(int i =0; i< 8; i++){

        if (array_basic[i_7] & mask) {

            ones_number++;

        }

        i_7++;

    }

}

void repetition(int size3) {

    int iteration_counter = 0;

    for (int l = 0; l < size3; l++) {

        if (letters_array[l] == "") {

            break;

        }

        for (int k = 0; k < 5; k++) {

            words_array_5[l] += letters_array[k + l];

        }

        //cout << words_array_5[l]<<endl;
        total_words_5++;
    }

    for (int l = 0; l < size3; l++) {

        if (letters_array[l] == "") {

            break;

        }

        for (int k = 0; k < 4; k++) {

            words_array_4[l] += letters_array[k + l];

        }

        //cout << words_array_4[l] << endl;
        total_words_4++;

    }

    for (int g = 0; g < size3; g++) {

        if (words_array_5[g] == "") {

            break;

        }

        for (int h = g+1; h < size3; h++) {

            if (words_array_5[g] == words_array_5[h]) {

                repetitions_counter++;

            }

            if (words_array_5[h] == "") {
   
                break;

            }
        }

        repetitions_5[g] += to_string(repetitions_counter);
        repetitions_5[g] += words_array_5[g];
        repetitions_counter_5 += repetitions_counter;
        repetitions_counter = 0;
        iteration_counter++;

    }

    //repetitions_counter_5 -= iteration_counter;

    iteration_counter = 0;

    for (int g = 0; g < size3; g++) {

        if (words_array_4[g] == "") {
            break;

        }

        for (int h = g+1; h < size3; h++) {

            if (words_array_4[g] == words_array_4[h]) {

                repetitions_counter++;

            }

            if (words_array_4[h] == "") {

                break;


            }
        }

        repetitions_4[g] += to_string(repetitions_counter);
        repetitions_4[g] += words_array_4[g];
        repetitions_counter_4 += repetitions_counter;
        repetitions_counter = 0;
        iteration_counter++;

    }

   //repetitions_counter_4 -= iteration_counter;

    iteration_counter = 0;

}

//Test 10 - bitstream

const long long int sizee = 2097152;
int array_basic_2[sizee];
int array_bit_words[sizee];
int counter_n = 0;
int the_same_n = 0;
int j_10 = 0;
long long int number_added = 0;
long long int bit_repetitions = 0;
long long int iteration_bit_number = 0;
long long int missing_words = 0;

void nBitStream(int array[], int size_number) {

    //2097152
    for (int i = 0; i < size_number; i++) {

        for (int l = 0; l < 20; l++) {

            number_added += array[j_10 + l];
            number_added <<= 1;

        }

        number_added >>= 1;

        array_bit_words[j_10] = number_added;

        //cout << array_bit_words[j_10] << endl;

        number_added = 0;

        j_10++;

    }

    int iteration_counter = 0;

    for (int g = 0; g < size_number; g++) {

        for (int h = 0; h < size_number; h++) {

            if (array_bit_words[g] == array_bit_words[h]) {

                bit_repetitions++;

            }

            
        }

        iteration_bit_number++;

    }
    bit_repetitions -= iteration_bit_number;

}


int main() {

    //data in
    bool check_one = 0;
    int j_craps = 0;

    myFileIn.open("vector.txt");

    //FILE* pFile;
    //pFile = fopen("vector.txt", "a");

    for (int i = 0; i < 50000; i++) {

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

            hash_string = createString(entropyArray[i - 1], timeDelta_array[i], bit_iteration);

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

       // cout << entropyArray[0];


        //save to file
        string tmp = entropyArray[i];

        //craps

       /* for (int j = 0; j < tmp.length(); j++) {

            //myFileIn << tmp[j];

            if (tmp[j] == 48) {

                array_bits[j_craps] = 0;

            }

            else if (tmp[j] == 49) {


                array_bits[j_craps] = 1;

            }

            else {

                array_bits[j_craps] = 0;

            }
                j_craps++;
           // counter++;

            //if (counter == 8) {

                //myFileIn << endl;
             //   counter = 0;

          //  }

        }

        if (check_one) {

            bit_to_decimal();
            one_game(throw1 + throw2);
            //array_counter[g] i win_counter;
            if (just_won) {

                myFileIn << array_counter[g - 1] << endl;
                just_won = 0;

            }
            //fwrite(&array_counter[g], sizeof(int), sizeof(array_counter[g]), pFile);



        }
        check_one = 1;

       // if (overall >= 200000) {

          //  break;

       // }*/

       //count the ones

        for (int j = 0; j < tmp.length(); j++) {

            if (tmp[j] == 48) {

                array_basic[j_craps] = 0;

            }

            else if (tmp[j] == 49) {


                array_basic[j_craps] = 1;

            }

            else {

                array_basic[j_craps] = 0;

            }
            //cout << array_basic[j_craps] << endl;
            j_craps++;

        }

        count_the_ones();
        letters();
        cout << letters_array[j_7 - 1] << " ";
    }
        //bitstream

       /*for (int j = 0; j < tmp.length(); j++) {

            if (tmp[j] == 48) {

                array_basic_2[j_craps] = 0;

            }

            else if (tmp[j] == 49) {


                array_basic_2[j_craps] = 1;

            }

            else {

                array_basic_2[j_craps] = 0;

            }
 
            //cout << array_basic_2[j_craps];

            j_craps++;

            if (j_craps >= 2097152) {

                break;

            }

        }


    }*/
    //count the ones
    repetition(j_craps);

    for (int i = 0; i < j_craps; i++) {

        string tmp = words_array_5[i];

        if (tmp == "") {


            break;

        }

        myFileIn << tmp << endl;
    }

    for (int i = 0; i < j_craps; i++) {

        string tmp = words_array_5[i];
        double prob = 1;

        if (tmp == "") {


            break;

        }

        for (int j = 0; j < tmp.length(); j++) {


            if (int(tmp[j]) == 65) {

                prob *= 0.1445;

            }

            else if (int(tmp[j]) == 66) {

                prob *= 0.2188;

            }

            else if (int(tmp[j]) == 67) {

                prob *= 0.2734;

            }

            else if (int(tmp[j]) == 68) {

                prob *= 0.2188;

            }

            else if (int(tmp[j]) == 69) {

                prob *= 0.1445;

            }


        }

        myFileIn << prob << endl;

    }

    myFileIn << "--------------------" << endl;

    for (int i = 0; i < j_craps; i++) {

        string tmp = words_array_4[i];

        if (tmp == "") {


            break;

        }

        myFileIn << tmp << endl;
    }

    for (int i = 0; i < j_craps; i++) {

        string tmp = words_array_4[i];
        double prob = 1;

        if (tmp == "") {


            break;

        }

        for (int j = 0; j < tmp.length(); j++) {


            if (int(tmp[j]) == 65) {

                prob *= 0.1445;

            }

            else if (int(tmp[j]) == 66) {

                prob *= 0.2188;

            }

            else if (int(tmp[j]) == 67) {

                prob *= 0.2734;

            }

            else if (int(tmp[j]) == 68) {

                prob *= 0.2188;

            }

            else if (int(tmp[j]) == 69) {

                prob *= 0.1445;

            }


        }

        cout << prob << endl;

        myFileIn << prob << endl;

    }
    //cout << "Repeated 4-letter words found: " << repetitions_counter_4 << " Total 4_words = "<< total_words_4 << " Repeated 5-letter words found: " << repetitions_counter_5 << "Total words 5= " << total_words_5;
//
    //bitstream
   // cout << endl;
   // nBitStream(array_basic_2, j_craps);

   // cout << "Number of total words: " << j_craps << " Number of missing bit words: " << bit_repetitions;
   //fwrite(&win_counter, sizeof(int), sizeof(win_counter), pFile);
   // fclose(pFile);

   // myFileIn << "Total wins: " << win_counter;

    myFileIn.close();
    return 0;
}