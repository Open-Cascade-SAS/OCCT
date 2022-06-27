// Created on: 1996-10-17
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_SurfaceExplorer_HeaderFile
#define _TopOpeBRepDS_SurfaceExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_Surface.hxx>
class TopOpeBRepDS_DataStructure;



class TopOpeBRepDS_SurfaceExplorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_SurfaceExplorer();
  
  Standard_EXPORT TopOpeBRepDS_SurfaceExplorer(const TopOpeBRepDS_DataStructure& DS, const Standard_Boolean FindOnlyKeep = Standard_True);
  
  Standard_EXPORT void Init (const TopOpeBRepDS_DataStructure& DS, const Standard_Boolean FindOnlyKeep = Standard_True);
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const TopOpeBRepDS_Surface& Surface() const;
  
  Standard_EXPORT Standard_Boolean IsSurface (const Standard_Integer I) const;
  
  Standard_EXPORT Standard_Boolean IsSurfaceKeep (const Standard_Integer I) const;
  
  Standard_EXPORT const TopOpeBRepDS_Surface& Surface (const Standard_Integer I) const;
  
  Standard_EXPORT Standard_Integer NbSurface();
  
  Standard_EXPORT Standard_Integer Index() const;




protected:





private:

  
  Standard_EXPORT void Find();


  Standard_Integer myIndex;
  Standard_Integer myMax;
  Standard_Address myDS;
  Standard_Boolean myFound;
  TopOpeBRepDS_Surface myEmpty;
  Standard_Boolean myFindKeep;


};







#endif // _TopOpeBRepDS_SurfaceExplorer_HeaderFile
