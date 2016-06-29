// Created on: 1997-09-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_TKI_HeaderFile
#define _TopOpeBRepDS_TKI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_HArray1OfDataMapOfIntegerListOfInterference.hxx>
#include <TopOpeBRepDS_DataMapIteratorOfDataMapOfIntegerListOfInterference.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepDS_Interference;



class TopOpeBRepDS_TKI 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_TKI();
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void FillOnGeometry (const TopOpeBRepDS_ListOfInterference& L);
  
  Standard_EXPORT void FillOnSupport (const TopOpeBRepDS_ListOfInterference& L);
  
  Standard_EXPORT Standard_Boolean IsBound (const TopOpeBRepDS_Kind K, const Standard_Integer G) const;
  
  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& Interferences (const TopOpeBRepDS_Kind K, const Standard_Integer G) const;
  
  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeInterferences (const TopOpeBRepDS_Kind K, const Standard_Integer G);
  
  Standard_EXPORT Standard_Boolean HasInterferences (const TopOpeBRepDS_Kind K, const Standard_Integer G) const;
  
  Standard_EXPORT void Add (const TopOpeBRepDS_Kind K, const Standard_Integer G);
  
  Standard_EXPORT void Add (const TopOpeBRepDS_Kind K, const Standard_Integer G, const Handle(TopOpeBRepDS_Interference)& HI);
  
  Standard_EXPORT void DumpTKIIterator (const TCollection_AsciiString& s1 = "", const TCollection_AsciiString& s2 = "");
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& Value (TopOpeBRepDS_Kind& K, Standard_Integer& G) const;
  
  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeValue (TopOpeBRepDS_Kind& K, Standard_Integer& G);




protected:





private:

  
  Standard_EXPORT void Reset();
  
  Standard_EXPORT Standard_Boolean MoreTI() const;
  
  Standard_EXPORT void NextTI();
  
  Standard_EXPORT Standard_Boolean MoreITM() const;
  
  Standard_EXPORT void FindITM();
  
  Standard_EXPORT void NextITM();
  
  Standard_EXPORT void Find();
  
  Standard_EXPORT Standard_Integer KindToTableIndex (const TopOpeBRepDS_Kind K) const;
  
  Standard_EXPORT TopOpeBRepDS_Kind TableIndexToKind (const Standard_Integer TI) const;
  
  Standard_EXPORT Standard_Boolean IsValidTI (const Standard_Integer TI) const;
  
  Standard_EXPORT Standard_Boolean IsValidK (const TopOpeBRepDS_Kind K) const;
  
  Standard_EXPORT Standard_Boolean IsValidG (const Standard_Integer G) const;
  
  Standard_EXPORT Standard_Boolean IsValidKG (const TopOpeBRepDS_Kind K, const Standard_Integer G) const;


  Standard_Integer mydelta;
  Handle(TopOpeBRepDS_HArray1OfDataMapOfIntegerListOfInterference) myT;
  Standard_Integer myTI;
  Standard_Integer myG;
  TopOpeBRepDS_DataMapIteratorOfDataMapOfIntegerListOfInterference myITM;
  TopOpeBRepDS_Kind myK;
  TopOpeBRepDS_ListOfInterference myEmptyLOI;
  TCollection_AsciiString myDummyAsciiString;


};







#endif // _TopOpeBRepDS_TKI_HeaderFile
