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

#ifndef _Standard_Persistent_proto_HeaderFile
#define _Standard_Persistent_proto_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_DefineAlloc_HeaderFile
#include <Standard_DefineAlloc.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

class Handle_Standard_Persistent;
class Standard_Type;
class Handle_Standard_Type;
class Standard_Type;

class Storage_stCONSTclCOM;
Standard_EXPORT const Handle_Standard_Type& Standard_Persistent_Type_();

class Standard_Persistent
{
friend class Handle(Standard_Persistent);
friend class Storage_Schema;

private:
  Standard_Integer count;
  Standard_Integer _typenum;
  Standard_Integer _refnum;
public:
  
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual Handle_Standard_Persistent This() const;
  Standard_EXPORT virtual Handle_Standard_Persistent ShallowCopy () const;
  Standard_EXPORT virtual void Delete() const;
  Standard_EXPORT virtual ~Standard_Persistent();
  
  Standard_EXPORT Standard_Persistent& operator= (const Standard_Persistent&);
                  Standard_Persistent() : count(0),_typenum(0),_refnum(0) {};
                  Standard_Persistent(const Standard_Persistent&) : count(0),_typenum(0),_refnum(0) {};
                  Standard_Persistent(const Storage_stCONSTclCOM&) : count(0),_typenum(0),_refnum(0) {}

  Standard_EXPORT virtual const Handle_Standard_Type& DynamicType() const;
  Standard_EXPORT         Standard_Boolean            IsKind(const Handle_Standard_Type&)const;
  Standard_EXPORT         Standard_Boolean            IsInstance(const Handle_Standard_Type&)const;  
  Standard_EXPORT virtual void                        ShallowDump(Standard_OStream& ) const;
};

#include <Handle_Standard_Persistent.hxx>

#endif








