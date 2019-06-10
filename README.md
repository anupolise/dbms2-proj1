#README

##Anu Polisetty 8916967

TODO: 
timing functions?

## HOW TO RUN
Make sure you ```make clean``` before creating a new tree, then call ```make``` - this will compile all the neccessary components. Lastly, run ```./btreeMain```. This will launch a commandline interface that will give you 3 choices: 1 - insert, 2 - bulk load, and 3 - search. The interface walks the user through these commands. It is important to note that when using bulkload and naming a file to bulk load, the file must be within the same directory.

That being said, if you don't want to create a new file, I left 3 files in this directory for testing:
smalldata: contains 2117 records
test: contains 8430 records
samplesorted.csv: contains all records

## BUFFER
I have a buffer of 10 pages. Everytime I want to read or write, I check the buffer. If the node that I'm looking for isn't in the buffer, I add it to the buffer and I continue with my operations. When adding to the buffer, there are two scenarios: either the buffer is empty, or it's full. If the buffer has empty spots, we can just write our page in the next empty avaliable spot. If the buffer is full, we start at 0 and we start switching pages out of the buffer. We take the original page in the buffer, and write it out to the disk, then we write our new page into the spot we just cleared up. We increment the index of which buffer page we're replacing (first time you replace the 0th buffer page, then 1st, then 2nd) so that we can assume that the next buffer page was less recently used than the one prior. It's basically just a round robin replacement when necessary.

