#include <stdio.h>
#include <string.h>
#include <math.h>

//블록사이즈
#define _BLOCKSIZE_			16
#define _KEY_SIZE_			16
#define _ROUND_KEY_SIZE_	16
#define _ROUND_				10

//폴더
#define _FOLD_				"C:\\Users\\rptns\\OneDrive - KookminUNIV\\문서\\부채널 분석\\sca project"
#define _TRACE_NAME_		"2022.05.20-17.58.50-2022.05.20-17.58.50_1000tr_932pt"
#define _PLANE_NAME_		"2022.05.20-17.58.50-2022.05.20-17.58.50_1000tr_932pt_plain"
#define _FILESIZE_			150

//파형 수
#define _TRACE_NUM_			1000

//분석 시작 바이트와 끝 바이트
#define _START_BYTE_		1
#define _END_BYTE_			3

#define _START_POINT_		1
#define _END_POINT_			233
#define PI					_END_POINT_*4

//분석 시작할 키와 수
#define _GUESS_KEY_START	0
#define _GUESS_KEY_NUM		255

//출력할 후보 키 수
#define _CANDIDATE_			5

//각 후보 키에 대한 상관계수 분석 결과 (미출력 0 출력 1)
#define _CORRELANT_TRACE_	1

//각 후보 키에 대한 상관계수 최대값 출력(미출력 0 출력 1)
//trace unit은 출력할 파형 단위 수
#define _MAX_PEAK_TRACE		0
#if _MAX_PEAK_TRACE_
#define _TRACE_UNIT_		10
#endif

static int Sbox[256] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };



static FILE* fpp; //pt_file
static FILE* fpt; //trace_file


void open_file() {
	char trace_file[256];
	char pt_file[256];

	
	
	sprintf_s(trace_file, _FILESIZE_ * sizeof(char), "%s\\%s.trace", _FOLD_, _TRACE_NAME_);
	sprintf_s(pt_file, _FILESIZE_ * sizeof(char), "%s\\%s.txt", _FOLD_, _PLANE_NAME_);
	
	fopen_s(&fpp, pt_file, "r");
	if (fpp == NULL) {
		printf("-------------------------------------\n");
		printf("			Failed Read\n");
		printf("--------------------------------------\n");
	
	}

	fopen_s(&fpt, trace_file, "rb");
	if (fpt == NULL) {
		printf("-------------------------------------\n");
		printf("			Failed Read\n");
		printf("--------------------------------------\n");

	}

	
}

void read_file() {
	int best_key[16] = { 0, };//예측키 저장을 위해 선언
	for (int round = 0; round < 16; round++) {
		char plaintext[_BLOCKSIZE_];
		float Temp_point[PI] = { 0.0, };
		float W_CS[PI] = { 0.0, }; //E[Y]
		float W_CSS[PI] = { 0.0, }; //E[Y^2]
		float H_S[_GUESS_KEY_NUM] = { 0, }; //E[X]
		float H_SS[_GUESS_KEY_NUM] = { 0, }; //E[X^2]
		float W_HS[_GUESS_KEY_NUM][PI] = { 0, }; //E[XY]

		//리스트에 저장하는 함수
		for (int tn = 0; tn < _TRACE_NUM_; tn++) {

			//파일 불러오고 상관계수 E[X], E[Y] 등 계산
			_fseeki64(fpt, 32 + (_END_POINT_ * tn + _START_POINT_ - 1) * 16, SEEK_SET);
			for (int pi = 0; pi < PI; pi++) {
				float F_TEMP;
				fread(&F_TEMP, sizeof(float), 1, fpt);
				Temp_point[pi] = F_TEMP;
				
				W_CS[pi] += Temp_point[pi];
				W_CSS[pi] += Temp_point[pi] * Temp_point[pi];
			}

			_fseeki64(fpp, (_BLOCKSIZE_ * 3 + 2) * tn, SEEK_SET);
			for (int i = 0; i < _BLOCKSIZE_; i++) {
				fscanf_s(fpp, "%hhx", &plaintext[i]);

			}
			for (int guess_key = 0; guess_key < _GUESS_KEY_NUM; guess_key++) {
				int key = Sbox[(plaintext[round]&0xff) ^ guess_key];
				//haming weight 계산
				int key_HW = (key & 0x01) + ((key >> 1) & 0x01) + ((key >> 2) & 0x01) + ((key >> 3) & 0x01)+((key >> 4) & 0x01)+ ((key >> 5) & 0x01)+ ((key >> 6) & 0x01)+ ((key >> 7 ) & 0x01);
				

				H_S[guess_key] += key_HW;
				H_SS[guess_key] += key_HW * key_HW;
				for (int pi = 0; pi < PI; pi++) {
					W_HS[guess_key][pi] += (double)key_HW * Temp_point[pi];
				}
			}
		}
		float guesses_key[256] = { 0, };
		for (int guess_key = 0; guess_key < _GUESS_KEY_NUM; guess_key++) {
			float max = 0;
			for (int pi = 0; pi < PI; pi++) {
				float Correlation;
				float Correlation_L = _TRACE_NUM_ * W_HS[guess_key][pi] - H_S[guess_key] * W_CS[pi]; //분자 상관계수 계산
				//분모 상관계수 계산
				float Correlation_R = (_TRACE_NUM_ * H_SS[guess_key] - (H_S[guess_key] * H_S[guess_key])) * (_TRACE_NUM_ * W_CSS[pi] -( W_CS[pi] * W_CS[pi]));

				if (Correlation_R <= (float)0) {//분모가 0이거나 음수일경우 0으로 저장
					Correlation = 0;
				}
				else {
					Correlation = Correlation_L / sqrt(Correlation_R);
					Correlation = fabs(Correlation);
				}

				if (max < Correlation) {//1000번 돌면서 상관계수가 제일 큰 값을 max에 저장
					max = Correlation;
				}
				
			}
			guesses_key[guess_key] = max; //guesses_key에 max를 저장하여 나중에 제일 큰 키를 후보키 선정
	
		}

		//후보키들 구하기
		double Corr[_CANDIDATE_+1] = {0,};
		int key[_CANDIDATE_+1] = { 0, };
		
		for (int guess_key = 0; guess_key < _GUESS_KEY_NUM; guess_key++) { 
			for (int i = 0; i < _CANDIDATE_; i++) {
				if (guesses_key[guess_key] > Corr[i]) { 
					Corr[i + 1] = Corr[i];
					key[i + 1] = key[i];
					Corr[i] = guesses_key[guess_key];
					key[i] = guess_key;
					break;
				}
			}
			
		}

		//후보키들 출력
		printf("%d-th key\n", round + 1);
		for (int i = 0; i < _CANDIDATE_; i++) {
			printf("candidate %d= %x, Correlate= %f ",i+1, key[i], Corr[i]);
			printf("\n");
			
		}
		//상관계수가 제일 큰 키를 예측키에 저장
		best_key[round] = key[0];
		}
	//예측 키 출력
	for (int i = 0; i < 16; i++) {
		printf("%x ", best_key[i]);
	}
	
}





int main() {
	open_file();
	read_file();
}