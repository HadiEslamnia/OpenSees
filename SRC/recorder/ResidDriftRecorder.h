/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 1395-12-08 22:25:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/ResidDriftRecorder.h,v $
                                                                        
#ifndef ResidDriftRecorder_h
#define ResidDriftRecorder_h

// Written: SAJalali
//
// Description: This file contains the class definition for 
// ResidDriftRecorder. 

#include <Recorder.h>
#include <ID.h>
#include <Vector.h>

class Domain;
class Node;

class ResidDriftRecorder: public Recorder
{
 public:
  ResidDriftRecorder();
  ResidDriftRecorder(int ndI, int ndJ, int dof, int perpDirn,
			Domain &theDomain, 
			OPS_Stream *theHandler,
		  int dataProcMethod, int procGrpNum,
		bool echoTime);
  
  ResidDriftRecorder(const ID &ndI, const ID &ndJ, int dof, int perpDirn,
			Domain &theDomain, 
			OPS_Stream *theHandler,
		int dataProcMethod, int procGrpNum,
		bool echoTime);
  
  ~ResidDriftRecorder();
  
  int record(int commitTag, double timeStamp);
  int restart(void);    
  
  int setDomain(Domain &theDomain);
  int sendSelf(int commitTag, Channel &theChannel);  
  int recvSelf(int commitTag, Channel &theChannel, 
	       FEM_ObjectBroker &theBroker);
  
 protected:
  
 private:	
#ifdef _CSS
	  int procDataMethod;  //flag to indicate element group processing method:
								  //0: no processing, print separate results
								  //1: sum of results
								  //2: maximum of results
								  //3: minimum of results
								  //4: maximum absolute of results
								  //5: minimum absolute of results
   int procGrpNum;
#endif // _CSS
	  int initialize(void);

  ID *ndI;
  ID *ndJ;
  Node **theNodes; // i & j nodes
  int dof;
  int perpDirn;
  Vector *oneOverL;
  
  Matrix *data;
  
  Domain *theDomain;
  OPS_Stream *theOutputHandler;

  bool initializationDone;
  int numNodes;
  bool echoTimeFlag;   // flag indicating whether time to be included in o/p
};

#endif
