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

#include <BOPAlgo_Options.hxx>

#include <BOPCol_DataMapOfIntegerListOfShape.hxx>

//! The class provides the root interface for the algorithms in Boolean Component.<br>
class BOPAlgo_Algo : public BOPAlgo_Options
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual void Perform() = 0;

protected:

  //! Default constructor
  Standard_EXPORT BOPAlgo_Algo();
  Standard_EXPORT virtual ~BOPAlgo_Algo();

  Standard_EXPORT BOPAlgo_Algo(const BOPCol_BaseAllocator& theAllocator);

  //! Checks input data
  Standard_EXPORT virtual void CheckData();

  //! Checks the obtained result
  Standard_EXPORT virtual void CheckResult();

};

#endif // _BOPAlgo_Algo_HeaderFile
