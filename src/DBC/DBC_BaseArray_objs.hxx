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

#ifndef _DBC_BaseArray_HeaderFile
#define _DBC_BaseArray_HeaderFile

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _DBC_DBVArray_HeaderFile
#include <DBC_DBVArray.hxx>
#endif
#ifndef _Standard_Storable_HeaderFile
#include <Standard_Storable.hxx>
#endif
#ifndef _Standard_Address_HeaderFile
#include <Standard_Address.hxx>
#endif
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif
class Standard_NullObject;
class Standard_NegativeValue;
class Standard_DimensionMismatch;


#ifndef Standard_EXPORT
#if defined(WNT) && !defined(HAVE_NO_DLL)
#define Standard_EXPORT __declspec(dllexport)
#else
#define Standard_EXPORT
#endif
#endif

Handle_Standard_Type& DBC_BaseArray_Type_();

class Standard_EXPORT DBC_BaseArray :
  public os_virtual_behavior /* New */ {

public:

 // Methods PUBLIC
 // 
DBC_BaseArray();
DBC_BaseArray(const Standard_Integer Size);
DBC_BaseArray(const DBC_BaseArray& BaseArray);
  Standard_Integer Length() const;
  Standard_Integer Upper() const;
  Standard_Address Lock() const;
  void Unlock() const;
virtual  void ShallowDump(Standard_OStream& S) const;
  void Destroy() ;
~DBC_BaseArray()
{
  Destroy();
}





 // Type management
 //
 friend Handle_Standard_Type& DBC_BaseArray_Type_();

protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //
Standard_Integer mySize;
DBC_DBVArray myData;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};


#include <DBC_BaseArray.lxx>



// other Inline functions and methods (like "C++: function call" methods)
//
void ShallowDump(const DBC_BaseArray& me,Standard_OStream& S) {
 me.ShallowDump(S);
}



#endif
