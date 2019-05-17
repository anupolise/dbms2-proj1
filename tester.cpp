#include <iostream>
#include "recordfile.h"
#include "pagefile.h"

using namespace std;

int main()
{
   cout<<"Hello world!"<<endl;
   recordfile r;
   const char* filename = "test.txt";
  
   const char* test = "test";
 	char* buf = (char*)malloc(sizeof(record));
   cout << sizeof(buf) << endl;
   cout << sizeof(record) << endl;

   int ret = r.open(filename, 'a');
   cout<<"Debug open:"<<ret<<endl;

   // r.readInCSV(test);

	memset(buf, 0, r.RECORD_SIZE);

   cout<<"READ------:"<<endl;

   record rec = record();

   r.read(1, buf);

		//print value in buffer
		for(int i=0;i<sizeof(record);i++){
			printf("%c ",buf[i]);
		}

   cout<<"\nREAD------:"<<endl;

   // record rec1 = record();
   record* rec_ptr = &rec;
   memset(rec_ptr, 0, sizeof(record));
   r.read(2, buf);
	memcpy((char*)&rec, buf, sizeof(record));
	cout<<rec.ssn<<endl;

	// r.read(1, buf);
	// memcpy((char*)&rec, buf, sizeof(r.RECORD_SIZE));
	// cout<<rec.empID<<endl;

	// r.read(2, buf);
	// memcpy((char*)&rec, buf, sizeof(r.RECORD_SIZE));
	// cout<<rec.fname<<endl;

   memset(buf,0, sizeof(record));
   r.read(10, buf);
   memcpy((char*)&rec, buf, sizeof(r.RECORD_SIZE));
   for(int i=0;i<sizeof(record);i++){
         printf("%c ",buf[i]);
      }
   cout<<rec.lname<<endl;


   cout<<"done"<<endl;

   // r.
   // int bro[2] = {20,30};
   // char* second = (char*)bro;
   // second = NULL;
   // cout<<"bro: "<<second<<endl;
   // r.write(0,second);
   // r.read(0, second);

   r.close();

    
}