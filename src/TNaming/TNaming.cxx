// Created on: 1998-01-20
// Created by: Yves FRICAUD
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



#include <TNaming.ixx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfOrientedShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfOrientedShapeShape.hxx>
#include <TDF_Label.hxx>
#include <TDF_ChildIterator.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TNaming_ShapesSet.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_ListOfNamedShape.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TNaming_RefShape.hxx>
#include <TNaming_DataMapOfShapePtrRefShape.hxx>
#include <TNaming_Tool.hxx>

#include <IntTools_FClass2d.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

// CopyShape
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <TNaming_TranslateTool.hxx>
#include <TopLoc_Datum3D.hxx>

#define BUC60862
#ifdef DEB
//#define MDTV_DEB
//#define MDTV_DEB_FSET
#endif
#ifdef MDTV_DEB
#include <BRepTools.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfOrientedShapeShape.hxx>
static void WriteS(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  char buf[256];
  if(strlen(filename) > 256) return;
#if defined WNT 
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
//function : MapShapes
//purpose  : TNaming
//=======================================================================

static void MapShapes(const TopoDS_Shape& SCible, const TopoDS_Shape& SSource, TopTools_DataMapOfShapeShape& M)
{
  M.Bind(SCible,SSource);
  TopoDS_Iterator icible(SCible);
  TopoDS_Iterator isource(SSource);
  while (icible.More()) {
    if (!M.IsBound(icible.Value())) MapShapes(icible.Value(),isource.Value(),M);
    icible.Next();
    isource.Next();
  }
}

//=======================================================================
//function : MapShapes
//purpose  : TNaming
//=======================================================================

static void MapShapes(const TDF_Label& LCible, const TDF_Label& LSource, TopTools_DataMapOfShapeShape& M)
{
  TNaming_Iterator icible(LCible);
  TNaming_Iterator isource(LSource);
  while (icible.More()) {
    if (!icible.OldShape().IsNull()) {
      if (!M.IsBound(icible.OldShape())) MapShapes(icible.OldShape(),isource.OldShape(),M);
    }
    if (!icible.NewShape().IsNull()) {
      if (!M.IsBound(icible.NewShape())) MapShapes(icible.NewShape(),isource.NewShape(),M);
    }
    icible.Next();
    isource.Next();
  }

  TDF_ChildIterator iccible(LCible);
  TDF_ChildIterator icsource(LSource);
  while (iccible.More()) {
    MapShapes(iccible.Value(),icsource.Value(),M);
    iccible.Next();
    icsource.Next();
  }
} 

//=======================================================================
//function : SubstituteShape
//purpose  : TNaming
//=======================================================================

void static SubstituteShape(const TopoDS_Shape& oldShape,
			    const TopoDS_Shape& newShape,
			    TNaming_DataMapOfShapePtrRefShape& amap) 
{
  if (oldShape.IsSame(newShape)) {
    cout <<"import_tool::Substitute : oldShape IsSame newShape"<<endl;
  }

  if (!amap.IsBound(oldShape)) {
      return;
  }
  TNaming_RefShape* pos;
  pos = amap.ChangeFind(oldShape);
  pos->Shape(newShape);
  amap.UnBind(oldShape);
  amap.Bind(newShape,pos);
}

//=======================================================================
//function : MakeShape
//purpose  : ANaming 
//=======================================================================

TopoDS_Shape TNaming::MakeShape (const TopTools_MapOfShape& MS) 
{  
  if (!MS.IsEmpty ()) {
    TopTools_MapIteratorOfMapOfShape it(MS);
    if (MS.Extent() == 1) {
      return it.Key();
    }
    else {
      TopoDS_Compound C;
      BRep_Builder B;
      B.MakeCompound(C);
      for (; it.More(); it.Next()){ 
	B.Add(C,it.Key());
      }
      return C;
    }
  }
  return TopoDS_Shape();  
}

//=======================================================================
//function : Substitute
//purpose  : TNaming
//=======================================================================

void TNaming::Substitute(const TDF_Label& LSource, 
			 const TDF_Label& LCible ,
			 TopTools_DataMapOfShapeShape&  M) 
{
//attention pour etre en parallele au niveau structure il faut que Lciblble corresponde au premier fils recopie
  MapShapes(LCible,LSource,M);
  Handle(TNaming_UsedShapes) US;
  LCible.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  TNaming_DataMapOfShapePtrRefShape& amap = US->Map();
  for (TopTools_DataMapIteratorOfDataMapOfShapeShape It(M);It.More();It.Next()) {
    SubstituteShape(It.Key(),It.Value(),amap);
 }   
}  

//=======================================================================
//function : SubstituteSShape
//purpose  : 
//=======================================================================
Standard_Boolean TNaming::SubstituteSShape(const TDF_Label& Lab, const TopoDS_Shape& From, TopoDS_Shape& To)
{
  Handle(TNaming_UsedShapes) US;
  Lab.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  TNaming_DataMapOfShapePtrRefShape& amap = US->Map();
  if (!amap.IsBound(To)) 
    return Standard_False;
  TNaming_RefShape* pos;
  pos = amap.ChangeFind(To);
  if(!amap.UnBind(To)) return Standard_False;
  //update shape
  To.Orientation(From.Orientation());
  pos->Shape(To);
  return amap.Bind(To, pos);
}

//=======================================================================
//function : Rebuild
//purpose  : TNaming
//=======================================================================

static Standard_Boolean  Rebuild (const TopoDS_Shape& S,
				  TopTools_DataMapOfShapeShape&  M)
{
  Standard_Boolean IsModified = Standard_False;
  if (M.IsBound(S)) return IsModified;
  
  BRep_Builder     B;
  TopoDS_Iterator  iteS (S.Oriented(TopAbs_FORWARD));
  
  // Reconstruction des sous shapes si necessaire.
  for (; iteS.More(); iteS.Next()) {
    const TopoDS_Shape& SS = iteS.Value();
    if (Rebuild (SS,M)) IsModified = Standard_True;
  }
  if (!IsModified) {
    M.Bind(S,S);
    return Standard_True;
  }

  // Reconstruction de S
  TopoDS_Shape NewS = S.Oriented(TopAbs_FORWARD);
  NewS.EmptyCopy();
  if (NewS.ShapeType() == TopAbs_EDGE) {
    Standard_Real f,l;
    BRep_Tool::Range(TopoDS::Edge(S),f,l);
    B.Range(TopoDS::Edge(NewS),f,l);
  }
  iteS.Initialize(S.Oriented(TopAbs_FORWARD));
  for (iteS.Initialize(S.Oriented(TopAbs_FORWARD)) ;iteS.More(); iteS.Next()) { 
    const TopoDS_Shape& OS = iteS.Value();
    const TopoDS_Shape& NS = M(OS);
    B.Add(NewS,NS.Oriented(OS.Orientation()));
  }
  M.Bind (S,NewS.Oriented(S.Orientation()));
  return IsModified;
}


//=======================================================================
//function : Update
//purpose  : TNaming
//=======================================================================

void TNaming::Update(const TDF_Label& L,
		     TopTools_DataMapOfShapeShape& M)

{
  //Reconstruction des shapes de L suite aux substitutions decrites dans la map M.
  // ex : si une face est remplacee par une autre il faut reconstruire les shapes
  //      qui contiennent cette face.
  Handle(TNaming_UsedShapes) US;
  L.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  TNaming_DataMapOfShapePtrRefShape& amap = US->Map();

  Standard_Boolean Change = Standard_False;
  for (TNaming_Iterator it(L); it.More(); it.Next()) {
    if (!it.OldShape().IsNull()) {
      const TopoDS_Shape& S = it.OldShape();
      if (!M.IsBound(S))
	if (Rebuild (S,M)) Change = Standard_True;
      SubstituteShape(S,M(S),amap);
    }
    if (!it.NewShape().IsNull()) {
      const TopoDS_Shape& S = it.NewShape();
      if (!M.IsBound(S)) 
	if (Rebuild (S,M)) Change = Standard_True;
      SubstituteShape(S,M(S),amap);
    }
  }

  // SI Les shapes dans les sous-labels sont des sous shapes des shapes de L
  // si les shapes de L n ont pas changes les shapes des sous-labels ne seront 
  // pas changes non plus.
  for (TDF_ChildIterator ciL(L); ciL.More(); ciL.Next()) {
    TNaming::Update (ciL.Value(),M);
  }
}

//=======================================================================
//function : BuilCompound
//purpose  : TNaming
//=======================================================================

static void BuildCompound(TopoDS_Compound& C,
			  const TDF_Label& L)
{
  BRep_Builder B;
  for (TNaming_Iterator it(L); it.More(); it.Next()) {
    if (!it.OldShape().IsNull()) {
      B.Add(C,it.OldShape());
    }
    if (!it.NewShape().IsNull()) {
      B.Add(C,it.NewShape());
    }
  }
  for (TDF_ChildIterator ciL(L); ciL.More(); ciL.Next()) {
    BuildCompound (C,ciL.Value());
  }
}

//=======================================================================
//function : Update
//purpose  : TNaming
//=======================================================================

static void BuildMap(const TDF_Label& L,
		     BRepBuilderAPI_Transform& Transformer,
		     TopTools_DataMapOfShapeShape& M)
{		   
  Handle(TNaming_UsedShapes) US;
  L.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  for (TNaming_Iterator it(L); it.More(); it.Next()) {
    if (!it.OldShape().IsNull()) {
      const TopoDS_Shape& S = it.OldShape();
      M.Bind(S,Transformer.ModifiedShape(S));
    }
    if (!it.NewShape().IsNull()) {
      const TopoDS_Shape& S = it.NewShape();
      M.Bind(S,Transformer.ModifiedShape(S));
    }
  }
  for (TDF_ChildIterator ciL(L); ciL.More(); ciL.Next()) {
    BuildMap (ciL.Value(),Transformer,M);
  }
}   

//=======================================================================
//function : LoadNamedShape
//purpose  : TNaming
//=======================================================================

static void LoadNamedShape (TNaming_Builder& B, 
			    TNaming_Evolution Evol, 
			    const TopoDS_Shape& OS, 
			    const TopoDS_Shape& NS)
{    
  switch (Evol) {
  case TNaming_PRIMITIVE :
    {
      B.Generated(NS);
      break;
    }
  case TNaming_GENERATED :
    {
      B.Generated(OS,NS);
      break;
    }
  case TNaming_MODIFY : 
    {
      B.Modify(OS,NS);
      break;
    }
  case TNaming_DELETE : 
    {
      B.Delete (OS);
      break;
    }
  case TNaming_SELECTED :
    {
      B.Select(NS,OS);
	  break;
    }
  default:
    break;
  }
}

//=======================================================================
//function : Displace
//purpose  : TNaming
//=======================================================================

void TNaming::Displace (const TDF_Label& L,
			const TopLoc_Location& Loc,
			const Standard_Boolean WithOld)
{  


  TopTools_ListOfShape Olds;
  TopTools_ListOfShape News;
  TNaming_Evolution    Evol;
  TNaming_Iterator     it(L);
  
  if (it.More()) {   // dp on continue de visiter les fils meme s'il y a pas de shape
    Evol = it.Evolution();
    for ( ; it.More(); it.Next()) {
      Olds.Append(it.OldShape());
      News.Append(it.NewShape());
      
    }
    
    TopTools_ListIteratorOfListOfShape itOlds(Olds);
    TopTools_ListIteratorOfListOfShape itNews(News);
    TNaming_Builder B(L);
    
    for ( ;itOlds.More() ; itOlds.Next(),itNews.Next()) {
      TopoDS_Shape OS,NS;
      const TopoDS_Shape& SO     = itOlds.Value();
      const TopoDS_Shape& SN     = itNews.Value();
      OS = SO;
      if (WithOld && !SO.IsNull()) OS = SO.Moved(Loc);
      if (!SN.IsNull()) NS = SN.Moved(Loc);
      
      LoadNamedShape ( B, Evol, OS, NS);
    }
  }
  for (TDF_ChildIterator ciL(L); ciL.More(); ciL.Next()) {
    Displace (ciL.Value(),Loc);
  }
}

//=======================================================================
//function : Replace
//purpose  : TNaming
//=======================================================================

static void Replace (const TDF_Label&                    L,
		     const TopTools_DataMapOfShapeShape& M)
{  
    
  TNaming_Evolution    Evol;
  TNaming_Iterator     it(L);
  
  if (!it.More()) return;
  Evol = it.Evolution();

  TNaming_Builder B(L);
  
  TopoDS_Shape OS,NS;

  for ( ; it.More(); it.Next()) {  
    if (!it.OldShape().IsNull()) {
      OS = it.OldShape();
      if (M.IsBound(OS)) OS = M(OS);
    }
    if (!it.NewShape().IsNull()) {
      NS = it.NewShape();
      if (M.IsBound(NS)) NS = M(NS);
    }
    LoadNamedShape ( B, Evol, OS, NS);
  }
  for (TDF_ChildIterator ciL(L); ciL.More(); ciL.Next()) {
    Replace (ciL.Value(),M);
  }
}

//=======================================================================
//function : Update
//purpose  : TNaming
//=======================================================================

void TNaming::Transform(const TDF_Label& L,
			const gp_Trsf&   T)
{

  //--------------------------------------------------------------------
  // Construction du compound qui contient tous les shapes sous le label
  // et ses fils.
  //--------------------------------------------------------------------
  TopoDS_Compound CompShape;
  BRep_Builder    B;
  B.MakeCompound(CompShape);

  BuildCompound (CompShape,L);

  //----------------------------
  // Transformation du compound.
  //-----------------------------
  BRepBuilderAPI_Transform Transformer(CompShape, T);

  //-----------------------------------------------------------
  //Remplacement des shapes initiaux par les shapes transformes.
  //-----------------------------------------------------------
  TopTools_DataMapOfShapeShape M;
  BuildMap (L,Transformer,M);
  Replace (L,M);
}

//=======================================================================
//function : IDList
//purpose  : TNaming
//=======================================================================

void TNaming::IDList(TDF_IDList& anIDList)
{ anIDList.Append(TNaming_NamedShape::GetID()); }


//=======================================================================
//function : Replicate
//purpose  : 
//=======================================================================

void TNaming::Replicate(const Handle(TNaming_NamedShape)& NS,
			const gp_Trsf& T,
			const TDF_Label& L)
{
  TopoDS_Shape SH = TNaming_Tool::CurrentShape(NS);
  TNaming::Replicate(SH, T, L);
}

//=======================================================================
//function : Replicate
//purpose  : TNaming
//=======================================================================

void TNaming::Replicate (const TopoDS_Shape& SH,
			 const gp_Trsf& T,
			 const TDF_Label& L)
{
  // transform
  BRepBuilderAPI_Transform opeTrsf(T);
  if (SH.ShapeType() == TopAbs_FACE || SH.ShapeType() == TopAbs_WIRE ) {  
    opeTrsf.Perform(SH, Standard_True); // pour le pattern de prism
  }
  else {
    opeTrsf.Perform(SH, Standard_False);
  }
  const TopoDS_Shape& newSH = opeTrsf.Shape();
  //BRepLib::UpdateTolerances(newSH, Standard_True);
  
  // principal shape

  TNaming_Builder Builder(L);
  Builder.Generated(SH, newSH);    
  
  // sub shape
  TopAbs_ShapeEnum SST = TopAbs_FACE;
  if (SH.ShapeType() == TopAbs_FACE || SH.ShapeType() == TopAbs_WIRE )  
    SST = TopAbs_EDGE;
  
  TNaming_Builder Builder2 (L.FindChild(1,Standard_True)); 
  for (TopExp_Explorer exp(SH, SST); exp.More(); exp.Next()) {
    const TopoDS_Shape& oldSubShape = exp.Current();
    const TopoDS_Shape& newSubShape = opeTrsf.ModifiedShape(oldSubShape);
    Builder2.Generated(oldSubShape, newSubShape);
  }
}


//=======================================================================
//function : ShapeCopy
//purpose  : TNaming
//=======================================================================

static TopoDS_Shape  ShapeCopy(const TopoDS_Shape& S, 
			       TopTools_DataMapOfShapeShape& M)
{
  if (S.IsNull())   return S;
  if (M.IsBound(S)) return M(S);
  //----------------------------
  //construction de la copie.
  // 1- copie des sous shapes.
  // 2- reconstruction du TShape
  //----------------------------
  BRep_Builder     B;
  TopoDS_Iterator  it (S.Oriented(TopAbs_FORWARD));

  for ( ; it.More(); it.Next()) {
    const TopoDS_Shape& SS    = it.Value();
    TopoDS_Shape        NewSS = ShapeCopy(SS,M);
  }
  TopoDS_Shape NewS = S.Oriented(TopAbs_FORWARD);
  NewS.EmptyCopy();
  if (NewS.ShapeType() == TopAbs_EDGE) {
    Standard_Real f,l;
    BRep_Tool::Range(TopoDS::Edge(S),f,l);
    B.Range(TopoDS::Edge(NewS),f,l);
  }
  for (it.Initialize(S.Oriented(TopAbs_FORWARD)) ;it.More(); it.Next()) { 
    const TopoDS_Shape& OS = it.Value();
    const TopoDS_Shape& NS = M(OS);
    B.Add(NewS,NS.Oriented(OS.Orientation()));
  }
  NewS.Orientation(S.Orientation());

  NewS.Free      (S.Free())      ;NewS.Modified(S.Modified());NewS.Checked (S.Checked());
  NewS.Orientable(S.Orientable());NewS.Closed  (S.Closed())  ;NewS.Infinite(S.Infinite());  
  NewS.Convex    (S.Convex());

  M.Bind (S,NewS);
  return NewS;
}

//=======================================================================
//function : ChangeShapes
//purpose  : TNaming
//=======================================================================

void TNaming::ChangeShapes(const TDF_Label&              L,
			   TopTools_DataMapOfShapeShape& M)
{
  TopTools_ListOfShape Olds;
  TopTools_ListOfShape News;

  Handle(TNaming_NamedShape) NS;
  L.FindAttribute(TNaming_NamedShape::GetID(),NS);

  if (!NS.IsNull()) {
    TNaming_Evolution Evol = NS->Evolution();
    for (TNaming_Iterator it(L); it.More(); it.Next()) { 
      const TopoDS_Shape& S1 = it.OldShape();
      const TopoDS_Shape& S2 = it.NewShape();
      Olds.Append(ShapeCopy(S1,M));News.Append(ShapeCopy(S2,M));
    }

    TopTools_ListIteratorOfListOfShape itOlds(Olds);
    TopTools_ListIteratorOfListOfShape itNews(News);

    TNaming_Builder B(L);
    
    for ( ;itOlds.More() ; itOlds.Next(),itNews.Next()) {
      LoadNamedShape ( B, Evol, itOlds.Value(), itNews.Value());
    }
  }

  for (TDF_ChildIterator ciL(L); ciL.More(); ciL.Next()) {
    ChangeShapes (ciL.Value(),M);
  }
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_OStream& TNaming::Print (const TNaming_Evolution EVOL,  Standard_OStream& s)
{
  switch(EVOL){
  case TNaming_PRIMITIVE :
    {
      s <<"PRIMITIVE"; break;
    }
  case TNaming_GENERATED :
    {
      s <<"GENERATED"; break;
    }  
  case TNaming_MODIFY :
    {
      s <<"MODIFY"; break;
    }
  case TNaming_DELETE :
    {
      s <<"DELETE"; break;
    }
  case TNaming_SELECTED :
    {
      s <<"SELECTED"; break;
    }
    default :
      s << "UNKNOWN_Evolution"; break;
    }
  return s;
}



//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_OStream& TNaming::Print (const TNaming_NameType NAME,  Standard_OStream& s)
{

  switch (NAME) {
  case TNaming_UNKNOWN :
    {
      s <<"UNKNOWN"; break;
    }
  case TNaming_IDENTITY :
    {
      s <<"IDENTITY"; break;
    }
  case TNaming_MODIFUNTIL :
    {
      s <<"MODIFUNTIL"; break;
    }
  case TNaming_GENERATION :
    {
      s <<"GENERATION"; break;
    }
  case TNaming_INTERSECTION :
    {
      s <<"INTERSECTION"; break;
    }
  case TNaming_UNION:
    {
      s <<"UNION"; break;
    }
  case TNaming_SUBSTRACTION :
    {
      s <<"SUBSTRACTION"; break;
    }
  case TNaming_CONSTSHAPE :
    {
      s <<"CONSTSHAPE"; break;
    }
  case TNaming_FILTERBYNEIGHBOURGS:
    {
      s <<"FILTERBYNEIGHBOURGS"; break;
    }
  case TNaming_ORIENTATION:
    {
      s <<"ORIENTATION"; break;
    }
  case TNaming_WIREIN:
    {
      s <<"WIREIN"; break;
    }
	case TNaming_SHELLIN:
    {
      s <<"SHELLIN"; break;
    }
    default :
      {
	s <<"UNKNOWN_NameType"; break;
      }
  } 
  return s;
}

#ifdef BUC60862

//=======================================================================
//function : Print
//purpose  : Prints UsedShapes.
//=======================================================================

Standard_OStream& TNaming::Print (const TDF_Label& ACCESS,  Standard_OStream& s) {
  Handle(TNaming_UsedShapes) US;
  if (!ACCESS.Root().FindAttribute(TNaming_UsedShapes::GetID(), US)) {
#ifdef DEB
    cout<<"TNaming::Print(US): Bad access"<<endl;
#endif
    return s;
  }
  return US->Dump(s);
}

#endif

//=======================================================================
//function : BuildMapIn
//purpose  : 
//=======================================================================
static void BuildMapIn(const TopoDS_Shape& Context, const TopAbs_ShapeEnum StopType, 
			 TopTools_DataMapOfOrientedShapeShape& Map)
{
  TopAbs_ShapeEnum aType;     
  if((Context.ShapeType() == TopAbs_SOLID || Context.ShapeType() == TopAbs_FACE) && (StopType - Context.ShapeType()) != 1)
    aType = (TopAbs_ShapeEnum)(Context.ShapeType() +2);
  else
    aType = (TopAbs_ShapeEnum)(Context.ShapeType()+1);
  for (TopExp_Explorer exp(Context,aType); exp.More(); exp.Next()) {
#ifdef MDTV_DEB
    if(!Map.Bind(exp.Current(), Context))
      cout << "Not bind = " <<exp.Current().ShapeType() <<endl; 
    else 
      cout <<"Bind = " <<exp.Current().ShapeType() << " to Context = " <<Context.ShapeType()<<endl;
#else
    Map.Bind(exp.Current(), Context);
#endif
    if(exp.Current().ShapeType() < StopType ) {
      BuildMapIn(exp.Current(), StopType, Map);
    }
  }
  // fix for NMT case
  if(Context.ShapeType() < StopType) {
    TopoDS_Iterator it(Context);
    for(;it.More();it.Next()) {
      if(it.Value().Orientation() != TopAbs_FORWARD && it.Value().Orientation() != TopAbs_REVERSED) {
	Map.Bind(it.Value(), Context);
	//cout << "INTERNAL || EXTERNAL Orientation found" <<endl;
      }
    }
  }
}
//=======================================================================
//function : BuildMapC0
//purpose  : builds data map: key - context, C0 - top context
//=======================================================================
static void BuildMapC0(const TopoDS_Shape& Context, const TopoDS_Shape& C0, const TopAbs_ShapeEnum StopType, 
		     TopTools_DataMapOfOrientedShapeShape& Map)
{
  TopoDS_Iterator anIt(Context);
  while(anIt.More()) {
    const TopoDS_Shape& aKey = anIt.Value();
#ifdef MDTV_DEB
    if(!Map.Bind(aKey, C0)) 
      cout << "Not bind = " <<aKey.ShapeType() <<endl;      
#else
    Map.Bind(aKey, C0);
#endif
    if(aKey.ShapeType() < StopType ) {
      if(aKey.ShapeType() < TopAbs_SOLID) {
	BuildMapC0(aKey, Context, StopType, Map);
      }
      else
	BuildMapIn(aKey, StopType, Map);
    }
    anIt.Next();
  }
}

//=======================================================================
//function : BuildMap
//purpose  : builds data map: key - context
//=======================================================================
static void BuildMap(const TopoDS_Shape& Context, const TopAbs_ShapeEnum StopType, 
		     TopTools_DataMapOfOrientedShapeShape& Map)
{
#ifdef MDTV_DEB
  TCollection_AsciiString Nam("Cnt_");
  Standard_Integer i = 0;
#endif
  TopoDS_Iterator anIt(Context);
  while(anIt.More()) {
    const TopoDS_Shape& aKey = anIt.Value();
#ifdef MDTV_DEB
    if(!Map.Bind(aKey, Context)) 
      cout << "Not bind = " <<aKey.ShapeType() <<endl;      
#else
    Map.Bind(aKey, Context);
#endif
    if(aKey.ShapeType() < StopType ) {
      if(aKey.ShapeType() < TopAbs_SOLID)
	BuildMapC0(aKey, Context, StopType, Map);
      else
	BuildMapIn(aKey, StopType, Map);
    }
    anIt.Next();
  }
}
//=======================================================================
//function : FindUniqueContext
//purpose  : Find unique context of selection
//=======================================================================
TopoDS_Shape TNaming::FindUniqueContext(const TopoDS_Shape& Selection, const TopoDS_Shape& Context)
{
  TopTools_DataMapOfOrientedShapeShape aMap; 
  BuildMap(Context, Selection.ShapeType(), aMap);
#ifdef MDTV_DEB
  TopTools_DataMapIteratorOfDataMapOfOrientedShapeShape it (aMap);
  for (;it.More();it.Next()) {
    cout <<"FindUniqueContext: Key - " <<it.Key().ShapeType()<< " " << it.Key().TShape() <<" OR = " <<it.Key().Orientation() <<
      "  Context - " << it.Value().ShapeType() << "  " << it.Value().TShape()  << " OR = " <<it.Value().Orientation() <<endl;
  }
#endif
  if(aMap.IsBound(Selection))
    return aMap.Find(Selection); 
  return TopoDS_Shape();
}

//=======================================================================
//function : FindUniqueContexts
//purpose  : Find unique context of selection which is pure concatenation 
//         : of atomic shapes (Compound)
//=======================================================================
TopoDS_Shape TNaming::FindUniqueContextSet(const TopoDS_Shape& Selection, const TopoDS_Shape& Context,
					   Handle(TopTools_HArray1OfShape)& Arr)
{
  if(Selection.ShapeType() == TopAbs_COMPOUND) {
    TopTools_DataMapOfOrientedShapeShape aMap;
    Standard_Integer Up(0);
    TopAbs_ShapeEnum aStopType(TopAbs_COMPOUND);
    TopoDS_Iterator it(Selection);
    for(;it.More(); it.Next()) {
      const TopoDS_Shape& aS = it.Value(); 
      if(aS.ShapeType() > aStopType)
	aStopType = aS.ShapeType();	
      Up++;
    }
    if(Up > 0) 
      Arr = new  TopTools_HArray1OfShape(1, Up);
    if(aStopType == TopAbs_SHAPE)
      aStopType = Selection.ShapeType();
    BuildMap(Context, aStopType, aMap);
    if(aMap.IsBound(Selection))
      return aMap.Find(Selection);
    else if(Selection.ShapeType() == TopAbs_COMPOUND) {
      Standard_Integer num1(0),num2(0);
      TopoDS_Compound CompShape;
      BRep_Builder    B;
      B.MakeCompound(CompShape);
      TopoDS_Iterator it(Selection);
      TopTools_MapOfShape aView;
      for(;it.More(); it.Next(),num1++) {
	if(aMap.IsBound(it.Value())) {
	  if(aView.Add(aMap.Find(it.Value()))) {
	    B.Add(CompShape, aMap.Find(it.Value()));	    
	  }
	  if(!Arr.IsNull()) 
	    Arr->SetValue(num1+1, aMap.Find(it.Value()));
	  
	  if(aMap.Find(it.Value()) == Context)
	    num2++;
	}
      }
      if(num1 == num2 && num2)
	return Context;
      else {
	TopoDS_Iterator it(CompShape);
	Standard_Integer n(0);
	TopoDS_Shape aCmp;
	for(;it.More();it.Next()) {
	    n++;
	    aCmp = it.Value();
	}
	if(n == 1) {
#ifdef MDTV_DEB_FSET
	  cout << "FindUniqueContextSet: n = " << n <<endl;
#endif
	  return aCmp;
	}
	return CompShape;
      }
    }
  }
  return TopoDS_Shape();
}

//=======================================================================
//function : OuterWire
//purpose  : Returns True & <theWire> if Outer wire is found.
//=======================================================================
Standard_Boolean TNaming::OuterWire(const TopoDS_Face& theFace, TopoDS_Wire& theWire)
{ 
  TopoDS_Face aFx;
  TopoDS_Wire aWx;
  BRep_Builder aBB;
  IntTools_FClass2d aFC;
  Standard_Boolean bFlag(Standard_False);
  Standard_Real aTol = BRep_Tool::Tolerance(theFace);
  TopoDS_Iterator aIt(theFace);
  for (; aIt.More(); aIt.Next()) {
    aWx=*((TopoDS_Wire*)&aIt.Value());
    aFx = theFace;
    aFx.EmptyCopy();
    aBB.Add(aFx, aWx);
    aFC.Init(aFx, aTol);
    bFlag = aFC.IsHole();
    if (!bFlag) 
      break;
  }
  theWire=aWx;
  return !bFlag;// if bFlag == True -  not found
}
//=======================================================================
//function : IsInternal
//purpose  :
//=======================================================================
static Standard_Boolean IsInternal(const TopoDS_Shape& aSx)
{
  Standard_Boolean bInternal;
  TopAbs_Orientation aOr;
  TopoDS_Iterator aIt;
  bInternal = Standard_False;
  aIt.Initialize(aSx);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSy=aIt.Value();
    aOr=aSy.Orientation();
    bInternal = (aOr == TopAbs_INTERNAL || aOr == TopAbs_EXTERNAL);
    break;
  }
  return bInternal;
}
//=======================================================================
//function : OuterShell
//purpose  : returns True & <theShell>, if Outer shell is found
//=======================================================================
Standard_Boolean TNaming::OuterShell(const TopoDS_Solid& theSolid, 
				             TopoDS_Shell& theShell)
{
  TopoDS_Solid aSDx;
  TopoDS_Shell aSHx;
  TopAbs_State aState;
  Standard_Boolean bFound(Standard_False);
  Standard_Real aTol(1.e-7);
  //
  BRep_Builder aBB;
  BRepClass3d_SolidClassifier aSC;
  TopoDS_Iterator aIt(theSolid);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx = aIt.Value();

    if (aSx.ShapeType() != TopAbs_SHELL)
      continue;
    if (IsInternal(aSx)) 
      continue;
    //
    aSHx = *((TopoDS_Shell*)&aSx);
    //
    aSDx = theSolid;
    aSDx.EmptyCopy();
    //
    aBB.Add(aSDx, aSHx);
    //
    aSC.Load(aSDx);
    //BRepClass3d_SolidClassifier& aSC=aCtx.SolidClassifier(aSDx);
    aSC.PerformInfinitePoint(aTol);
    aState = aSC.State();
    if(aState == TopAbs_OUT) {
      bFound = Standard_True;
      break;
    }
  }
  theShell = aSHx;

  return bFound;
}



