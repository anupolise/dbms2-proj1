#include <iostream>
#include "recordfile.h"
#include "pagefile.h"
#include "btree.h"
#include <chrono>

using namespace std;

void test_recordfile();
void test_simplePageFile();
void test_singleNodeManipulations();
void endlessTesting();
void searchTesting(btree t);
void bulkLoadTesting(btree t);
void insertTesting(btree t);


int main()
{
   //OTHER TESTING FUNCTIONS 
   //test_recordfile();
   //test_simplePageFile();
   //test_singleNodeManipulations();

   endlessTesting();

}

void endlessTesting(){
   int i=1;

   btree tree =  btree();
   // tree.readInCSV("smalldata");
   const char* filen = "treefile.txt";
   // tree.pageFile.open(filen);

   while(i){
      int i = 0;
      cout<<"\nPlease choose a choice from the following, press 0 to quit: "<<endl;
      cout<<"1 - Insert\n2 - Bulk Load\n3 - Search"<<endl;
      cin>>i;
      if(i==1)
      {
         insertTesting(tree);
      }
      else if (i==2)
      {
         bulkLoadTesting(tree);
      }
      else if(i==3)
      {
         searchTesting(tree);
      }
      else if(i==0){
         break;
      }
      else{
         cout<<"Sorry, that wasn't one of the choices, please try again.";
      }
   }

   tree.pageFile.close();
   tree.recFile.close();

   return;
}

void insertTesting(btree t){
   record currRec;
   int empID = 0;
   string temp;

   cout<<"INSERT: Enter an emp ID: ";
   cin>>empID;
   currRec.empID = empID;
   
   cout<<"\nEnter first name: ";
   cin>>temp;
   temp = temp.substr(0,14);
   strcpy(currRec.fname, temp.c_str());
   
   cout<<"\nEnter last name: ";
   cin>>temp;
   temp = temp.substr(0,14);
   strcpy(currRec.lname, temp.c_str());

   cout<<"\nEnter ssn: ";
   cin>>temp;
   temp = temp.substr(0,12);
   strcpy(currRec.ssn, temp.c_str());

   cout<<"\nEnter username: ";
   cin>>temp;
   temp = temp.substr(0,14);
   strcpy(currRec.username, temp.c_str());

   cout<<"\nEnter password: ";
   cin>>temp;
   temp = temp.substr(0,14);
   strcpy(currRec.password, temp.c_str());

   std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
   t.insert(currRec);
   std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> time_span = end_time - start_time;
   std::cout << std::fixed << "INSERT: " << time_span.count() << "s" << std::endl;
   return;
}

void searchTesting(btree t){
   int empID;

   cout<<"SEARCH: Enter an employee ID to search for: ";
   cin>>empID;
   cout<<endl;
   std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
   int recID = t.search(empID);
   std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> time_span = end_time - start_time;
   std::cout << std::fixed << "SEARCH: " << time_span.count() << "s" << std::endl;

   if(recID == -1)
   {
      cout<<"Employee ID not found";
      return;
   }

   // t.recFile.open(t.recordfileName);
   char* buf = (char*)malloc(sizeof(record));
   memset(buf, 0, t.recFile.RECORD_SIZE);
   record rec = record();
   t.recFile.read(recID, buf);

   record* rec_ptr = &rec;
   memset(rec_ptr, 0, sizeof(record));

   memcpy((char*)&rec, buf, sizeof(record));
   cout<<"\nSEARCH RESULTS"<<endl;
   cout<<"Employee ID: "<<rec.empID<<endl;
   cout<<"Name: "<<rec.fname<<" "<<rec.lname<<endl;
   cout<<"SSN: "<<rec.ssn<<endl;
   cout<<"Username and password: "<<rec.username<<" "<<rec.password<<endl;


   //print value in buffer
   // for(int i=0;i<sizeof(record);i++){
   //    printf("%c ",buf[i]);
   // }
   // t.recFile.close();
   return;

}

void bulkLoadTesting(btree t){
   string fileName;
   cout<<"BULK LOAD: Enter a file name to bulk load: ";
   cin>>fileName;
   cout<<endl;
   const char* fileN = fileName.c_str();
   std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
   t.readInCSV(fileN);
   std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
   
   std::chrono::duration<double> time_span = end_time - start_time;
   std::cout << std::fixed << "INSERT: " << time_span.count() << "s" << std::endl;
   return;
}

void test_singleNodeManipulations()
{
   pagefile p;
   node tester  = p.nodeConstructor(8);
   for(int i = 0; i<8; i++)
   {
      tester.keys[i] = (i+1)*10;
   }
   for(int i = 0; i<9; i++)
   {
      tester.pointers[i] = (i+1);
   }

   btree tree; 
   p.printNode(tester);
   cout<<"get next page *80*:  " << tree.getNextPage(80, tester) <<endl;
   cout<<"get next page *53*:  " << tree.getNextPage(53, tester) <<endl;
   cout<<"get next page *9*:  " << tree.getNextPage(9, tester) <<endl;

   cout<<"PAGE INSERTION RECORD 35,p100:  "<<endl;

   p.printNode(tree.insertVal(35,100, tester));

   cout<<"PAGE INSERTION page 35,111,222:  "<<endl;
   p.printNode(tree.insertValPage(35,111, 222, tester));

   tester.keys[0] = 4;
   tester.keys[1] = 15;
   tester.keys[2] = 20;
   tester.keys[3] = -1;

   tester.pointers[0] = 1;
   tester.pointers[1] = 2;
   tester.pointers[2] = 3;
   tester.pointers[3] = 4;
   tester.pointers[4] = -1;

   cout<<"original node:  "<<endl;
   p.printNode(tester);
   cout<<"PAGE INSERTION page 2,111,222:  "<<endl;
   p.printNode(tree.insertValPage(2,111, 222, tester));

   cout<<"PAGE INSERTION page 10,111,222:  "<<endl;
   p.printNode(tree.insertValPage(10,111, 222, tester));

   cout<<"PAGE INSERTION page 45,111,222:  "<<endl;
   p.printNode(tree.insertValPage(45,111, 222, tester));

   tester.keys[0] = 4;
   tester.keys[1] = 15;
   tester.keys[2] = -1;
   tester.keys[3] = -1;

   tester.pointers[0] = 1;
   tester.pointers[1] = 2;
   tester.pointers[2] = 3;
   tester.pointers[3] = -1;
   tester.pointers[4] = -1;

   cout<<"PAGE INSERTION page 2,111,222:  "<<endl;
   p.printNode(tree.insertValPage(2,111, 222, tester));

   cout<<"PAGE INSERTION page 10,111,222:  "<<endl;
   p.printNode(tree.insertValPage(10,111, 222, tester));

   cout<<"PAGE INSERTION page 45,111,222:  "<<endl;
   p.printNode(tree.insertValPage(45,111, 222, tester));

   tester.keys[0] = 4;
   tester.keys[1] = -1;
   tester.keys[2] = -1;
   tester.keys[3] = -1;

   tester.pointers[0] = 1;
   tester.pointers[1] = 2;
   tester.pointers[2] = -1;
   tester.pointers[3] = -1;
   tester.pointers[4] = -1;

   cout<<"PAGE INSERTION page 2,111,222:  "<<endl;
   p.printNode(tree.insertValPage(2,111, 222, tester));

   cout<<"PAGE INSERTION page 10,111,222:  "<<endl;
   p.printNode(tree.insertValPage(10,111, 222, tester));


}

void test_simplePageFile()
{
   cout<<"TESTING PAGE FILE 1 -----------------------------"<<endl;

   pagefile p;
   const char* filename = "treefile.txt";
   int ret = p.open(filename);
   cout<<"Debug Open: "<<ret<<endl;
   cout<<"num pages "<<p.getTotalPages()<<endl;
   cout<<"root page "<<p.getRootNode()<<endl;
   p.incrPageHeaderNumPages();
   p.setRootNode(7);
   cout<<"num pages "<<p.getTotalPages()<<endl;
   cout<<"root page "<<p.getRootNode()<<endl;

   node node1 = p.nodeConstructor(7);
   cout<<"szie of node" <<sizeof(node1)<<endl;;
   node red = node();
   p.write(7,node1);
   red = p.read(7);
   cout<<"page 7 page num -  "<<red.pageNum<<endl;
   cout<<"page 7 leafmarker - "<<red.leafNode<<endl;
   p.close();

}

void test_recordfile()
{
   cout<<"TESTING RECORD FILE -----------------------------"<<endl;
   recordfile r;
   const char* filename = "recordfile.txt";
  
   const char* test = "test";
   char* buf = (char*)malloc(sizeof(record));

   int ret = r.open(filename);
   cout<<"Debug Open: "<<ret<<endl;

    r.readInCSV("smalldata");

   memset(buf, 0, r.RECORD_SIZE);

   cout<<"READ BUF 1 ------:"<<endl;

   record rec = record();

   r.read(1, buf);

      //print value in buffer
      for(int i=0;i<sizeof(record);i++){
         printf("%c ",buf[i]);
      }

   cout<<"\nEND READ------:"<<endl;

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
   cout<<"\nREC 0 FNAME: "<<rec2.fname<<endl;

 
    cout<<"DONE ----------------"<<endl;

   

   r.close();

}