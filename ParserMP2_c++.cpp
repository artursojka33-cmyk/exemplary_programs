#include <iostream>
#include <fstream>
#include "stdint.h"
#include <string>
#pragma warning(disable:4996)

using namespace std;

//Lab2 variables
const int size_ = 188;
uint8_t array_header[size_];  //array for purposes of storing the header information, 8 bits
int counter = 0; //counts the number of the packet currently displayed

int SB; //8 bits
int E; //1 bit
int S; //1 bit
int P; //1 bit
int PID; //13 bits
int TSC; //2 bits
int AF; //2 bits
int CC; //4 bits

//

//Lab3 variables

int AFL; //8 bits
int DC; //all 1 bit
int RA;
int SPI;
int PR;
int OR;
int SPF;
int TP;
int EX;

//

//Lab4 variables

int Stuffing;
int PSCP; //24 bits
int S_ID; //8 bits
int PES_length; //16 bits
int PES_total_length;

//

//Lab5 variables
int PTS_DTS_I;
int ESCR_F;
int ES_R_F;
int DSM_TMF;
int ACIF;
int CRCF;
int EF;
int PDF;
int PHFF;
int PPSCF;
int PBF;
int PEF_2;
int HeaderLength=14;
int Payload = 2880;  // DataLength = pes_total_length - header length

//

//Lab2 functions

void getHeaderSegments(uint8_t array[188]) { //function allowing the segmentation of the header

	unsigned int mask = 0x80;

	SB = array[0];

	E = bool(array[1] & mask);
	mask >>= 1;

	S = bool(array[1] & mask);
	mask >>= 1;

	P = bool(array[1] & mask);
	mask -= 1;

	PID = array[1] & mask;
	mask = 0xff;

	PID <<= 8;
	PID |= array[2] & mask;
	mask = 0xc0;

	TSC = array[3] & mask;
	mask >>= 2;
	TSC >>= 6;

	AF = array[3] & mask;
	mask >>= 2;
	mask += 3;
	AF >>= 4;

	CC = array[3] & mask;

	counter++;
}

//

//Lab3 functions

void getAFSegments(uint8_t array[188]) { // function allowing the segmentation of AF

	unsigned int mask = 0x80;

	AFL = array[4];

	DC = array[5] & mask;
	mask >>= 1;

	RA = array[5] & mask;
	mask >>= 1;

	SPI = array[5] & mask;
	mask >>= 1;

	PR = array[5] & mask;
	mask >>= 1;

	OR = array[5] & mask;
	mask >>= 1;

	SPF = array[5] & mask;
	mask >>= 1;

	TP = array[5] & mask;
	mask >>= 1;

	EX = array[5] & mask;
	mask >>= 1;

	Stuffing = AFL - 1;

}

//

//Lab 4 functions

void getPESinfo(uint8_t array[188], int startingPoint) {

	PSCP = array[startingPoint];
	PSCP <<= 8;

	PSCP += array[startingPoint + 1];
	PSCP <<= 8;

	PSCP += array[startingPoint + 2];

	S_ID = array[startingPoint + 3];

	PES_length = array[startingPoint + 4];
	PES_length <<= 8;

	PES_length += array[startingPoint + 5];

}

void getLengthInfo() {


	if (S == 1 && PID == 136) {

		PES_total_length = 0;

	}

	if (AF < 2 && PID == 136) {

		PES_total_length += 184;

	}

	else if (PID == 136) {

		PES_total_length += 183 - AFL;

	}

}


//

//Lab5 functions

void checkPESHeader(uint8_t array[188], int startingPoint) { //AFL +11

	HeaderLength = 0;

	HeaderLength += 9;

	int additional_Starting_Point = startingPoint;

	if (S_ID != 0xBC && S_ID != 0xBE && S_ID != 0xBF && S_ID != 0xF0 && S_ID != 0xF1 && S_ID != 0xFF && S_ID != 0xF2 && S_ID != 0xF8) {

		unsigned int mask = 0xc0;

		PTS_DTS_I = array[startingPoint] & mask;

		PTS_DTS_I >>= 6;

		if (PTS_DTS_I == 2) {

			HeaderLength += 5;
			additional_Starting_Point += 5;

		}

		else if (PTS_DTS_I == 3) {

			HeaderLength += 10;
			additional_Starting_Point += 10;

		}

		mask = 0x20;

		ESCR_F = bool(array[startingPoint] & mask);

		if (ESCR_F >= 1) {

			HeaderLength += 6;
			additional_Starting_Point += 6;

		}

		mask >>= 1; // 0x10

		ES_R_F = bool(array[startingPoint] & mask);

		if (ES_R_F == 1) {

			HeaderLength += 3;
			additional_Starting_Point += 3;

		}

		mask >>= 1; //0x08

		DSM_TMF = bool(array[startingPoint] & mask);

		mask >>= 1; //0x04

		ACIF = bool(array[startingPoint] & mask);

		if (ACIF == 1) {

			HeaderLength += 1;
			additional_Starting_Point += 1;

		}

		mask >>= 1; //0x02

		CRCF = bool(array[startingPoint] & mask);

		if (CRCF == 1) {

			HeaderLength += 2;
			additional_Starting_Point += 2;

		}

		mask >>= 1; //0x01

		EF = bool(array[startingPoint] & mask);

		if (EF == 1) {

			HeaderLength += 1;

			mask = 0x80;

			PDF = bool(array[additional_Starting_Point] & mask);

			if (PDF == 1) {

				HeaderLength += 2;

			}

			mask >>= 1; //0x40

			PHFF = bool(array[additional_Starting_Point] & mask);

			if (PHFF == 1) {

				HeaderLength += 1;

			}

			mask >>= 1; //0x20

			PPSCF = bool(array[additional_Starting_Point] & mask);

			if (PPSCF == 1) {

				HeaderLength += 2;

			}

			mask >>= 1; //0x10

			PBF = bool(array[additional_Starting_Point] & mask);

			if (PBF == 1) {

				HeaderLength += 2;

			}

			mask >>= 4; //0x01

			PEF_2 = bool(array[additional_Starting_Point] & mask);

			if (PEF_2 == 1) {

				HeaderLength += 2;

			}

		}

	}

}

void saveToFile(uint8_t array_ex[188], int where_at) {

	ofstream output;
	output.open("PID136.mp2", ios::out | ios::app | ios::binary);

	if (where_at == 0) {

		for (int i = HeaderLength + 5 + AFL; i < 188; i++) {

			output << array_ex[i];
	
		}

	}

	else if (where_at == 1) {

		for (int i = 4; i < 188; i++) {

			output << array_ex[i];

		}

	}

	else if (where_at == 2) {

		for (int i = AFL + 5; i < 188; i++) {

			output << array_ex[i];
		}


	}

	output.close();

}

//


int main() {

	int i = 0;
	bool check_AF = 0;
	bool check_started = 0;

	ifstream input("example_new.ts", ios::in | ios::binary);

	while (!input.eof()) {

		array_header[i] = input.get();

		if (i == 187) {

			getHeaderSegments(array_header);

			cout << counter << " TS: SB = " << SB << ", E = " << E << ", S = " << S
				<< ", P = " << P << ", PID = " << PID << ", TSC = " << TSC << ", AF = " << AF << ", CC = " << CC;

			if (AF > 1) {

				getAFSegments(array_header);
				check_AF = 1;

			}

			getLengthInfo();

			if (S == 1 && PID == 136 && AF > 1 && check_started == 0) {

				getPESinfo(array_header, AFL + 5);

				checkPESHeader(array_header, AFL + 11);

				cout << " Started | PES: PSCP = " << PSCP << ", SID = " << S_ID << ", L = " << PES_length << endl;
				check_started = 1;

				saveToFile(array_header, 0);

			}

			else if (check_started && AF < 2 && PID ==136) {

				cout << " Continue |" << endl;

				saveToFile(array_header, 1);

			}

			else {

				if (check_started && CC == 15 && PID == 136) {

					saveToFile(array_header, 2);

					Payload = PES_total_length - HeaderLength;

					cout << " Finished | PES: PcktLen = " << PES_total_length << ", HeadLen = " << HeaderLength << ", DataLen = " << Payload << endl;
					check_started = 0;


				}

				else {

					cout << endl;

				}
			}

			if (check_AF) {

				cout << "           AF info: L = " << AFL << ", DC = " << DC << ", RA = " << RA << ", SPI = " << SPI
					<< ", PR = " << PR << ", OR = " << OR << ", SPF = " << SPF << ", TP = " << TP << ", EX = " << EX << ", Stuffing = " << Stuffing << endl;

				check_AF = 0;

			}

		}

		i++;

		if (i == 188) {

			i = 0;

		}

	}

	return 0;
}

