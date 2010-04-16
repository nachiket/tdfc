#include "ScoreStream.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "segment_leaf.h"
#include "segment_compose.h"
using namespace std;

ofstream* ScoreOperator::fout=NULL;

int main(int argc, char *argv[]) {

    score_init();

    UNSIGNED_SCORE_SEGMENT contents = NEW_UNSIGNED_SCORE_SEGMENT(256, 16);

    NEW_segment_compose(contents);

    score_exit();
}

