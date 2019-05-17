#include <iostream>
#include "recordfile.h"
#include "pagefile.h"

using namespace std;

void test_recordfile();

int main()
{
  test_recordfile();

    
}

void test_recordfile()
{
   cout<<"TESTING RECORD FILE -----------------------------"<<endl;
   recordfile r;
   const char* filename = "test.txt";
  
   const char* test = "test";
   char* buf = (char*)malloc(sizeof(record));

   int ret = r.open(filename);
   cout<<"Debug Open: "<<ret<<endl;

   // r.readInCSV(test);

   memset(buf, 0, r.RECORD_SIZE);

   cout<<"READ BUF 1 ------:"<<endl;

   record rec = record();

   r.read(1, buf);

      //print value in buffer
      for(int i=0;i<sizeof(record);i++){
         printf("%c ",buf[i]);
      }

   cout<<"\n END READ------:"<<endl;

   // record rec1 = record();
   record* rec_ptr = &rec;
   memset(rec_ptr, 0, sizeof(record));
   r.read(2, buf);
   memcpy((char*)&rec, buf, sizeof(record));
   cout<<"REC 2 SSN: "<<rec.ssn<<endl;

   record rec1 = record();
   memset(buf, 0, sizeof(record));
   r.read(1, buf);
   memcpy((char*)&rec1, buf, sizeof(record));
   cout<<"REC 1 EMPID: "<<rec1.empID<<endl;

   record rec2 = record();
   memset(buf, 0, sizeof(record));
   r.read(0, buf);
   for(int i=0;i<sizeof(record);i++){
         printf("%c ",buf[i]);
   }
   memcpy((char*)&rec2, buf, sizeof(record));
   cout<<"\nREC 2 FNAME: "<<rec2.fname<<endl;

 
    cout<<"DONE ----------------"<<endl;

   

   r.close();

}