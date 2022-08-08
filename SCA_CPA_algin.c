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
	
	int best_key[16] = { 0, };//����Ű ������ ���� ����
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
			for (int j = 0; j < PI; j++) {
				W_HS[i][j] = 0; //E[XY]
			}
		
		}
		_fseeki64(fpt, 16 + (_END_POINT_  + _START_POINT_ - 1) * 16, SEEK_SET);
		float pot;
		fread(&pot, sizeof(float), 1, fpt);
		//����Ʈ�� �����ϴ� �Լ�
		for (int tn = 0; tn < _TRACE_NUM_; tn++) {

			//���� �ҷ����� ������ E[X], E[Y] �� ���
			
			float F_TEMP;
			_fseeki64(fpt, 16 + (_END_POINT_ * tn + _START_POINT_ - 1) * 16, SEEK_SET);
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
				//haming weight ���
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
				float Correlation_L = _TRACE_NUM_ * W_HS[guess_key][pi] - H_S[guess_key] * W_CS[pi]; //���� ������ ���
				//�и� ������ ���
				float Correlation_R = (_TRACE_NUM_ * H_SS[guess_key] - (H_S[guess_key] * H_S[guess_key])) * (_TRACE_NUM_ * W_CSS[pi] - (W_CS[pi] * W_CS[pi]));

				if (Correlation_R <= (float)0) {//�и� 0�̰ų� �����ϰ�� 0���� ����
					Correlation = 0;
				}
				else {
					Correlation = Correlation_L / sqrt(Correlation_R);
					Correlation = fabs(Correlation);
				}

				if (max < Correlation) {//1000�� ���鼭 �������� ���� ū ���� max�� ����
					max = Correlation;
				}

			}
			guesses_key[guess_key] = max; //guesses_key�� max�� �����Ͽ� ���߿� ���� ū Ű�� �ĺ�Ű ����

		}

		//�ĺ�Ű�� ���ϱ�
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

		//�ĺ�Ű�� ���
		printf("%d-th key\n", round + 1);
		fprintf(fp, "%d-th key\n", round + 1);

		for (int i = 0; i < _CANDIDATE_; i++) {
			printf("candidate %d= %x, Correlate= %f ", i + 1, key[i], Corr[i]);
			fprintf(fp, "candidate %d= %x, Correlate= %f ", i + 1, key[i], Corr[i]);
			printf("\n");
			fprintf(fp, "\n");
		}
		//�������� ���� ū Ű�� ����Ű�� ����
		best_key[round] = key[0];
	}
	//���� Ű ���
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
	
	fclose(fp); //���� ������ �ݱ�

}

void close_file() {
	fclose(fpp);
	fclose(fpt);
	
}





