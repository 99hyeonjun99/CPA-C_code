#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "bintotxt.h"


void open_file_to_binary() {
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

void read_file_to_binary() {
	FILE* fp, *fptt;	//plain   trace
	fopen_s(&fp, "plain.txt","w");
	fopen_s(&fptt,"trace.txt", "w");
	char P_TEMP;
	_fseeki64(fpt, 16, SEEK_SET);
	int cnt= 0;
	while (1) {
		cnt++; 
		fread(&P_TEMP, sizeof(char), 1, fpt);
		fprintf(fp, "%hhx ", P_TEMP);
		if (cnt % 16 == 0) {
			fprintf(fp, "\n");
		}


		if (feof(fpt))
			break;
	}

	_fseeki64(fpp, 16, SEEK_SET);

	float F_TEMP;
	while(1) {
		
		fread(&F_TEMP, sizeof(float), 1, fpp);
		
		fprintf(fptt, "%f ", F_TEMP);
		
		
		fprintf(fptt,"\n");
		
		if (feof(fpp)) 
			break;
	}
	
	
	
	fclose(fptt);
	fclose(fp);
}

void close_file_to_binary() {
	fclose(fpp);
	fclose(fpt);

}