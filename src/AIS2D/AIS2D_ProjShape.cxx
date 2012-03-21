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

#include <AIS2D_ProjShape.ixx>
#include <TopExp_Explorer.hxx>	 
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListOfShape.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_PolyHLRToShape.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopLoc_Location.hxx>
#include <Standard_Assert.hxx>

#include <V2d_Viewer.hxx>

#include <AIS2D_InteractiveContext.hxx>


AIS2D_ProjShape::AIS2D_ProjShape( ) 
 :
   AIS2D_InteractiveObject(),

   myProjector( HLRAlgo_Projector() ),
   myNbIsos( 3 ),
   myIsPolyAlgo( Standard_False ),
   myIsHiddenLine( Standard_True ),
   myESharp( Standard_True ),
   myESmooth( Standard_True ),
   myESewn( Standard_True ),
   myEOutline( Standard_True ),
   myEIsoline( Standard_True )

{
}

AIS2D_ProjShape::AIS2D_ProjShape( const HLRAlgo_Projector& aProjector,
                                  const Standard_Integer nbIsos,
                                  const Standard_Boolean isPolyAlgo,
                                  const Standard_Boolean visHL ) 
 :
  AIS2D_InteractiveObject(),
  myProjector( aProjector ),
  myNbIsos( nbIsos ),
  myIsPolyAlgo( isPolyAlgo ),
  myIsHiddenLine( visHL ),
  myESharp( Standard_True ),
  myESmooth( Standard_True ),
  myESewn( Standard_True ),
  myEOutline( Standard_True ),
  myEIsoline( Standard_True )
  
{
}


void AIS2D_ProjShape::Add( const TopoDS_Shape& aShape ) {
 
	myListOfShape.Append( aShape );
	myAlgo.Nullify();
	myPolyAlgo.Nullify();
    ComputeShape();
}										  

void AIS2D_ProjShape::ComputeShape() {

  TopoDS_Shape VCompound, Rg1LineVCompound, RgNLineVCompound,
               OutLineVCompound, IsoLineVCompound, 
               HCompound, Rg1LineHCompound, RgNLineHCompound,
               OutLineHCompound, IsoLineHCompound;

  if ( myIsPolyAlgo ) {
      
	if ( myPolyAlgo.IsNull() ) CreatePolyAlgo();

    HLRBRep_PolyHLRToShape aPolyHLRToShape;
    aPolyHLRToShape.Update( myPolyAlgo );

    mySetOfSegments  = new Graphic2d_SetOfSegments( this );
    myHSetOfSegments = new Graphic2d_SetOfSegments( this );

    if ( myESharp ) {
      VCompound        = aPolyHLRToShape.VCompound();
      DrawSegments( Rg1LineVCompound, mySetOfSegments );         
    }
    if ( myESmooth ) {
      Rg1LineVCompound = aPolyHLRToShape.Rg1LineVCompound();
      DrawSegments( RgNLineVCompound, mySetOfSegments );
    }
    if ( myESewn ) {
      RgNLineVCompound = aPolyHLRToShape.RgNLineVCompound();
      DrawSegments( OutLineVCompound, mySetOfSegments );
    }
    if ( myEOutline ) {
      OutLineVCompound = aPolyHLRToShape.OutLineVCompound();
      DrawSegments( VCompound, mySetOfSegments );
    }
    
    if ( myIsHiddenLine ) {
     if ( myESharp ) {
      HCompound        = aPolyHLRToShape.HCompound();
      DrawSegments( Rg1LineHCompound, myHSetOfSegments );
     }
     if ( myESmooth ) {
      Rg1LineHCompound = aPolyHLRToShape.Rg1LineHCompound();
	  DrawSegments( RgNLineHCompound, myHSetOfSegments );
     }
     if ( myESewn ) {
      RgNLineHCompound = aPolyHLRToShape.RgNLineHCompound();
      DrawSegments( OutLineHCompound, myHSetOfSegments );
     }
     if ( myEOutline ) {
      OutLineHCompound = aPolyHLRToShape.OutLineHCompound();
      DrawSegments( HCompound, myHSetOfSegments );
     }
    } // end if is HiddenLine
  } else {
  
	if ( myAlgo.IsNull() ) 	CreateAlgo();
    
	HLRBRep_HLRToShape aHLRToShape( myAlgo );
    
    mySetOfCurves  = new GGraphic2d_SetOfCurves( this );
    myHSetOfCurves = new GGraphic2d_SetOfCurves( this );

    if ( myESharp ) {
     VCompound        = aHLRToShape.VCompound();
     DrawCurves( VCompound, mySetOfCurves );
    }
    if ( myESmooth ) {
      Rg1LineVCompound = aHLRToShape.Rg1LineVCompound();
      DrawCurves( Rg1LineVCompound, mySetOfCurves );
    }
    if ( myESewn ) {
      RgNLineVCompound = aHLRToShape.RgNLineVCompound();
      DrawCurves( RgNLineVCompound, mySetOfCurves );
    }
    if ( myEOutline ) {
      OutLineVCompound = aHLRToShape.OutLineVCompound();
      DrawCurves( OutLineVCompound, mySetOfCurves );
    }
    if ( myEIsoline ) {
      IsoLineVCompound = aHLRToShape.IsoLineVCompound();
	  DrawCurves( IsoLineVCompound, mySetOfCurves );
    }
    
    if ( myIsHiddenLine ) {
      if ( myESharp ) {
        HCompound        = aHLRToShape.HCompound();
        DrawCurves( HCompound, myHSetOfCurves );
      }
      if ( myESmooth ) {
        Rg1LineHCompound = aHLRToShape.Rg1LineHCompound();
        DrawCurves( Rg1LineHCompound, myHSetOfCurves );
      }
      if ( myESewn ) {
        RgNLineHCompound = aHLRToShape.RgNLineHCompound();
        DrawCurves( RgNLineHCompound, myHSetOfCurves );
       }
      if ( myEOutline ) {
        OutLineHCompound = aHLRToShape.OutLineHCompound();
        DrawCurves( OutLineHCompound, myHSetOfCurves );   
      }
      if ( myEIsoline ) {
        IsoLineHCompound = aHLRToShape.IsoLineHCompound();
        DrawCurves( IsoLineHCompound, myHSetOfCurves );
      }
    } //end if is HiddenLine
      
  } // end if is PolyAlgo
    
}

inline HLRAlgo_Projector AIS2D_ProjShape::Projector() const {
  return myProjector;
}

void AIS2D_ProjShape::SetProjector(const HLRAlgo_Projector& aProjector) {

   myProjector = aProjector;
   myAlgo.Nullify();
   myPolyAlgo.Nullify();
   if ( Length() ) {
    RemovePrimitives();
    ComputeShape();
   }
}

void AIS2D_ProjShape::SetNbIsos( const Standard_Integer aNbIsos ) {
	
   myNbIsos = aNbIsos; 
   myAlgo.Nullify(); 
   if ( Length() ) {
     RemovePrimitives();
     ComputeShape();
   }
}

void AIS2D_ProjShape::SetPolyAlgo( const Standard_Boolean aIsPoly ) {
	
	myIsPolyAlgo = aIsPoly;
    if ( Length() ) {
      RemovePrimitives();
      ComputeShape();
    }
}

void AIS2D_ProjShape::SetHLMode( const Standard_Boolean aIsHLM ) {

	myIsHiddenLine = aIsHLM;
    if ( Length() ) {
      RemovePrimitives();
      ComputeShape();
    }
}

Standard_Boolean AIS2D_ProjShape::IsHLMode() const {
  return myIsHiddenLine;
}

Standard_Integer AIS2D_ProjShape::GetNbIsos() const {
  return myNbIsos;
}

Standard_Boolean AIS2D_ProjShape::GetIsPoly() const {
  return myIsPolyAlgo;
}

Handle(Graphic2d_Line) AIS2D_ProjShape::GetPrimitives() const {
    if ( myIsPolyAlgo ) 
        return mySetOfSegments;
    else 
        return mySetOfCurves;
}

Handle(Graphic2d_Line) AIS2D_ProjShape::GetHLPrimitives() const {
    if ( myIsPolyAlgo ) 
        return myHSetOfSegments;
    else 
        return myHSetOfCurves;
}

void AIS2D_ProjShape::CreateAlgo() {

  myAlgo = new HLRBRep_Algo();
  TopTools_ListIteratorOfListOfShape anIterator( myListOfShape );
  for ( ; anIterator.More(); anIterator.Next() ) 
	  myAlgo->Add( anIterator.Value(), myNbIsos );
  myAlgo->Projector( myProjector );
  myAlgo->Update();
  myAlgo->Hide();

}

void AIS2D_ProjShape::CreatePolyAlgo() {

  myPolyAlgo = new HLRBRep_PolyAlgo();
  TopTools_ListIteratorOfListOfShape anIterator(myListOfShape);
  for ( ; anIterator.More(); anIterator.Next() ) 
	  myPolyAlgo->Load( anIterator.Value() );
  myPolyAlgo->Projector( myProjector );
  myPolyAlgo->Update();
}

void AIS2D_ProjShape::DrawSegments( const TopoDS_Shape& aShape, 
                                    Handle(Graphic2d_SetOfSegments)& aSofS ) {

  if ( aShape.IsNull() ) return;

  TopExp_Explorer theExp( aShape, TopAbs_EDGE );
  while ( theExp.More() ) {
    const TopoDS_Edge& CurrentEdge = TopoDS::Edge( theExp.Current() );
    const TopoDS_Vertex& FirstVertex = TopExp::FirstVertex(CurrentEdge);
    const TopoDS_Vertex& LastVertex = TopExp::LastVertex(CurrentEdge);
    gp_Pnt FirstPoint = BRep_Tool::Pnt(FirstVertex);
    gp_Pnt LastPoint  = BRep_Tool::Pnt(LastVertex);
    aSofS->Add( FirstPoint.X(), FirstPoint.Y(), LastPoint.X(), LastPoint.Y() );
    theExp.Next();
  }
 }

void AIS2D_ProjShape::DrawCurves( const TopoDS_Shape& aShape,
                                  Handle(GGraphic2d_SetOfCurves)& aSofC) {
  
  if ( aShape.IsNull() ) return;

  TopExp_Explorer theExp( aShape, TopAbs_EDGE );
  Handle(Geom2d_Curve) aCurve;
  Handle(Geom_Surface) aSurface;
  TopLoc_Location theLoc;
  Standard_Real f, l;
  while ( theExp.More() ) {

    const TopoDS_Edge& CurrentEdge = TopoDS::Edge( theExp.Current() );
    BRep_Tool::CurveOnSurface( CurrentEdge, aCurve, aSurface, theLoc, f, l );
    Standard_ASSERT (theLoc.IsIdentity(), "Unexpected edge with non-identity location", continue);
    Standard_ASSERT (! aCurve.IsNull(), "Null PCurve", continue);
    Handle(Geom2d_TrimmedCurve) theCurve = new Geom2d_TrimmedCurve(aCurve,f,l);
    aSofC->Add(theCurve);
    theExp.Next();
  }
}

void AIS2D_ProjShape::ShowEdges( const Standard_Boolean aSharp,
                                 const Standard_Boolean aSmooth,
                                 const Standard_Boolean aSewn,
                                 const Standard_Boolean anOutline,
                                 const Standard_Boolean anIsoline )  {
   myESharp   = aSharp;
   myESmooth  = aSmooth;
   myESewn    = aSewn;
   myEOutline = anOutline;
   myEIsoline = anIsoline;

   if ( Length() ) {
     RemovePrimitives();
     ComputeShape();
   }
}
