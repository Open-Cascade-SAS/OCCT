// Created by: Peter KURNEV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _BRepAlgoAPI_BuilderAlgo_HeaderFile
#define _BRepAlgoAPI_BuilderAlgo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <BOPAlgo_PPaveFiller.hxx>
#include <BOPAlgo_PBuilder.hxx>
#include <BOPAlgo_GlueEnum.hxx>
#include <Standard_Real.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepAlgoAPI_Algo.hxx>
#include <Standard_Boolean.hxx>
class BOPAlgo_PaveFiller;
class TopoDS_Shape;


//! The class contains API level of the General Fuse algorithm.<br>
//!
//! Additionally to the options defined in the base class, the algorithm has
//! the following options:<br>
//! - *Safe processing mode* - allows to avoid modification of the input
//!                            shapes during the operation (by default it is off);
//! - *Gluing options* - allows to speed up the calculation of the intersections
//!                      on the special cases, in which some sub-shapes are coinciding.
//! - *Disabling the check for inverted solids* - Disables/Enables the check of the input solids
//!                          for inverted status (holes in the space). The default value is TRUE,
//!                          i.e. the check is performed. Setting this flag to FALSE for inverted solids,
//!                          most likely will lead to incorrect results.
//!
//! It returns the following Error statuses:<br>
//! - 0 - in case of success;<br>
//! - *BOPAlgo_AlertTooFewArguments* - in case there are no enough arguments to perform the operation;<br>
//! - *BOPAlgo_AlertIntersectionFailed* - in case the intersection of the arguments has failed;<br>
//! - *BOPAlgo_AlertBuilderFailed* - in case building of the result shape has failed.<br>
//!
//! Warnings statuses from underlying DS Filler and Builder algorithms
//! are collected in the report.
class BRepAlgoAPI_BuilderAlgo  : public BRepAlgoAPI_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_BuilderAlgo();
Standard_EXPORT virtual ~BRepAlgoAPI_BuilderAlgo();
  
  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_BuilderAlgo(const BOPAlgo_PaveFiller& thePF);
  
  //! Sets the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT void SetNonDestructive(const Standard_Boolean theFlag);

  //! Returns the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT Standard_Boolean NonDestructive() const;

  //! Sets the glue option for the algorithm
  Standard_EXPORT void SetGlue(const BOPAlgo_GlueEnum theGlue);
  
  //! Returns the glue option of the algorithm
  Standard_EXPORT BOPAlgo_GlueEnum Glue() const;

  //! Enables/Disables the check of the input solids for inverted status
  void SetCheckInverted(const Standard_Boolean theCheck)
  {
    myCheckInverted = theCheck;
  }

  //! Returns the flag defining whether the check for input solids on inverted status
  //! should be performed or not.
  Standard_Boolean CheckInverted() const
  {
    return myCheckInverted;
  }

  //! Sets the arguments
  Standard_EXPORT void SetArguments (const TopTools_ListOfShape& theLS);
  
  //! Gets the arguments
  Standard_EXPORT const TopTools_ListOfShape& Arguments() const;
  
  //! Performs the algorithm
  //!
  //! H  I  S  T  O  R  Y
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;
  
  //! Returns the list  of shapes modified from the shape <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& aS) Standard_OVERRIDE;
  
  //! Returns true if the shape S has been deleted. The
  //! result shape of the operation does not contain the shape S.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& aS) Standard_OVERRIDE;
  
  //! Returns the list  of shapes generated from the shape <S>.
  //! For use in BRepNaming.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Returns true if there is at least one modified shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasModified() const;
  
  //! Returns true if there is at least one generated shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasGenerated() const;
  
  //! Returns true if there is at least one deleted shape.
  //! For use in BRepNaming.
  //!
  //! protected methods
  Standard_EXPORT virtual Standard_Boolean HasDeleted() const;

  //! Returns the Intersection tool
  const BOPAlgo_PPaveFiller& DSFiller() const
  {
    return myDSFiller;
  }

  //! Returns the Building tool
  const BOPAlgo_PBuilder& Builder() const
  {
    return myBuilder;
  }

protected:

  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  Standard_Integer myEntryType;
  BOPAlgo_PPaveFiller myDSFiller;
  BOPAlgo_PBuilder myBuilder;
  Standard_Boolean myNonDestructive;
  TopTools_ListOfShape myArguments;
  BOPAlgo_GlueEnum myGlue;
  Standard_Boolean myCheckInverted;

private:

};

#endif // _BRepAlgoAPI_BuilderAlgo_HeaderFile
