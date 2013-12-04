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


