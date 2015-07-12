// Created by: Peter Kurnev
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

#ifndef _BOPAlgo_CheckerSI_HeaderFile
#define _BOPAlgo_CheckerSI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPAlgo_PaveFiller.hxx>


//! Checks shape on self-interference.
class BOPAlgo_CheckerSI  : public BOPAlgo_PaveFiller
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPAlgo_CheckerSI();
Standard_EXPORT virtual ~BOPAlgo_CheckerSI();
  
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;
  
  //! Sets the level of checking shape on self-interference.
  //! It defines which interferferences will be checked:
  //! 0 - only V/V;
  //! 1 - V/V and V/E;
  //! 2 - V/V, V/E and E/E;
  //! 3 - V/V, V/E, E/E and V/F;
  //! 4 - V/V, V/E, E/E, V/F and E/F;
  //! 5 - all interferences, default value.
  Standard_EXPORT void SetLevelOfCheck (const Standard_Integer theLevel);
  
  //! Sets the flag <theFlag> that defines
  //! the mode of the treatment:
  //! the copy of the argument when theFlag is true
  //! the argument itself  when theFlag is false
  Standard_EXPORT void SetNonDestructive (const Standard_Boolean theFlag);
  
  //! Returns the flag that defines the
  //! mode of the treatment:
  //! true when the copy of the argument is used
  //! false when the argument itself  is used
  Standard_EXPORT Standard_Boolean NonDestructive() const;




protected:

  
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;
  
  //! Provides post-treatment actions
  Standard_EXPORT void PostTreat();
  
  Standard_EXPORT virtual void PrepareCopy();
  
  //! Provides post-treatment actions for the copy
  Standard_EXPORT void PostTreatCopy();


  Standard_Integer myLevelOfCheck;
  Standard_Boolean myNonDestructive;
  BOPCol_DataMapOfShapeShape myNewOldMap;


private:





};







#endif // _BOPAlgo_CheckerSI_HeaderFile
