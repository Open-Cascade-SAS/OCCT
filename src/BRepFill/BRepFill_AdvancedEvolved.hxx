// Created on: 2018-03-14
// Created by: Nikolai BUKHALOV
// Copyright (c) 1999-2018 OPEN CASCADE SAS
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

#ifndef _BRepFill_AdvancedEvolved_HeaderFile
#define _BRepFill_AdvancedEvolved_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

class BOPAlgo_MakerVolume;
class TopoDS_Face;

//! Constructs an evolved volume from a spine (wire or face)
//! and  a profile ( wire).
class BRepFill_AdvancedEvolved
{
public:
  DEFINE_STANDARD_ALLOC;

  //! Constructor
  Standard_EXPORT BRepFill_AdvancedEvolved()
      : myErrorStatus(BRepFill_AdvancedEvolved_Empty),
        myFuzzyValue(0.0),
        myIsParallel(Standard_True),
        myDebugShapesPath("C:\\Temp")
  {
  }

  Standard_EXPORT void Perform(const TopoDS_Wire&     theSpine,
                               const TopoDS_Wire&     theProfile,
                               const Standard_Real    theTolerance,
                               const Standard_Boolean theSolidReq = Standard_True);

  Standard_Boolean IsDone(unsigned int* theErrorCode = 0) const
  {
    if (theErrorCode)
      *theErrorCode = myErrorStatus;

    return (myErrorStatus == BRepFill_AdvancedEvolved_OK);
  }

  //! returns the resulting shape.
  const TopoDS_Shape& Shape() const { return myResult; }

  //! Sets directory where the debug shapes will be saved
  void SetTemporaryDirectory(const Standard_CString& thePath) { myDebugShapesPath = thePath; }

  //! Sets/Unsets computation in parallel mode
  void SetParallelMode(const Standard_Boolean theVal) { myIsParallel = theVal; }

protected:
  Standard_EXPORT void PerformSweep();

  Standard_EXPORT void GetLids();

  Standard_EXPORT void BuildSolid();

  Standard_EXPORT void RemoveExcessSolids(const TopTools_ListOfShape& theLSplits,
                                          const TopoDS_Shape&         theShape,
                                          TopTools_ListOfShape&       theArgsList,
                                          BOPAlgo_MakerVolume&        theMV);

  Standard_EXPORT void ExtractOuterSolid(TopoDS_Shape& theShape, TopTools_ListOfShape& theArgsList);

  Standard_EXPORT void GetSpineAndProfile(const TopoDS_Wire& theSpine,
                                          const TopoDS_Wire& theProfile);

  Standard_EXPORT void UnifyShape();

  Standard_EXPORT Standard_Boolean PerformBoolean(const TopTools_ListOfShape& theArgsList,
                                                  TopoDS_Shape&               theResult) const;

  Standard_EXPORT Standard_Boolean
    CheckSingularityAndAdd(const TopoDS_Face&    theF,
                           const Standard_Real   theFuzzyToler,
                           TopTools_ListOfShape& theListOfFaces,
                           TopTools_ListOfShape& theListOfSplits) const;

  Standard_EXPORT Standard_Boolean IsLid(const TopoDS_Face&                theF,
                                         const TopTools_IndexedMapOfShape& theMapOfLids) const;

private:
  enum
  {
    BRepFill_AdvancedEvolved_Empty = 0,
    BRepFill_AdvancedEvolved_NotPlanarSpine,
    BRepFill_AdvancedEvolved_SweepError,
    BRepFill_AdvancedEvolved_NoLids,
    BRepFill_AdvancedEvolved_NotSolid,
    BRepFill_AdvancedEvolved_NotVolume,
    BRepFill_AdvancedEvolved_OK = UINT_MAX
  } myErrorStatus;

  TopoDS_Wire      mySpine;
  TopoDS_Wire      myProfile;
  TopoDS_Shape     myPipeShell;
  TopoDS_Compound  myTopBottom; // Lids can be split on several faces
  TopoDS_Shape     myResult;
  Standard_Real    myFuzzyValue;
  Standard_Boolean myIsParallel;
  Standard_CString myDebugShapesPath;
};

#endif // _BRepFill_AdvancedEvolved_HeaderFile
