#ifndef recordfile_H
#define recordfile_H
#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>

using namespace std;

struct record{
			int empID;
			char fname[15];
			char lname[15];
			char ssn[12];
			char username[15];
			char password[15];
		};
		
class recordfile
{
	private:
		static const int PAGE_SIZE = 1024;
		static const int HEADER_SIZE = sizeof(int); 
		const char* recordFileName;
		FILE * pFile;

	public:

		
		static const int RECORD_SIZE =  sizeof(record);
		void readInCSV(const char* filename);
		void readInCSV2(const char* filename);
		int open(const char* filename);
		int close();
        void read(int recordID, char* buffer);
        int append(char* buffer);
        int write(int recordID, const char* buffer);


};
#endif