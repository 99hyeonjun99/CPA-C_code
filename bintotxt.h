#ifndef BINTOTXT_H
#define BINTOTXT_H

#define _FOLD_			"C:\\Users\\rptns\\OneDrive - KookminUNIV\\문서\\부채널 분석\\새 폴더\\2. AES-128 alignment"
#define _TRACE_NAME_	"CTF-2-AES-ALIGN-plain"
#define _PLANE_NAME_	"CTF-2-AES-ALIGN-trace"

#define _TRACE_NUM_		8000
#define _FILESIZE_		160

static FILE* fpp; //pt_file
static FILE* fpt; //trace_file

void open_file_to_binary();
void read_file_to_binary();
void close_file_to_binary();

#endif // BINTOTXT_H
