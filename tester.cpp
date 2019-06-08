#include <iostream>
#include "recordfile.h"
#include "pagefile.h"
#include "btree.h"

using namespace std;

void test_recordfile();
void test_simplePageFile();
void test_singleNodeManipulations();

int main()
{
   //test_recordfile();
   // test_simplePageFile();
   //test_singleNodeManipulations();
   btree tree =  btree();
   tree.readInCSV("smalldata");
   const char* filen = "treefile.txt";
   tree.pageFile.open(filen);

   // int root = tree.pageFile.getRootNode();
   // cout<<"FINAL TREE PRINT"<<endl;
   // cout<<"root: "<<root<<endl;
   // tree.pageFile.printNode(tree.pageFile.read(root));

   tree.pageFile.close();

}

void endlessTesting(){
   int i=1;

   while(i){
      int i = 0;
      cout<<"Please choose a choice from the following, press 0 to quit: "<<endl;
      cout<<"1 - Insert\n 2 - Bulk Load\n 3 - Search"<<endl;
      cin>>i;
      if(i==1)
      {
         // insertTesting();
      }
      else if (i==2)
      {
         // bulkLoadTesting();
      }
      else if(i==3)
      {
         // searchTesting();
      }
      else{
         cout<<"Sorry, that wasn't one of the choices, please try again.";
      }
   }
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
   cout<<"\nREC 0 FNAME: "<<rec2.fname<<endl;

 
    cout<<"DONE ----------------"<<endl;

   

   r.close();

}