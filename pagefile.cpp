#include <iostream>
#include "pagefile.h"
using namespace std;

int pagefile::open(const char* &filename)
{
	pageFileName = filename;
	pagefileHeader header = pagefileHeader();
	char* headerbuff = (char*)malloc(sizeof(header));
	memset(headerbuff, 0, sizeof(header));


	errno = 0;
	pFile = fopen(filename, "ab+");
	if(pFile==NULL){
		printf("Error %d \n", errno);
		//return -1;
	}

	//check if pFile has things in it
	fseek(pFile, 0, SEEK_END);
	int size = ftell(pFile);
	
	//if file exists
	if(size > 0){
		//reopen the file in read write mode
		pFile = fopen(filename, "rb+");

		if(!pFile) return -1;

	} 
	//if file doesnt exist 
	else {
		//reopen the file in read write mode
		pFile = fopen(filename, "rb+");
		if(!pFile) return -1;

		//write 0s to header
		header.numPages = 0;
		header.pageRoot = 0;
		memcpy(headerbuff, &header, sizeof(header));
		fwrite(headerbuff, 1, sizeof(header), pFile);
	}
	return 0;
}


int pagefile::close()
{
	fclose(pFile);
    return 0;
}

// node pagefile::nodeAccessor(int pageNum){
// 	return nodeConstructor(-100);
// }

int pagefile::getTotalRecords(int pageID){
	node n = node();
	char* buffer =  (char*)malloc(PAGE_SIZE);
	int position = pageID * PAGE_SIZE + FILE_HEADER_SIZE;
	fseek(pFile, position, SEEK_SET);
	fread(buffer, PAGE_SIZE, 1,pFile);
	memcpy((char*)&n, buffer, PAGE_SIZE);
	return n.numTuples;    
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
	memset(buffer, 0, PAGE_SIZE);
	memcpy( buffer, (char*)&page, PAGE_SIZE);
	fwrite(buffer, 1, PAGE_SIZE,pFile);
    return;
}

void pagefile::incrPageHeaderNumPages()
{
	int numPages = getTotalPages();
	pagefileHeader header = pagefileHeader();
	header.pageRoot = getRootNode();
	header.numPages = ++numPages;

	char* buf = (char*)malloc(sizeof(header));
	fseek(pFile, 0, SEEK_SET);
	memcpy(buf, &header, FILE_HEADER_SIZE);
	fwrite(buf,1, FILE_HEADER_SIZE, pFile);
	return;
}

int pagefile::getTotalPages()
{
	pagefileHeader header = pagefileHeader();
	char* buf = (char*)malloc(FILE_HEADER_SIZE);
	fseek(pFile, 0, SEEK_SET);
	fread(buf, FILE_HEADER_SIZE, 1, pFile);
	memcpy((char*)&header, buf, FILE_HEADER_SIZE);
	return header.numPages;
}

int pagefile::getRootNode()
{
	pagefileHeader header = pagefileHeader();
	char* buf = (char*)malloc(FILE_HEADER_SIZE);
	fseek(pFile, 0, SEEK_SET);
	fread(buf, FILE_HEADER_SIZE, 1, pFile);
	memcpy((char*)&header, buf, FILE_HEADER_SIZE);
	return header.pageRoot;
}



void pagefile::setRootNode(int pageNum)
{
	pagefileHeader header = pagefileHeader();
	header.pageRoot = pageNum;
	header.numPages = getTotalPages();

	char* buf = (char*)malloc(sizeof(header));
	fseek(pFile, 0, SEEK_SET);
	memcpy(buf, &header, FILE_HEADER_SIZE);
	fwrite(buf,1, FILE_HEADER_SIZE, pFile);
	return;

}


node pagefile::nodeConstructor(int pageNum)
{
	node n = node();
	n.pageNum = pageNum;
	n.leafNode = 1;
	n.numTuples = 0;
	for(int i=0; i<MAX_NUM_KEYS; i++)
	{
		n.keys[i] = -1;
	}
	for(int i=0; i<MAX_NUM_KEYS+1; i++)
	{
		n.pointers[i] = -1;
	}
	return n;
}

void pagefile::printNode(node page)
{
	cout<<"___________________________________________"<<endl;
	cout<<"page num ----- "<<page.pageNum<<endl;
	cout<<"is LeafNode ----- "<<page.leafNode<<endl;
	cout<<"num of tuples ----- "<<page.numTuples<<endl;


	cout<<"KEYS -----"<<endl;
	for(int i = 0; i<MAX_NUM_KEYS; i++)
   {
      cout<<page.keys[i]<<" ";
   }
	cout<<"\nPTRS -----"<<endl;
    for(int i = 0; i<MAX_NUM_KEYS+1; i++)
   {
      cout<<page.pointers[i]<<" ";
   }
	cout<<"\n___________________________________________"<<endl;
	return;

}


