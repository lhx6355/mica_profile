#ifndef CALCCRITICALPATH_H
#define CALCCRITICALPATH_H

#define ROB_SIZE	40	//[40]
#define C_DIST_SIZE	41 // the indecies start from 1
#define DELAY 1

class AdjListNode
{
	friend class LongestDepGraph;
	int v;	//SREG
	int weight;

public:
	AdjListNode(const int& _v, const int& _w) { v = _v; weight = _w; }

	~AdjListNode() {}

	int getV() const { return v; }

};

class LongestDepGraph
{
	int numV; // No. of vertices // [LHX] ROB size
	deque<AdjListNode>* adj;

public:
	LongestDepGraph(const int& V) {
		this->numV = V;
		adj = new deque<AdjListNode>[numV];
	}

	~LongestDepGraph() {
		delete[] adj;
	}

	void addEdge(const int& u, const int& v, const int& weight);

	void topologicalSortUtil(const int& v, bool* visited, stack<int>& Stack);

	int longestPath(const int& s);

	void printadj(LongestDepGraph* dg);
};

class CriticalPath {
	vector<InstReg> committedInstList;

	//int *criticalPathLength;
	//int *fpCritPath;
	/* for debug */
	//int criticalPathLength[C_DIST_SIZE];  // by shen 10/19
	int* criticalPathLength;
	uint32_t robSize; /* ROB size */
	int distSize;/* distribution array size */

public:
	/* set ROB size ,default is 40 */
	CriticalPath(const int& size) : robSize(size), distSize(size + 1) {
		//criticalPathLength = new int[distSize];
		//fpCritPath = new int[distSize];
		committedInstList.reserve(robSize);  //by shen 10/19
		criticalPathLength = new int[robSize + 1];		//[pz]
		for (int i = 0; i < distSize; ++i)
			criticalPathLength[i] = 0;
	}

	CriticalPath() : CriticalPath(ROB_SIZE) {};

	~CriticalPath() {
		delete[] criticalPathLength;
	}

	void calCriticalPath(const InstReg& instReg);

	const int* getCritPathLen() const { return criticalPathLength; }

	//const int * getFpCritPathLen() const { return fpCritPath; }

	const int& getSize() const { return distSize; }

	//[pz]calculate K(ROB)
	double cal_KROB();
};

#endif
