#ifndef pagefile_H
#define pagefile_H
#include <vector>
#include <errno.h>


static const int MAX_NUM_KEYS = 4;

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
// static const int PAGE_SIZE = 1024;
static const int PAGE_SIZE = sizeof(node);
static const int BUFFER_SIZE = 10;
static node buffer[10];




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