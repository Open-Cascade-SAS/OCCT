// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BOPAlgo_Algo_HeaderFile
#define _BOPAlgo_Algo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Message_ProgressRange.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <BOPAlgo_Options.hxx>

class BOPAlgo_PISteps;

//! The class provides the root interface for the algorithms in Boolean Component.<br>
class BOPAlgo_Algo : public BOPAlgo_Options
{
public:

  DEFINE_STANDARD_ALLOC

  //! The main method to implement the operation
  //! Providing the range allows to enable Progress indicator User break functionalities.
  Standard_EXPORT virtual void Perform(const Message_ProgressRange& theRange = Message_ProgressRange()) = 0;

protected:

  //! Default constructor
  Standard_EXPORT BOPAlgo_Algo();
  Standard_EXPORT virtual ~BOPAlgo_Algo();

  Standard_EXPORT BOPAlgo_Algo(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Checks input data
  Standard_EXPORT virtual void CheckData();

  //! Checks the obtained result
  Standard_EXPORT virtual void CheckResult();

protected: //! @name Analyzing operations to fill progress indicator

  //! Analyze progress steps of the whole operation.
  //! @param theWhole - sum of progress of all operations.
  //! @oaram theSteps - steps of the operations supported by PI
  //!
  //! To use this method, one has to override the following methods:
  //! * fillPIConstants - method filling values for constant operations.
  //! * fillPISteps - method filling steps for the rest of operations.
  Standard_EXPORT void analyzeProgress(const Standard_Real theWhole,
                                       BOPAlgo_PISteps& theSteps) const;

  //! Fills the values for constant operations - the operations having constant relative running time.
  //! @param theWhole - sum of all operations supported by PI, i.e. the value to normalize the steps to, if necessary.
  //! @param theSteps - steps of the operations supported by PI
  Standard_EXPORT virtual void fillPIConstants(const Standard_Real theWhole,
                                               BOPAlgo_PISteps& theSteps) const;

  //! Fills the values for the operations dependent on the inputs.
  //! Filled values may not be normalized to represent percentage of total running time.
  //! The values should just correlate to each other.
  //! E.g. if progress depends on the number of input shapes, the values may look like this:
  //! step1 = number_of_input_vertices;
  //! step2 = 2 * number_of_input_edges;
  //! step3 = 10 * number_of_input_faces.
  //! Normalization of these values will be done automatically in analyzeProgress() method.
  Standard_EXPORT virtual void fillPISteps(BOPAlgo_PISteps& theSteps) const;
};

//! Additional root class to provide interface to be launched from parallel vector.
//! It already has the range as a field, and has to be used with caution to create
//! scope from the range only once.
class BOPAlgo_ParallelAlgo : public BOPAlgo_Algo
{
public:
  DEFINE_STANDARD_ALLOC

  //! The main method to implement the operation
  Standard_EXPORT virtual void Perform() = 0;

public:
  //! Sets the range for a single run
  void SetProgressRange(const Message_ProgressRange& theRange)
  {
    myProgressRange = theRange;
  }

private:
  //! Disable the range enabled method
  virtual void Perform(const Message_ProgressRange& /*theRange*/ = Message_ProgressRange()) {};

protected:
  Message_ProgressRange myProgressRange;
};

//! Class for representing the relative contribution of each step of
//! the operation to the whole progress
class BOPAlgo_PISteps
{
public:
  //! Constructor
  BOPAlgo_PISteps(const Standard_Integer theNbOp)
    : mySteps(0, theNbOp - 1)
  {
    mySteps.Init(0);
  }

  //! Returns the steps
  const TColStd_Array1OfReal& Steps() const { return mySteps; }
  //! Returns modifiable steps
  TColStd_Array1OfReal& ChangeSteps() { return mySteps; }

  //! Assign the value theStep to theOperation
  void SetStep(const Standard_Integer theOperation, const Standard_Real theStep)
  {
    if (theOperation >= mySteps.Lower() && theOperation <= mySteps.Upper())
    {
      mySteps(theOperation) = theStep;
    }
  }

  //! Returns the step assigned to the operation
  Standard_Real GetStep(const Standard_Integer theOperation)
  {
    if (theOperation < mySteps.Lower() || theOperation > mySteps.Upper())
    {
      return 0.;
    }
    return mySteps(theOperation);
  }

protected:
  TColStd_Array1OfReal mySteps;
};

#endif // _BOPAlgo_Algo_HeaderFile
