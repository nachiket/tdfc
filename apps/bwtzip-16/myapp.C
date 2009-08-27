#include "ScoreStream.h"
#include "entire.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

long int file_size(FILE *fp) {
    long int orig = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long int size = ftell(fp);
    fseek(fp, orig, SEEK_SET);
    return size;
}

void file_close(FILE *fp) {
    if (fclose(fp) == EOF) {
        cout << "ERROR: Problem while closing file." << endl;
        exit(EXIT_FAILURE);
    }
}

FILE * file_open_rb(const char *name) {
    FILE *fp = fopen(name, "rb");
    if (fp == NULL) {
        cout << "ERROR: Cannot open " << name << " for input." << endl;
        exit(EXIT_FAILURE);
    }
    return fp;
}

FILE * file_open_wb(const char *name) {
    if (fopen(name, "rb") != NULL) {
        cout << "ERROR: Output file " << name << " already exists." << endl;
        exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(name, "wb");
    if (fp == NULL) {
        cout << "ERROR: Cannot open file " << name << " for output." << endl;
        exit(EXIT_FAILURE);
    }
    return fp;
}

#define FETCH_ME_MY_SEGMENTS(num) \
    UNSIGNED_SCORE_SEGMENT     v ## num = NEW_UNSIGNED_SCORE_SEGMENT(5001, 9 ); \
      SIGNED_SCORE_SEGMENT   pos ## num =   NEW_SIGNED_SCORE_SEGMENT(5001, 32); \
      SIGNED_SCORE_SEGMENT   prm ## num =   NEW_SIGNED_SCORE_SEGMENT(5001, 32); \
      SIGNED_SCORE_SEGMENT count ## num =   NEW_SIGNED_SCORE_SEGMENT(5001, 32); \
     BOOLEAN_SCORE_SEGMENT    bh ## num =  NEW_BOOLEAN_SCORE_SEGMENT(5002, 1 ); \
     BOOLEAN_SCORE_SEGMENT   b2h ## num =  NEW_BOOLEAN_SCORE_SEGMENT(5002, 1 ); \
    UNSIGNED_SCORE_SEGMENT stack ## num = NEW_UNSIGNED_SCORE_SEGMENT( 256, 8 ); \
    UNSIGNED_SCORE_SEGMENT  freq ## num = NEW_UNSIGNED_SCORE_SEGMENT( 257, 32); \
    UNSIGNED_SCORE_SEGMENT cfreq ## num = NEW_UNSIGNED_SCORE_SEGMENT( 258, 32);

#define LIST_ME_MY_SEGMENTS(num) v ## num, pos ## num, prm ## num, count ## num, \
    bh ## num, b2h ## num, stack ## num, freq ## num, cfreq ## num,

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "USAGE: myapp infile outfile" << endl;
        exit(EXIT_SUCCESS);
    }

    FILE *ifp = file_open_rb(argv[1]);
    FILE *ofp = file_open_wb(argv[2]);

    score_init();

    UNSIGNED_SCORE_STREAM  w     = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);
    UNSIGNED_SCORE_STREAM  r     = NEW_READ_UNSIGNED_SCORE_STREAM(8);

// Begin varying parallelization here

    FETCH_ME_MY_SEGMENTS(1)
    FETCH_ME_MY_SEGMENTS(2)
    FETCH_ME_MY_SEGMENTS(3)
    FETCH_ME_MY_SEGMENTS(4)
    FETCH_ME_MY_SEGMENTS(5)
    FETCH_ME_MY_SEGMENTS(6)
    FETCH_ME_MY_SEGMENTS(7)
    FETCH_ME_MY_SEGMENTS(8)
    FETCH_ME_MY_SEGMENTS(9)
    FETCH_ME_MY_SEGMENTS(10)
    FETCH_ME_MY_SEGMENTS(11)
    FETCH_ME_MY_SEGMENTS(12)
    FETCH_ME_MY_SEGMENTS(13)
    FETCH_ME_MY_SEGMENTS(14)
    FETCH_ME_MY_SEGMENTS(15)
    FETCH_ME_MY_SEGMENTS(16)

    NEW_entire(w,
        LIST_ME_MY_SEGMENTS(1)
        LIST_ME_MY_SEGMENTS(2)
        LIST_ME_MY_SEGMENTS(3)
        LIST_ME_MY_SEGMENTS(4)
        LIST_ME_MY_SEGMENTS(5)
        LIST_ME_MY_SEGMENTS(6)
        LIST_ME_MY_SEGMENTS(7)
        LIST_ME_MY_SEGMENTS(8)
        LIST_ME_MY_SEGMENTS(9)
        LIST_ME_MY_SEGMENTS(10)
        LIST_ME_MY_SEGMENTS(11)
        LIST_ME_MY_SEGMENTS(12)
        LIST_ME_MY_SEGMENTS(13)
        LIST_ME_MY_SEGMENTS(14)
        LIST_ME_MY_SEGMENTS(15)
        LIST_ME_MY_SEGMENTS(16)
    r);

// End varying parallelization here

    for (size_t i = 0; i < file_size(ifp); i++) {
        STREAM_WRITE(w, getc(ifp));
    }

    STREAM_CLOSE(w);
    file_close(ifp);

    while (!STREAM_EOS(r)) {
        unsigned char byte;
        STREAM_READ(r, byte);
        putc(byte, ofp);
    }

    STREAM_FREE(r);
    file_close(ofp);
    score_exit();
}
