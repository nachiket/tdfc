#include "ScoreStream.h"
#include "ScoreSegment.h"
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

//    exit(1);

//	printf("Case 2.-1: data() %lu ",contents->data());
//	printf("hiy %d %lu\n",contents->hiy,&contents->hiy);
    cout << "Case 2: size " << sizeof(*contents) << " size2 "<< sizeof(ScoreSegment)<< " contents" << contents << " dataPtr=" << (contents->dataPtr) << " &dataPtr=" << &(contents->dataPtr) << endl;
    int* ptr;
    ptr = (int *)&contents->dataPtr;
    ptr+=5;

    unsigned long * ulptr;
    ulptr=(unsigned long *)ptr;

    cout << "Case: " << &ptr << " " << ptr << " " << &ulptr << " " <<ulptr << " "
    << (void *)*ulptr << endl;

    cout << "Case +: contents" << contents << " dataPtr=" << *((long*)((int*)(&contents->dataPtr+5))) << " &dataPtr=" << (int*)(&(contents->dataPtr)+5) << endl;

//    cout << "CtrlPtr=" << contents << " Data pointer=" << ((ScoreSegment *)contents)->dataPtr << " dataId=" << contents->dataID << " width=" << contents->segWidth << " length=" << contents->segLength << endl;
//    UNSIGNED_SCORE_SEGMENT contents1 = NEW_UNSIGNED_SCORE_SEGMENT(256, 16);
//    cout << "CtrlPtr=" << contents << " Data pointer=" << contents->data() << " dataId=" << contents->dataID << endl;
//    cout << "CtrlPtr=" << contents1 << " Data1 pointer=" << contents1->data() << " dataId=" << contents1->dataID << endl;
//    UNSIGNED_SCORE_SEGMENT contents2 = NEW_UNSIGNED_SCORE_SEGMENT(256, 16);
//    cout << "CtrlPtr=" << contents << " Data pointer=" << contents->data() << " dataId=" << contents->dataID << endl;
//    cout << "CtrlPtr=" << contents1 << " Data1 pointer=" << contents1->data() << " dataId=" << contents1->dataID << endl;
//    cout << "CtrlPtr=" << contents2 << " Data2 pointer=" << contents2->data() << " dataId=" << contents2->dataID << endl;
    //unsigned long long *data = (unsigned long long *)contents->data();

    unsigned long long *data = (unsigned long long *)GET_SEGMENT_DATA(contents);
    unsigned long long *data1 = (unsigned long long *)contents->data();
    unsigned long long *data2 = (unsigned long long *)contents->dataPtr;
//    cout << "Data pointer=" << data << endl;
//    cout << "Data pointer=" << data1 << endl;
//    for(int i=0;i<256;i++) {
//	   data[i]=256;
//    }

    NEW_segment_compose(contents);

    while(1) {
    }

    score_exit();
}

