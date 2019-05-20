btree: pagefile.cpp recordfile.cpp btree.cpp tester.cpp
	g++ -std=c++11 pagefile.cpp recordfile.cpp btree.cpp  tester.cpp -c
	g++ -std=c++11 pagefile.o recordfile.o btree.o tester.o -o btreeMain
	
clean:
	rm *.o
	rm btreemain
	rm recordfile.txt
	rm treefile.txt