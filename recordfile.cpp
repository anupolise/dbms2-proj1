#include <iostream>
#include <fstream>
#include <string>

#include "recordfile.h"

void recordfile::readInCSV(const char* filename)
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

		append(buffer);

		//SANITY CHECK
		record rec = record();
		memcpy((char*)&rec, buffer, sizeof(record));

		// printf("Copied byte array is:\n");

		// //print value in buffer
		// for(int i=0;i<sizeof(record);i++){
		// 	printf("%c ",buffer[i]);
		// }
		
		printf("\n");

		//reset info	
		temp = "";
		numOfCommas = 0;

	}

	inputF.close();
	// fclose(pFile);
	return;
}


//this read in was for the other csv that was really long
void recordfile::readInCSV2(const char* filename)
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

		append(buffer);

		//SANITY CHECK
		record rec = record();
		memcpy((char*)&rec, buffer, sizeof(record));

		printf("Copied byte array is:\n");

		// //print value in buffer
		// for(int i=0;i<sizeof(record);i++){
		// 	printf("%c ",buffer[i]);
		// }
		
		printf("\n");

		//reset info	
		temp = "";
		numOfCommas = 0;

	}

	inputF.close();
	// fclose(pFile);
	return;
}

int recordfile::open(const char* filename)
{
	recordFileName = filename;
	char* headerbuff = (char*)malloc(HEADER_SIZE);


	pFile = fopen(filename, "ab+");
	if(!pFile){
		cout<<"recordfile failed to open"<<endl;
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

		//get num of records
		fseek(pFile, 0, SEEK_SET);
   	 	fgets(headerbuff, HEADER_SIZE, pFile);
    	int numRecs = atoi(headerbuff);

    	//debug message
    	// cout<<"Number of recs  -----  " <<numRecs<<endl;
	} 
	//if file doesnt exist 
	else {
		//reopen the file in read write mode
		pFile = fopen(filename, "rb+");
		if(!pFile) return -1;

		//write 0 to header
		// headerbuff= (char*)0;

		//print value in buffer
		// for(int i = 0; i < sizeof(int);i++){
		// 	printf("%d\n ", (int)headerbuff);
		// }

		// sprintf(headerbuff, "%d", 0);
		int temp = 0;
		int* header = &temp;
		fseek(pFile, 0, SEEK_SET);
		// int header = 0;
		// fwrite(headerbuff,1, sizeof(headerbuff), pFile);
		fwrite((char*)header, sizeof(int), 1, pFile);

		
		// fprintf(pFile, "%d", header);
	}
	return 0;
}


int recordfile::close()
{
	fclose(pFile);
	return 0;
}



void recordfile::read(int recordID, char* buffer)
{
	int position = recordID * RECORD_SIZE + HEADER_SIZE;
	fseek(pFile, position, SEEK_SET);
	fread(buffer, RECORD_SIZE, 1,pFile);
	return;    
}



int recordfile::append(char* buffer)
{
	if(!pFile){
		return -1;
	}
	//change header
	char* headerbuff = (char*)malloc(HEADER_SIZE);
	int header = 0;
	fseek(pFile, 0, SEEK_SET);
    fgets(headerbuff, HEADER_SIZE, pFile);

    //get number of records in file and do math
    int* numRecs = (int*)headerbuff;
    // int numRecs = atoi(headerbuff);
    cout<<"insertion number: "<<*numRecs<<endl;
    int position = *numRecs *RECORD_SIZE + HEADER_SIZE;
    (*numRecs)++;
   	
   	headerbuff = (char*)numRecs;
	// sprintf(headerbuff,"%d", *numRecs);
	fseek(pFile, 0, SEEK_SET);
	fwrite((char*)headerbuff, sizeof(headerbuff), 1, pFile);


	//write record to the file
	cout<<"position " <<position <<endl;
	fseek(pFile, position, SEEK_SET);

	//print value in buffer
	for(int i=0;i<sizeof(record);i++){
		printf("%c ",buffer[i]);
	}


	fwrite(buffer, sizeof(record), 1, pFile);

	return *numRecs-1;
}


//only use for overwriting a record
int recordfile::write(int recordID, const char* buffer)
{

	//write the buffer to file
	int position = recordID * RECORD_SIZE + HEADER_SIZE;
	fseek(pFile, position, SEEK_SET);
	fwrite(buffer, sizeof(buffer), 1, pFile);
	return 0;
}
