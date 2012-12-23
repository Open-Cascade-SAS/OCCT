// Created on: 1997-04-22
// Created by: Prestataire Mary FABIEN
// Copyright (c) 1997-1999 Matra Datavision
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



#include <TopOpeBRepDS_Filter.ixx>

#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>

#ifdef DEB
extern Standard_Boolean TopOpeBRepDS_GettracePI();
extern Standard_Boolean TopOpeBRepDS_GettracePCI();
static Standard_Boolean TRCC() {
  Standard_Boolean b2 = TopOpeBRepDS_GettracePI();
  Standard_Boolean b3 = TopOpeBRepDS_GettracePCI();
  return (b2 || b3);
}
#endif

//=======================================================================
//function : ProcessCurveInterferences
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Filter::ProcessCurveInterferences
(const Standard_Integer CIX)
{
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();
  TopOpeBRepDS_ListOfInterference& LI = BDS.ChangeCurveInterferences(CIX);
  TopOpeBRepDS_ListIteratorOfListOfInterference it1(LI);
  
  // process interferences of LI with VERTEX geometry
  while( it1.More() ) {
    const Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    Standard_Integer G1 = I1->Geometry();
    TopOpeBRepDS_Kind GT1 = I1->GeometryType();
    TopAbs_Orientation O1 = I1->Transition().Orientation(TopAbs_IN);
    
    if ( GT1 == TopOpeBRepDS_VERTEX ) {
      
      TopOpeBRepDS_ListIteratorOfListOfInterference it2(it1);
      it2.Next();
      
      while ( it2.More() ) {
	const Handle(TopOpeBRepDS_Interference)& I2 = it2.Value();
	Standard_Integer G2 = I2->Geometry();
	TopOpeBRepDS_Kind GT2 = I2->GeometryType();
	TopAbs_Orientation O2 = I2->Transition().Orientation(TopAbs_IN);
	
//	Standard_Boolean remove = (GT2 == GT1) && (G2 == G1);
	// xpu140898 : USA60111 : CPI(FORWARD,v10,C1) + CPIREV(REVERSED,v10,C1)
	//             do NOT delete CPIREV!!
	Standard_Boolean remove = (GT2 == GT1) && (G2 == G1) && (O1 == O2);
	if ( remove ) {
#ifdef DEB
	  if ( TRCC() ){
	    cout<<"remove ";I2->Dump(cout);cout<<" from C "<<CIX<<endl;
	  }
#endif
	  LI.Remove(it2);
	}
	else it2.Next();
      }
    }
    it1.Next();
  }
}
