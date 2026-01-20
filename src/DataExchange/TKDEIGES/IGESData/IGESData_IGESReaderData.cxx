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

#include <IGESData_DirPart.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_ParamSet.hxx>
#include <Interface_ParamType.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_IGESReaderData, Interface_FileReaderData)

IGESData_IGESReaderData::IGESData_IGESReaderData(const int nbe, const int nbp)
    : Interface_FileReaderData(nbe, nbp),
      thectyp(0, 0),
      thedirs(0, nbe)
{
  thestep = IGESData_ReadDir;
  thedefw = 0.;
  theparh = new Interface_ParamSet(30);
  thestar = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thechk  = new Interface_Check;
}

void IGESData_IGESReaderData::AddStartLine(const char* const aval)
{
  thestar->Append(new TCollection_HAsciiString(aval));
}

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IGESData_IGESReaderData::
  StartSection() const
{
  return thestar;
}

void IGESData_IGESReaderData::AddGlobal(const Interface_ParamType atype, const char* const aval)
{
  theparh->Append(aval, (int)strlen(aval), atype, 0);
}

void IGESData_IGESReaderData::SetGlobalSection()
{
  thehead.Init(theparh, thechk);
}

const IGESData_GlobalSection& IGESData_IGESReaderData::GlobalSection() const
{
  return thehead;
}

void IGESData_IGESReaderData::SetDirPart(const int         num,
                                         const int         i1,
                                         const int         i2,
                                         const int         i3,
                                         const int         i4,
                                         const int         i5,
                                         const int         i6,
                                         const int         i7,
                                         const int         i8,
                                         const int         i9,
                                         const int         i10,
                                         const int         i11,
                                         const int         i12,
                                         const int         i13,
                                         const int         i14,
                                         const int         i15,
                                         const int         i16,
                                         const int         i17,
                                         const char* const res1,
                                         const char* const res2,
                                         const char* const label,
                                         const char* const subs)
{
  IGESData_DirPart& DP = thedirs(num);
  DP.Init(i1,
          i2,
          i3,
          i4,
          i5,
          i6,
          i7,
          i8,
          i9,
          i10,
          i11,
          i12,
          i13,
          i14,
          i15,
          i16,
          i17,
          res1,
          res2,
          label,
          subs);
  ////  thedirs(num) = DP;  // check if useful
  // InitParams(num); gka optimization memory
}

const IGESData_DirPart& IGESData_IGESReaderData::DirPart(const int num) const
{
  return thedirs(num);
}

void IGESData_IGESReaderData::DirValues(const int    num,
                                        int&         i1,
                                        int&         i2,
                                        int&         i3,
                                        int&         i4,
                                        int&         i5,
                                        int&         i6,
                                        int&         i7,
                                        int&         i8,
                                        int&         i9,
                                        int&         i10,
                                        int&         i11,
                                        int&         i12,
                                        int&         i13,
                                        int&         i14,
                                        int&         i15,
                                        int&         i16,
                                        int&         i17,
                                        const char*& res1,
                                        const char*& res2,
                                        const char*& label,
                                        const char*& subs) const
{
  thedirs(num).Values(i1,
                      i2,
                      i3,
                      i4,
                      i5,
                      i6,
                      i7,
                      i8,
                      i9,
                      i10,
                      i11,
                      i12,
                      i13,
                      i14,
                      i15,
                      i16,
                      i17,
                      res1,
                      res2,
                      label,
                      subs);
}

IGESData_IGESType IGESData_IGESReaderData::DirType(const int num) const
{
  return thedirs(num).Type();
}

int IGESData_IGESReaderData::NbEntities() const
{
  return thedirs.Upper();
}

int IGESData_IGESReaderData::FindNextRecord(const int num) const
{
  if (num >= thedirs.Upper())
    return 0;
  else
    return (num + 1);
}

// Reference to other entities: this is both very simple and problematic
// Very simple: a reference to an entity is a number (in directory list)
// which equals (2*N-1) if N is the true rank of the entity
// Problematic: this number is an Integer... nothing distinguishes it from another
// Hence criterion: any odd integer less than 2*NbRecords CAN be a
// reference... It's up to each entity to sort it out afterwards...
//  Attention, a reference can be given as "Negative Pointer"
// N.B.: DirPart not concerned (specific reading ensured by IGESEntity)

void IGESData_IGESReaderData::SetEntityNumbers()
{
  //   We try to rely solely on IGESRead calculation
  /*
    int nbd = thedirs.Upper();
    for (int i = 1; i <= nbd; i ++) {
      int nbp = NbParams(i);
      for (int j = 1; j <= nbp; j ++) {
        Interface_FileParameter& FP = ChangeParam(i,j);
        if (FP.ParamType() == Interface_ParamInteger) {
      int val = atoi(FP.CValue());
      if (val > 0) {
        if (val != ((val/2) *2) && val < 2*nbd) {  // candidat possible
          FP.SetEntityNumber((val+1)/2);
        }
      } else if (val < 0) {
        int mval = -val;
        if (mval != ((mval/2) *2) && mval < 2*nbd) {  // candidat possible
          FP.SetEntityNumber((mval+1)/2);
        }
      }

        }
      }
    }
  */
}

//=================================================================================================

occ::handle<Interface_Check> IGESData_IGESReaderData::GlobalCheck() const
{
  return thechk;
}

//=================================================================================================

void IGESData_IGESReaderData::SetDefaultLineWeight(const double defw)
{
  thedefw = defw;
}

//=================================================================================================

double IGESData_IGESReaderData::DefaultLineWeight() const
{
  return thedefw;
}
