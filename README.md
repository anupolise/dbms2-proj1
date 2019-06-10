#README

##Anu Polisetty 8916967

## HOW TO RUN
Make sure you ```make clean``` before creating a *new* tree, this clears the recordfile and pagefile. If you  then call ```make``` - this will compile all the neccessary components. Lastly, run ```./btreeMain```. This will launch a commandline interface that will give you 3 choices: 1 - insert, 2 - bulk load, and 3 - search. The interface walks the user through these commands. It is important to note that when using bulkload and naming a file to bulk load, the file must be within the same directory.

That being said, if you don't want to create a new file, I left 4 files in this directory for testing:
smalldata: contains 1-2117 records
smalldata2: contains 2118-2217 records
test: contains 1-8430 records
samplesorted.csv: contains all records

## BUFFER
I have a buffer of 10 pages. Everytime I want to read or write, I check the buffer. If the node that I'm looking for isn't in the buffer, I add it to the buffer and I continue with my operations. When adding to the buffer, there are two scenarios: either the buffer is empty, or it's full. If the buffer has empty spots, we can just write our page in the next empty avaliable spot. If the buffer is full, we start at 0 and we start switching pages out of the buffer. We take the original page in the buffer, and write it out to the disk, then we write our new page into the spot we just cleared up. We increment the index of which buffer page we're replacing (first time you replace the 0th buffer page, then 1st, then 2nd) so that we can assume that the next buffer page was less recently used than the one prior. It's basically just a round robin replacement when necessary.

## NOTES
As you may have noticed there are a lot of print statements - I left them in so if you wanted you could check the validity of my tree. They can get spammy, especially for insert andb bulk load, but if you lower the node size you can really see what's going on. 
A note on the page node, each node has a pageID, boolean isLeaf, int numTuples, and two arrays. The first array represents all the keys in the node and the second array represents the left and right pointers for these keys. The ith key has a left pointer and right pointer in i and i+1 respectively in the pointers array.
A note on the buffer, because of the way I handle splits, each node is completely edited when written into the buffer. This means that there is no such thing as a dirty read, because I only write back to the buffer if the node is "finished". 
My code also works for entering data points out of order, although it's just less efficient for this, it doesn't implement any external sorting.
