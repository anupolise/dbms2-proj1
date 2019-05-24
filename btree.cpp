#include <iostream>
#include <stack>
#include "btree.h"

btree::btree()
{
   pagefileName = "treefile.txt";
   recordfileName = "recordfile.txt";
   pageFile.open(pagefileName);
   node rootNode =  pageFile.nodeConstructor(0);
   pageFile.write(0, rootNode);
   pageFile.setRootNode(0);
   pageFile.close();
}

void btree::insert(record rec)
{
	pageFile.open(pagefileName);
	recFile.open(recordfileName);

	//append the record to the record file
	char* buffer =  (char*)malloc(sizeof(record));
	memset(buffer, 0, sizeof(record));
	memcpy(buffer, &rec, sizeof(rec));
	int recordID = recFile.append(buffer);

	//get last leaf node
	node currNode = pageFile.read(pageFile.getRootNode());

	pageFile.printNode(currNode);

	stack<int> traversalPages;
	traversalPages.push(currNode.pageNum);
	int nextPage = 0;
	while(!pageFile.isLeafNode(currNode.pageNum))
	{
		nextPage = getNextPage(rec.empID, currNode);
		currNode =  pageFile.read(nextPage);
		traversalPages.push(currNode.pageNum);

	}

	bool complete = false;
	node traversePage = pageFile.nodeConstructor(0);
	//note the values in traverse page are garbo to start
	int keyToAdd= rec.empID;
	int parentPage =  traversalPages.top();
	traversalPages.pop();
	vector<int> splitPages;
	while(!complete)
	{
		if(currNode.numTuples >= MAX_NUM_KEYS)
		{
			splitPages = splitNode(currNode, keyToAdd, 1, splitPages);
			keyToAdd = splitPages[2];

			//there's no parent node we must create
			if(traversalPages.empty())
			{
				node newParentPage = pageFile.nodeConstructor(pageFile.getTotalPages());
				newParentPage.leafNode = 0;
				pageFile.incrPageHeaderNumPages();
				insertValPage(splitPages[2], splitPages[0],splitPages[1], newParentPage);
				pageFile.setRootNode(newParentPage.pageNum);
				complete = true;
			}
			else
			{
				currNode = pageFile.read(traversalPages.top());
				traversalPages.pop();
			}
		}
		else
		{
			if(currNode.leafNode)
			{
				cout<<"single leaf insert"<<endl;
				cout<<"keytoADD  "<< keyToAdd<<endl;
				cout<<"recordId  "<< recordID<<endl;

				node rewrite = insertVal(keyToAdd, recordID, currNode);
				pageFile.write(currNode.pageNum, rewrite);
				cout<< "refreshed node "<<endl;
				pageFile.printNode(currNode);


			}
			else
			{
				insertValPage(keyToAdd, splitPages[0], splitPages[1], currNode);
			}
			complete = true;
		}
		
	}
	pageFile.close();
	recFile.close();
	return;

}

node btree::insertValPage(int key, int leftPage, int rightPage, node pageNode)
{
	if(pageNode.numTuples >=  MAX_NUM_KEYS)
	{
		cout<<"FULL"<<endl;
		return pageNode;
	}

	bool replace = false;
	int tempKey = 0;
	int tempPtr = 0;
	int nextKey = key;
	int nextPtr = leftPage;
	int holdPos = 0;
	for(int i = 0; i<MAX_NUM_KEYS; i++)
	{
		if(replace)
		{
			nextKey = pageNode.keys[i];
			nextPtr = pageNode.pointers[i];
			pageNode.keys[i] = tempKey;
			pageNode.pointers[i] = tempPtr;
			tempKey = nextKey;
			tempPtr = nextPtr;
		}
		else if(key<pageNode.keys[i] || pageNode.keys[i] == -1)
		{
			replace = true;
			holdPos = i;
			tempKey = pageNode.keys[i];
			tempPtr = pageNode.pointers[i];
			pageNode.keys[i] = nextKey;
			pageNode.pointers[i] = nextPtr;
		}
	}
	//sanity check
	pageNode.pointers[holdPos] = leftPage;
	pageNode.pointers[holdPos +1] = rightPage;

	pageNode.numTuples+=1;

	return pageNode;

}


vector<int> btree::splitNode(node currNode, int key, bool isLeaf, vector<int> pastPages)
{
	vector<int> returnPages;

	int pageNumL = currNode.pageNum;
	//create new page for split node
	int pageNumR = pageFile.getTotalPages();
	returnPages.push_back(pageNumL);
	returnPages.push_back(pageNumR);

	node recPageL = currNode;
	node recPageR = pageFile.nodeConstructor(pageNumR);

	//physically split the nodes
	for(int i=MAX_NUM_KEYS/2+1; i<MAX_NUM_KEYS; i++)
	{
		recPageR.keys[i-MAX_NUM_KEYS/2+1] = recPageL.keys[i];
		recPageR.pointers[i-MAX_NUM_KEYS/2+1] = recPageL.pointers[i];
		recPageL.keys[i] = -1;
		recPageL.pointers[i] = -1;
	}

	//if there was a previous split make sure to put those pages and the last value in the right place
	if(pastPages.size()!=0)
	{
		if(pastPages[2]<recPageR.keys[0])
		{
			insertValPage(pastPages[2], pastPages[0], pastPages[1], recPageL);
		}
		else
		{
			insertValPage(pastPages[2], pastPages[0], pastPages[1], recPageR);
		}


	}
	//set page information
	recPageL.pageNum = pageNumL;
	recPageR.pageNum = pageNumR;

	// TODO: FIX they cant all be leaf
	recPageL.leafNode = true;
	recPageR.leafNode = true;

	recPageL.numTuples = MAX_NUM_KEYS/2;
	recPageR.numTuples = MAX_NUM_KEYS - (MAX_NUM_KEYS/2+1);

	//set the next pointer to the next leaf node
	recPageL.pointers[MAX_NUM_KEYS/2+1] = pageNumR;

	//leftmost (0) key is the key we're pushing up 
	returnPages[2] = recPageR.keys[0];

	//write the files in their respective places
	pageFile.write(pageNumL, recPageL);
	pageFile.write(pageNumR, recPageR);

	//incr total num of pages cuz we created another one
	pageFile.incrPageHeaderNumPages();

	return returnPages;

}


node btree::insertVal(int key, int recID, node pageNode)
{
	if(pageNode.numTuples >=  MAX_NUM_KEYS)
	{
		cout<<"FULL"<<endl;
		return pageNode;
	}

	bool replace = false;
	int tempKey = 0;
	int tempPtr = 0;
	int nextKey = key;
	int nextPtr = recID;
	for(int i = 0; i<MAX_NUM_KEYS; i++)
	{
		if(replace)
		{
			nextKey = pageNode.keys[i];
			nextPtr = pageNode.pointers[i];
			pageNode.keys[i] = tempKey;
			pageNode.pointers[i] = tempPtr;
			tempKey = nextKey;
			tempPtr = nextPtr;
		}
		else if(key<pageNode.keys[i] || pageNode.keys[i] == -1)
		{
			replace = true;
			tempKey = pageNode.keys[i];
			tempPtr = pageNode.pointers[i];
			pageNode.keys[i] = nextKey;
			pageNode.pointers[i] = nextPtr;
		}
	}

	//TODO:increment number of records
	pageNode.numTuples+=1;

	return pageNode;

}

int btree::search(int key)
{
	node root = pageFile.read(pageFile.getRootNode());
	// vector<int> traver
	int nextPage = 0;
	while(!pageFile.isLeafNode(root.pageNum))
	{
		nextPage = getNextPage(key, root);
		root = pageFile.read(nextPage);
	}

	int recID = searchValue(key, root);
	
	return recID;

}

int btree::getNextPage(int key, node currNode)
{
	for(int i = 0; i<MAX_NUM_KEYS; i++)
	{
		if(key<currNode.keys[i] || currNode.keys[i] == -1)
		{
			return currNode.pointers[i];
		}
	}
	return currNode.pointers[MAX_NUM_KEYS];
	
}

int btree::searchValue(int key, node currNode)
{
	for(int i = 0; i<MAX_NUM_KEYS; i++)
	{
		if(key == currNode.keys[i])
		{
			return currNode.pointers[i];
		}
	}

	return -1;
}

void btree::readInCSV(const char* filename)
{
	// TODO: complete parsing this function
 	// FILE * csvFile = fopen(filename, "rb+");
 	std::ifstream  inputF(filename);
	std::string line;
	int numOfCommas = 0;
	std:string temp;

	record currRec = record();
	while(getline(inputF,line))
	{
		line += ",";
		//concatenates strings between commas in the line
		for(int i=0; i<line.length(); i++)
		{
			if(line[i] == ',')
			{
				numOfCommas++;

				//puts whatever parsed value in the struct
					switch(numOfCommas)
					{
						case 1:
							//empID
							cout<<"TESTTTTT    "<<currRec.empID<<endl;
							currRec.empID = std::stoi(temp);
							break;
						case 3:
							//fname
							temp = temp.substr(0,14);
							strcpy(currRec.fname, temp.c_str());
							break;
						case 5:
							//lname
							temp = temp.substr(0,14);
							strcpy(currRec.lname, temp.c_str());

							break;
						case 28:
							//ssn
							temp = temp.substr(0,12);
							strcpy(currRec.ssn, temp.c_str());

							break;
						case 36:
							//username
							temp = temp.substr(0,14);
							strcpy(currRec.username, temp.c_str());
							break;
						case 37:
							//pswd
							temp = temp.substr(0,14);
							strcpy(currRec.password, temp.c_str());
							break;
						default:
							break;

					}

				temp = "";


			} 
			else
			{
				temp+=line[i];
			}

		}

		char* buffer =  (char*)malloc(sizeof(record));
		memset(buffer, 0, sizeof(record));
		memcpy(buffer, &currRec, sizeof(currRec));
		// printf("%s", buffer);

		// recFile.append(buffer);

		//SANITY CHECK
		record rec = record();
		memcpy((char*)&rec, buffer, sizeof(record));

		//testing
		cout<<"recrid : " <<rec.empID<<endl;

		// printf("Copied byte array is:\n");

		// //print value in buffer
		// for(int i=0;i<sizeof(record);i++){
		// 	printf("%c ",buffer[i]);
		// }
		
		// printf("\n");

		insert(rec);
		//reset info	
		temp = "";
		numOfCommas = 0;

	}

	inputF.close();
	// fclose(pFile);
	return;
}