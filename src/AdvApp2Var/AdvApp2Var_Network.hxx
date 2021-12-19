// Created on: 1996-04-10
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _AdvApp2Var_Network_HeaderFile
#define _AdvApp2Var_Network_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AdvApp2Var_SequenceOfPatch.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Boolean.hxx>
class AdvApp2Var_Patch;



class AdvApp2Var_Network 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT AdvApp2Var_Network();
  
  Standard_EXPORT AdvApp2Var_Network(const AdvApp2Var_SequenceOfPatch& Net, const TColStd_SequenceOfReal& TheU, const TColStd_SequenceOfReal& TheV);
  
  //! search the Index of the first Patch not approximated,
  //! if all Patches are approximated Standard_False is returned
  Standard_EXPORT Standard_Boolean FirstNotApprox (Standard_Integer& Index) const;
  
  AdvApp2Var_Patch& ChangePatch (const Standard_Integer Index) { return *myNet.Value(Index); }
  AdvApp2Var_Patch& operator()  (const Standard_Integer Index) { return ChangePatch(Index); }
  
  Standard_EXPORT void UpdateInU (const Standard_Real CuttingValue);
  
  Standard_EXPORT void UpdateInV (const Standard_Real CuttingValue);
  
  Standard_EXPORT void SameDegree (const Standard_Integer iu, const Standard_Integer iv, Standard_Integer& ncfu, Standard_Integer& ncfv);
  
  Standard_EXPORT Standard_Integer NbPatch() const;
  
  Standard_EXPORT Standard_Integer NbPatchInU() const;
  
  Standard_EXPORT Standard_Integer NbPatchInV() const;
  
  Standard_EXPORT Standard_Real UParameter (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Real VParameter (const Standard_Integer Index) const;

  const AdvApp2Var_Patch& Patch (const Standard_Integer UIndex, const Standard_Integer VIndex) const
  {
    return *myNet.Value ((VIndex-1)*(myUParameters.Length()-1) + UIndex);
  }

  const AdvApp2Var_Patch& operator() (const Standard_Integer UIndex, const Standard_Integer VIndex) const
  {
    return Patch(UIndex,VIndex);
  }

private:

  AdvApp2Var_SequenceOfPatch myNet;
  TColStd_SequenceOfReal myUParameters;
  TColStd_SequenceOfReal myVParameters;

};

#endif // _AdvApp2Var_Network_HeaderFile
