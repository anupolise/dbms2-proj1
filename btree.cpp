#include <iostream>
#include <stack>
#include "btree.h"

btree::btree()
{
   pagefileName = "treefile.txt";
   recordfileName = "recordfile.txt";
   int isNewFile = pageFile.open(pagefileName);
   if(isNewFile){
   	   node rootNode =  pageFile.nodeConstructor(0);
	   pageFile.write(0, rootNode);
	   pageFile.setRootNode(0);
	   pageFile.incrPageHeaderNumPages();
	   cout<<"NUM PAGE FILES " << pageFile.getTotalPages()<<endl;
   		cout<<"ROOT NODE " << pageFile.getRootNode()<<endl;
	   pageFile.close();
   } else {
   		pageFile.setRootNode(pageFile.getRootNode());
   		cout<<"NUM PAGE FILES " << pageFile.getTotalPages()<<endl;
   		cout<<"ROOT NODE " << pageFile.getRootNode()<<endl;
   }

  
   pageFile.open(pagefileName);
   recFile.open(recordfileName);
}

void btree::insert(record rec)
{

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
			if(currNode.leafNode) 
			{
				splitPages.push_back(keyToAdd);
				splitPages.push_back(recordID);
				splitPages = splitNode(currNode, 1, splitPages);
				cout<<"\nPRINT RECIEVED PAGES -- IN INSERT --- LEAF"<<endl;
				for(int i =0; i< splitPages.size(); i++)
				{
					cout<<splitPages[i]<<" ";
				}
			}
			else
			{
				splitPages = splitNode(currNode, 0, splitPages);
				cout<<"\nPRINT RECIEVED PAGES -- IN INSERT --- NODE"<<endl;
				for(int i =0; i< splitPages.size(); i++)
				{
					cout<<splitPages[i]<<" ";
				}
			}

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
					//uhh so I think what I'm doing here is iterating, so that if its passing smt up, it'll insert next round
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
	
	return;

}



vector<int> btree::splitNode(node currNode, bool isLeaf, vector<int> pastPages)
{
	cout<<"\nPRINT PAST PAGES **********+++++++++++++++++++++++"<<endl;
	for(int i =0; i< pastPages.size(); i++)
	{
		cout<<pastPages[i]<<" ";
	}

	//return pages: LPageID, RPageID, key
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

	//set page information
	recPageL.pageNum = pageNumL;
	recPageR.pageNum = pageNumR;

	// CASE: leaf node where key[i] = ptr[i] 
	//pastpages: 0-key, 1-recordID
	if(isLeaf)
	{
		//to write everything to the same array
		int tempCounter = 0;
        int i=0;
        bool valObtained = false;
        while(tempCounter<MAX_NUM_KEYS+1)
		{	
			if((currNode.keys[i]<pastPages[0] || valObtained) && i<MAX_NUM_KEYS){
				tempKeys[tempCounter] = currNode.keys[i];
				tempPointers[tempCounter] = currNode.pointers[i];
				i++;
			} 
			else{
				tempKeys[tempCounter] = pastPages[0];
				tempPointers[tempCounter] = pastPages[1];
				valObtained = true;
			    
			}
			tempCounter++;
		}

		// split temp array into two nodes
		for (int i = 0; i < MAX_NUM_KEYS+1; i++) 
		{
			if (i < (MAX_NUM_KEYS+1) / 2) {
				recPageL.keys[i] = tempKeys[i];
				recPageL.pointers[i] = tempPointers[i];
				// cout<<"\n"<<i<<" "<<tempPointers[i]<<endl;
			}
			else {

				recPageR.keys[i - (MAX_NUM_KEYS+1) / 2] = tempKeys[i];
				recPageR.pointers[i -(MAX_NUM_KEYS+1) / 2] = tempPointers[i];
				//clean left/currNode
				if(i<MAX_NUM_KEYS){
					recPageL.keys[i] = -1;
				}
				recPageL.pointers[i] = -1;
				
			}
		}
		
		//make sure you set the last pointer 
		recPageL.pointers[MAX_NUM_KEYS] =  recPageR.pageNum;
		recPageR.pointers[MAX_NUM_KEYS] =  currNode.pointers[MAX_NUM_KEYS];

		//making some leaves
		recPageL.leafNode = true;
		recPageR.leafNode = true;


		//change respective sizes of the nodes
		//if odd
		if(MAX_NUM_KEYS%2)
		{
			recPageL.numTuples = (MAX_NUM_KEYS+1)/2;
			recPageR.numTuples = (MAX_NUM_KEYS+1)/2;
		}
		//if even
		else
		{
			recPageL.numTuples = (MAX_NUM_KEYS)/2;
			recPageR.numTuples = (MAX_NUM_KEYS)/2+1;
		}

		//leftmost (0) key of right page is the key we're pushing up 

		returnPages.push_back(recPageR.keys[0]);
		
	}

	// CASE: internal node where key[i] = ptr[i + 1]
	//pastpages: 0-lPage, 1-rPage, 2-key
	else
	{
		returnPages.push_back(0);
		// cout<<"size of return pages"
		//to write everything to the same array
		int tempCounter = 0;
        int i=0;
        bool valObtained = false;
        tempPointers[0] = currNode.pointers[0];
        while(tempCounter<MAX_NUM_KEYS+1)
		{	
			if((currNode.keys[i]<pastPages[2] || valObtained) && i<MAX_NUM_KEYS){
				tempKeys[tempCounter] = currNode.keys[i];
				tempPointers[tempCounter+1] = currNode.pointers[i+1];
				i++;
			}
			else{
				tempKeys[tempCounter] = pastPages[2];
				//we only need to write right page id, assuming left pageid stays the same
				tempPointers[tempCounter+1] = pastPages[1];
				valObtained = true;
			}
			tempCounter++;
		}

		// cout<<"\nKEYS ADN POITNERS"<<endl;
		// for(int i =0; i<MAX_NUM_KEYS+1; i++)
		// {
		// 	cout<<tempKeys[i]<<" ";
		// }
		// cout<<"\nPOINTERS\n";
		// for(int i =0; i<MAX_NUM_KEYS+2; i++)
		// {
		// 	cout<<tempPointers[i]<<" ";
		// }

		// split temp array into two nodes
		for (int i = 0; i < MAX_NUM_KEYS+1; i++) 
		{
			if (i < (MAX_NUM_KEYS+1) / 2) {
				recPageL.keys[i] = tempKeys[i];
				recPageL.pointers[i] = tempPointers[i];
			}
			else if(i==(MAX_NUM_KEYS+1) / 2){
			    //this val gets pushed up
			    returnPages[2] = tempKeys[i];

			    recPageL.keys[i] = -1;
			    recPageL.pointers[i] = tempPointers[i];
			    recPageR.pointers[0] = tempPointers[i+1];
			}
			else {
				recPageR.keys[i - (MAX_NUM_KEYS+1) / 2-1] = tempKeys[i];
				recPageR.pointers[i - (MAX_NUM_KEYS+1) / 2] = tempPointers[i+1];
				//clean left/currNode
				if(i<MAX_NUM_KEYS){
					recPageL.keys[i] = -1;
				}
				recPageL.pointers[i] = -1;
			}
		}

		//NOT LEAVES
		recPageL.leafNode = false;
		recPageR.leafNode = false;

		//change respective sizes of the nodes
		//if odd
		if(MAX_NUM_KEYS%2)
		{
			recPageL.numTuples = (MAX_NUM_KEYS)/2;
			recPageR.numTuples = (MAX_NUM_KEYS)/2-1;
		}
		//if even
		else
		{
			recPageL.numTuples = (MAX_NUM_KEYS+1)/2;
			recPageR.numTuples = (MAX_NUM_KEYS+1)/2;
		}

	}
	
	//write the files in their respective places
	pageFile.write(pageNumL, recPageL);
	pageFile.write(pageNumR, recPageR);
	cout<<"\nPUSHED UP: "<<returnPages[2]<<endl;
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
	pageNode.pointers[MAX_NUM_KEYS] = nextPtr;

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

	pageNode.numTuples+=1;


	return pageNode;

}

int btree::search(int key)
{
	node root = pageFile.read(pageFile.getRootNode());
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
						case 2:
							//fname
							temp = temp.substr(0,14);
							strcpy(currRec.fname, temp.c_str());
							break;
						case 3:
							//lname
							temp = temp.substr(0,14);
							strcpy(currRec.lname, temp.c_str());

							break;
						case 4:
							//ssn
							temp = temp.substr(0,12);
							strcpy(currRec.ssn, temp.c_str());

							break;
						case 5:
							//username
							temp = temp.substr(0,14);
							strcpy(currRec.username, temp.c_str());
							break;
						case 6:
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
	return;
}