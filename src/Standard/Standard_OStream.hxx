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

#ifndef _Standard_OStream_HeaderFile
#define _Standard_OStream_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_TypeDef.hxx>
#endif
#ifndef _Standard_Stream_HeaderFile
#include <Standard_Stream.hxx>
#endif

class Handle_Standard_Type;

__Standard_API const Handle_Standard_Type& Standard_OStream_Type_();

typedef ostream Standard_OStream;

__Standard_API void ShallowDump(const Standard_Boolean, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_CString, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_Character, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_ExtCharacter, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_ExtString, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_Integer, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_Real, Standard_OStream& );
__Standard_API void ShallowDump(const Standard_ShortReal, Standard_OStream& );
class Handle(Standard_Transient);
__Standard_API void ShallowDump(const Handle(Standard_Transient)&, Standard_OStream& );
class Handle(Standard_Persistent);
__Standard_API void ShallowDump(const Handle(Standard_Persistent)&, Standard_OStream& );

#endif


