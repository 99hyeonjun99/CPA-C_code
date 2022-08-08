#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "SCA_CPA.h"



void open_file() {
	char trace_file[256];
	char pt_file[256];

	

	sprintf_s(trace_file, _FILESIZE_ * sizeof(char), "%s\\%s.bin", _FOLD_, _TRACE_NAME_);
	sprintf_s(pt_file, _FILESIZE_ * sizeof(char), "%s\\%s.bin", _FOLD_, _PLANE_NAME_);

	fopen_s(&fpp, pt_file, "rb");
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
	FILE* fp;
	fopen_s(&fp,"result.txt", "w");
	char plaintext[_BLOCKSIZE_];
	float *Temp_point, * W_CS, * W_CSS, * H_S, *H_SS, **W_HS, * guesses_key;
	float W_CS1,  W_CSS1,  H_S1,  H_SS1, W_HS1;
	
	Temp_point = (float*)malloc(sizeof(float) * PI);
	W_CS= (float*)malloc(sizeof(float) * PI);				//E[Y]
	W_CSS = (float*)malloc(sizeof(float) * PI);				//E[Y^2]
	H_S = (int*)malloc(sizeof(int) * _GUESS_KEY_NUM);	//E[X]
	H_SS = (int*)malloc(sizeof(int) * _GUESS_KEY_NUM);	//E[X^2]
	W_HS = (float**)malloc(sizeof(float*) * _GUESS_KEY_NUM);
	
	guesses_key = (float*)malloc(sizeof(int) * (_GUESS_KEY_NUM + 1));
	for (int i = 0; i < _GUESS_KEY_NUM; i++) {
		W_HS[i] = (float*)malloc(sizeof(float) * PI);
		
	}

	
	int best_key[16] = { 0, };//예측키 저장을 위해 선언
	float F_TEMP;
	for (int round = 0; round < 16; round++) {
		
		for (int i = 0; i < PI; i++) {
			Temp_point[i] = 0;
			W_CS[i] = 0; //E[Y]
			W_CSS[i] = 0; //E[Y^2]
			
			
		}
		for (int i = 0; i < _GUESS_KEY_NUM; i++) {
			H_S[i] =  0; //E[X]
			H_SS[i] =  0; //E[X^2]
			for (int j = 0; j < PI; j++)
				W_HS[i][j] = 0;
		
		
		
		}

		/*/AES-ALGIN
		float F_TEMP;
		H_S1 = 0;
		H_SS1 = 0;
		_fseeki64(fpt, 16 + (_START_POINT_ - 1) * 16, SEEK_SET);
		for (int pi = 0; pi < PI; pi++) {


			fread(&F_TEMP, sizeof(float), 1, fpt);

			Temp_point[pi] = F_TEMP;

			H_S1 += Temp_point[pi];
			H_SS1 += Temp_point[pi] * Temp_point[pi];
		}

		int max_LR[500];
		max_LR[0] = 0;
		for (int tn = 1; tn < _TRACE_NUM_; tn++) {
			//파일 불러오고 상관계수 E[X], E[Y] 등 계산
			float max=0;
			int max_left;
			if (round == 0) {
				for (int left = 0; left < 200; left++) {

					W_CS1 = 0;				//E[Y]
					W_CSS1 = 0;				//E[Y^2]
					W_HS1 = 0;
					_fseeki64(fpt, 16 + (_END_POINT_ * tn + _START_POINT_ - 100 + left) * 4, SEEK_SET);
					for (int pi = 0; pi < PI; pi++) {

						fread(&F_TEMP, sizeof(float), 1, fpt);
						W_HS1 += F_TEMP * Temp_point[pi]; //E[X][Y]


						W_CS1 += F_TEMP;
						W_CSS1 += F_TEMP * F_TEMP;
					}

					float Correlation = 0;
					float Correlation_L = (PI)*W_HS1 - H_S1 * W_CS1; //분자 상관계수 계산
					//분모 상관계수 계산
					float Correlation_R = ((PI)*H_SS1 - (H_S1 * H_S1)) * ((PI)*W_CSS1 - (W_CS1 * W_CS1));


					if (Correlation_R <= (float)0) {//분모가 0이거나 음수일경우 0으로 저장
						Correlation = 0;
					}
					else {
						Correlation = Correlation_L / sqrt(Correlation_R);
						Correlation = fabs(Correlation);
					}
					if (max < Correlation) {//1000번 돌면서 상관계수가 제일 큰 값을 max에 저장
						max = Correlation;
						max_left = left;

					}

				}
				max_LR[tn] = max_left;
			}
		}

		//END
		*/
		for (int i = 0; i < PI; i++) 
			Temp_point[i] = 0;
		_fseeki64(fpt, 16 + (_END_POINT_  + _START_POINT_ - 1) * 16, SEEK_SET);
		float pot;
		fread(&pot, sizeof(float), 1, fpt);
		//리스트에 저장하는 함수
		for (int tn = 0; tn < _TRACE_NUM_; tn++) {

			//파일 불러오고 상관계수 E[X], E[Y] 등 계산
			
			_fseeki64(fpt, 16 + (_END_POINT_ * tn + _START_POINT_ - 1)*4, SEEK_SET);
			for (int pi = 0; pi < PI; pi++) {
				
				fread(&F_TEMP, sizeof(float), 1, fpt);
				
				Temp_point[pi] = F_TEMP;
		
				W_CS[pi] += Temp_point[pi];
				W_CSS[pi] += Temp_point[pi] * Temp_point[pi];
				
			}
			

			_fseeki64(fpp, 16 + (16*tn), SEEK_SET);
			for (int i = 0; i < _BLOCKSIZE_; i++) {
				int P_TEMP;
				fread(&P_TEMP, sizeof(char),1,fpp);

				plaintext[i] = P_TEMP;
				

			}
			
			for (int guess_key = 0; guess_key < _GUESS_KEY_NUM; guess_key++) {
				int key = Sbox[(plaintext[round] & 0xff) ^ guess_key];
				//haming weight 계산
				int key_HW = (key & 0x01) + ((key >> 1) & 0x01) + ((key >> 2) & 0x01) + ((key >> 3) & 0x01) + ((key >> 4) & 0x01) + ((key >> 5) & 0x01) + ((key >> 6) & 0x01) + ((key >> 7) & 0x01);


				H_S[guess_key] += key_HW;
				H_SS[guess_key] += key_HW * key_HW;
				for (int pi = 0; pi < PI; pi++) {
					W_HS[guess_key][pi] += (double)key_HW * Temp_point[pi];
				}
			}
		}
		for (int i = 0; i < _GUESS_KEY_NUM; i++) {
			guesses_key[i] = 0;
		}
		
		for (int guess_key = 0; guess_key < _GUESS_KEY_NUM; guess_key++) {
			float max = 0;
			for (int pi = 0; pi < PI; pi++) {
				float Correlation;
				float Correlation_L = _TRACE_NUM_ * W_HS[guess_key][pi] - H_S[guess_key] * W_CS[pi]; //분자 상관계수 계산
				//분모 상관계수 계산
				float Correlation_R = (_TRACE_NUM_ * H_SS[guess_key] - (H_S[guess_key] * H_S[guess_key])) * (_TRACE_NUM_ * W_CSS[pi] - (W_CS[pi] * W_CS[pi]));

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
		double Corr[_CANDIDATE_ + 1] = { 0, };
		int key[_CANDIDATE_ + 1] = { 0, };

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
		fprintf(fp, "%d-th key\n", round + 1);

		for (int i = 0; i < _CANDIDATE_; i++) {
			printf("candidate %d= %x, Correlate= %f ", i + 1, key[i], Corr[i]);
			fprintf(fp, "candidate %d= %x, Correlate= %f ", i + 1, key[i], Corr[i]);
			printf("\n");
			fprintf(fp, "\n");
		}
		//상관계수가 제일 큰 키를 예측키에 저장
		best_key[round] = key[0];
	}
	//예측 키 출력
	for (int i = 0; i < 16; i++) {
		printf("%x ", best_key[i]);
		fprintf(fp, "%x ", best_key[i]);
	}
	free(Temp_point);
	free(W_CS);
	free(W_CSS);
	free(H_S);
	free(H_SS);
	for (int i = 0; i < _GUESS_KEY_NUM; i++) {
		free(W_HS[i]);
	}
	free(W_HS);
	
	fclose(fp); //파일 포인터 닫기

}

void close_file() {
	fclose(fpp);
	fclose(fpt);
	
}





