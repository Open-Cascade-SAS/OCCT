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

#include <AdvApp2Var_Patch.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Boolean.hxx>
class AdvApp2Var_Patch;

class AdvApp2Var_Network
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT AdvApp2Var_Network();

  Standard_EXPORT AdvApp2Var_Network(const NCollection_Sequence<occ::handle<AdvApp2Var_Patch>>& Net,
                                     const NCollection_Sequence<double>& TheU,
                                     const NCollection_Sequence<double>& TheV);

  //! search the Index of the first Patch not approximated,
  //! if all Patches are approximated false is returned
  Standard_EXPORT bool FirstNotApprox(int& Index) const;

  AdvApp2Var_Patch& ChangePatch(const int Index) { return *myNet.Value(Index); }

  AdvApp2Var_Patch& operator()(const int Index) { return ChangePatch(Index); }

  Standard_EXPORT void UpdateInU(const double CuttingValue);

  Standard_EXPORT void UpdateInV(const double CuttingValue);

  Standard_EXPORT void SameDegree(const int iu, const int iv, int& ncfu, int& ncfv);

  Standard_EXPORT int NbPatch() const;

  Standard_EXPORT int NbPatchInU() const;

  Standard_EXPORT int NbPatchInV() const;

  Standard_EXPORT double UParameter(const int Index) const;

  Standard_EXPORT double VParameter(const int Index) const;

  const AdvApp2Var_Patch& Patch(const int UIndex, const int VIndex) const
  {
    return *myNet.Value((VIndex - 1) * (myUParameters.Length() - 1) + UIndex);
  }

  const AdvApp2Var_Patch& operator()(const int UIndex, const int VIndex) const
  {
    return Patch(UIndex, VIndex);
  }

private:
  NCollection_Sequence<occ::handle<AdvApp2Var_Patch>> myNet;
  NCollection_Sequence<double>                        myUParameters;
  NCollection_Sequence<double>                        myVParameters;
};

#endif // _AdvApp2Var_Network_HeaderFile
