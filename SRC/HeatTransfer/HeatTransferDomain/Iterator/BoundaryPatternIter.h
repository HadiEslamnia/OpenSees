/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
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
** Fire & Heat Transfer modules developed by:                         **
**   Yaqiang Jiang (y.jiang@ed.ac.uk)                                 **
**   Asif Usmani (asif.usmani@ed.ac.uk)                               **
**                                                                    **
** ****************************************************************** */

// This class is a modified version of LoadPatternIter 
// for the heat transfer module
// Modified by: Yaqiang Jiang(y.jiang@ed.ac.uk)

// LoadPatternIter
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A

#ifndef BoundaryPatternIter_h
#define BoundaryPatternIter_h

class BoundaryPattern;
class TaggedObjectStorage;
class TaggedObjectIter;

class BoundaryPatternIter
{
  public:
    BoundaryPatternIter(TaggedObjectStorage* theStorage);
    virtual ~BoundaryPatternIter();

    virtual BoundaryPattern* operator()(void);
    virtual void reset(void);

  protected:
    
  private:
    TaggedObjectIter& myIter;
    
};

#endif
