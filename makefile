btree: pagefile.cpp recordfile.cpp tester.cpp
	g++ -std=c++11 pagefile.cpp recordfile.cpp tester.cpp -c
	g++ -std=c++11 pagefile.o recordfile.o tester.o -o btreeMain
	
clean:
	rm *.o
	rm btreemain