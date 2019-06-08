#ifndef main_H
#define main_H
#include <vector>
#include <errno.h>
#include "recordfile.h"
#include "pagefile.h"

class btree
{
	private:
		const char* pagefileName = "treefile.txt";;
		const char* recordfileName = "recordfile.txt";	

	public:
		recordfile recFile;
		pagefile pageFile;
		btree();
		void insert(record rec);
		int search(int key);
		node insertVal(int key, int recID, node pageNode);
		node insertValPage(int key, int leftPage, int rightPage, node pageNode);
		int  getNextPage(int key, node currNode);
		int searchValue(int key, node currNode);
		vector<int>  splitNode(node currNode, bool isLeaf, vector<int> pastPages);
		void readInCSV(const char* filename);

};
#endif