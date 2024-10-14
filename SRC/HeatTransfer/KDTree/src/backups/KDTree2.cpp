#include <KDTree.h>
#include <ID.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <vector>
using std::cout;
using std::endl;

int (*KDTree::logmsg)(const char *,...)=printf;

KDTree::KDTree()
:workArr(0),copyPoints(false),points(0),pntsInRange(0),
 nodeMem(0),nodeMemCnt(0),nodeMemAlloc(true),intArrMem(0),
 intArrMemCnt(0),verbosity(0),PTSInRange(0)
{
  //workArr = (int *) NULL;
  //copyPoints = false;
  //points = (double *)NULL;
  //pntsInRange = (int *)0;
  //nodeMem = (struct _Node *)0;
  //nodeMemCnt = 0;
  //nodeMemAlloc = true;
  //intArrMem = (int *)0;
  //intArrMemCnt = 0;
  //verbosity=0;
  //return;
}				// end of constructor

KDTree::KDTree(double *setpoints, int N, int setndim)
:workArr(0),copyPoints(false),points(0),pntsInRange(0),
 nodeMem(0),nodeMemCnt(0),nodeMemAlloc(true),intArrMem(0),
 intArrMemCnt(0),verbosity(0),PTSInRange(0)
{

  this->create(setpoints, N, setndim);
  //return;
}	 // end of constructor

      
KDTree::~KDTree()
{
  if (workArr)
    delete[]workArr;
  if (copyPoints && points) {
    // Delete the 1-D array of data
    // Delete the pointer to the points
    delete[] points;
  }
	if(pntsInRange) delete[] pntsInRange;
	if(nodeMem && nodeMemAlloc) delete[] nodeMem;
	if(PTSInRange) delete [] PTSInRange;
  return;
}				// end of destructor

//int KDTree::get_serialize_length(int snpnts, int sdim)
//{
//	return
//		// The data points to be copied
//		snpnts*sdim*sizeof(double)+
//      // The header information (8 bytes only)
//		sizeof(int)*2+
//      // The tree has twice as many nodes as points
//		(snpnts*2*(sizeof(struct _Node)));
//} // end of get_serialize_length
//
//
//KDTree *KDTree::unserialize(void *mem)
//{
//
//	char *cmem = (char *)mem;
//	KDTree *kdtree = new KDTree;
//	kdtree->npoints = ((int *)mem)[0];
//	kdtree->ndim = ((int *)mem)[1];
//	kdtree->points = (double *)(cmem+sizeof(int)*2);
//	kdtree->nodeMem = (struct _Node *)		
//		(cmem+sizeof(int)*2+sizeof(double)*kdtree->ndim*kdtree->npoints);
//	kdtree->nodeMemAlloc = false;
//	kdtree->copyPoints = false;
//	return kdtree;
//} // end of unserialize

struct _Node *KDTree::node_alloc()
{
	nodeMemCnt++;
	if(nodeMemCnt > npoints*2)
		return (struct _Node *)0;
	return (struct _Node *)(nodeMem+nodeMemCnt-1);
} // end of node_alloc

//int KDTree::create(double *setpoints, int setnpoints, int setndim,
//									 void *mem)
//{
//	char *cmem = (char *)mem;
//	((int *)mem)[0] = setnpoints;
//	((int *)mem)[1] = setndim;
//	for(int i=0;i<setnpoints*setndim;i++)
//		((double *)mem)[i+2] = setpoints[i];
//	//memcpy(cmem+sizeof(int)*2,setpoints,sizeof(float)*setnpoints*setndim);
//	return
//		create((float *)(cmem+sizeof(int)*2),
//					 setnpoints,setndim,false,
//					 (struct _Node *)
//					 (cmem+sizeof(int)*2+sizeof(double)*setnpoints*setndim));
//} // end of create

// This function creates a KD tree with the given
// points, array, and length
int KDTree::create(double *setpoints, int setnpoints, int setndim,
									 bool setCopy, struct _Node *setNodeMem)
{
  ndim = setndim;
  npoints = setnpoints;
  typedef int *intptr;
  
  // Copy the points from the original array, if necessary
  copyPoints = setCopy;
  if (copyPoints) {
    if(points) delete[] points;
    points = new double[ndim*npoints];
    memcpy(points,setpoints,sizeof(double)*ndim*npoints);
  }
  // If we are not copying, just set the pointer
  else
    points = setpoints;

  // Allocate some arrays;
  if (workArr)
    delete[]workArr;
  workArr = new int[npoints];

	if(!setNodeMem) {
		if(nodeMem) delete[] nodeMem;
		nodeMem = new struct _Node[npoints*2+1];
		nodeMemAlloc = true;
	}
	else {
		nodeMem = setNodeMem;
		nodeMemAlloc = false;
	}
	nodeMemCnt = 0;

	// Alocate array used for indexing
	if(intArrMem) delete[] intArrMem;
	intArrMem = 
		new int[(int)((double)(npoints+4)*
									ceil(log((double)npoints)/log(2.0)))];
	intArrMemCnt = 0;

  // Create the "sortidx" array by 
  // sorting the range tree points on each dimension
  int **sortidx = new intptr[ndim];
	if(verbosity>1)
		logmsg("KDTree: Sorting points\n");
  for (int i = 0; i < ndim; i++) {
    // Initialize the sortidx array for this
    // dimension
    sortidx[i] = new int[npoints];

    // Initialize the "tmp" array for the sort
    int *tmp = new int[npoints];
    for (int j = 0; j < npoints; j++)
      tmp[j] = j;

    // Sort the points on dimension i, putting
    // indexes in array "tmp"
    this->heapsort(i,tmp,npoints);

    // sortidx is actually the inverse of the 
    // index sorts
    for (int j = 0; j < npoints; j++)
      sortidx[i][tmp[j]] = j;

    delete[] tmp;
  }
	if(verbosity > 1)
		logmsg("KDTree: Done sorting points\n");

  // Create an initial list of points that references 
  // all the points
  int *pidx = new int[npoints];
  for (int i = 0; i < npoints; i++)
    pidx[i] = i;

  // Build a KD Tree
  this->build_kdtree(sortidx,	// array of sort values
							 0,	// The current dimension
							 pidx, npoints);	// The list of points
  // Delete the sort index
  for (int i = 0; i < ndim; i++)
    delete[]sortidx[i];
  delete[] sortidx;

  // Delete the initial list of points
  delete[] pidx;

	// Delete the sort arrays
	delete[] intArrMem;

	// delete the work array
  if(workArr) {
    delete[] workArr;
    workArr = (int *)NULL;
  }

	if(verbosity > 1)
		logmsg("KDTree: Done creating tree\n");
  return 0;
}				// end of create      


int KDTree::heapsort(int dim, int *idx, int len)
{
  unsigned int n = len,i=len/2,parent,child;
  int t;

  for(;;) {
    if(i>0) {
      i--;
      t = idx[i];
    }
    else {
      n--;
      if(n ==0) return 0;
      t  = idx[n];
      idx[n] = idx[0];
    }
  
    parent = i;
    child = i*2+1;
		
    while(child < n) {
      if((child +1 < n) &&
				 (points[(idx[child+1])*ndim+dim] > 
					points[idx[child]*ndim + dim])) {
				child++;
      }
      if(points[idx[child]*ndim+dim] > points[t*ndim+dim]) {
				idx[parent] = idx[child];
				parent = child;
				child = parent*2+1;
      }
      else {
				break;
      }
    }
    idx[parent] = t;
  } // end of for loop
  return 0;
} // end of heapsort


int *KDTree::int_alloc(int len)
{
	if(!intArrMem) return (int *)0;
	int *ret = intArrMem+intArrMemCnt;
	intArrMemCnt += len;
	return ret;
} // end of int_alloc


// This function build a node of the kdtree with the
// points indexed by pidx with length "len"
// sortidx is a pre-computed array using the heapsort
// algorithm above
int KDTree::build_kdtree(int **sortidx, int dim,
                                   int *pidx, int len)
{
	int ncnt = nodeMemCnt;
  struct _Node *node = node_alloc();

  if (len == 1) {
    node->leftIdx = -1;
    node->rightIdx = -1;
    node->pntidx = pidx[0];
    node->key = 0;
    return ncnt;
  }
  
  // If not, we must make a node
  int pivot = -1;
  int lcnt = 0, rcnt = 0;
	int *larray, *rarray;

  // Find the pivot (index of median point of available
  // points on current dimension).

	// If heapsorting the current list of points is quicker than
	// iterating through all the points, just do that instead 
	// (heapsort if of order n*log(n)
	// This test could probably use some fine tuning
	if(len*log((double)len) < npoints) {
		heapsort(dim,pidx,len);
		larray = pidx;
		rarray = pidx+len/2;
		pivot = pidx[len/2];
		lcnt = len/2;
		rcnt = len/2 + (len%2==0 ? 0 : 1);
	}
	else {
		// Use the previously calculated sort index
		// to make this a process linear in npoints
		// This gets a little confusing, but it works.
		// Sortidx:: sortidx[dim][idx] = val 
		// idx = the index to the point
		// val = the order in the array
		int *parray = workArr;
		
		// Setting parray to -1 indicates we are not using 
		// the point
		for (int i = 0; i < npoints; i++)
			parray[i] = -1;
		// Populate "parray" with the points that we
		// are using, indexed in the order they occur
		// on the current dimension
		for (int i = 0; i < len; i++)
			parray[sortidx[dim][pidx[i]]] = pidx[i];
		int cnt = 0;
		larray = this->int_alloc(len/2+1);
		rarray = this->int_alloc(len/2+1);
		
		// The middle valid value of parray is the pivot,
		// the left go to a node on the left, the right
		// go to a node on the right.
		for (int i = 0; i < npoints; i++) {
			if (parray[i] == -1)
				continue;
			if (cnt == len / 2) {
				pivot = parray[i];
				rarray[rcnt++] = parray[i];
			} else if (cnt > len / 2)
				rarray[rcnt++] = parray[i];
			else
				larray[lcnt++] = parray[i];
			cnt++;
			if(cnt>len)
				break;
		}
	}

  // Create the node
  node->pntidx = -1;
  node->key = points[pivot*ndim+dim];
  int check = points[pivot*ndim+dim];
  // Create nodes to the left
	node->leftIdx = 
		this->build_kdtree(sortidx, (dim + 1) % ndim, larray, lcnt);
	
	// Create nodes to the right
	node->rightIdx = 
		this->build_kdtree(sortidx, (dim + 1) % ndim, rarray, rcnt);

  return ncnt;
}				// end of build_kdtree


// This function operates a search on a node for points within
// the specified range.
// It assumes the current node is at a depth corresponding to 
// dimension "dim"
int KDTree::range_search(int nodeIdx, Range * range, int dim)
{
  if (nodeIdx < 0)
    return 0;
	
	struct _Node *node = nodeMem+nodeIdx;
  // If this is a leaf node, check to see if the 
  // data is in range.  If so, operate on it.
  if (node->pntidx>=0) {
    // Return if not in range
    for (int i = 0; i < ndim; i++) {
			int idx = node->pntidx*ndim + i;
      if (points[idx] < range[i][0] || points[idx] > range[i][1])
				return 0;
    }
		pntsInRange[nPntsInRange++] = node->pntidx;
    return 0;
  }
  double x1 = range[0][0]; 
  double x2 = range[0][1]; 
  double y1 = range[1][0]; 
  double y2 = range[1][1]; 
  // Search left, if necessary
  if (node->key >= range[dim][0])
    this->range_search(node->leftIdx, range, (dim + 1) % ndim);
  // Search right,if necessary
  if (node->key <= range[dim][1])
    this->range_search(node->rightIdx, range, (dim + 1) % ndim);
  return 0;
}				// end of range_search

// This is the public function that will call the
// function "opFunction" on all points in the array
// within "range"
const std::vector<int>& 
KDTree::get_points_in_range(Range * range)
{
  if(!pntsInRange) 
	  pntsInRange = new int[npoints];
  nPntsInRange = 0;

  this->range_search(0, range, 0);
   
  if(!PTSInRange){
	  PTSInRange = new std::vector<int>(nPntsInRange);
	  for(int i=0;i<nPntsInRange;i++){
		  (*PTSInRange)[i] = pntsInRange[i];
		  }
	  } else {
		  (*PTSInRange).erase((*PTSInRange).begin(),(*PTSInRange).end());
		  for(int i=0;i<nPntsInRange;i++){
			  (*PTSInRange).push_back(pntsInRange[i]);
			  }
	  }

  //cout << "Number of points in range: " << nPntsInRange << endl;
  //cout << "The following points are in range:" << endl;

  return *PTSInRange;
}
