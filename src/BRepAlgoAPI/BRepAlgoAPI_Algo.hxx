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

#ifndef _BRepAlgoAPI_Algo_HeaderFile
#define _BRepAlgoAPI_Algo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BOPAlgo_Options.hxx>
class Message_ProgressIndicator;
class TopoDS_Shape;


//! Provides the root interface for the API algorithms

class BRepAlgoAPI_Algo : public BRepBuilderAPI_MakeShape,
                         protected BOPAlgo_Options
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual const TopoDS_Shape& Shape() Standard_OVERRIDE;

  // Provide access to methods of protected base class BOPAlgo_Options
  // (inherited as protected to avoid problems with SWIG wrapper)
  using BOPAlgo_Options::Clear;
  using BOPAlgo_Options::SetRunParallel;
  using BOPAlgo_Options::RunParallel;
  using BOPAlgo_Options::SetFuzzyValue;
  using BOPAlgo_Options::FuzzyValue;
  using BOPAlgo_Options::HasErrors;
  using BOPAlgo_Options::HasWarnings;
  using BOPAlgo_Options::HasError;
  using BOPAlgo_Options::HasWarning;
  using BOPAlgo_Options::DumpErrors;
  using BOPAlgo_Options::DumpWarnings;
  using BOPAlgo_Options::ClearWarnings;
  using BOPAlgo_Options::GetReport;
  using BOPAlgo_Options::SetProgressIndicator;
  using BOPAlgo_Options::SetUseOBB;

protected:

  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_Algo();

  //! Destructor
  Standard_EXPORT virtual ~BRepAlgoAPI_Algo();

  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_Algo(const Handle(NCollection_BaseAllocator)& theAllocator);

private:

};

#endif // _BRepAlgoAPI_Algo_HeaderFile
