// Created on: 2000-02-14
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>

#ifdef OCCT_DEBUG_DESC
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <BRepTools.hxx>
static void WriteS(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  char buf[256];
  if(strlen(filename) > 255) return;
#ifdef _MSC_VER
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
		      TopTools_IndexedMapOfShape&     MS,
		      const TDF_LabelMap&             Updated,
		      const TDF_LabelMap&             Forbiden)
{ 
  Standard_Boolean YaModif = Standard_False;
  for (; it.More(); it.Next()) {
    const TDF_Label&    Lab = it.Label();
#ifdef OCCT_DEBUG_DESC
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
	const TopoDS_Shape& aS   = it.Shape();
	MS.Add (aS);  // Modified
      }
      else
	LastModif(it2,it.Shape(),MS,Updated,Forbiden);
    } 
  }
  if (!YaModif) 
    MS.Add(S);    
}
//=======================================================================
static void ApplyOrientation (TopTools_IndexedMapOfShape& MS, 
			      const TopAbs_Orientation OrientationToApply)
{
#ifdef OCCT_DEBUG_APPLY
 if (!MS.IsEmpty ()) {
   cout <<"OrientationToApply = " <<OrientationToApply <<endl;
   for (Standard_Integer anItMS1 = 1; anItMS1 <= MS.Extent(); ++anItMS1) {
     cout << "ApplyOrientation: TShape = " << MS (anItMS1).TShape()->This() << " OR = " << MS (anItMS1).Orientation() <<endl;
   }
 }
#endif
  for (Standard_Integer anItMS = 1; anItMS <= MS.Extent(); ++anItMS)
  {
    MS.Substitute (anItMS, MS (anItMS).Oriented (OrientationToApply));
  }
}
//=======================================================================
//function : CurrentShape
//purpose  : ANaming
//=======================================================================
void TNaming_NamingTool::CurrentShape(const TDF_LabelMap&               Valid,
				      const TDF_LabelMap&               Forbiden,
				      const Handle(TNaming_NamedShape)& Att,
				      TopTools_IndexedMapOfShape& MS)
{
  TDF_Label Lab = Att->Label();
#ifdef OCCT_DEBUG_DESC
    TCollection_AsciiString entry;
    TDF_Tool::Entry(Lab, entry);
    cout << "NamingTool:: LabelEntry = "<< entry <<  endl;
#endif
  if (!Valid.IsEmpty() && !Valid.Contains(Lab)) {
#ifdef OCCT_DEBUG_DESC
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
#ifdef OCCT_DEBUG_DESC
    WriteS(S, "CS_NewShape.brep");
    if(itL.OldShape().IsNull())
      cout <<"OldShape is Null" <<endl;
    else 
        WriteS(itL.OldShape(), "CS_OldShape.brep");
#endif
    Standard_Boolean YaOrientationToApply(Standard_False);
    TopAbs_Orientation OrientationToApply(TopAbs_FORWARD);
    if(Att->Evolution() == TNaming_SELECTED) {
      if (itL.More() && itL.NewShape().ShapeType() != TopAbs_VERTEX) {//OR-N
		Handle (TNaming_Naming)  aNaming;
		Lab.FindAttribute(TNaming_Naming::GetID(), aNaming);
		if(!aNaming.IsNull()) {
		  if(aNaming->GetName().Type() == TNaming_ORIENTATION) {
			 OrientationToApply = aNaming->GetName().Orientation();
		  } else {
	        Handle (TNaming_Naming)  aNaming2;
			TDF_ChildIterator it(aNaming->Label());
			for(;it.More();it.Next()) {
			  const TDF_Label& aLabel = it.Value();
			  aLabel.FindAttribute(TNaming_Naming::GetID(), aNaming2);
	          if(!aNaming2.IsNull()) {
			    if(aNaming2->GetName().Type() == TNaming_ORIENTATION) {
				  OrientationToApply = aNaming2->GetName().Orientation();
				  break;
				}
			  }
			}
		  }
		  if(OrientationToApply == TopAbs_FORWARD || OrientationToApply == TopAbs_REVERSED)
			YaOrientationToApply = Standard_True;	    		
		}
	  } //
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
      TopTools_IndexedMapOfShape MS2; 
      LastModif(it, S, MS2, Valid, Forbiden);
      if (YaOrientationToApply) ApplyOrientation (MS2, OrientationToApply);//the solution to be refined
      for (Standard_Integer anItMS2 = 1; anItMS2 <= MS2.Extent(); ++anItMS2)
        MS.Add (MS2 (anItMS2));
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
					       TopTools_IndexedMapOfShape&       MS)
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
#ifdef OCCT_DEBUG_DESC
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
#ifdef OCCT_DEBUG_DESC
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
#ifdef OCCT_DEBUG_DESC
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
#ifdef OCCT_DEBUG_DESC
	TCollection_AsciiString entry;
	TDF_Tool::Entry(ONS->Label(), entry);
	cout<< "MakeDescendants_Old: Label = " <<entry <<endl;	  
#endif
	BuildDescendants2(ONS, NS->Label(), Descendants);
      }
    }  
  }
}
