//5150 Project: Random Walks
//https://www.usenix.org/conference/atc20/presentation/wang-rui 

#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <fstream>
#include <vector>
#include <array>
#include <sstream>
#include <string>
#include <string.h>
#include <math.h>
#include <time.h>

using namespace std;

unsigned long long int defineWalk (int source,int current,int step);

void splitWalk(unsigned long long int walk, int &source, int &current, int &step);

int initializeWalks(int numberOfWalks);

int findNextBlock(vector<string> walkPools);

int findBlockGivenAVertex(int vertex, vector<int> block, vector<string> walkPools);

void updateWalkPools(int walkTo[], unsigned long long int walkFrom[], vector<int> block, vector<string> walkPools);

unsigned long long int ulli = 18446744073709551615;


int main() {
    cout << "5150 Project: Random Walks\nBy: James Durflinger\n";
	
	srand(time(NULL));
	
	//Read in all csv files
	ifstream file;
	string line;
	
	
	/*
	a = 9223372036854775807;
	a = 16383;
	a = 6107787201519201;
	int b = 5555; // source - 000000000001010110110011
	int c = 6666; // current - 00000000000001101000001010
	int d = 7777; // step - 01111001100001
	All Together - 000000000001010110110011 - 00000000000001101000001010 - 01111001100001
	               000000000001010110110011   00000000000001101000001010   01111001100001
	a = defineWalk(b,c,d);
	cout << "\n";
	splitWalk(a,b,c,d);
	cout << "\n";
	*/
	
	char whichDataset[80];
	cout << "Pick a Dataset, Enter Name:";
	cin >> whichDataset;
	//Pokec is the only data set that can work without reading everything in
	
	int noe;
	cout << "Enter a Maximum Number of Edges, \"noe\", to Use (noe < 1 For All Nodes, 2 Edges minimum): \n";
	cin >> noe;
	if (noe == 1 || noe == 0)
	{
		noe = 2;
		cout << "Value Overwritten to 2\n";
	}
	
	// The default size is described in the paper as “2(log10 R+2)” in megabytes (R=Total # of walks). 
	int blockSize;
	cout << "Enter a Block Size(Number of Edges Per Block, There Should Atleast Be 2 Blocks): \n";
	cin >> blockSize;
	if (noe/2.0 < blockSize || blockSize < 1)
	{
		blockSize = floor(noe/2.0);
		cout << "Value Overwritten to " << to_string(blockSize) << "\n";
	}
	
	
	int numberOfWalks;
	cout << "Enter Number of Walks (1024 Walks in a WalkPool, 1 Walk Minimum): \n";
	cin >> numberOfWalks;
	
	if (numberOfWalks < 1)
	{
		numberOfWalks = 1;
		cout << "Value Overwritten to 1\n";
	}
	
	//For Demo
	if (numberOfWalks > 1023)
	{
		numberOfWalks = 1023;
	}
	
	char maxSteps;
	cout << "Enter Max Number of Steps Per Walk:";
	cin >> maxSteps;
	
	vector<array<int, 2>> allEdges;
	vector<int> allNodes;
	
	cout << "Begin Reading\n";
	
	if (noe > 0)
	{
		if (strcmp(whichDataset, "friendster-edges.csv") == 0)
		{
			cout << "Opening: friendster-edges.csv\n";
			/*file.open("friendster-nodes.csv");
			while(!file.eof())
			{
				file>>line;
				allNodes.push_back(stoi(line));
				//cout<<line<<" ";
			}*/
			file.open("friendster-edges.csv");
			while(!file.eof() && noe > 0)
			{
				file>>line;
				stringstream s_stream(line);
				int i;
				int arr[2] = {0,0};
				for(i = 0; i < 2; i++)
				{
					string substr;
					getline(s_stream, substr, ',');
					arr[i] = stoi(substr);
				}
				allEdges.push_back({arr[0],arr[1]});
				noe -= 1;
			}
			file.close();
		}
		else if (strcmp(whichDataset, "livejournal-edges.csv") == 0)
		{
			cout << "Opening: livejournal-edges.csv\n";
			file.open("livejournal-edges.csv");
			while(!file.eof() && noe > 0)
			{
				file>>line;
				stringstream s_stream(line);
				int i;
				int arr[2] = {0,0};
				for(i = 0; i < 2; i++)
				{
					string substr;
					getline(s_stream, substr, ',');
					arr[i] = stoi(substr);
				}
				allEdges.push_back({arr[0],arr[1]});
				noe -= 1;
			}
			file.close();
		}
		else if (strcmp(whichDataset, "soc-pokec-relationships.txt") == 0)
		{
			cout << "Opening: soc-pokec-relationships.txt\n";
		
			file.open("soc-pokec-relationships.txt");
			while(!file.eof() && noe > 0)
			{
				file>>line;
				int arr[2] = {0,0};
				string substr;
				arr[0] = stoi(line);
				file >> line;
				arr[1] = stoi(line);
				allEdges.push_back({arr[0],arr[1]});
				noe -= 1;
			}
			file.close();
		}
		else
		{
			cout << "Opening: " << whichDataset <<"\n";
		
			file.open(whichDataset);
			while(!file.eof() && noe > 0)
			{
				file>>line;
				int arr[2] = {0,0};
				string substr;
				arr[0] = stoi(line);
				file >> line;
				arr[1] = stoi(line);
				allEdges.push_back({arr[0],arr[1]});
				noe -= 1;
			}
			file.close();
		}
	}
	
	//Define a walk
	unsigned long long int walk;
	int i;
	int j;
	
	//cout << "Edges:\n";
	//for (i = 0; i < allEdges.size(); i++)
	//{
	//	cout << allEdges[i][0] << " " << allEdges[i][1] << endl;
	//}
	
	
	cout << "File Read\n";
	
	//Order Edges
	vector<array<int, 2>> allEdgesOrdered;
	for (i = 0; i < allEdges.size(); i++)
	{
		int index = i;
		int stop = 0;
		for (index = i; index > 0; index--)
		{
			if (allEdgesOrdered.size() > 0)
			{
				if(allEdgesOrdered[index][0] == allEdges[i][0])
				{
					if(allEdgesOrdered[index][1] <= allEdges[i][1])
					{
						allEdgesOrdered.insert(allEdgesOrdered.begin()+index, {allEdges[i][0],allEdges[i][1]});
						stop = 1;
					}
				}
				else
				{
					allEdgesOrdered.push_back({allEdges[i][0],allEdges[i][1]});
					stop = 1;
				}
			}
			else
			{
				allEdgesOrdered.push_back({allEdges[i][0],allEdges[i][1]});
				stop = 1;
			}
			if (stop == 1)
				break;
		}
	}
	
	cout << "Order Edges\n";
	
	//cout << "Edges:\n";
	//for (i = 0; i < allEdgesOrdered.size(); i++)
	//{
	//	cout << allEdgesOrdered[i][0] << " " << allEdgesOrdered[i][1] << endl;
	//}
	
	//CSR Vector
	vector<int> csr;
	i = 0;
	for (i = 0; i < allEdgesOrdered.size(); i++)
	{
		csr.push_back(allEdgesOrdered[i][1]);
	}
	
	cout << "CSR info created\n";
	
	//Index Vector
	vector<int> index;
	i = 0;
	int last = -999999999;
	for (i = 0; i < allEdgesOrdered.size(); i++)
	{
		if (last != allEdgesOrdered[i][0])
		{
			last = allEdgesOrdered[i][0];
			index.push_back(i);
		}
	}	
	
	cout << "Index info created\n";
	
	//Block Vector
	vector<int> block;
	int beginningBlock = 0;
	int currentBlockSize = 0;
	for (i = 0; i < index.size(); i++)
	{

		if ((index[i] - beginningBlock) == blockSize)
		{
			block.push_back(beginningBlock);
			beginningBlock = index[i];
		}
		else if ((index[i] - beginningBlock) > blockSize)
		{
			block.push_back(beginningBlock);
			beginningBlock = index[i-1];
		}
	}	
	
	cout << "Block info created\n";
	
	//Create Walk Pools for each Block (After each step)(Also initialize all walks at block 0)
	vector<string> walkPools;
	
	int filesCreated;
	filesCreated = initializeWalks(numberOfWalks);
	for (i = 0; i < filesCreated; i++)
	{
		string name;
		name = "0 " + to_string(i) + " walkpool.txt";
		walkPools.push_back(name.c_str());
	}
	
	unsigned long long int localWalkPool[1024];
	
	//load 2 blocks initially(blocks 0 and 1)
	int currentBlock = 0;
	int nextBlock = 1;
	bool deadEnd = false;

	cout << "Block:\n";
	for (i = 0; i < block.size(); i++)
	{
		cout << block[i] << ", ";
	}
	
	cout << "\nIndex:\n";
	for (i = 0; i < index.size(); i++)
	{
		cout << index[i] << ", ";
	}
	
	cout << "\nCSR:\n";
	for (i = 0; i < csr.size(); i++)
	{
		cout << csr[i] << ", ";
	}
	cout << "\n";
	// Main Loop ----------------------------------------------------------------
	//           ----------------------------------------------------------------
	while (!deadEnd)
	{
		vector <int> allNodesInThisBlock;
		//Define Graph
		if (currentBlock < (block.size()-1) )
		{
			int next;
			next = block[currentBlock + 1];
			for (i = 0; i < (next - block[currentBlock]); i++)
			{
				allNodesInThisBlock.push_back(block[currentBlock] + i);
			}
		}
		else // current block is the last block
		{
			for (i = 0; i < (index.size() - block[currentBlock]); i++)
			{
				allNodesInThisBlock.push_back(block[currentBlock] + i);
			}
		}
		
		
		vector <int> allPools;
		//Get all Walks
		for (i = 0; i < walkPools.size(); i++)
		{
			stringstream s_stream(walkPools[i]);
			string substr;
			getline(s_stream, substr, ' ');
			if (stoi(substr) == currentBlock)
			{
				allPools.push_back(i);
			}
		}
		//Update all walks
		for(i = 0; i < allPools.size(); i++)
		{
			int walksThatLeft[1024];
			//Read in current walk Pool into local walk pool
			for (j = 0; j < 1024; j++)
			{
				localWalkPool[j] = ulli;
				walksThatLeft[j] = -1;
			}
			int walkIndex = 0;
			file.open(walkPools[allPools[i]].c_str());
			while(!file.eof())
			{
				file>>line;
				unsigned long long int temp;
				temp = stoull(line);
				if (temp != ulli)
				{
					localWalkPool[walkIndex] = temp;
					walkIndex += 1;
				}
			}
			file.close();
			
			//Update walks in this walk pool
			int source;
			int current;
			int steps;
			bool stillUpdating = true;
			while(stillUpdating)
			{
				int walksInTheWalkPool = 0;
				for (j = 0; j < 1024; j++)
				{
					if (localWalkPool[j] != ulli)
					{
						vector <int> nextOption;
						unsigned long long int temp;
						temp = localWalkPool[j];
						splitWalk(temp, source, current, steps);
						if (steps > maxSteps)
						{
							int k;
							if (current < (index.size()-1))
							{
								for (k = 0; k < index[current+1]-index[current]; k++)
								{
									nextOption.push_back(csr[index[current] + k]);
								}
							}
							else
							{
								for (k = 0; k < csr.size() - index[current] - 1; k++)
								{
									nextOption.push_back(csr[index[current] + k]);
								}
							}
							
							int options = nextOption.size()-1;
							double myRandom;
							myRandom = rand() % options;
							current = nextOption[myRandom];
							
							steps += 1;
							temp = defineWalk(source,current,steps);
							localWalkPool[j] = temp;
							bool leftBlock = true;
							for (k = 0; k < allNodesInThisBlock.size(); k++)
							{
								if(current == allNodesInThisBlock[k])
								{
									leftBlock = false;
								}
							}
							if (leftBlock)
							{
								walksThatLeft[j] = current;
							}
							
							walksInTheWalkPool += 1;
						}
					}
				}
				//Check if all walks left
				int leftBlock = 0;
				for (j = 0; j < 1024; j++)
				{
					if(walksThatLeft[j] != -1)
					{
						leftBlock += 1;
					}
				}
				//cout << leftBlock << " " << walksInTheWalkPool << endl;
				if (leftBlock >= walksInTheWalkPool)
				{
					stillUpdating = false;
					updateWalkPools(walksThatLeft, localWalkPool, block, walkPools);
				}
			}
			//Empty Out This Walk Pool
			file.open(walkPools[allPools[i]].c_str());
			file.clear();
			file.close();
		}
		//load new block
		currentBlock = findNextBlock(walkPools);
		
		bool theEnd = true;
		for (j = 0; j < walkPools.size(); j++)
		{
			if (theEnd)
			{
				file.open(walkPools[allPools[j]].c_str());
				while(!file.eof())
				{
					file>>line;
					unsigned long long int temp;
					temp = stoull(line);
					if (temp != ulli)
					{
						theEnd = false;
						break;
					}
				}
				file.close();
			}
		}
		if (theEnd)
		{
			deadEnd = true;
			cout << "All Walks Came to an End Point.\n";
		}
		
	}
	
    return 0;
}


//After Updating a walk pool file
void updateWalkPools(int walkTo[], unsigned long long int walkFrom[], vector<int> block, vector<string> walkPools)
{
	ifstream file;
	ofstream fileOut;
	string line;
	unsigned long long int pool[1024];
	int sizeOfPool = 0;
	int i;
	int j;

	for (i = 0; i < 1024; i++)
	{
		if (walkTo[i] != -1)
		{
			
			for(j = 0; j < 1024; j++)
			{
				pool[j] = ulli;
			}
			int where;
			where = findBlockGivenAVertex(walkTo[i], block, walkPools);
			if (where != -1)
			{
				file.open(walkPools[where].c_str());
				//while (!file.eof())
				//{
				//	file>>line;
				//	sizeOfPool += 1;
				//}
				//file.close();
				//if (sizeOfPool > 1023)
				file.open(walkPools[where].c_str());
				while (!file.eof())
				{
					file>>line;
					unsigned long long int number;
					number = stoull(line);
					pool[sizeOfPool] = number;
					sizeOfPool += 1;
				}
				pool[sizeOfPool] = walkFrom[i];
				file.close();
				
				fileOut.open(walkPools[where].c_str());
				for(j = 0; j < 1024; j++)
				{
					if (pool[j] != ulli)
						fileOut << pool[j] << "\n";
				}
				fileOut.close();
			}
		}
	}
	return;
}


//Initializing Walks, only at beginning
int initializeWalks(int numberOfWalks)
{
	cout << "Initializing Walks\n";
	unsigned long long int walkPool[1024];
	ofstream file;
	string name;
	int i;
	int j;
	int files;
	files = 1;
	
	for(j = 0; j < 1024; j++)
	{
		walkPool[j] = ulli;
	}
	
	
	double d = numberOfWalks / 1024.0;
	double floorD = floor(d);
	for (i = 0; i < floorD; i++)
	{
		for(j = 0; j < 1024; j++)
		{
			unsigned long long int walk;
			walk = defineWalk(0,0,0);
			walkPool[i] = walk;
		}
		name = "0 " + to_string(files-1) + " walkpool.txt";
		files += 1;
		file.open(name.c_str());
		for(j = 0; j < 1024; j++)
		{
			if (walkPool[i] != ulli)
				file << walkPool[j] << "\n";
		}
		file.close();
	}
	
	for(j = 0; j < 1024; j++)
	{
		walkPool[i] = ulli;
	}
	
	if ((numberOfWalks-(floorD*1024)) > 0)
	{
		name = "0 " + to_string(files-1) + " walkpool.txt";
		file.open(name.c_str());
		for (i = 0; i < (numberOfWalks-(floorD*1024)); i++)
		{
			unsigned long long int walk;
			walk = defineWalk(0,0,0);
			walkPool[i] = walk;
		}
		for(i = 0; i < 1024; i++)
		{
			if (walkPool[i] != ulli)
				file << walkPool[i] << "\n";
		}
		file.close();
	}
	cout << files <<" Walk Pool(s)\n";
	return files;
}


//Find a block/walkpool based on a node (when leaving one block to another)
int findBlockGivenAVertex (int vertex, vector<int> block, vector<string> walkPools)
{
	int foundBlock;
	int whereInWalkPools;
	int i;
	
	foundBlock = -1;
	whereInWalkPools = -1;
	for (i = 0; i < block.size(); i++)
	{
		if(block[i] > vertex)
		{
			foundBlock = block[i-1];
			break;
		}
	}
	
	if (foundBlock != -1)
	{
		for (i = 0; i < walkPools.size(); i++)
		{
			stringstream s_stream(walkPools[i]);
			string substr;
			getline(s_stream, substr, ' ');
			if (stoi(substr) == foundBlock)
			{
				whereInWalkPools = i;
				break;
			}
		}
	}
	//if dead end, return -1
	return whereInWalkPools;
}



//Function: (80%)Find most walks or (20%)find block with least steps
int findNextBlock (vector<string> walkPools)
{
	ifstream file;
	int chosenBlock;
	int i;
	string line;
	
	int temp[walkPools.size()];
	int lastBlock = -1;
	int same = 0;
	
	double myRandom;
	myRandom = rand() % 100;
	
	if (myRandom > 80)
	{
		int smallestSteps = 9999999;
		int smallestIndex = -1;
		for(i = 0; i < walkPools.size(); i++)
		{
			file.open(walkPools[i].c_str());
			while(!file.eof())
			{
				file>>line;
				unsigned long long int number;
				number = stoull(line);
				if (number != ulli)
				{
					int a;
					int b;
					int steps;
					splitWalk(number,a,b,steps);
					if (steps < smallestSteps)
					{
						smallestSteps = steps;
						smallestIndex = i;
					}
				}
			}
			file.close();
		}
		stringstream s_stream(walkPools[smallestIndex]);
		string substr;
		getline(s_stream, substr, ' ');
		chosenBlock = stoi(substr);
	}
	else
	{
		for(i = 0; i < walkPools.size(); i++)
		{
			stringstream s_stream(walkPools[i]);
			string substr;
			getline(s_stream, substr, ' ');
			if (lastBlock == stoi(substr))
			{
				same += 1;
			}
			else
			{
				same = 0;
			}
			file.open(walkPools[i].c_str());
			while(!file.eof())
			{
				file>>line;
				unsigned long long int number;
				number = stoull(line);
				if (number != ulli)
				{
					temp[i-same] += 1;
				}
			}
			file.close();
			lastBlock = stoi(substr);
		}
		
		int biggestIndex = -1;
		int biggestValue = -1;
		for(i = 0; i < walkPools.size(); i++)
		{
			if (temp[i] > biggestValue)
			{
				biggestIndex = i;
				biggestValue = temp[i];
			}
		}
		stringstream s_stream(walkPools[biggestIndex]);
		string substr;
		getline(s_stream, substr, ' ');
		chosenBlock = stoi(substr);
	}

	return chosenBlock;
}


//63-source(24)-40 39-current(26)-14 13-step(14)-0
unsigned long long int defineWalk (int source,int current,int step)
{
	unsigned long long int walk;
	int binary[64];
	int binaryStep[14];
	int binaryCurrent[26];
	int binarySource[24];
	int i;
	i = 0;
	//next
	while (step > 0)
	{
		binaryStep[i] = step % 2;
		step = step/2;
		i++;
	}
	int x;
	for (x = i; x < 14; x++)
	{
		binaryStep[x] = 0;
	}
	//next
	i = 0;
	while (current > 0)
	{
		binaryCurrent[i] = current % 2;
		current = current/2;
		i++;
	}
	for (x = i; x < 26; x++)
	{
		binaryCurrent[x] = 0;
	}
	//next
	i = 0;
	while (source > 0)
	{
		binarySource[i] = source % 2;
		source = source/2;
		i++;
	}
	for (x = i; x < 24; x++)
	{
		binarySource[x] = 0;
	}
	
	x = 0;
	for (i = 23; i >= 0; i--)
	{
		binary[x] = binarySource[i];
		x++;
	}
	for (i = 25; i >= 0; i--)
	{
		binary[x] = binaryCurrent[i];
		x++;
	}
	for (i = 13; i >= 0; i--)
	{
		binary[x] = binaryStep[i];
		x++;
	}
	
	//for (i = 0; i < 64; i++)
	//{
	//	cout << binary[i];
	//}
	//cout << "\n";
	walk = 0;
	unsigned long long int mult;
	mult = 1;
	for (i = 63; i >= 0; i--)
	{
		walk += binary[i] * mult;
		mult *= 2;
	}
	//cout << walk;
	//cout << "\n";
	return walk;
}


// splitWalk(walk,a,b,c);
void splitWalk(unsigned long long int walk, int &source, int &current, int &step)
{
	int binary[64];
	int i;
	while (walk > 0)
	{
		binary[i] = walk % 2;
		//cout << binary[i];
		walk = walk/2;
		i++;
	}
	int x;
	for (x = i; x < 64; x++)
	{
		binary[x] = 0;
		//cout << binary[x];
	}
	
	//for (x = 63; x >= 0; x--)
	//{
	//	cout << binary[x];
	//}
	//cout << "\n";
	
	source = 0;
    current = 0;
    step = 0;
	
	unsigned long long int mult;
	mult = 8388608;
	int output = 0;
	for (x = 63; x >= 0; x--)
	{
		output += binary[x] * mult;
		if (x == 39)
		{
			source = output;
			mult = 33554432;
			output = 0;
		}
		if (x == 13)
		{
			current = output;
			mult = 8192;
			output = 0;
		}
		if (x == 0)
		{
			step = output;
		}
		mult /= 2;
	}
	//cout << "source: " << source << "current: " << current << "step: " << step << "\n";
}

