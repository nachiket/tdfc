#include "ScoreStream.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "bwt.h"
#include "bwtsingle.h"
#include "sentineler.h"
using namespace std;

long int file_size(FILE *fp);
void     file_close(FILE *fp);
FILE *   file_open_rb(const char *name);
FILE *   file_open_wb(const char *name);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "USAGE: myapp infile outfile" << endl;
        exit(EXIT_FAILURE);
    }

    FILE *ifp = file_open_rb(argv[1]);
    FILE *ofp = file_open_wb(argv[2]);

    score_init();

    UNSIGNED_SCORE_STREAM  w     = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);

    UNSIGNED_SCORE_SEGMENT v     = NEW_UNSIGNED_SCORE_SEGMENT(5001, 9);
    SIGNED_SCORE_SEGMENT   pos   = NEW_SIGNED_SCORE_SEGMENT(5001, 32);
    SIGNED_SCORE_SEGMENT   prm   = NEW_SIGNED_SCORE_SEGMENT(5001, 32);
    SIGNED_SCORE_SEGMENT   count = NEW_SIGNED_SCORE_SEGMENT(5001, 32);
    BOOLEAN_SCORE_SEGMENT  bh    = NEW_BOOLEAN_SCORE_SEGMENT(5002);
    BOOLEAN_SCORE_SEGMENT  b2h   = NEW_BOOLEAN_SCORE_SEGMENT(5002);

    UNSIGNED_SCORE_STREAM  r     = NEW_READ_UNSIGNED_SCORE_STREAM(8);

    NEW_bwtsingle(w, v, pos, prm, count, bh, b2h, r);

    // Dump file into stream
    for (int i = 0; i < file_size(ifp); i++) {
        unsigned char byte = getc(ifp);
        STREAM_WRITE(w, byte);
    }

    STREAM_CLOSE(w);
    file_close(ifp);

    // Dump stream into file
    while (!STREAM_EOS(r)) {
        unsigned char byte;
        STREAM_READ(r, byte);
        putc(byte, ofp);
    }

    STREAM_FREE(r);
    file_close(ofp);

    score_exit();
}

// Return the size of a given binary file, without changing the position of file
long int file_size(FILE *fp) {
    long int orig = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long int size = ftell(fp);
    fseek(fp, orig, SEEK_SET);
    return size;
}

// Close a file safely
void file_close(FILE *fp) {
    if (fclose(fp) == EOF) {
        cout << "ERROR: Problem while closing file." << endl;
        exit(EXIT_FAILURE);
    }
}

// Open a file for binary read safely
FILE * file_open_rb(const char *name) {
    FILE *fp = fopen(name, "rb");
    if (fp == NULL) {
        cout << "ERROR: Cannot open " << name << " for input." << endl;
        exit(EXIT_FAILURE);
    }
    return fp;
}

// Open a file for binary write safely
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
