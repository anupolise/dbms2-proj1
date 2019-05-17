#ifndef pagefile_H
#define pagefile_H
#include <vector>

struct pagefileHeader{
	int pageRoot;
	int numPages;
};


static const int FILE_HEADER_SIZE = sizeof(pagefileHeader);
static const int PAGE_SIZE = 1024;
static const int NUM_KEYS = ((PAGE_SIZE - sizeof(bool)-sizeof(int))/sizeof(int) - 1)/2;

struct node{
	bool leafNode;
	int numRecords;
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
        node read(int pageID);
        int endPID();
        void write(int pageID, node page);
        int getTotalRecords(int pageID);
        int isLeafNode(int pageID);

};
#endif