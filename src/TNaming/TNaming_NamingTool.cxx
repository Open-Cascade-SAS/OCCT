// Created on: 2000-02-14
// Created by: Denis PASCAL
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <TNaming_NamingTool.ixx>
#include <TNaming_Tool.hxx>
#include <TNaming_NewShapeIterator.hxx> 
#include <TNaming_Iterator.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_Tool.hxx>
#ifdef DEB
//#define MDTV_DEB_DESC
//#define MDTV_DEB_APPLY
#ifdef MDTV_DEB_DESC
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <BRepTools.hxx>
static void WriteS(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  char buf[256];
  if(strlen(filename) > 255) return;
#ifdef WNT
  strcpy_s (buf, filename);
#else
  strcpy (buf, filename);
#endif
  char* p = buf;
  while (*p) {
    if(*p == ':')
      *p = '-';
    p++;
  }
  ofstream save (buf);
  if(!save) 
    cout << "File " << buf << " was not created: rdstate = " << save.rdstate() << endl;
  save << "DBRep_DrawableShape" << endl << endl;
  if(!shape.IsNull()) BRepTools::Write(shape, save);
  save.close();
}
#endif
#endif
//=======================================================================
//function : IsForbiden
//purpose  : ANaming voir NamingTool
//=======================================================================

static Standard_Boolean IsForbiden(const TDF_LabelMap& Forbiden,
				   const TDF_Label&    Lab)
{
  if (Lab.IsRoot()) {
    return Standard_False;
  }
  if (Forbiden.Contains(Lab)) 
    return Standard_True;
  else {
    return IsForbiden(Forbiden,Lab.Father());
  }
}

//=======================================================================
//function : LastModif 
//purpose  : ANaming 
//=======================================================================
static void LastModif(      TNaming_NewShapeIterator& it,
		      const TopoDS_Shape&             S,
		            TopTools_MapOfShape&      MS,
		      const TDF_LabelMap&             Updated,
		      const TDF_LabelMap&             Forbiden)
{ 
  Standard_Boolean YaModif = Standard_False;
  for (; it.More(); it.Next()) {
    const TDF_Label&    Lab = it.Label();
#ifdef MDTV_DEB
    TCollection_AsciiString entry;
    TDF_Tool::Entry(Lab, entry);
    cout << "NamingTool:: LastModif LabelEntry = "<< entry <<  endl;
#endif
    if (!Updated.IsEmpty() && !Updated.Contains(Lab))  continue;
    if (IsForbiden(Forbiden, Lab))                     continue; 
    if (it.IsModification()) {
      YaModif = Standard_True;
      TNaming_NewShapeIterator it2(it);
      if (!it2.More()) {
	const TopoDS_Shape& S   = it.Shape();
	MS.Add (S);  // Modified
      }
      else
	LastModif(it2,it.Shape(),MS,Updated,Forbiden);
    } 
  }
  if (!YaModif) 
    MS.Add(S);    
}
//=======================================================================
static void ApplyOrientation (TopTools_MapOfShape& MS, 
			      const TopAbs_Orientation OrientationToApply)
{
 if (!MS.IsEmpty ()) {
#ifdef MDTV_DEB_APPLY
   cout <<"OrientationToApply = " <<OrientationToApply <<endl;
   TopTools_MapIteratorOfMapOfShape it1(MS);
   for (; it1.More(); it1.Next()) {
     cout << "ApplyOrientation: TShape = " << it1.Key().TShape()->This() << " OR = " <<it1.Key().Orientation() <<endl;
   }
#endif
   TopTools_MapOfShape aMS;
   aMS.Assign(MS);
   TopTools_MapIteratorOfMapOfShape it(aMS);
   for (; it.More(); it.Next()) {
     if(it.Key().Orientation() != OrientationToApply) {
       TopoDS_Shape aS = it.Key();
       MS.Remove(aS);
       aS.Orientation(OrientationToApply);
       MS.Add(aS);
     }
   }
 }
}
//=======================================================================
//function : CurrentShape
//purpose  : ANaming
//=======================================================================
void TNaming_NamingTool::CurrentShape(const TDF_LabelMap&               Valid,
				      const TDF_LabelMap&               Forbiden,
				      const Handle(TNaming_NamedShape)& Att,
				      TopTools_MapOfShape&              MS)
{
  TDF_Label Lab = Att->Label();
#ifdef MDTV_DEB
    TCollection_AsciiString entry;
    TDF_Tool::Entry(Lab, entry);
    cout << "NamingTool:: LabelEntry = "<< entry <<  endl;
#endif
  if (!Valid.IsEmpty() && !Valid.Contains(Lab)) {
#ifdef MDTV_DEB
    TCollection_AsciiString entry;
    TDF_Tool::Entry(Lab, entry);
    cout << "NamingTool:: LabelEntry = "<< entry << " is out of Valid map" <<  endl;
#endif
    return;
  }

  TNaming_Iterator itL (Att);
  for (; itL.More(); itL.Next()) {
    const TopoDS_Shape& S = itL.NewShape();
    if (S.IsNull()) continue;
#ifdef MDTV_DEB
    WriteS(S, "CS_NewShape.brep");
    if(itL.OldShape().IsNull())
      cout <<"OldShape is Null" <<endl;
    else 
        WriteS(itL.OldShape(), "CS_OldShape.brep");
#endif
    Standard_Boolean YaOrientationToApply(Standard_False);
    TopAbs_Orientation OrientationToApply(TopAbs_FORWARD);
    if(Att->Evolution() == TNaming_SELECTED) {
      if (itL.More() && itL.NewShape().ShapeType() != TopAbs_VERTEX &&
	  !itL.OldShape().IsNull() && itL.OldShape().ShapeType() == TopAbs_VERTEX) {
	YaOrientationToApply = Standard_True;
	OrientationToApply = itL.OldShape().Orientation();
      }
    }
    TNaming_NewShapeIterator it(itL);
    if (!it.More()) {
      if (YaOrientationToApply)
	MS.Add(S.Oriented(OrientationToApply));
      else
	MS.Add(S);
    }
    else {      
//     LastModif(it, S, MS, Valid, Forbiden);
      TopTools_MapOfShape MS2; 
      LastModif(it, S, MS2, Valid, Forbiden);
      if (YaOrientationToApply) ApplyOrientation (MS2, OrientationToApply);//the solution to be refined
      for (TopTools_MapIteratorOfMapOfShape itMS2(MS2); itMS2.More();itMS2.Next()) 
	MS.Add(itMS2.Key());
    }
  }
}

//=======================================================================
//function : CurrentShapeFromShape
//purpose  : ANaming
//=======================================================================
             
void TNaming_NamingTool::CurrentShapeFromShape(const TDF_LabelMap&               Valid,
					       const TDF_LabelMap&               Forbiden,
					       const TDF_Label&                  Acces,
					       const TopoDS_Shape&               S,
					       TopTools_MapOfShape&              MS)
{
  TNaming_NewShapeIterator it(S,Acces);

  Handle(TNaming_NamedShape) NS = it.NamedShape(); 
  if(!NS.IsNull() && NS->Evolution() == TNaming_SELECTED)
    MS.Add(TNaming_Tool::GetShape(NS));
  else {
    if (!it.More()) 
      MS.Add(S);
    else 
      LastModif(it, S, MS, Valid, Forbiden);
  }
}

//=======================================================================
//function : MakeDescendants
//purpose  : ANaming
//=======================================================================

static void MakeDescendants (TNaming_NewShapeIterator&         it,
			     TDF_LabelMap&                     Descendants)
{
  for (; it.More(); it.Next()) {
    Descendants.Add(it.Label());
#ifdef MDTV_DEB_DESC
    TCollection_AsciiString entry;
    TDF_Tool::Entry(it.Label(), entry);
    cout<< "MakeDescendants: Label = " <<entry <<endl;
#endif
    if (!it.Shape().IsNull()) {
      TNaming_NewShapeIterator it2(it);
      MakeDescendants (it2,Descendants);
    }
  }
}
//=======================================================================
void BuildDescendants2 (const Handle(TNaming_NamedShape)& NS, const TDF_Label& ForbLab, TDF_LabelMap& Descendants)
{
  if (NS.IsNull()) return;
  TNaming_NewShapeIterator it(NS); 
  for(;it.More();it.Next()) {
    if(!it.NamedShape().IsNull()) {
#ifdef MDTV_DEB_DESC
      TCollection_AsciiString entry;
      TDF_Tool::Entry(it.Label(), entry);
      cout<< "MakeDescendants2: Label = " <<entry <<endl;
#endif      
      if(ForbLab == it.Label()) continue;
      Descendants.Add(it.Label());
      TNaming_NewShapeIterator it2(it); 
      MakeDescendants (it2,Descendants); 
    }
  }
}
//=======================================================================
//function : BuildDescendants
//purpose  : ANaming
//=======================================================================

void TNaming_NamingTool::BuildDescendants (const Handle(TNaming_NamedShape)& NS,
					   TDF_LabelMap&                     Descendants)
{
  if (NS.IsNull()) return;
  Descendants.Add(NS->Label());
  TNaming_NewShapeIterator it(NS); 
#ifdef MDTV_DEB_DESC
    TCollection_AsciiString entry;
    TDF_Tool::Entry(NS->Label(), entry);
    cout<< "MakeDescendants: Label = " <<entry <<endl;
#endif
  MakeDescendants (it,Descendants);
  TNaming_OldShapeIterator it2(NS); 
  for (; it2.More(); it2.Next()) {
    if(!it2.Shape().IsNull()) {
      Handle(TNaming_NamedShape) ONS = TNaming_Tool::NamedShape(it2.Shape(), NS->Label());
      if(!ONS.IsNull()) {
#ifdef MDTV_DEB_DESC
	TCollection_AsciiString entry;
	TDF_Tool::Entry(ONS->Label(), entry);
	cout<< "MakeDescendants_Old: Label = " <<entry <<endl;	  
#endif
	BuildDescendants2(ONS, NS->Label(), Descendants);
      }
    }  
  }
}
