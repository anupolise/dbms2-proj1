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
   pageFile.incrPageHeaderNumPages();
   cout<<"NUM PAGE FILES " << pageFile.getTotalPages();
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

	//get head
	node currNode = pageFile.read(pageFile.getRootNode());

	//prints the node before insertion
	//pageFile.printNode(currNode);

	//traverse tree, push pages that we have visited
	stack<int> traversalPages;
	traversalPages.push(currNode.pageNum);
	int nextPage = 0;
	while(!pageFile.isLeafNode(currNode.pageNum))
	{
		nextPage = getNextPage(rec.empID, currNode);
		currNode = pageFile.read(nextPage);
		traversalPages.push(currNode.pageNum);

	}

	bool complete = false;
	node traversePage = pageFile.nodeConstructor(0);
	//note the values in traverse page are garbo to start
	int keyToAdd = rec.empID;
	int parentPage = traversalPages.top();
	traversalPages.pop();
	vector<int> splitPages;
	while(!complete)
	{
		if(currNode.numTuples >= MAX_NUM_KEYS)
		{
			//this is confusing bc split pages is used to put in the val and also reutrn val
			if(currNode.leafNode) {
				splitPages.push_back(recordID);
			}
			splitPages = splitNode(currNode, keyToAdd, 1, splitPages);
			// keyToAdd = splitPages[2];

			//there's no parent node we must create
			if(traversalPages.empty())
			{
				cout<<"parent node CREATED :  "<<pageFile.getTotalPages()<<endl;

				//create new root page and incr total pages
				int parentPageID = pageFile.getTotalPages();
				node newParentPage = pageFile.nodeConstructor(parentPageID);
				newParentPage.leafNode = 0;
				pageFile.incrPageHeaderNumPages();

				newParentPage = insertValPage(splitPages[2], splitPages[0],splitPages[1], newParentPage);
				pageFile.write(parentPageID, newParentPage);
				pageFile.setRootNode(newParentPage.pageNum);
				pageFile.printNode(newParentPage);
				complete = true;
			}
			else
			{
				//TODO: actually implement for multiple levels
				//uhh so I think what I'm doing here is iterating, so that if its passing smt up, it'll insert next round
				cout<<"WE R HERE ***********************"<<endl;
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
				cout<<"refreshed node"<< endl;
				pageFile.printNode(rewrite);


			}
			else
			{
				cout<<"KEY TO ADD: "<<splitPages[2]<<endl;
				cout<<"LPage: "<<splitPages[0]<<endl;
				cout<<"RPage: "<<splitPages[1]<<endl;

				currNode = insertValPage(splitPages[2], splitPages[0], splitPages[1], currNode);
				pageFile.write(currNode.pageNum, currNode);
				pageFile.printNode(currNode);
			}
			complete = true;
		}
		
	}
	pageFile.close();
	recFile.close();
	return;

}



vector<int> btree::splitNode(node currNode, int key, bool isLeaf, vector<int> pastPages)
{
	vector<int> returnPages;

	int tempKeys[MAX_NUM_KEYS+1];
	int tempPointers[MAX_NUM_KEYS+2];


	int pageNumL = currNode.pageNum;
	//create new page for split node
	int pageNumR = pageFile.getTotalPages();
	//incr total num of pages cuz we created another one
	pageFile.incrPageHeaderNumPages();

	returnPages.push_back(pageNumL);
	returnPages.push_back(pageNumR);

	node recPageL = currNode;
	node recPageR = pageFile.nodeConstructor(pageNumR);

	// case: leaf node where key[i] = ptr[i] 
	if(isLeaf){
		int tempCounter = 0; 

		// write key/pointers to temp array
		for (int i=0; i<MAX_NUM_KEYS; i++){
			
			if(currNode.keys[i]<key){
				tempKeys[tempCounter] = currNode.keys[i];
				tempPointers[tempCounter] = currNode.pointers[i];
				tempCounter++;
			} 
			else{
				tempKeys[i] = key;
				tempPointers[i] = pastPages[0];
			}
		}

		// split temp array into two nodes
		for (int i = 0; i < MAX_NUM_KEYS; i++) {
			if (i < MAX_NUM_KEYS / 2) {
				recPageL.keys[i] = tempKeys[i];
				recPageL.pointers[i] = tempPointers[i];
			}
			else {
				recPageR.keys[i - MAX_NUM_KEYS / 2] = tempKeys[i];
				recPageR.pointers[i - MAX_NUM_KEYS / 2] = pastPages[1];
			}
		}
	}

	// case: internal node where key[i] = ptr[i + 1]
	else
	{
		int tempCounter = 0; 

		// write key/pointers to temp array
		for (int i=0; i<MAX_NUM_KEYS; i++){
			
			if(currNode.keys[i]<key){
				tempKeys[tempCounter] = currNode.keys[i];
				tempPointers[tempCounter] = currNode.pointers[i];
				tempCounter++;
			} 
			else{
				tempKeys[i] = key;
				tempPointers[i] = currNode.pointers[i];
			}
		}
		tempPointers[tempCounter] = currNode.pointers[i];

		// split temp array into two nodes
		for (int i = 0; i < MAX_NUM_KEYS; i++) {
			if (i < MAX_NUM_KEYS / 2) {
				recPageL.keys[i] = tempKeys[i];
				recPageL.pointers[i] = tempPointers[i];
			}
			else {
				recPageR.keys[i - MAX_NUM_KEYS / 2] = tempKeys[i];
				recPageR.pointers[i - MAX_NUM_KEYS / 2] = tempPointers[i];
			}
		}
	}
	
	//make sure you set the last pointer 
	recPageR.pointers[MAX_NUM_KEYS-MAX_NUM_KEYS/2] =  recPageL.pointers[MAX_NUM_KEYS];

	//change respective sizes of the nodes
	recPageL.numTuples = (MAX_NUM_KEYS+1)/2;
	//if there are an even number of key, there will be one more right key than left key
	recPageR.numTuples = (MAX_NUM_KEYS+1)/2;

	//if there was a previous split make sure to put those pages and the last value in the right place
	if(pastPages.size()!=0)
	{	
		cout<<"Last insert "<<endl;
		if(key>recPageR.keys[MAX_NUM_KEYS/2-1])
		{
			cout<<"insert left "<<endl;
			recPageL = insertVal(key, pastPages[0], recPageL);
		}
		else
		{
			cout<<"insert right "<<endl;
			recPageR = insertVal(key, pastPages[0], recPageR);
		}


	}
	//set page information
	recPageL.pageNum = pageNumL;
	recPageR.pageNum = pageNumR;

	// TODO: FIX they cant all be leaf
	recPageL.leafNode = true;
	recPageR.leafNode = true;

	
	//set the next pointer to the next leaf node TODO: if it's a leaf node ONLY
	recPageL.pointers[MAX_NUM_KEYS/2+1] = pageNumR;

	//leftmost (0) key is the key we're pushing up 
	returnPages[2] = recPageR.keys[0];

	//write the files in their respective places
	pageFile.write(pageNumL, recPageL);
	pageFile.write(pageNumR, recPageR);

	cout<<"CREATED L: "<<pageNumL<<endl;
	pageFile.printNode(recPageL);
	cout<<"CREATED R: "<<pageNumR<<endl;
	pageFile.printNode(recPageR);



	return returnPages;

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