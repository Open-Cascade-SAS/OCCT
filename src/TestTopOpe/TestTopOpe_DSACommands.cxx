// Created on: 1997-12-01
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



#include <TestTopOpe.ixx>
#include <BRepAlgo_DSAccess.hxx>
#include <DBRep.hxx>
#include <TopOpeBRepDS_define.hxx>

Standard_Integer DSACCESSCOMMANDS(Draw_Interpretor& interpretor,Standard_Integer na, const char** a)
{ 
  static BRepAlgo_DSAccess DSA;
//  Standard_Integer i;

  if (!strcmp(a[0],"dsload"))
    if (na == 3) {
      TopoDS_Shape SS1 = DBRep::Get(a[1]);
      TopoDS_Shape& S1 = SS1;
      TopoDS_Shape SS2 = DBRep::Get(a[2]);
      TopoDS_Shape& S2 = SS2;
      TestTopOpe::CurrentDS(DSA.DS());
      DSA.Load(S1, S2);
      return 0;
    }
    else 
      return 0;

  else if(!strcmp(a[0],"dsinit")) {
    DSA.Init();
  }
  else if(!strcmp(a[0],"dsinters"))
    if (na == 4)
      if(!strcmp(a[1],"-sd")) {
	const TopoDS_Shape& S1 = DBRep::Get(a[2]);
	const TopoDS_Shape& S2 = DBRep::Get(a[3]);
	DSA.SameDomain(S1, S2);
      }
      else 
	return 0;
    else {
      if(na == 3) {
	const TopoDS_Shape& S1 = DBRep::Get(a[1]);
	const TopoDS_Shape& S2 = DBRep::Get(a[2]);
	DSA.Intersect(S1, S2);
      }
      else
	if(na == 1) 
	  DSA.Intersect();
    }
  else if(!strcmp(a[0],"dsgetsect")) {
    if((na == 2) || (na == 4)) {
      TopoDS_Shape EmptyShape;
      const TopoDS_Shape& S1 = ((na == 2) ? EmptyShape : DBRep::Get(a[2]));
      const TopoDS_Shape& S2 = ((na == 2) ? EmptyShape : DBRep::Get(a[3]));
      const TopTools_ListOfShape& list = 
	((na == 2) ? DSA.GetSectionEdgeSet() : 
	 DSA.GetSectionEdgeSet(S1, S2));
      Standard_Integer index = 1;
      if(list.Extent() > 1) {
	TopTools_ListIteratorOfListOfShape it(list);
	for(; it.More(); it.Next(), index++) {
	  TCollection_AsciiString enam(a[1]);
	  enam=enam + "_" + TCollection_AsciiString(index);
	  DBRep::Set(enam.ToCString(),it.Value());
	}
      }
      else
	if(list.Extent() == 1) {
	  TCollection_AsciiString enam(a[1]);
	  DBRep::Set(enam.ToCString(),list.First());
	}	
	else
	  return 0;
    }
  }
  
  else if(!strcmp(a[0],"dssuppsect")) {
    if(na == 2) {
      const TopoDS_Shape& S = DBRep::Get(a[1]);
      DSA.SuppressEdgeSet(S);
    }
    else
      return 0;
  }
  
  else if(!strcmp(a[0],"dsperform")) {
    if(na == 3) {
      TopoDS_Shape S;
      TCollection_AsciiString enam(a[1]);
      if(!strcmp(enam.ToCString(),"fus")) 
	S = DSA.Merge(TopAbs_OUT, TopAbs_OUT);
      else if(!strcmp(enam.ToCString(),"c12")) 
	S = DSA.Merge(TopAbs_OUT, TopAbs_IN);
      else if(!strcmp(enam.ToCString(),"c21")) 
	S = DSA.Merge(TopAbs_IN, TopAbs_OUT);
      else if(!strcmp(enam.ToCString(),"com")) 
	S = DSA.Merge(TopAbs_IN, TopAbs_IN);
      DBRep::Set(a[2], S);
      TopOpeBRepDS_DataStructure& DS = DSA.DS()->ChangeDS();
      Standard_Integer i,j,nbsh = DS.NbShapes();
      for(i = 1;i<=nbsh;i++) {
	TopTools_ListOfShape& los = DS.ChangeShapeSameDomain(i);
	j = los.Extent();

	TopTools_ListIteratorOfListOfShape li(los);
	for(; li.More(); li.Next()) {
#ifdef DEB
	  const TopoDS_Shape& Shap =
#endif
                                     li.Value();
	}
      }
    }
    else
      return 0;
  }
  
  else if(!strcmp(a[0],"dshelp")) {

    interpretor<<"\n";
    interpretor<<"  dsinit"<<"\n";
    interpretor<<"  dsload shape1 shape2"<<"\n";
    interpretor<<"  dsinters [-sd] [f1 f2]"<<"\n";
    interpretor<<"    if -sd, the faces are samedomain"<<"\n";
    interpretor<<"    no argument = intersect two input Shapes"<<"\n";
    interpretor<<"  dsgetsect result [f1 f2] (result = compound)"<<"\n";
    interpretor<<"  dssuppsect comp"<<"\n";
    interpretor<<"  dsperform fus/c12/c21/com resultShape "<<"\n";
    interpretor<<"\n";

    return 0;
  }
  return 0;
}
//=======================================================================
//function : BOOPCommands
//purpose  : 
//=======================================================================
void TestTopOpe::DSACommands(Draw_Interpretor& theCommands)
{
  const char* g = "TestTopOpe DSACommands";
  theCommands.Add("dsinit","Taper dshelp",__FILE__,DSACCESSCOMMANDS,g);
  theCommands.Add("dsload","Taper dshelp",__FILE__,DSACCESSCOMMANDS,g);
  theCommands.Add("dsinters","Taper dshelp",__FILE__,DSACCESSCOMMANDS,g);
  theCommands.Add("dsgetsect","Taper dshelp",__FILE__,DSACCESSCOMMANDS,g);
  theCommands.Add("dssuppsect","Taper dshelp",__FILE__,DSACCESSCOMMANDS,g);
  theCommands.Add("dsperform","Taper dshelp",__FILE__,DSACCESSCOMMANDS,g);
  theCommands.Add("dshelp","Taper dshelp",__FILE__,DSACCESSCOMMANDS,g);
}
