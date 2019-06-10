#ifndef pagefile_H
#define pagefile_H
#include <vector>
#include <errno.h>

static const int PAGE_VALUE = 1024;
static const int MAX_NUM_KEYS = (PAGE_VALUE-sizeof(int)*3-sizeof(bool))/(2*sizeof(int));

struct pagefileHeader{
	int pageRoot;
	int numPages;
};

struct node{
	int pageNum;
	bool leafNode;
	int numTuples;
	int keys[MAX_NUM_KEYS];
	int pointers[MAX_NUM_KEYS+1];
};


static const int FILE_HEADER_SIZE = sizeof(pagefileHeader);
static const int PAGE_SIZE = sizeof(node); //sanity check
static const int BUFFER_SIZE = 10;
static int bufferCounter = 0;
static node nodeBuffer[BUFFER_SIZE];


class pagefile
{
	private:
		const char* pageFileName;
		FILE * pFile;

       
	public:
		int open(const char* &filename);
		int close();
        node read(int pageID);
        int endPID();
        void write(int pageID, node page);
        void writeToFile(int pageID, node page);
        int getTotalRecords(int pageID);
        int isLeafNode(int pageID);
        void incrPageHeaderNumPages();
        void setRootNode(int pageNum);
		int getTotalPages();
		int getRootNode();
		node nodeConstructor(int pageNum);
		void printNode(node page);
		node bufferAccessor(int pageNum);



};
#endif