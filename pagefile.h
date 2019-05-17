#ifndef pagefile_H
#define pagefile_H
#include <vector>

struct pagefileHeader{
	int pageRoot;
	int numPages;
};

struct pageHeader{
	bool leafNode;
	int numRecords;
};

static const int FILE_HEADER_SIZE = sizeof(pagefileHeader);
static const int PAGE_HEADER_SIZE = sizeof(pageHeader); 
static const int PAGE_SIZE = 1024;
static const int NUM_KEYS = ((PAGE_SIZE - PAGE_HEADER_SIZE)/sizeof(int) - 1)/2;

struct node{
	int keys[NUM_KEYS];
	int pointers[NUM_KEYS+1];
};


class pagefile
{
	private:
		const char* pageFileName;
		FILE * pFile;

       
	public:
		int open(const char* &filename);
		int close();
        void read(int pageID, void* buffer);
        int endPID();
        int write(int pageID, const void* buffer);
        int getTotalRecords(int pageID);
        int isLeafNode(int pageID);

};
#endif