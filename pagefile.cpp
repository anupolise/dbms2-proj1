#include <iostream>
#include "pagefile.h"
using namespace std;

int pagefile::open(const char* &filename)
{
	pageFileName = filename;
	char* headerbuff = (char*)malloc(sizeof(int));


	pFile = fopen(filename, "r");
	if(!pFile){
		return -1;
	}

	//check if pFile has things in it
	fseek(pFile, 0, SEEK_END);
	int size = ftell(pFile);
	
	//if file exists
	if(size > 0){
		//reopen the file in read write mode
		pFile = fopen(filename, "rb+");
		if(!pFile) return -1;

		//TODO: WHY DO I DO THIS?
		//get num of records
		// fseek(pFile, 0, SEEK_SET);
  //  	 	fgets(headerbuff, HEADER_SIZE, pFile);
  //   	int numRecs = atoi(headerbuff);
  //   	cout<<"numrecs-----" <<numRecs<<endl;
	} 
	//if file doesnt exist 
	else {
		//reopen the file in read write mode
		pFile = fopen(filename, "rb+");
		if(!pFile) return -1;

		//write 0 to header
		sprintf(headerbuff, "%d%d", 0,0);
		fwrite(headerbuff, 1, sizeof(int)*2, pFile);
	}
	return 0;
}


int pagefile::close()
{
	fclose(pFile);
    return 0;
}

int pagefile::getTotalRecords(int pageID){
	node n = node();
	char* buffer =  (char*)malloc(PAGE_SIZE);
	int position = pageID * PAGE_SIZE + FILE_HEADER_SIZE;
	fseek(pFile, position, SEEK_SET);
	fread(buffer, PAGE_SIZE, 1,pFile);
	memcpy((char*)&n, buffer, PAGE_SIZE);
	return n.numRecords;    
}

int pagefile::isLeafNode(int pageID){
	node n = node();
	char* buffer =  (char*)malloc(PAGE_SIZE);
	int position = pageID * PAGE_SIZE + FILE_HEADER_SIZE;
	fseek(pFile, position, SEEK_SET);
	fread(buffer, PAGE_SIZE, 1,pFile);
	memcpy((char*)&n, buffer, PAGE_SIZE);
	return n.leafNode;
}

node pagefile::read(int pageID)
{
	node n = node();
	char* buffer =  (char*)malloc(PAGE_SIZE);
	int position = pageID * PAGE_SIZE + FILE_HEADER_SIZE;
	fseek(pFile, position, SEEK_SET);
	fread(buffer, PAGE_SIZE, 1,pFile);
	memcpy((char*)&n, buffer, PAGE_SIZE);
	return n;
}

int pagefile::endPID()
{
    return 0;
}

void pagefile::write(int pageID, node page)
{
	char* buffer =  (char*)malloc(PAGE_SIZE);
	int position = pageID * PAGE_SIZE + FILE_HEADER_SIZE;
	fseek(pFile, position, SEEK_SET);
	memcpy((char*)&page, buffer, PAGE_SIZE);
	fwrite(buffer, 1, PAGE_SIZE,pFile);
    return;
}

