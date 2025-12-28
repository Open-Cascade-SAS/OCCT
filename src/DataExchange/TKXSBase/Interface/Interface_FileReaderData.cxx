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

//====================================================================
// #10 smh 22.12.99 Protection (case of unexisting directory entry in file)
// sln 21.01.2002 OCC133: Exception handling was added in method
// Interface_FileReaderData::BoundEntity
//====================================================================

#include <Interface_FileParameter.hxx>
#include <Interface_FileReaderData.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_ParamSet.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_FileReaderData, Standard_Transient)

//  Stores Data from a File (Preserved in Literal form)
//  Each standard can use it as a base (literal parameter lists,
//  associated entities) and add its own data to it.
//  Works under the control of FileReaderTool
//  Optimization : Fields not possible, because Param is const. Too bad
//  So, we assume that we read one file at a time (reasonable assumption)
//  We note in field a file number, relative to which we optimize
static int thefic = 0;
static int thenm0 = -1;
static int thenp0 = -1;

Interface_FileReaderData::Interface_FileReaderData(const int nbr,
                                                   const int npar)
    : therrload(0),
      thenumpar(0, nbr),
      theents(0, nbr)
{
  theparams = new Interface_ParamSet(npar);
  thenumpar.Init(0);
  thenm0  = -1;
  thenum0 = ++thefic;
}

int Interface_FileReaderData::NbRecords() const
{
  return thenumpar.Upper();
}

int Interface_FileReaderData::NbEntities() const
{
  int nb  = 0;
  int num = 0;
  while ((num = FindNextRecord(num)) > 0)
    nb++;
  return nb;
}

//  ....            Management of Parameters attached to Records            ....

void Interface_FileReaderData::InitParams(const int num)
{
  thenumpar.SetValue(num, theparams->NbParams());
}

void Interface_FileReaderData::AddParam(const int /*num*/,
                                        const char*    aval,
                                        const Interface_ParamType atype,
                                        const int    nument)
{
  theparams->Append(aval, -1, atype, nument);
}

void Interface_FileReaderData::AddParam(const int /*num*/,
                                        const TCollection_AsciiString& aval,
                                        const Interface_ParamType      atype,
                                        const int         nument)
{
  theparams->Append(aval.ToCString(), aval.Length(), atype, nument);
}

void Interface_FileReaderData::AddParam(const int /*num*/,
                                        const Interface_FileParameter& FP)
{
  theparams->Append(FP);
}

void Interface_FileReaderData::SetParam(const int         num,
                                        const int         nump,
                                        const Interface_FileParameter& FP)
{
  theparams->SetParam(thenumpar(num - 1) + nump, FP);
}

int Interface_FileReaderData::NbParams(const int num) const
{
  if (num > 1)
    return (thenumpar(num) - thenumpar(num - 1));
  else if (num == 1)
    return thenumpar(num);
  else
    return theparams->NbParams();
}

occ::handle<Interface_ParamList> Interface_FileReaderData::Params(const int num) const
{
  if (num == 0)
    return theparams->Params(0, 0); // complet
  else if (num == 1)
    return theparams->Params(0, thenumpar(1));
  else
    return theparams->Params(thenumpar(num - 1) + 1, (thenumpar(num) - thenumpar(num - 1)));
}

const Interface_FileParameter& Interface_FileReaderData::Param(const int num,
                                                               const int nump) const
{
  if (thefic != thenum0)
    return theparams->Param(thenumpar(num - 1) + nump);
  if (thenm0 != num)
  {
    thenp0 = thenumpar(num - 1);
    thenm0 = num;
  }
  return theparams->Param(thenp0 + nump);
}

Interface_FileParameter& Interface_FileReaderData::ChangeParam(const int num,
                                                               const int nump)
{
  if (thefic != thenum0)
    return theparams->ChangeParam(thenumpar(num - 1) + nump);
  if (thenm0 != num)
  {
    thenp0 = thenumpar(num - 1);
    thenm0 = num;
  }
  return theparams->ChangeParam(thenp0 + nump);
}

Interface_ParamType Interface_FileReaderData::ParamType(const int num,
                                                        const int nump) const
{
  return Param(num, nump).ParamType();
}

const char* Interface_FileReaderData::ParamCValue(const int num,
                                                       const int nump) const
{
  return Param(num, nump).CValue();
}

bool Interface_FileReaderData::IsParamDefined(const int num,
                                                          const int nump) const
{
  return (Param(num, nump).ParamType() != Interface_ParamVoid);
}

int Interface_FileReaderData::ParamNumber(const int num,
                                                       const int nump) const
{
  return Param(num, nump).EntityNumber();
}

const occ::handle<Standard_Transient>& Interface_FileReaderData::ParamEntity(
  const int num,
  const int nump) const
{
  return BoundEntity(Param(num, nump).EntityNumber());
}

Interface_FileParameter& Interface_FileReaderData::ChangeParameter(const int numpar)
{
  return theparams->ChangeParam(numpar);
}

void Interface_FileReaderData::ParamPosition(const int numpar,
                                             int&      num,
                                             int&      nump) const
{
  int nbe = thenumpar.Upper();
  if (numpar <= 0)
  {
    num = nump = 0;
    return;
  }
  for (int i = 1; i <= nbe; i++)
  {
    if (thenumpar(i) > numpar)
    {
      num  = i;
      nump = numpar - thenumpar(i) + 1;
      return;
    }
  }
  num  = nbe;
  nump = numpar - thenumpar(nbe) + 1;
}

int Interface_FileReaderData::ParamFirstRank(const int num) const
{
  return thenumpar(num);
}

void Interface_FileReaderData::SetErrorLoad(const bool val)
{
  therrload = (val ? 1 : -1);
}

bool Interface_FileReaderData::IsErrorLoad() const
{
  return (therrload != 0);
}

bool Interface_FileReaderData::ResetErrorLoad()
{
  bool res = (therrload > 0);
  therrload            = 0;
  return res;
}

//  ....        Management of Entities Associated with File Data       ....

const occ::handle<Standard_Transient>& Interface_FileReaderData::BoundEntity(
  const int num) const
//      {  return theents(num);  }
{
  if (num >= theents.Lower() && num <= theents.Upper())
  {
    return theents(num);
  }
  else
  {
    static occ::handle<Standard_Transient> dummy;
    return dummy;
  }
}
/*  //static occ::handle<Standard_Transient> dummy;
  {
  //smh#10 Protection. If iges entity does not exist, return null pointer.
    try {
      OCC_CATCH_SIGNALS
      occ::handle<Standard_Transient> temp = theents.Value(num);
    }
  ////sln 21.01.2002 OCC133: Exception handling
 // catch (Standard_OutOfRange) {
 //   std::cout<<" Catch of sln"<<std::endl;

 //   return dummy;
 // }
    catch (Standard_Failure) {

    // some work-around, the best would be to modify CDL to
    // return "occ::handle<Standard_Transient>" not "const occ::handle<Standard_Transient>&"
      static occ::handle<Standard_Transient> dummy;
     // std::cout<<" Catch of smh"<<std::endl;
    return dummy;
    }
  }
   //std::cout<<" Normal"<<std::endl;
  if (theents.Value(num).IsImmutable()) std::cout << "IMMUTABLE:"<<num<<std::endl;
  return theents(num);
}
*/

void Interface_FileReaderData::BindEntity(const int            num,
                                          const occ::handle<Standard_Transient>& ent)
//      {  theents.SetValue(num,ent);  }
{
  //  #ifdef OCCT_DEBUG
  //    if (ent.IsImmutable())
  //      std::cout << "Bind IMMUTABLE:"<<num<<std::endl;
  //  #endif
  theents.SetValue(num, ent);
}

void Interface_FileReaderData::Destroy() {}

double Interface_FileReaderData::Fastof(const char* ligne)
{
  return Strtod(ligne, 0);
}
