// ColoredShapes.cpp: implementation of the CColoredShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <afxtempl.h>

#include "ColoredShapes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColoredShapes::CColoredShapes()
{
}


void CColoredShapes::Add(const Quantity_NameOfColor aColor, const TopoDS_Shape& aShape)
{
	m_shapeList.Append(aShape);
	m_colorMap.Bind(aShape, aColor);
}

void CColoredShapes::Remove(const TopoDS_Shape& aShape)
{
	m_colorMap.UnBind(aShape);
	for ( TopoDS_ListIteratorOfListOfShape iter(m_shapeList); iter.More(); iter.Next() ) {
		if(iter.Value() == aShape) {
			m_shapeList.Remove(iter);
			break;
		}
	}
}

IMPLEMENT_SERIAL(CColoredShapes, CObject,1);

// This schema contains all the Persistent Geometry and Topology
#include <ShapeSchema.hxx>  

// Tools to store TopoDS_Shape
#include <MgtBRep.hxx>
#include <PTopoDS_HShape.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <TopoDS_Shape.hxx>

// Tools to put Persistent Object in an archive
#include <FSD_Archive.hxx>
#include <Storage_Data.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_Root.hxx>
#include <PTColStd_PersistentTransientMap.hxx>

void CColoredShapes::Serialize(CArchive & ar)
{
	CObject::Serialize(ar);

	// an I/O driver
	FSD_Archive f( &ar );

	// the applicative Schema containing Persistent Topology and Geometry
	// Note that it inherits from the class Storage_Schema
	Handle(ShapeSchema) s = new ShapeSchema;

	if ( ar.IsStoring() ) 
	{   
		// Write number of shapes to be serialized

		ar << (int)m_colorMap.Extent();
	
		for ( TopoDS_ListIteratorOfListOfShape iter(m_shapeList); iter.More(); iter.Next() )
		{
			//Create the persistent Shape
			PTColStd_TransientPersistentMap aMap;

			Handle(PTopoDS_HShape) aPShape = 
				MgtBRep::Translate(iter.Value(), aMap, MgtBRep_WithoutTriangle);

			// Store the Persistent shape in the archive
			Handle(Storage_Data) d = new Storage_Data;
			d->AddRoot("ObjectName", aPShape);
			s->Write( f, d);

			// Check
			if (d->ErrorStatus() != Storage_VSOk) 
			{
				::MessageBox(NULL, " Error while writing... ", " Error ",MB_OK) ;
			}
	
			// Store the color in the archive
			ar << (Standard_Integer)m_colorMap.Find(iter.Value());
		}
	}
	else
	{
		// Get numbe of stored shapes
		int nbShapes;
		ar >> nbShapes;

		Standard_Integer tmp;
		Quantity_NameOfColor theColor;

		// Reading all shapes
		for ( int i = 0; i < nbShapes; i++ )
		{
			// Read the Persistent Shape from the archive
			Handle(Storage_Data) d = s->Read( f );
			Handle(Storage_HSeqOfRoot)  roots = d->Roots();
			Handle(Standard_Persistent) p;
			Handle(Storage_Root) r;
			Handle(PTopoDS_HShape) aPShape;
			TopoDS_Shape theShape;

			r = roots->Value(1);
			p = r->Object();
			aPShape  = Handle(PTopoDS_HShape)::DownCast(p);

			// Create the shape
			PTColStd_PersistentTransientMap aMap;

			MgtBRep::Translate(aPShape,aMap,theShape,MgtBRep_WithoutTriangle);

			m_shapeList.Append(theShape);
			
			// Read the Color from the archive
			ar >> tmp;
			theColor = (Quantity_NameOfColor) tmp;
			m_colorMap.Bind(theShape, theColor);
		}	
	}
}

void CColoredShapes::Display(Handle(AIS_InteractiveContext)& anAIScontext)
{
	for ( TopoDS_ListIteratorOfListOfShape iter(m_shapeList); iter.More(); iter.Next() )
	{
		Handle(AIS_Shape) ais = new AIS_Shape(iter.Value());
		anAIScontext->SetColor(ais, (Quantity_NameOfColor)m_colorMap.Find(iter.Value()));
                anAIScontext->SetMaterial(ais, Graphic3d_NOM_GOLD, Standard_False);
		anAIScontext->Display(ais, Standard_False);
	}
}
