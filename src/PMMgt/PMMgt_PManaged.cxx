// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
