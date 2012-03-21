// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <TopOpeBRepDS_samdom.hxx>

#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepDS_define.hxx>

static TopTools_IndexedDataMapOfShapeListOfShape *Gps1 = NULL; 
static TopTools_IndexedDataMapOfShapeListOfShape *Gps2 = NULL; 
//modified by NIZNHY-PKV Sun Dec 15 17:57:12 2002 f
//static Handle(TopOpeBRepDS_HDataStructure)      Ghds;
static Handle(TopOpeBRepDS_HDataStructure)*      Ghds;
//modified by NIZNHY-PKV Sun Dec 15 17:57:18 2002 t

//modified by NIZNHY-PKV Sun Dec 15 17:41:43 2002 f
//=======================================================================
//function :FDSSDM_Close
//purpose  : 
//=======================================================================
Standard_EXPORT void FDSSDM_Close()
{
  if (Gps1) {
    delete Gps1;
    Gps1=NULL;
  }
  //
  if (Gps2) {
    delete Gps2;
    Gps2=NULL;
  }
}
//modified by NIZNHY-PKV Sun Dec 15 17:56:02 2002 t
//=======================================================================
//function :FDSSDM_prepare
//purpose  : 
//=======================================================================
Standard_EXPORT void FDSSDM_prepare(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  if (Gps1 == NULL) {
    Gps1 = (TopTools_IndexedDataMapOfShapeListOfShape*) new TopTools_IndexedDataMapOfShapeListOfShape();
  }
  if (Gps2 == NULL) {
    Gps2 = (TopTools_IndexedDataMapOfShapeListOfShape*) new TopTools_IndexedDataMapOfShapeListOfShape();
  }
  //modified by NIZNHY-PKV Sun Dec 15 17:58:28 2002 f
  //Ghds = HDS;
  void *anAddr=(void *) &HDS;
  Ghds = (Handle(TopOpeBRepDS_HDataStructure)*) anAddr;
  //modified by NIZNHY-PKV Sun Dec 15 17:58:31 2002 t
  Gps1->Clear();
  Gps2->Clear();
  Standard_Integer i,n = HDS->NbShapes();
  for (i=1; i<=n; i++) {
    const TopoDS_Shape& s = HDS->Shape(i);
    Standard_Boolean hsd = HDS->HasSameDomain(s); if (!hsd) continue;
    TopTools_ListOfShape thelist, thelist1;
    if (!Gps1->Contains(s)) Gps1->Add(s, thelist);
    if (!Gps2->Contains(s)) Gps2->Add(s, thelist1);
    TopTools_ListOfShape& LS1 = Gps1->ChangeFromKey(s);
    TopTools_ListOfShape& LS2 = Gps2->ChangeFromKey(s);
    FDSSDM_makes1s2(s,LS1,LS2);
  }
} //prepare
//=======================================================================
//function :FDSSDM_makes1s2
//purpose  : 
//=======================================================================
Standard_EXPORT void FDSSDM_makes1s2(const TopoDS_Shape& S,
				     TopTools_ListOfShape& L1,
				     TopTools_ListOfShape& L2) 
//L1 = S1, complete lists L1,L2 with the shapes of the DS having same domain
{
  //modified by NIZNHY-PKV Sun Dec 15 17:59:11 2002 f
  //const Handle(TopOpeBRepDS_HDataStructure)& HDS = Ghds;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = *Ghds;
  //modified by NIZNHY-PKV Sun Dec 15 17:59:15 2002 t
  L1.Append(S);

  Standard_Integer i; 
  Standard_Integer nl1 = L1.Extent(), nl2 = L2.Extent();
  
  while ( nl1 > 0 || nl2 > 0 )  {
    
    TopTools_ListIteratorOfListOfShape it1(L1);
    for (i=1 ; i<=nl1; i++) {
      const TopoDS_Shape& S1 = it1.Value();
//                HDS->Shape(S1);
      TopTools_ListIteratorOfListOfShape itsd(HDS->SameDomain(S1));
      for (; itsd.More(); itsd.Next() ) {
	const TopoDS_Shape& S2 = itsd.Value();
//                  HDS->Shape(S2);
	Standard_Boolean found = FDSSDM_contains(S2,L2);
	if ( ! found ) {
	  L2.Prepend(S2);
	  nl2++;
	}
      }
      it1.Next();
    }
    nl1 = 0;
    
    TopTools_ListIteratorOfListOfShape it2(L2);
    for (i=1 ; i<=nl2; i++) {
      const TopoDS_Shape& S2 = it2.Value();
//      HDS->Shape(S2);
      TopTools_ListIteratorOfListOfShape itsd(HDS->SameDomain(S2));
      for (; itsd.More(); itsd.Next() ) {
	const TopoDS_Shape& S1 = itsd.Value();
//                  HDS->Shape(S1);
	Standard_Boolean found = FDSSDM_contains(S1,L1);
	if ( ! found ) {
	  L1.Prepend(S1);
	  nl1++;
	}
      }
      it2.Next();
    }

    nl2 = 0;
  }
} // makes1s2
//=======================================================================
//function :FDSSDM_s1s2makesordor
//purpose  : 
//=======================================================================
Standard_EXPORT void FDSSDM_s1s2makesordor(const TopTools_ListOfShape& LS1,
					   const TopTools_ListOfShape& LS2,
					   TopTools_ListOfShape& LSO,
					   TopTools_ListOfShape& LDO)
{
  //modified by NIZNHY-PKV Sun Dec 15 17:59:37 2002 f
  //const Handle(TopOpeBRepDS_HDataStructure)& HDS = Ghds;  
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = *Ghds;  
  //modified by NIZNHY-PKV Sun Dec 15 17:59:43 2002 t
  TopTools_ListIteratorOfListOfShape it(LS1); if (!it.More()) return;
  const TopoDS_Shape& sref = it.Value();
#ifdef DEB
//  Standard_Integer  iref =
#endif
              HDS->SameDomainReference(sref);
  TopOpeBRepDS_Config oref = HDS->SameDomainOrientation(sref);
  
  for (it.Initialize(LS1); it.More(); it.Next() ) {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = HDS->SameDomainOrientation(s);
//  HDS->Shape(s);
    if      ( o == oref && !FDSSDM_contains(s,LSO) ) LSO.Append(s);
    else if ( o != oref && !FDSSDM_contains(s,LDO) ) LDO.Append(s);
  }
  
  for (it.Initialize(LS2); it.More(); it.Next() ) {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = HDS->SameDomainOrientation(s);
//             HDS->Shape(s);
    if      ( o == oref && !FDSSDM_contains(s,LSO) ) LSO.Append(s);
    else if ( o != oref && !FDSSDM_contains(s,LDO) ) LDO.Append(s);
  }
} // s1s2makesordor

Standard_EXPORT Standard_Boolean FDSSDM_hass1s2(const TopoDS_Shape& S)
{
  Standard_Boolean b1 = Gps1->Contains(S);
  Standard_Boolean b2 = Gps2->Contains(S);
  Standard_Boolean b = (b1 && b2);
  return b;
} // hass1s2

Standard_EXPORT void FDSSDM_s1s2(const TopoDS_Shape& S,TopTools_ListOfShape& LS1,TopTools_ListOfShape& LS2)
{
  LS1.Clear(); LS2.Clear();
  Standard_Boolean b = FDSSDM_hass1s2(S);
  if (!b) {
    FDSSDM_makes1s2(S,LS1,LS2);
    return;
  }
  const TopTools_ListOfShape& L1 = Gps1->FindFromKey(S);
  const TopTools_ListOfShape& L2 = Gps2->FindFromKey(S);
  FDSSDM_copylist(L1,LS1);
  FDSSDM_copylist(L2,LS2);
} // s1s2

Standard_EXPORT void FDSSDM_sordor(const TopoDS_Shape& S,TopTools_ListOfShape& LSO,TopTools_ListOfShape& LDO)
{
  LSO.Clear(); LDO.Clear();
  TopTools_ListOfShape LS1,LS2; 
  FDSSDM_s1s2(S,LS1,LS2);
  FDSSDM_s1s2makesordor(LS1,LS2,LSO,LDO);
} // sordor

Standard_EXPORT Standard_Boolean FDSSDM_contains(const TopoDS_Shape& S,const TopTools_ListOfShape& L)
// True if S IsSame a shape of list L.
{
  for (TopTools_ListIteratorOfListOfShape it(L); it.More(); it.Next() ) {
    const TopoDS_Shape& SL = it.Value();
    Standard_Boolean issame = SL.IsSame(S);
    if ( issame ) return Standard_True;
  }
  return Standard_False;
} // contains

Standard_EXPORT void FDSSDM_copylist(const TopTools_ListOfShape& Lin,const Standard_Integer I1,const Standard_Integer I2,TopTools_ListOfShape& Lou)
// copie des elements [i1..i2] de Lin dans Lou. 1er element de Lin = index 1 
{
  TopTools_ListIteratorOfListOfShape it(Lin);
  Standard_Integer nadd = 0;
  for ( Standard_Integer i = 1; it.More(); it.Next(),i++ ) {
    const TopoDS_Shape& EL = it.Value();
    if ( i >= I1 && i <= I2 ) {
      Lou.Append(EL);
      nadd++;
    }
  }
} // copylist

Standard_EXPORT void FDSSDM_copylist(const TopTools_ListOfShape& Lin,TopTools_ListOfShape& Lou)
// copy de Lin dans Lou
{
  const Standard_Integer I1 = 1;
  const Standard_Integer I2 = Lin.Extent();
  FDSSDM_copylist(Lin,I1,I2,Lou);
} // copylist
