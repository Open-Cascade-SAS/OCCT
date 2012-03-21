// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopAbs.hxx>
#include <TopOpeBRepDS_Interference.ixx>


//=======================================================================
//function : TopOpeBRepDS_Interference
//purpose  : 
//=======================================================================

TopOpeBRepDS_Interference::TopOpeBRepDS_Interference()
{
}

//=======================================================================
//function : TopOpeBRepDS_Interference
//purpose  : 
//=======================================================================

TopOpeBRepDS_Interference::TopOpeBRepDS_Interference
    (const TopOpeBRepDS_Transition& T, 
     const TopOpeBRepDS_Kind ST,
     const Standard_Integer S, 
     const TopOpeBRepDS_Kind GT, 
     const Standard_Integer G) :
     myTransition(T),
     mySupport(S),
     myGeometry(G),
     mySupportType(ST),
     myGeometryType(GT)
{
}

//=======================================================================
//function : TopOpeBRepDS_Interference
//purpose  : 
//=======================================================================

TopOpeBRepDS_Interference::TopOpeBRepDS_Interference
    (const Handle(TopOpeBRepDS_Interference)& I ) :
    myTransition(I->Transition()),
    mySupport(I->Support()),
    myGeometry(I->Geometry()),
    mySupportType(I->SupportType()),
    myGeometryType(I->GeometryType())
{
}


//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Transition& TopOpeBRepDS_Interference::Transition() const 
{
  return myTransition;
}


//=======================================================================
//function : ChangeTransition
//purpose  : 
//=======================================================================

TopOpeBRepDS_Transition&  TopOpeBRepDS_Interference::ChangeTransition()
{
  return myTransition;
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::Transition(const TopOpeBRepDS_Transition& T)
{
  myTransition = T;
}

//=======================================================================
//function : SupportType
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Interference::GKGSKS(TopOpeBRepDS_Kind& GK,
				       Standard_Integer& G,
				       TopOpeBRepDS_Kind& SK,
				       Standard_Integer& S) const
{
  GK = myGeometryType;
  G = myGeometry;
  SK = mySupportType;
  S = mySupport;
}

//=======================================================================
//function : SupportType
//purpose  : 
//=======================================================================

TopOpeBRepDS_Kind  TopOpeBRepDS_Interference::SupportType()const 
{
  return mySupportType;
}


//=======================================================================
//function : Support
//purpose  : 
//=======================================================================

Standard_Integer  TopOpeBRepDS_Interference::Support()const 
{
  return mySupport;
}


//=======================================================================
//function : GeometryType
//purpose  : 
//=======================================================================

TopOpeBRepDS_Kind  TopOpeBRepDS_Interference::GeometryType()const 
{
  return myGeometryType;
}


//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

Standard_Integer  TopOpeBRepDS_Interference::Geometry()const 
{
  return myGeometry;
}

//=======================================================================
//function : SetGeometry
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Interference::SetGeometry(const Standard_Integer GI)
{
  myGeometry = GI;
}


//=======================================================================
//function : SupportType
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::SupportType(const TopOpeBRepDS_Kind ST)
{
  mySupportType = ST;
}


//=======================================================================
//function : Support
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::Support(const Standard_Integer S)
{
  mySupport = S;
}


//=======================================================================
//function : GeometryType
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::GeometryType(const TopOpeBRepDS_Kind GT)
{
  myGeometryType = GT;
}


//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::Geometry(const Standard_Integer G)
{
  myGeometry = G;
}

//=======================================================================
//function : HasSameSupport 
//purpose  : 
//=======================================================================

Standard_Boolean  TopOpeBRepDS_Interference::HasSameSupport(const Handle(TopOpeBRepDS_Interference)& I)
const
{
  return (mySupportType == I->mySupportType && mySupport == I->mySupport);
}

//=======================================================================
//function : HasSameGeometry
//purpose  : 
//=======================================================================

Standard_Boolean  TopOpeBRepDS_Interference::HasSameGeometry(const Handle(TopOpeBRepDS_Interference)& I)
const
{
  return (myGeometryType == I->myGeometryType && myGeometry == I->myGeometry);
}


//=======================================================================
//function : DumpS
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Interference::DumpS(Standard_OStream& OS) const
{
#ifdef DEB
  TopOpeBRepDS::Print(mySupportType,mySupport,OS,"S=","");
  OS.flush();
#endif
  return OS;
}


//=======================================================================
//function : DumpG
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Interference::DumpG(Standard_OStream& OS) const
{
#ifdef DEB
  TopOpeBRepDS::Print(myGeometryType,myGeometry,OS,"G=","");
#endif
  return OS;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Interference::Dump
(Standard_OStream& OS) const
{
#ifdef DEB
  myTransition.Dump(OS); OS<<" "; DumpG(OS); OS<<" on "; DumpS(OS);
#endif
  return OS;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Interference::Dump
(Standard_OStream& OS,
 const TCollection_AsciiString& s1,
 const TCollection_AsciiString& s2) const
{
#ifdef DEB
  OS<<s1;
  Dump(OS);
  OS<<s2;
  OS.flush();
#endif
  return OS;
}
