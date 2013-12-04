// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PMMgt_PManaged.ixx>
#include <Standard_TypeMismatch.hxx>

#ifdef TRACE
#define  PRINT(X) cout X
#define  SHALLOWDUMP(X) X.ShallowDump(cout)
#else
#define  PRINT(X) 
#define  SHALLOWDUMP(X)
#endif

PMMgt_PManaged::PMMgt_PManaged()
{
}

//============================================================================
#ifndef CSFDB
class _Oid_PMMgt_PManaged : public Oid_PMMgt_PManaged {
    friend class Handle_PMMgt_PManaged;
};

void Handle_PMMgt_PManaged::Delete()
{
//  PMMgt_PManaged* aMng;
  _Oid_PMMgt_PManaged *aOid;
  aOid = (_Oid_PMMgt_PManaged *) operator->();

  Standard_TypeMismatch_Raise_if(!aOid->IsKind(STANDARD_TYPE(PMMgt_PManaged)),
				 "Handle(PMMgt_PManaged)::TypeMisPMatch");

//  aMng = (PMMgt_PManaged*) aOid->UpdateAccess();
  aOid->UpdateAccess();
  aOid->EndAccess();

#ifdef OBJY
  //==== Remove the "Oid" from MDTV Cache ===================================
  // void CacheRemove(const Standard_Persistent*);
  // CacheRemove(aMng);
#endif 

  Nullify();
//  delete aMng;
}
#endif
#ifdef OBJY
void PHandle_PMMgt_PManaged::Delete()
{
}

void PMMgt_PManaged::operator delete (void *, size_t)
{
}
#endif
