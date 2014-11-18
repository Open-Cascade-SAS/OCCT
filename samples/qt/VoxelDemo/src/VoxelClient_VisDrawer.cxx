// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include "VoxelClient_VisDrawer.h"

#include <OpenGl_GlCore11.hxx>

#include <ElSLib.hxx>
#include <gp_Dir.hxx>
#include <Geom_Plane.hxx>
#include <gce_MakePln.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_CUserDraw.hxx>

#include <InterfaceGraphic_telem.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_NamedStatus.hxx>
#include <OpenGl_Workspace.hxx>

/**************************************************************************/

class VoxelClient_VisDrawer::VisElement : public OpenGl_Element
{
public:

  VisElement (Voxel_VisData*);
  virtual ~VisElement();

  void EvaluateBounds (Graphic3d_BndBox4f& theMinMax);

  void Render (const Handle(OpenGl_Workspace) &theWorkspace) const;

  virtual void Release (OpenGl_Context* theContext)
  {
    //
  }

private:

  VoxelClient_VisDrawer* myHandler;

public:

  DEFINE_STANDARD_ALLOC

};

//=======================================================================
//function : VisElement
//purpose  : Constructor
//=======================================================================

VoxelClient_VisDrawer::VisElement::VisElement (Voxel_VisData* theData)
{
  myHandler = new VoxelClient_VisDrawer (theData);
}

//=======================================================================
//function : ~VisElement
//purpose  : Destructor
//=======================================================================

VoxelClient_VisDrawer::VisElement::~VisElement ()
{
  delete myHandler;
}

//=======================================================================
//function : EvaluateBounds
//purpose  :
//=======================================================================

void VoxelClient_VisDrawer::VisElement::EvaluateBounds
  (Graphic3d_BndBox4f& theMinMax)
{
  myHandler->EvalMinMax (theMinMax);
}

//=======================================================================
//function : Render
//purpose  : display element
//=======================================================================

void VoxelClient_VisDrawer::VisElement::Render
  (const Handle (OpenGl_Workspace) &theWorkspace) const
{
  const Standard_Boolean aHl = (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT);
  myHandler->Display (aHl);
}

//=======================================================================
//function : VisDrawerCallBack
//purpose  : visdrawer element create callback, adds an element to graphic
//           driver's structure
//=======================================================================

static OpenGl_Element* VisDrawerCallBack (const Graphic3d_CUserDraw* theUserDraw)
{
  if (theUserDraw == 0)
    return 0;

  // Retrieve the user structure
  Voxel_VisData* aUserData = (Voxel_VisData*) (theUserDraw->Data);

  if (aUserData == 0)
    return 0;

  VoxelClient_VisDrawer::VisElement *aElem = 
    new VoxelClient_VisDrawer::VisElement (aUserData);

  if (theUserDraw->Bounds != 0)
    aElem->EvaluateBounds (*(theUserDraw->Bounds));

  return aElem;
}

/**************************************************************************/
void VoxelClient_VisDrawer::Init (Handle(OpenGl_GraphicDriver)& theDriver)
{
    static Standard_Boolean isInitializeded(Standard_False);

    if (!isInitializeded)
    {
        isInitializeded = Standard_True;
        theDriver->UserDrawCallback() = VisDrawerCallBack;
    }
}

/**************************************************************************/
VoxelClient_VisDrawer::VoxelClient_VisDrawer(Voxel_VisData * theData):myData(theData)
{

}

/**************************************************************************/
VoxelClient_VisDrawer::~VoxelClient_VisDrawer()
{
    if (myData)
    {
        // Because a pointer to the data is copied, 
        // it is possible to make an attempt to delete GL lists for
        // a structure, which is already deleted.
        // Such a situation may happen on close of the application.
        // Therefore, this try / catch is used.

        try
        {
            Standard_Integer idir;

            // Points

            // BoolDS
            if (myData->myDisplay.myBoolPointsList > 0)
            {
                glDeleteLists(myData->myDisplay.myBoolPointsList, 1);
                myData->myDisplay.myBoolPointsList = -1;
            }
            for (idir = Xminus; idir <= Zplus; idir++)
            {
                if (myData->myDisplay.myBoolNearestPointsList[idir] > 0)
                {
                    glDeleteLists(myData->myDisplay.myBoolNearestPointsList[idir], 1);
                    myData->myDisplay.myBoolNearestPointsList[idir] = -1;
                }
            }

            // ColorDS
            if (myData->myDisplay.myColorPointsList > 0)
            {
                glDeleteLists(myData->myDisplay.myColorPointsList, 1);
                myData->myDisplay.myColorPointsList = -1;
            }
            for (idir = Xminus; idir <= Zplus; idir++)
            {
                if (myData->myDisplay.myColorNearestPointsList[idir] > 0)
                {
                    glDeleteLists(myData->myDisplay.myColorNearestPointsList[idir], 1);
                    myData->myDisplay.myColorNearestPointsList[idir] = -1;
                }
            }

            // ROctBoolDS
            if (myData->myDisplay.myROctBoolPointsList > 0)
            {
                glDeleteLists(myData->myDisplay.myROctBoolPointsList, 1);
                myData->myDisplay.myROctBoolPointsList = -1;
            }
            for (idir = Xminus; idir <= Zplus; idir++)
            {
                if (myData->myDisplay.myROctBoolNearestPointsList[idir] > 0)
                {
                    glDeleteLists(myData->myDisplay.myROctBoolNearestPointsList[idir], 1);
                    myData->myDisplay.myROctBoolNearestPointsList[idir] = -1;
                }
            }

            // Triangulation
            if (myData->myDisplay.myTriangulationList > 0)
            {
                glDeleteLists(myData->myDisplay.myTriangulationList, 1);
                myData->myDisplay.myTriangulationList = -1;
            }
        }
        catch (...)
        {

        }
    }
}

/**************************************************************************/
void VoxelClient_VisDrawer::EvalMinMax(Graphic3d_BndBox4f& theMinMax) const
{
    Graphic3d_Vec4 aMinPt (FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
    Graphic3d_Vec4 aMaxPt (-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);

    if(!myData)
        return;
    if(myData->myBoolVoxels)
    {
        Graphic3d_Vec4 aBoolVoxelsMin (RealToShortReal (myData->myBoolVoxels->GetX()),
                                       RealToShortReal (myData->myBoolVoxels->GetY()),
                                       RealToShortReal (myData->myBoolVoxels->GetZ()),
                                       1.0f);
        Graphic3d_Vec4 aBoolVoxelsMax (
          RealToShortReal (myData->myBoolVoxels->GetX() + myData->myBoolVoxels->GetXLen()),
          RealToShortReal (myData->myBoolVoxels->GetY() + myData->myBoolVoxels->GetYLen()),
          RealToShortReal (myData->myBoolVoxels->GetZ() + myData->myBoolVoxels->GetZLen()),
          1.0f);

        aMinPt = aMinPt.cwiseMin (aBoolVoxelsMin);
        aMaxPt = aMaxPt.cwiseMax (aBoolVoxelsMax);
    }
    if(myData->myColorVoxels)
    {
        Graphic3d_Vec4 aColorVoxelsMin (RealToShortReal (myData->myColorVoxels->GetX()),
                                        RealToShortReal (myData->myColorVoxels->GetY()),
                                        RealToShortReal (myData->myColorVoxels->GetZ()),
                                        1.0f);
        Graphic3d_Vec4 aColorVoxelsMax (
          RealToShortReal (myData->myColorVoxels->GetX() + myData->myColorVoxels->GetXLen()),
          RealToShortReal (myData->myColorVoxels->GetY() + myData->myColorVoxels->GetYLen()),
          RealToShortReal (myData->myColorVoxels->GetZ() + myData->myColorVoxels->GetZLen()),
          1.0f);

        aMinPt = aMinPt.cwiseMin (aColorVoxelsMin);
        aMaxPt = aMaxPt.cwiseMax (aColorVoxelsMax);
    }
    if(myData->myROctBoolVoxels)
    {
        Graphic3d_Vec4 aROctBoolVoxelsMin (RealToShortReal (myData->myROctBoolVoxels->GetX()),
                                           RealToShortReal (myData->myROctBoolVoxels->GetY()),
                                           RealToShortReal (myData->myROctBoolVoxels->GetZ()),
                                           1.0f);
        Graphic3d_Vec4 aROctBoolVoxelsMax (
          RealToShortReal (myData->myROctBoolVoxels->GetX() + myData->myROctBoolVoxels->GetXLen()),
          RealToShortReal (myData->myROctBoolVoxels->GetY() + myData->myROctBoolVoxels->GetYLen()),
          RealToShortReal (myData->myROctBoolVoxels->GetZ() + myData->myROctBoolVoxels->GetZLen()),
          1.0f);

        aMinPt = aMinPt.cwiseMin (aROctBoolVoxelsMin);
        aMaxPt = aMaxPt.cwiseMax (aROctBoolVoxelsMax);
    }
    if (!myData->myTriangulation.IsNull())
    {
        Standard_Real x, y, z;
        const TColgp_Array1OfPnt& nodes = myData->myTriangulation->Nodes();
        Standard_Integer inode = nodes.Lower(), nb_nodes = nodes.Upper();
        for (; inode <= nb_nodes; inode++)
        {
            nodes.Value(inode).Coord(x, y, z);
            Graphic3d_Vec4 aNodeCoord (RealToShortReal (x),
                                       RealToShortReal (y),
                                       RealToShortReal (z),
                                       1.0f);
            aMinPt = aMinPt.cwiseMin (aNodeCoord);
            aMaxPt = aMaxPt.cwiseMax (aNodeCoord);
        }
    }

    if (theMinMax.IsValid())
    {
      theMinMax.CornerMin() = aMinPt;
      theMinMax.CornerMax() = aMaxPt;
    }
    else
    {
      theMinMax.Add (aMinPt);
      theMinMax.Add (aMaxPt);
    }
}

/**************************************************************************/
void VoxelClient_VisDrawer::Display(const Standard_Boolean theHighlight)
{
    if (!myData)
        return;
    if (myData->myBoolVoxels)
        DisplayVoxels(theHighlight);
    if (myData->myColorVoxels)
        DisplayVoxels(theHighlight);
    if (myData->myROctBoolVoxels)
        DisplayVoxels(theHighlight);
    if (!myData->myTriangulation.IsNull())
        DisplayTriangulation(theHighlight);
}

// Some static method to define Open GL visual attributes
// COlor
static void setColor(const Quantity_Color& color, const Standard_Boolean highlight)
{
    static Quantity_Color highlight_color(Quantity_NOC_BLUE1);
		if(highlight)
        glColor3f((GLfloat)highlight_color.Red(), (GLfloat)highlight_color.Green(), (GLfloat)highlight_color.Blue());
    else
        glColor3d(color.Red(), color.Green(), color.Blue());
}

// Type of Line
static void setTypeOfLine(const Aspect_TypeOfLine type)
{
    if(type == Aspect_TOL_SOLID)
    {
        glDisable(GL_LINE_STIPPLE);
    }
    else
    {
        glEnable(GL_LINE_STIPPLE);
        if(type == Aspect_TOL_DOT)
            glLineStipple(1, 0xCCCC);
        else if(type == Aspect_TOL_DASH)
            glLineStipple(1, 0xFFC0);
        else if(type == Aspect_TOL_DOTDASH)
            glLineStipple(1, 0xFF18);
    }
}

// Width of Line
static void setWidthOfLine(const Standard_Integer width)
{
    glLineWidth((Standard_ShortReal) width);
}

// Normal of the view
static void getNormal(gp_Dir& normal) 
{
	Standard_Real x, y, z;
	GLint viewport[4];
	GLdouble model_matrix[16], proj_matrix[16];

	glGetDoublev(GL_MODELVIEW_MATRIX,  model_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(viewport[0], viewport[1], 0., model_matrix, proj_matrix, viewport, &x, &y, &z);
	gp_Pnt p1(x, y, z);
	gluUnProject(viewport[0] + viewport[2], viewport[1], 0., model_matrix, proj_matrix, viewport, &x, &y, &z);
	gp_Pnt p2(x, y, z);
	gluUnProject(viewport[0], viewport[1] + viewport[3], 0., model_matrix, proj_matrix, viewport, &x, &y, &z);
	gp_Pnt p3(x, y, z);

	gce_MakePln mkNormal(p1, p2, p3);
	if (mkNormal.IsDone()) 
	{
		const gp_Pln& normal_plane = mkNormal.Value();
		normal = normal_plane.Axis().Direction();
	}
	else
	{
		normal = gp::DZ();
	}
}

// Normal 2 VoxelDirection converter
static VoxelDirection getVoxelDirection(const gp_Dir& viewnormal)
{
    VoxelDirection vdir;
	Standard_Real fabsviewnormalx = fabs(viewnormal.X());
	Standard_Real fabsviewnormaly = fabs(viewnormal.Y());
	Standard_Real fabsviewnormalz = fabs(viewnormal.Z());
	if (fabsviewnormalx >= fabsviewnormaly &&
		fabsviewnormalx >= fabsviewnormalz)
	{
		if (viewnormal.X() > 0)
			vdir = Xminus;
		else
			vdir = Xplus;
	}
	else if (fabsviewnormaly >= fabsviewnormalx &&
			 fabsviewnormaly >= fabsviewnormalz)
	{
		if (viewnormal.Y() > 0)
			vdir = Yminus;
		else
			vdir = Yplus;
	}
	else if (fabsviewnormalz >= fabsviewnormalx &&
			 fabsviewnormalz >= fabsviewnormaly)
	{
		if (viewnormal.Z() > 0)
			vdir = Zminus;
		else
			vdir = Zplus;
	}
    return vdir;
}

// Normal 2 VoxelDirection 3 converter
static void getVoxel3Directions(const gp_Dir& viewnormal,
                                VoxelDirection& vdir1,
                                VoxelDirection& vdir2,
                                VoxelDirection& vdir3)
{
    Standard_Boolean vdir1_set = Standard_False, vdir2_set = Standard_False, vdir3_set = Standard_False;

    // Test X minus
    Standard_Real dot = viewnormal.Dot(-gp::DX());
    if (dot >= 0.0)
    {
        if (!vdir1_set)
        {
            vdir1 = Xminus;
            vdir1_set = Standard_True;
        }
        else if (!vdir2_set)
        {
            vdir2 = Xminus;
            vdir2_set = Standard_True;
        }
        else if (!vdir3_set)
        {
            vdir3 = Xminus;
            vdir3_set = Standard_True;
        }
    }

    // Test X plus
    dot = viewnormal.Dot(gp::DX());
    if (dot >= 0.0)
    {
        if (!vdir1_set)
        {
            vdir1 = Xplus;
            vdir1_set = Standard_True;
        }
        else if (!vdir2_set)
        {
            vdir2 = Xplus;
            vdir2_set = Standard_True;
        }
        else if (!vdir3_set)
        {
            vdir3 = Xplus;
            vdir3_set = Standard_True;
        }
    }

    // Test Y minus
    dot = viewnormal.Dot(-gp::DY());
    if (dot >= 0.0)
    {
        if (!vdir1_set)
        {
            vdir1 = Yminus;
            vdir1_set = Standard_True;
        }
        else if (!vdir2_set)
        {
            vdir2 = Yminus;
            vdir2_set = Standard_True;
        }
        else if (!vdir3_set)
        {
            vdir3 = Yminus;
            vdir3_set = Standard_True;
        }
    }

    // Test Y plus
    dot = viewnormal.Dot(gp::DY());
    if (dot >= 0.0)
    {
        if (!vdir1_set)
        {
            vdir1 = Yplus;
            vdir1_set = Standard_True;
        }
        else if (!vdir2_set)
        {
            vdir2 = Yplus;
            vdir2_set = Standard_True;
        }
        else if (!vdir3_set)
        {
            vdir3 = Yplus;
            vdir3_set = Standard_True;
        }
    }

    // Test Z minus
    dot = viewnormal.Dot(-gp::DZ());
    if (dot >= 0.0)
    {
        if (!vdir1_set)
        {
            vdir1 = Zminus;
            vdir1_set = Standard_True;
        }
        else if (!vdir2_set)
        {
            vdir2 = Zminus;
            vdir2_set = Standard_True;
        }
        else if (!vdir3_set)
        {
            vdir3 = Zminus;
            vdir3_set = Standard_True;
        }
    }

    // Test Y plus
    dot = viewnormal.Dot(gp::DZ());
    if (dot >= 0.0)
    {
        if (!vdir1_set)
        {
            vdir1 = Zplus;
            vdir1_set = Standard_True;
        }
        else if (!vdir2_set)
        {
            vdir2 = Zplus;
            vdir2_set = Standard_True;
        }
        else if (!vdir3_set)
        {
            vdir3 = Zplus;
            vdir3_set = Standard_True;
        }
    }
}

static Standard_Boolean CheckSize(Voxel_DS* voxels, 
                      const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
                      const Standard_Real xmin, const Standard_Real xmax,
                      const Standard_Real ymin, const Standard_Real ymax,
                      const Standard_Real zmin, const Standard_Real zmax,
                      Standard_Real& xc, Standard_Real& yc, Standard_Real& zc)
{
    voxels->GetCenter(ix, iy, iz, xc, yc, zc);
    if (xc < xmin || xc > xmax)
        return Standard_False;
    if (yc < ymin || yc > ymax)
        return Standard_False;
    if (zc < zmin || zc > zmax)
        return Standard_False;
    return Standard_True;
}

static Standard_Boolean CheckSize(Voxel_ROctBoolDS* voxels, 
                      const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, 
                      const Standard_Integer i, const Standard_Integer j,
                      const Standard_Real xmin, const Standard_Real xmax,
                      const Standard_Real ymin, const Standard_Real ymax,
                      const Standard_Real zmin, const Standard_Real zmax,
                      Standard_Real& xc, Standard_Real& yc, Standard_Real& zc)
{
    if (j == -1)
        voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
    else
        voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
    if (xc < xmin || xc > xmax)
        return Standard_False;
    if (yc < ymin || yc > ymax)
        return Standard_False;
    if (zc < zmin || zc > zmax)
        return Standard_False;
    return Standard_True;
}

static void drawBoolPoints(const VoxelDirection vdir, const Standard_Boolean nearest,
                           Voxel_BoolDS* voxels,
                           const Standard_Real xmin, const Standard_Real xmax,
                           const Standard_Real ymin, const Standard_Real ymax,
                           const Standard_Real zmin, const Standard_Real zmax)
{
	Standard_Real xc, yc, zc;
	Standard_Integer ix = 0, nbx = voxels->GetNbX();
	Standard_Integer iy = 0, nby = voxels->GetNbY();
	Standard_Integer iz = 0, nbz = voxels->GetNbZ();

    Standard_Boolean check_size = (xmin <= DBL_MAX && xmax >= DBL_MAX &&
                       ymin <= DBL_MAX && ymax >= DBL_MAX &&
                       zmin <= DBL_MAX && zmax >= DBL_MAX);
    check_size = !check_size;

    glBegin(GL_POINTS);
    switch (vdir)
	{
		case Xminus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = 0; ix < nbx; ix++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
						Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						if (value)
						{
                            if (!check_size)
							    voxels->GetCenter(ix, iy, iz, xc, yc, zc);
							glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
						}
					}
				}
			}
			break;
		}
		case Xplus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = nbx - 1; ix >= 0; ix--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
						Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						if (value)
						{
                            if (!check_size)
    							voxels->GetCenter(ix, iy, iz, xc, yc, zc);
							glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
						}
					}
				}
			}
			break;
		}
		case Yminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = 0; iy < nby; iy++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
						Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						if (value)
						{
                            if (!check_size)
    							voxels->GetCenter(ix, iy, iz, xc, yc, zc);
							glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
						}
					}
				}
			}
			break;
		}
		case Yplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = nby - 1; iy >= 0; iy--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
						Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						if (value)
						{
                            if (!check_size)
    							voxels->GetCenter(ix, iy, iz, xc, yc, zc);
							glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
						}
					}
				}
			}
			break;
		}
		case Zminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = 0; iz < nbz; iz++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
						Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						if (value)
						{
                            if (!check_size)
    							voxels->GetCenter(ix, iy, iz, xc, yc, zc);
							glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
						}
					}
				}
			}
			break;
		}
		case Zplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = nbz - 1; iz >= 0; iz--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
						Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						if (value)
						{
                            if (!check_size)
    							voxels->GetCenter(ix, iy, iz, xc, yc, zc);
							glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
						}
					}
				}
			}
			break;
		}
	}
    glEnd();
}

static void drawROctBoolPoints(const VoxelDirection vdir, const Standard_Boolean nearest,
                               Voxel_ROctBoolDS* voxels,
                               const Standard_Real xmin, const Standard_Real xmax,
                               const Standard_Real ymin, const Standard_Real ymax,
                               const Standard_Real zmin, const Standard_Real zmax)
{
	Standard_Real xc, yc, zc;
	Standard_Integer ix = 0, nbx = voxels->GetNbX();
	Standard_Integer iy = 0, nby = voxels->GetNbY();
	Standard_Integer iz = 0, nbz = voxels->GetNbZ();
    Standard_Integer i, j;

    Standard_Boolean check_size = (xmin <= DBL_MAX && xmax >= DBL_MAX &&
                       ymin <= DBL_MAX && ymax >= DBL_MAX &&
                       zmin <= DBL_MAX && zmax >= DBL_MAX);
    check_size = !check_size;

    glBegin(GL_POINTS);
    switch (vdir)
	{
		case Xminus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = 0; ix < nbx; ix++)
					{
                        switch (voxels->Deepness(ix, iy, iz))
                        {
                            case 0:
                            {
                                if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                    continue;
						        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						        if (value)
						        {
                                    if (!check_size)
							            ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
							        glVertex3d(xc, yc, zc);
                                    if (nearest)
                                        break;
						        }
                                break;
                            }
                            case 1:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
						            Standard_Boolean value = voxels->Get(ix, iy, iz, i) == Standard_True;
						            if (value)
						            {
                                        if (!check_size)
							                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
							            glVertex3d(xc, yc, zc);
                                        if (nearest)
                                            break;
						            }
                                }
                                break;
                            }
                            case 2:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    for (j = 0; j < 8; j++)
                                    {
                                        if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                            continue;
						                Standard_Boolean value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
						                if (value)
						                {
                                            if (!check_size)
							                    voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
							                glVertex3d(xc, yc, zc);
                                            if (nearest)
                                                break;
						                }
                                    }
                                }
                                break;
                            }
                        }
					}
				}
			}
			break;
		}
		case Xplus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = nbx - 1; ix >= 0; ix--)
					{
                        switch (voxels->Deepness(ix, iy, iz))
                        {
                            case 0:
                            {
                                if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                    continue;
						        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						        if (value)
						        {
                                    if (!check_size)
							            ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
							        glVertex3d(xc, yc, zc);
                                    if (nearest)
                                        break;
						        }
                                break;
                            }
                            case 1:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
						            Standard_Boolean value = voxels->Get(ix, iy, iz, i) == Standard_True;
						            if (value)
						            {
                                        if (!check_size)
							                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
							            glVertex3d(xc, yc, zc);
                                        if (nearest)
                                            break;
						            }
                                }
                                break;
                            }
                            case 2:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    for (j = 0; j < 8; j++)
                                    {
                                        if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                            continue;
						                Standard_Boolean value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
						                if (value)
						                {
                                            if (!check_size)
							                    voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
							                glVertex3d(xc, yc, zc);
                                            if (nearest)
                                                break;
						                }
                                    }
                                }
                                break;
                            }
                        }
					}
				}
			}
			break;
		}
		case Yminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = 0; iy < nby; iy++)
					{
                        switch (voxels->Deepness(ix, iy, iz))
                        {
                            case 0:
                            {
                                if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                    continue;
						        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						        if (value)
						        {
                                    if (!check_size)
							            ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
							        glVertex3d(xc, yc, zc);
                                    if (nearest)
                                        break;
						        }
                                break;
                            }
                            case 1:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
						            Standard_Boolean value = voxels->Get(ix, iy, iz, i) == Standard_True;
						            if (value)
						            {
                                        if (!check_size)
							                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
							            glVertex3d(xc, yc, zc);
                                        if (nearest)
                                            break;
						            }
                                }
                                break;
                            }
                            case 2:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    for (j = 0; j < 8; j++)
                                    {
                                        if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                            continue;
						                Standard_Boolean value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
						                if (value)
						                {
                                            if (!check_size)
							                    voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
							                glVertex3d(xc, yc, zc);
                                            if (nearest)
                                                break;
						                }
                                    }
                                }
                                break;
                            }
                        }
					}
				}
			}
			break;
		}
		case Yplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = nby - 1; iy >= 0; iy--)
					{
                        switch (voxels->Deepness(ix, iy, iz))
                        {
                            case 0:
                            {
                                if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                    continue;
						        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						        if (value)
						        {
                                    if (!check_size)
							            ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
							        glVertex3d(xc, yc, zc);
                                    if (nearest)
                                        break;
						        }
                                break;
                            }
                            case 1:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
						            Standard_Boolean value = voxels->Get(ix, iy, iz, i) == Standard_True;
						            if (value)
						            {
                                        if (!check_size)
							                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
							            glVertex3d(xc, yc, zc);
                                        if (nearest)
                                            break;
						            }
                                }
                                break;
                            }
                            case 2:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    for (j = 0; j < 8; j++)
                                    {
                                        if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                            continue;
						                Standard_Boolean value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
						                if (value)
						                {
                                            if (!check_size)
							                    voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
							                glVertex3d(xc, yc, zc);
                                            if (nearest)
                                                break;
						                }
                                    }
                                }
                                break;
                            }
                        }
					}
				}
			}
			break;
		}
		case Zminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = 0; iz < nbz; iz++)
					{
                        switch (voxels->Deepness(ix, iy, iz))
                        {
                            case 0:
                            {
                                if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                    continue;
						        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						        if (value)
						        {
                                    if (!check_size)
							            ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
							        glVertex3d(xc, yc, zc);
                                    if (nearest)
                                        break;
						        }
                                break;
                            }
                            case 1:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
						            Standard_Boolean value = voxels->Get(ix, iy, iz, i) == Standard_True;
						            if (value)
						            {
                                        if (!check_size)
							                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
							            glVertex3d(xc, yc, zc);
                                        if (nearest)
                                            break;
						            }
                                }
                                break;
                            }
                            case 2:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    for (j = 0; j < 8; j++)
                                    {
                                        if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                            continue;
						                Standard_Boolean value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
						                if (value)
						                {
                                            if (!check_size)
							                    voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
							                glVertex3d(xc, yc, zc);
                                            if (nearest)
                                                break;
						                }
                                    }
                                }
                                break;
                            }
                        }
					}
				}
			}
			break;
		}
		case Zplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = nbz - 1; iz >= 0; iz--)
					{
                        switch (voxels->Deepness(ix, iy, iz))
                        {
                            case 0:
                            {
                                if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                    continue;
						        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
						        if (value)
						        {
                                    if (!check_size)
							            ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
							        glVertex3d(xc, yc, zc);
                                    if (nearest)
                                        break;
						        }
                                break;
                            }
                            case 1:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
						            Standard_Boolean value = voxels->Get(ix, iy, iz, i) == Standard_True;
						            if (value)
						            {
                                        if (!check_size)
							                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
							            glVertex3d(xc, yc, zc);
                                        if (nearest)
                                            break;
						            }
                                }
                                break;
                            }
                            case 2:
                            {
                                for (i = 0; i < 8; i++)
                                {
                                    for (j = 0; j < 8; j++)
                                    {
                                        if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                            continue;
						                Standard_Boolean value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
						                if (value)
						                {
                                            if (!check_size)
							                    voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
							                glVertex3d(xc, yc, zc);
                                            if (nearest)
                                                break;
						                }
                                    }
                                }
                                break;
                            }
                        }
					}
				}
			}
			break;
		}
	}
    glEnd();
}



static void drawColorPoints(const VoxelDirection vdir, const Standard_Boolean nearest,
                            Voxel_ColorDS* voxels, const Handle(Quantity_HArray1OfColor)& hcolors,
                            const Standard_Byte minvalue, const Standard_Byte maxvalue,
                            const Standard_Real xmin, const Standard_Real xmax,
                            const Standard_Real ymin, const Standard_Real ymax,
                            const Standard_Real zmin, const Standard_Real zmax)
{
  	Standard_Real xc, yc, zc;
	Standard_Integer ix = 0, nbx = voxels->GetNbX();
	Standard_Integer iy = 0, nby = voxels->GetNbY();
	Standard_Integer iz = 0, nbz = voxels->GetNbZ();
    Standard_Byte value;

    // Colors
    const Quantity_Array1OfColor& colors = hcolors->Array1();

    Standard_Boolean check_size = (xmin <= DBL_MAX && xmax >= DBL_MAX &&
                       ymin <= DBL_MAX && ymax >= DBL_MAX &&
                       zmin <= DBL_MAX && zmax >= DBL_MAX);
    check_size = !check_size;

    glBegin(GL_POINTS);
    switch (vdir)
	{
		case Xminus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = 0; ix < nbx; ix++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
	                    value = voxels->Get(ix, iy, iz);
	                    if (value >= minvalue && value <= maxvalue)
	                    {
                            if (!check_size)
		                        voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            setColor(colors.Value(value), Standard_False);
		                    glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Xplus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = nbx - 1; ix >= 0; ix--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
	                    value = voxels->Get(ix, iy, iz);
	                    if (value >= minvalue && value <= maxvalue)
	                    {
                            if (!check_size)
		                        voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            setColor(colors.Value(value), Standard_False);
		                    glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Yminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = 0; iy < nby; iy++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
	                    value = voxels->Get(ix, iy, iz);
	                    if (value >= minvalue && value <= maxvalue)
	                    {
                            if (!check_size)
		                        voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            setColor(colors.Value(value), Standard_False);
		                    glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Yplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = nby - 1; iy >= 0; iy--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
	                    value = voxels->Get(ix, iy, iz);
	                    if (value >= minvalue && value <= maxvalue)
	                    {
                            if (!check_size)
		                        voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            setColor(colors.Value(value), Standard_False);
		                    glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Zminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = 0; iz < nbz; iz++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
	                    value = voxels->Get(ix, iy, iz);
	                    if (value >= minvalue && value <= maxvalue)
	                    {
                            if (!check_size)
		                        voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            setColor(colors.Value(value), Standard_False);
		                    glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Zplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = nbz - 1; iz >= 0; iz--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
	                    value = voxels->Get(ix, iy, iz);
	                    if (value >= minvalue && value <= maxvalue)
	                    {
                            if (!check_size)
		                        voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            setColor(colors.Value(value), Standard_False);
		                    glVertex3d(xc, yc, zc);
                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
    }
    glEnd();
}

static void drawBoolQuadrangles(Voxel_BoolDS* voxels, const VoxelDirection vdir, 
                                const gp_Dir& viewnormal, const Standard_Boolean nearest,
                                const gp_Pnt& p1, const gp_Pnt& p2, const gp_Pnt& p3, const gp_Pnt& p4,
                                const Standard_Real xmin, const Standard_Real xmax,
                                const Standard_Real ymin, const Standard_Real ymax,
                                const Standard_Real zmin, const Standard_Real zmax)
{
    gp_Vec vc;
    gp_Pnt pc1, pc2, pc3, pc4;
    Standard_Real xc, yc, zc, xn = 0.0, yn = 0.0, zn = 1.0;
    Standard_Integer ix, iy, iz, nbx = voxels->GetNbX(), nby = voxels->GetNbY(), nbz = voxels->GetNbZ();

    // Normal
    viewnormal.Coord(xn, yn, zn);

    Standard_Boolean check_size = (xmin <= DBL_MAX && xmax >= DBL_MAX &&
                       ymin <= DBL_MAX && ymax >= DBL_MAX &&
                       zmin <= DBL_MAX && zmax >= DBL_MAX);
    check_size = !check_size;

    glBegin(GL_QUADS);
    switch (vdir)
	{
		case Xminus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = 0; ix < nbx; ix++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
                        if (value)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Display
                            glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
                    }
				}
			}
			break;
		}
		case Xplus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = nbx - 1; ix >= 0; ix--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
                        if (value)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Display
                            glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Yminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = 0; iy < nby; iy++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
                        if (value)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Display
                            glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Yplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = nby - 1; iy >= 0; iy--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
                        if (value)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Display
                            glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Zminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = 0; iz < nbz; iz++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
                        if (value)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Display
                            glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Zplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = nbz - 1; iz >= 0; iz--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        Standard_Boolean value = voxels->Get(ix, iy, iz) == Standard_True;
                        if (value)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Display
                            glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
    }
    glEnd();
}

static void drawROctBoolQuadrangles(Voxel_ROctBoolDS* voxels, const VoxelDirection vdir, 
                                    const gp_Dir& viewnormal, const Standard_Boolean nearest,
                                    const gp_Pnt& p1, const gp_Pnt& p2, const gp_Pnt& p3, const gp_Pnt& p4,
                                    const Standard_Real xmin, const Standard_Real xmax,
                                    const Standard_Real ymin, const Standard_Real ymax,
                                    const Standard_Real zmin, const Standard_Real zmax)
{
    gp_Vec vc;
    gp_Pnt pc1, pc2, pc3, pc4;
    Standard_Real xc, yc, zc, xn = 0.0, yn = 0.0, zn = 1.0;
    Standard_Integer ix, iy, iz, nbx = voxels->GetNbX(), nby = voxels->GetNbY(), nbz = voxels->GetNbZ(), i, j, deepness;

    // Normal
    viewnormal.Coord(xn, yn, zn);

    Standard_Boolean check_size = (xmin <= DBL_MAX && xmax >= DBL_MAX &&
                       ymin <= DBL_MAX && ymax >= DBL_MAX &&
                       zmin <= DBL_MAX && zmax >= DBL_MAX);
    check_size = !check_size;

    glBegin(GL_QUADS);
    switch (vdir)
	{
		case Xminus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = 0; ix < nbx; ix++)
					{
                        deepness = voxels->Deepness(ix, iy, iz);
                        for (i = 0; i < 8; i++)
                        {
                            for (j = 0; j < 8; j++)
                            {
                                if (deepness == 0 && j)
                                {
                                    i = 8;
                                    break;
                                }
                                if (deepness == 1 && j)
                                    break;
                                if (deepness == 0)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 1)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 2)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }

                                Standard_Boolean value;
                                switch (deepness)
                                {
                                    case 0:
                                        value = voxels->Get(ix, iy, iz) == Standard_True;
                                        break;
                                    case 1:
                                        value = voxels->Get(ix, iy, iz, i) == Standard_True;
                                        break;
                                    case 2:
                                        value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
                                        break;
                                }
                                
                                if (value)
                                {
                                    // Define translation vector
                                    if (!check_size)
                                    {
                                        switch (deepness)
                                        {
                                            case 0:
                                                ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
                                                break;
                                            case 1:
                                                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
                                                break;
                                            case 2:
                                                voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
                                                break;
                                        }
                                    }
                                    vc.SetCoord(xc, yc, zc);

                                    // Translate
                                    pc1 = p1.Translated(vc);
                                    pc2 = p2.Translated(vc);
                                    pc3 = p3.Translated(vc);
                                    pc4 = p4.Translated(vc);

                                    // Display
                                    glNormal3d(xn, yn, zn);
                                    pc1.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc2.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc3.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc4.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);

                                    if (nearest)
                                        break;
                                }
                            }
                        }
                    }
				}
			}
			break;
		}
		case Xplus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = nbx - 1; ix >= 0; ix--)
					{
                        deepness = voxels->Deepness(ix, iy, iz);
                        for (i = 0; i < 8; i++)
                        {
                            for (j = 0; j < 8; j++)
                            {
                                if (deepness == 0 && j)
                                {
                                    i = 8;
                                    break;
                                }
                                if (deepness == 1 && j)
                                    break;
                                if (deepness == 0)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 1)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 2)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }

                                Standard_Boolean value;
                                switch (deepness)
                                {
                                    case 0:
                                        value = voxels->Get(ix, iy, iz) == Standard_True;
                                        break;
                                    case 1:
                                        value = voxels->Get(ix, iy, iz, i) == Standard_True;
                                        break;
                                    case 2:
                                        value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
                                        break;
                                }
                                
                                if (value)
                                {
                                    // Define translation vector
                                    if (!check_size)
                                    {
                                        switch (deepness)
                                        {
                                            case 0:
                                                ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
                                                break;
                                            case 1:
                                                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
                                                break;
                                            case 2:
                                                voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
                                                break;
                                        }
                                    }
                                    vc.SetCoord(xc, yc, zc);

                                    // Translate
                                    pc1 = p1.Translated(vc);
                                    pc2 = p2.Translated(vc);
                                    pc3 = p3.Translated(vc);
                                    pc4 = p4.Translated(vc);

                                    // Display
                                    glNormal3d(xn, yn, zn);
                                    pc1.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc2.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc3.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc4.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);

                                    if (nearest)
                                        break;
                                }
                            }
                        }
					}
				}
			}
			break;
		}
		case Yminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = 0; iy < nby; iy++)
					{
                        deepness = voxels->Deepness(ix, iy, iz);
                        for (i = 0; i < 8; i++)
                        {
                            for (j = 0; j < 8; j++)
                            {
                                if (deepness == 0 && j)
                                {
                                    i = 8;
                                    break;
                                }
                                if (deepness == 1 && j)
                                    break;
                                if (deepness == 0)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 1)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 2)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }

                                Standard_Boolean value;
                                switch (deepness)
                                {
                                    case 0:
                                        value = voxels->Get(ix, iy, iz) == Standard_True;
                                        break;
                                    case 1:
                                        value = voxels->Get(ix, iy, iz, i) == Standard_True;
                                        break;
                                    case 2:
                                        value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
                                        break;
                                }
                                
                                if (value)
                                {
                                    // Define translation vector
                                    if (!check_size)
                                    {
                                        switch (deepness)
                                        {
                                            case 0:
                                                ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
                                                break;
                                            case 1:
                                                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
                                                break;
                                            case 2:
                                                voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
                                                break;
                                        }
                                    }
                                    vc.SetCoord(xc, yc, zc);

                                    // Translate
                                    pc1 = p1.Translated(vc);
                                    pc2 = p2.Translated(vc);
                                    pc3 = p3.Translated(vc);
                                    pc4 = p4.Translated(vc);

                                    // Display
                                    glNormal3d(xn, yn, zn);
                                    pc1.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc2.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc3.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc4.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);

                                    if (nearest)
                                        break;
                                }
                            }
                        }
					}
				}
			}
			break;
		}
		case Yplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = nby - 1; iy >= 0; iy--)
					{
                        deepness = voxels->Deepness(ix, iy, iz);
                        for (i = 0; i < 8; i++)
                        {
                            for (j = 0; j < 8; j++)
                            {
                                if (deepness == 0 && j)
                                {
                                    i = 8;
                                    break;
                                }
                                if (deepness == 1 && j)
                                    break;
                                if (deepness == 0)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 1)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 2)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }

                                Standard_Boolean value;
                                switch (deepness)
                                {
                                    case 0:
                                        value = voxels->Get(ix, iy, iz) == Standard_True;
                                        break;
                                    case 1:
                                        value = voxels->Get(ix, iy, iz, i) == Standard_True;
                                        break;
                                    case 2:
                                        value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
                                        break;
                                }
                                
                                if (value)
                                {
                                    // Define translation vector
                                    if (!check_size)
                                    {
                                        switch (deepness)
                                        {
                                            case 0:
                                                ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
                                                break;
                                            case 1:
                                                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
                                                break;
                                            case 2:
                                                voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
                                                break;
                                        }
                                    }
                                    vc.SetCoord(xc, yc, zc);

                                    // Translate
                                    pc1 = p1.Translated(vc);
                                    pc2 = p2.Translated(vc);
                                    pc3 = p3.Translated(vc);
                                    pc4 = p4.Translated(vc);

                                    // Display
                                    glNormal3d(xn, yn, zn);
                                    pc1.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc2.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc3.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc4.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);

                                    if (nearest)
                                        break;
                                }
                            }
                        }
					}
				}
			}
			break;
		}
		case Zminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = 0; iz < nbz; iz++)
					{
                        deepness = voxels->Deepness(ix, iy, iz);
                        for (i = 0; i < 8; i++)
                        {
                            for (j = 0; j < 8; j++)
                            {
                                if (deepness == 0 && j)
                                {
                                    i = 8;
                                    break;
                                }
                                if (deepness == 1 && j)
                                    break;
                                if (deepness == 0)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 1)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 2)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }

                                Standard_Boolean value;
                                switch (deepness)
                                {
                                    case 0:
                                        value = voxels->Get(ix, iy, iz) == Standard_True;
                                        break;
                                    case 1:
                                        value = voxels->Get(ix, iy, iz, i) == Standard_True;
                                        break;
                                    case 2:
                                        value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
                                        break;
                                }
                                
                                if (value)
                                {
                                    // Define translation vector
                                    if (!check_size)
                                    {
                                        switch (deepness)
                                        {
                                            case 0:
                                                ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
                                                break;
                                            case 1:
                                                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
                                                break;
                                            case 2:
                                                voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
                                                break;
                                        }
                                    }
                                    vc.SetCoord(xc, yc, zc);

                                    // Translate
                                    pc1 = p1.Translated(vc);
                                    pc2 = p2.Translated(vc);
                                    pc3 = p3.Translated(vc);
                                    pc4 = p4.Translated(vc);

                                    // Display
                                    glNormal3d(xn, yn, zn);
                                    pc1.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc2.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc3.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc4.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);

                                    if (nearest)
                                        break;
                                }
                            }
                        }
					}
				}
			}
			break;
		}
		case Zplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = nbz - 1; iz >= 0; iz--)
					{
                        deepness = voxels->Deepness(ix, iy, iz);
                        for (i = 0; i < 8; i++)
                        {
                            for (j = 0; j < 8; j++)
                            {
                                if (deepness == 0 && j)
                                {
                                    i = 8;
                                    break;
                                }
                                if (deepness == 1 && j)
                                    break;
                                if (deepness == 0)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 1)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, -1, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }
                                else if (deepness == 2)
                                {
                                    if (check_size && !CheckSize(voxels, ix, iy, iz, i, j, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                                        continue;
                                }

                                Standard_Boolean value;
                                switch (deepness)
                                {
                                    case 0:
                                        value = voxels->Get(ix, iy, iz) == Standard_True;
                                        break;
                                    case 1:
                                        value = voxels->Get(ix, iy, iz, i) == Standard_True;
                                        break;
                                    case 2:
                                        value = voxels->Get(ix, iy, iz, i, j) == Standard_True;
                                        break;
                                }
                                
                                if (value)
                                {
                                    // Define translation vector
                                    if (!check_size)
                                    {
                                        switch (deepness)
                                        {
                                            case 0:
                                                ((Voxel_DS*)voxels)->GetCenter(ix, iy, iz, xc, yc, zc);
                                                break;
                                            case 1:
                                                voxels->GetCenter(ix, iy, iz, i, xc, yc, zc);
                                                break;
                                            case 2:
                                                voxels->GetCenter(ix, iy, iz, i, j, xc, yc, zc);
                                                break;
                                        }
                                    }
                                    vc.SetCoord(xc, yc, zc);

                                    // Translate
                                    pc1 = p1.Translated(vc);
                                    pc2 = p2.Translated(vc);
                                    pc3 = p3.Translated(vc);
                                    pc4 = p4.Translated(vc);

                                    // Display
                                    glNormal3d(xn, yn, zn);
                                    pc1.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc2.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc3.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);
                                    pc4.Coord(xc, yc, zc);
                                    glVertex3d(xc, yc, zc);

                                    if (nearest)
                                        break;
                                }
                            }
                        }
					}
				}
			}
			break;
		}
    }
    glEnd();
}



static void drawColorQuadrangles(Voxel_ColorDS* voxels, const VoxelDirection vdir, 
                                 const gp_Dir& viewnormal, const Standard_Boolean nearest,
                                 const Handle(Quantity_HArray1OfColor)& hcolors,
                                 const gp_Pnt& p1, const gp_Pnt& p2, const gp_Pnt& p3, const gp_Pnt& p4,
                                 const Standard_Byte minvalue, const Standard_Byte maxvalue,
                                 const Standard_Real xmin, const Standard_Real xmax,
                                 const Standard_Real ymin, const Standard_Real ymax,
                                 const Standard_Real zmin, const Standard_Real zmax)
{
    gp_Vec vc;
    gp_Pnt pc1, pc2, pc3, pc4;
    Standard_Real xc, yc, zc, xn = 0.0, yn = 0.0, zn = 0.0;
    Standard_Integer ix, iy, iz, nbx = voxels->GetNbX(), nby = voxels->GetNbY(), nbz = voxels->GetNbZ();
    Standard_Byte value;

    // Normal
    //viewnormal.Coord(xn, yn, zn);
    glNormal3d(xn, yn, zn);

    // Colors
    const Quantity_Array1OfColor& colors = hcolors->Array1();

    Standard_Boolean check_size = (xmin <= DBL_MAX && xmax >= DBL_MAX &&
                       ymin <= DBL_MAX && ymax >= DBL_MAX &&
                       zmin <= DBL_MAX && zmax >= DBL_MAX);
    check_size = !check_size;

    glBegin(GL_QUADS);
    switch (vdir)
	{
		case Xminus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = 0; ix < nbx; ix++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        value = voxels->Get(ix, iy, iz);
                        if (value >= minvalue && value <= maxvalue)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Color
                            setColor(colors.Value(value), Standard_False);

                            // Display
                            //glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
                    }
				}
			}
			break;
		}
		case Xplus:
		{
			for (iy = 0; iy < nby; iy++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (ix = nbx - 1; ix >= 0; ix--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        value = voxels->Get(ix, iy, iz);
                        if (value >= minvalue && value <= maxvalue)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Color
                            setColor(colors.Value(value), Standard_False);

                            // Display
                            //glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Yminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = 0; iy < nby; iy++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        value = voxels->Get(ix, iy, iz);
                        if (value >= minvalue && value <= maxvalue)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Color
                            setColor(colors.Value(value), Standard_False);

                            // Display
                            //glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Yplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iz = 0; iz < nbz; iz++)
				{
					for (iy = nby - 1; iy >= 0; iy--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        value = voxels->Get(ix, iy, iz);
                        if (value >= minvalue && value <= maxvalue)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Color
                            setColor(colors.Value(value), Standard_False);

                            // Display
                            //glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Zminus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = 0; iz < nbz; iz++)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        value = voxels->Get(ix, iy, iz);
                        if (value >= minvalue && value <= maxvalue)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Color
                            setColor(colors.Value(value), Standard_False);

                            // Display
                            //glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
		case Zplus:
		{
			for (ix = 0; ix < nbx; ix++)
			{
				for (iy = 0; iy < nby; iy++)
				{
					for (iz = nbz - 1; iz >= 0; iz--)
					{
                        if (check_size && !CheckSize(voxels, ix, iy, iz, xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc))
                            continue;
                        value = voxels->Get(ix, iy, iz);
                        if (value >= minvalue && value <= maxvalue)
                        {
                            // Define translation vector
                            if (!check_size)
                                voxels->GetCenter(ix, iy, iz, xc, yc, zc);
                            vc.SetCoord(xc, yc, zc);

                            // Translate
                            pc1 = p1.Translated(vc);
                            pc2 = p2.Translated(vc);
                            pc3 = p3.Translated(vc);
                            pc4 = p4.Translated(vc);

                            // Color
                            setColor(colors.Value(value), Standard_False);

                            // Display
                            //glNormal3d(xn, yn, zn);
                            pc1.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc2.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc3.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);
                            pc4.Coord(xc, yc, zc);
                            glVertex3d(xc, yc, zc);

                            if (nearest)
                                break;
                        }
					}
				}
			}
			break;
		}
    }
    glEnd();
}

static void genListIndex(GLint& index)
{
    GLint i = 0;
    while (++i <= INT_MAX)
    {
        if (!glIsList(i))
        {
            index = i;
            break;
        }
    }
}

static void setPlaneNormal(const VoxelDirection& dir, 
                           const Standard_Real dx, const Standard_Real dy, const Standard_Real dz,
                           gp_Pln& plane, gp_Pnt& p1, gp_Pnt& p2, gp_Pnt& p3, gp_Pnt& p4)
{
    gp_Ax3 axes = plane.Position();
    Standard_Real dx2 = 0.5 * dx, dy2 = 0.5 * dy, dz2 = 0.5 * dz;
    switch (dir)
    {
        case Xminus:
            p1.SetCoord(-dx2, -dy2, dz2);
            p2.SetCoord(-dx2, -dy2, -dz2);
            p3.SetCoord(-dx2, dy2, -dz2);
            p4.SetCoord(-dx2, dy2, dz2);
            axes.SetDirection(-gp::DX());
            break;
        case Xplus:
            p1.SetCoord(dx2, -dy2, dz2);
            p2.SetCoord(dx2, -dy2, -dz2);
            p3.SetCoord(dx2, dy2, -dz2);
            p4.SetCoord(dx2, dy2, dz2);
            axes.SetDirection(gp::DX());
            break;
        case Yminus:
            p1.SetCoord(dx2, -dy2, dz2);
            p2.SetCoord(dx2, -dy2, -dz2);
            p3.SetCoord(-dx2, -dy2, -dz2);
            p4.SetCoord(-dx2, -dy2, dz2);
            axes.SetDirection(-gp::DY());
            break;
        case Yplus:
            p1.SetCoord(dx2, dy2, dz2);
            p2.SetCoord(dx2, dy2, -dz2);
            p3.SetCoord(-dx2, dy2, -dz2);
            p4.SetCoord(-dx2, dy2, dz2);
            axes.SetDirection(gp::DY());
            break;
        case Zminus:
            p1.SetCoord(dx2, dy2, -dz2);
            p2.SetCoord(-dx2, dy2, -dz2);
            p3.SetCoord(-dx2, -dy2, -dz2);
            p4.SetCoord(dx2, -dy2, -dz2);
            axes.SetDirection(-gp::DZ());
            break;
        case Zplus:
            p1.SetCoord(dx2, dy2, dz2);
            p2.SetCoord(-dx2, dy2, dz2);
            p3.SetCoord(-dx2, -dy2, dz2);
            p4.SetCoord(dx2, -dy2, dz2);
            axes.SetDirection(gp::DZ());
            break;
    }
}

/**************************************************************************/
void VoxelClient_VisDrawer::DisplayVoxels(const Standard_Boolean theHighlight)
{
    if(!myData)
        return;

    glEnable(GL_DEPTH_TEST);

	// Boolean voxels
	if (myData->myBoolVoxels)
    {
		// Points
		if (myData->myDisplay.myDisplayMode == Voxel_VDM_POINTS ||
            myData->myDisplay.myDisplayMode == Voxel_VDM_NEARESTPOINTS)
		{
			glDisable(GL_LIGHTING);
            if (myData->myDisplay.mySmoothPoints)
			    glEnable(GL_POINT_SMOOTH);
            else
                glDisable(GL_POINT_SMOOTH);

			// Draw the points of voxels (center points of the voxels)
			// starting visualization from the side looking out from the user.
			setColor(myData->myDisplay.myColor, theHighlight);
			glPointSize((Standard_ShortReal) myData->myDisplay.myPointSize);

            // Display
            DisplayPoints(myData->myDisplay.myDisplayMode == Voxel_VDM_NEARESTPOINTS);
        }
	}

    // Color values
	if (myData->myColorVoxels)
    {
		// Points
		if (myData->myDisplay.myDisplayMode == Voxel_VDM_POINTS ||
            myData->myDisplay.myDisplayMode == Voxel_VDM_NEARESTPOINTS)
		{
			glDisable(GL_LIGHTING);
            if (myData->myDisplay.mySmoothPoints)
			    glEnable(GL_POINT_SMOOTH);
            else
                glDisable(GL_POINT_SMOOTH);

			// Draw the points of voxels (center points of the voxels)
			// starting visualization from the side looking out from the user.
			glPointSize((Standard_ShortReal) myData->myDisplay.myPointSize);

            // Display
            DisplayPoints(myData->myDisplay.myDisplayMode == Voxel_VDM_NEARESTPOINTS);
        }
	}

	// Recursive Octree Boolean voxels
	if (myData->myROctBoolVoxels)
    {
		// Points
		if (myData->myDisplay.myDisplayMode == Voxel_VDM_POINTS ||
            myData->myDisplay.myDisplayMode == Voxel_VDM_NEARESTPOINTS)
		{
			glDisable(GL_LIGHTING);
            if (myData->myDisplay.mySmoothPoints)
			    glEnable(GL_POINT_SMOOTH);
            else
                glDisable(GL_POINT_SMOOTH);

			// Draw the points of voxels (center points of the voxels)
			// starting visualization from the side looking out from the user.
			setColor(myData->myDisplay.myColor, theHighlight);
			glPointSize((Standard_ShortReal) myData->myDisplay.myPointSize);

            // Display
            DisplayPoints(myData->myDisplay.myDisplayMode == Voxel_VDM_NEARESTPOINTS);
        }
	}

    // Shading drawn by boxes
    if (myData->myDisplay.myDisplayMode == Voxel_VDM_BOXES || 
        myData->myDisplay.myDisplayMode == Voxel_VDM_NEARESTBOXES)
    {
		glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

		// Draw quadrangles of voxels looking to the user.
		setColor(myData->myDisplay.myColor, theHighlight);

        // Display
        DisplayBoxes(myData->myDisplay.myDisplayMode == Voxel_VDM_NEARESTBOXES);

        glDisable(GL_COLOR_MATERIAL);
    }

    // Highlighted voxel
    HighlightVoxel();
}

/**************************************************************************/
void VoxelClient_VisDrawer::DisplayPoints(const Standard_Boolean nearest)
{
    //OSD_Timer timer;
    //timer.Start();

    // Find the side of the cube which normal looks to (or out) the user's eye.
	gp_Dir viewnormal;
	getNormal(viewnormal);

    // Range of displayed data
    Standard_Real xmin = myData->myDisplay.myDisplayedXMin;
    Standard_Real xmax = myData->myDisplay.myDisplayedXMax;
    Standard_Real ymin = myData->myDisplay.myDisplayedYMin;
    Standard_Real ymax = myData->myDisplay.myDisplayedYMax;
    Standard_Real zmin = myData->myDisplay.myDisplayedZMin;
    Standard_Real zmax = myData->myDisplay.myDisplayedZMax;

    // Boolean points
	if (myData->myBoolVoxels)
    {
        if (nearest || myData->myDisplay.myDegenerateMode)
        {
	        VoxelDirection vdir1, vdir2, vdir3;
            getVoxel3Directions(viewnormal, vdir1, vdir2, vdir3);

            if (myData->myDisplay.myUsageOfGLlists)
            {
                // Clean all allocated GL lists for the case of first call.
                if (myData->myDisplay.myBoolNearestPointsFirst)
                {
                    for (Standard_Integer idir = Xminus; idir <= Zplus; idir++)
                    {
                        if (myData->myDisplay.myBoolNearestPointsList[idir] > 0)
                        {
                            glDeleteLists(myData->myDisplay.myBoolNearestPointsList[idir], 1);
                            myData->myDisplay.myBoolNearestPointsList[idir] = -1;
                        }
                    }
                    myData->myDisplay.myBoolNearestPointsFirst = Standard_False;
                }

                // Generate GL lists if needed.
                if (myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir1] < 0)
                {
                    genListIndex(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir1]);
                    glNewList(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir1], GL_COMPILE);
                    drawBoolPoints(vdir1, Standard_True, myData->myBoolVoxels,
                                   xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
                if (myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir2] < 0)
                {
                    genListIndex(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir2]);
                    glNewList(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir2], GL_COMPILE);
                    drawBoolPoints(vdir2, Standard_True, myData->myBoolVoxels,
                                   xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
                if (myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir3] < 0)
                {
                    genListIndex(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir3]);
                    glNewList(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir3], GL_COMPILE);
                    drawBoolPoints(vdir3, Standard_True, myData->myBoolVoxels,
                                   xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
                glCallList(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir1]);
                glCallList(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir2]);
                glCallList(myData->myDisplay.myBoolNearestPointsList[(Standard_Integer) vdir3]);
            }
            else
            {
                drawBoolPoints(vdir1, Standard_True, myData->myBoolVoxels,
                               xmin, xmax, ymin, ymax, zmin, zmax);
                drawBoolPoints(vdir2, Standard_True, myData->myBoolVoxels,
                               xmin, xmax, ymin, ymax, zmin, zmax);
                drawBoolPoints(vdir3, Standard_True, myData->myBoolVoxels,
                               xmin, xmax, ymin, ymax, zmin, zmax);
            }
        }
        else
        {
            if (myData->myDisplay.myUsageOfGLlists)
            {
                if (myData->myDisplay.myBoolPointsFirst)
                {
                    // Delete previous GL list.
                    if (myData->myDisplay.myBoolPointsList > 0)
                    {
                        glDeleteLists(myData->myDisplay.myBoolPointsList, 1);
                        myData->myDisplay.myBoolPointsList = -1;
                    }

                    // Generate a new GL list
                    genListIndex(myData->myDisplay.myBoolPointsList);
                    glNewList(myData->myDisplay.myBoolPointsList, GL_COMPILE);
                    VoxelDirection vdir = getVoxelDirection(viewnormal);
                    drawBoolPoints(vdir, Standard_False, myData->myBoolVoxels,
                                   xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();

                    // The first call has just been passed...
                    myData->myDisplay.myBoolPointsFirst = Standard_False;
                }
                glCallList(myData->myDisplay.myBoolPointsList);
            }
            else
            {
                VoxelDirection vdir = getVoxelDirection(viewnormal);
                drawBoolPoints(vdir, Standard_False, myData->myBoolVoxels,
                               xmin, xmax, ymin, ymax, zmin, zmax);
            }
        }
	}

    // Color points
	if (myData->myColorVoxels)
    {
        if (nearest || myData->myDisplay.myDegenerateMode)
        {
	        VoxelDirection vdir1, vdir2, vdir3;
            getVoxel3Directions(viewnormal, vdir1, vdir2, vdir3);

            if (myData->myDisplay.myUsageOfGLlists)
            {
                // Clean all allocated GL lists for the case of first call.
                if (myData->myDisplay.myColorNearestPointsFirst)
                {
                    for (Standard_Integer idir = Xminus; idir <= Zplus; idir++)
                    {
                        if (myData->myDisplay.myColorNearestPointsList[idir] > 0)
                        {
                            glDeleteLists(myData->myDisplay.myColorNearestPointsList[idir], 1);
                            myData->myDisplay.myColorNearestPointsList[idir] = -1;
                        }
                    }
                    myData->myDisplay.myColorNearestPointsFirst = Standard_False;
                }

                // Generate GL lists if needed.
                if (myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir1] < 0)
                {
                    genListIndex(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir1]);
                    glNewList(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir1], GL_COMPILE);
                    drawColorPoints(vdir1, Standard_True, myData->myColorVoxels, myData->myDisplay.myColors,
                                    myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                                    xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
                if (myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir2] < 0)
                {
                    genListIndex(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir2]);
                    glNewList(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir2], GL_COMPILE);
                    drawColorPoints(vdir2, Standard_True, myData->myColorVoxels, myData->myDisplay.myColors,
                                    myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                                    xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
                if (myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir3] < 0)
                {
                    genListIndex(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir3]);
                    glNewList(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir3], GL_COMPILE);
                    drawColorPoints(vdir3, Standard_True, myData->myColorVoxels, myData->myDisplay.myColors,
                                    myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                                    xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
            
                glCallList(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir1]);
                glCallList(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir2]);
                glCallList(myData->myDisplay.myColorNearestPointsList[(Standard_Integer) vdir3]);
            }
            else
            {
                drawColorPoints(vdir1, Standard_True, myData->myColorVoxels, myData->myDisplay.myColors,
                                myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                                xmin, xmax, ymin, ymax, zmin, zmax);
                drawColorPoints(vdir2, Standard_True, myData->myColorVoxels, myData->myDisplay.myColors,
                                myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                                xmin, xmax, ymin, ymax, zmin, zmax);
                drawColorPoints(vdir3, Standard_True, myData->myColorVoxels, myData->myDisplay.myColors,
                                myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                                xmin, xmax, ymin, ymax, zmin, zmax);
            }
        }
        else
        {
            if (myData->myDisplay.myUsageOfGLlists)
            {
                if (myData->myDisplay.myColorPointsFirst)
                {
                    // Delete previous GL list.
                    if (myData->myDisplay.myColorPointsList > 0)
                    {
                        glDeleteLists(myData->myDisplay.myColorPointsList, 1);
                        myData->myDisplay.myColorPointsList = -1;
                    }

                    // Generate a new GL list
                    genListIndex(myData->myDisplay.myColorPointsList);
                    glNewList(myData->myDisplay.myColorPointsList, GL_COMPILE);
                    VoxelDirection vdir = getVoxelDirection(viewnormal);
                    drawColorPoints(vdir, Standard_False, myData->myColorVoxels, myData->myDisplay.myColors,
                                    myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                                    xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();

                    // The first call has just been passed...
                    myData->myDisplay.myColorPointsFirst = Standard_False;
                }
                glCallList(myData->myDisplay.myColorPointsList);
            }
            else
            {
                VoxelDirection vdir = getVoxelDirection(viewnormal);
                drawColorPoints(vdir, Standard_False, myData->myColorVoxels, myData->myDisplay.myColors,
                                myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                                xmin, xmax, ymin, ymax, zmin, zmax);
            }
        }
	}

    // Recursive Octree Boolean points
	if (myData->myROctBoolVoxels)
    {
        if (nearest || myData->myDisplay.myDegenerateMode)
        {
	        VoxelDirection vdir1, vdir2, vdir3;
            getVoxel3Directions(viewnormal, vdir1, vdir2, vdir3);

            if (myData->myDisplay.myUsageOfGLlists)
            {
                // Clean all allocated GL lists for the case of first call.
                if (myData->myDisplay.myROctBoolNearestPointsFirst)
                {
                    for (Standard_Integer idir = Xminus; idir <= Zplus; idir++)
                    {
                        if (myData->myDisplay.myROctBoolNearestPointsList[idir] > 0)
                        {
                            glDeleteLists(myData->myDisplay.myROctBoolNearestPointsList[idir], 1);
                            myData->myDisplay.myROctBoolNearestPointsList[idir] = -1;
                        }
                    }
                    myData->myDisplay.myROctBoolNearestPointsFirst = Standard_False;
                }

                // Generate GL lists if needed.
                if (myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir1] < 0)
                {
                    genListIndex(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir1]);
                    glNewList(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir1], GL_COMPILE);
                    drawROctBoolPoints(vdir1, Standard_True, myData->myROctBoolVoxels,
                                       xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
                if (myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir2] < 0)
                {
                    genListIndex(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir2]);
                    glNewList(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir2], GL_COMPILE);
                    drawROctBoolPoints(vdir2, Standard_True, myData->myROctBoolVoxels,
                                       xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
                if (myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir3] < 0)
                {
                    genListIndex(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir3]);
                    glNewList(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir3], GL_COMPILE);
                    drawROctBoolPoints(vdir3, Standard_True, myData->myROctBoolVoxels,
                                       xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();
                }
                glCallList(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir1]);
                glCallList(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir2]);
                glCallList(myData->myDisplay.myROctBoolNearestPointsList[(Standard_Integer) vdir3]);
            }
            else
            {
                drawROctBoolPoints(vdir1, Standard_True, myData->myROctBoolVoxels,
                                   xmin, xmax, ymin, ymax, zmin, zmax);
                drawROctBoolPoints(vdir2, Standard_True, myData->myROctBoolVoxels,
                                   xmin, xmax, ymin, ymax, zmin, zmax);
                drawROctBoolPoints(vdir3, Standard_True, myData->myROctBoolVoxels,
                                   xmin, xmax, ymin, ymax, zmin, zmax);
            }
        }
        else
        {
            if (myData->myDisplay.myUsageOfGLlists)
            {
                if (myData->myDisplay.myROctBoolPointsFirst)
                {
                    // Delete previous GL list.
                    if (myData->myDisplay.myROctBoolPointsList > 0)
                    {
                        glDeleteLists(myData->myDisplay.myROctBoolPointsList, 1);
                        myData->myDisplay.myROctBoolPointsList = -1;
                    }

                    // Generate a new GL list
                    genListIndex(myData->myDisplay.myROctBoolPointsList);
                    glNewList(myData->myDisplay.myROctBoolPointsList, GL_COMPILE);
                    VoxelDirection vdir = getVoxelDirection(viewnormal);
                    drawROctBoolPoints(vdir, Standard_False, myData->myROctBoolVoxels,
                                       xmin, xmax, ymin, ymax, zmin, zmax);
                    glEndList();

                    // The first call has just been passed...
                    myData->myDisplay.myROctBoolPointsFirst = Standard_False;
                }
                glCallList(myData->myDisplay.myROctBoolPointsList);
            }
            else
            {
                VoxelDirection vdir = getVoxelDirection(viewnormal);
                drawROctBoolPoints(vdir, Standard_False, myData->myROctBoolVoxels,
                                   xmin, xmax, ymin, ymax, zmin, zmax);
            }
        }
	}

    //timer.Stop();
	//Standard_Real seconds, cpu;
	//Standard_Integer minutes, hours;
	//timer.Show(seconds, minutes, hours, cpu);
    //cout<<"DisplayPoints()"<<" took "<<minutes<<" minutes, "<<seconds<<" seconds"<<endl;
}

/**************************************************************************/
void VoxelClient_VisDrawer::DisplayBoxes(const Standard_Boolean nearest)
{
    // Range of displayed data
    Standard_Real xmin = myData->myDisplay.myDisplayedXMin;
    Standard_Real xmax = myData->myDisplay.myDisplayedXMax;
    Standard_Real ymin = myData->myDisplay.myDisplayedYMin;
    Standard_Real ymax = myData->myDisplay.myDisplayedYMax;
    Standard_Real zmin = myData->myDisplay.myDisplayedZMin;
    Standard_Real zmax = myData->myDisplay.myDisplayedZMax;

    // Find the side of the cube which normal looks to (or out) the user's eye.
	gp_Dir viewnormal;
	getNormal(viewnormal);

    // Get three sides of the box looking to the user.
	VoxelDirection vdir1, vdir2, vdir3;
    getVoxel3Directions(viewnormal, vdir1, vdir2, vdir3);

    // Three quadrangles with normals looking to the user
    gp_Pln plane1(gp::Origin(), viewnormal);
    gp_Pln plane2(plane1), plane3(plane1);

    // Boolean boxes
	if (myData->myBoolVoxels &&
        myData->myBoolVoxels->GetNbX() &&
        myData->myBoolVoxels->GetNbY() &&
        myData->myBoolVoxels->GetNbZ())
    {
        // Compute size
        Standard_Real dx = myData->myBoolVoxels->GetXLen() / (Standard_Real) myData->myBoolVoxels->GetNbX();
        Standard_Real dy = myData->myBoolVoxels->GetYLen() / (Standard_Real) myData->myBoolVoxels->GetNbY();
        Standard_Real dz = myData->myBoolVoxels->GetZLen() / (Standard_Real) myData->myBoolVoxels->GetNbZ();
        Standard_Real d  = 0.01 * (Standard_Real) myData->myDisplay.myQuadrangleSize;
        dx *= d;
        dy *= d;
        dz *= d;

        // Translatethe quadrangles to the side of the voxel
        gp_Pnt p11, p12, p13, p14, p21, p22, p23, p24, p31, p32, p33, p34;
        setPlaneNormal(vdir1, dx, dy, dz, plane1, p11, p12, p13, p14);
        setPlaneNormal(vdir2, dx, dy, dz, plane2, p21, p22, p23, p24);
        setPlaneNormal(vdir3, dx, dy, dz, plane3, p31, p32, p33, p34);

        // Display
        Standard_Boolean skin = nearest || myData->myDisplay.myDegenerateMode;
        drawBoolQuadrangles(myData->myBoolVoxels, vdir1, plane1.Axis().Direction(), 
                            skin, p11, p12, p13, p14,
                            xmin, xmax, ymin, ymax, zmin, zmax);
        drawBoolQuadrangles(myData->myBoolVoxels, vdir2, plane2.Axis().Direction(), 
                            skin, p21, p22, p23, p24,
                            xmin, xmax, ymin, ymax, zmin, zmax);
        drawBoolQuadrangles(myData->myBoolVoxels, vdir3, plane3.Axis().Direction(), 
                            skin, p31, p32, p33, p34,
                            xmin, xmax, ymin, ymax, zmin, zmax);
    }
    // Color quadrangles
	else if (myData->myColorVoxels &&
             myData->myColorVoxels->GetNbX() &&
             myData->myColorVoxels->GetNbY() &&
             myData->myColorVoxels->GetNbZ())
    {
        // Compute size
        Standard_Real dx = myData->myColorVoxels->GetXLen() / (Standard_Real) myData->myColorVoxels->GetNbX();
        Standard_Real dy = myData->myColorVoxels->GetYLen() / (Standard_Real) myData->myColorVoxels->GetNbY();
        Standard_Real dz = myData->myColorVoxels->GetZLen() / (Standard_Real) myData->myColorVoxels->GetNbZ();
        Standard_Real d  = 0.01 * (Standard_Real) myData->myDisplay.myQuadrangleSize;
        dx *= d;
        dy *= d;
        dz *= d;

        // Translatethe quadrangles to the side of the voxel
        gp_Pnt p11, p12, p13, p14, p21, p22, p23, p24, p31, p32, p33, p34;
        setPlaneNormal(vdir1, dx, dy, dz, plane1, p11, p12, p13, p14);
        setPlaneNormal(vdir2, dx, dy, dz, plane2, p21, p22, p23, p24);
        setPlaneNormal(vdir3, dx, dy, dz, plane3, p31, p32, p33, p34);

        // Display
        Standard_Boolean skin = nearest || myData->myDisplay.myDegenerateMode;
        drawColorQuadrangles(myData->myColorVoxels, vdir1, plane1.Axis().Direction(), skin, 
                             myData->myDisplay.myColors, p11, p12, p13, p14,
                             myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                             xmin, xmax, ymin, ymax, zmin, zmax);
        drawColorQuadrangles(myData->myColorVoxels, vdir2, plane2.Axis().Direction(), skin, 
                             myData->myDisplay.myColors, p21, p22, p23, p24,
                             myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                             xmin, xmax, ymin, ymax, zmin, zmax);
        drawColorQuadrangles(myData->myColorVoxels, vdir3, plane3.Axis().Direction(), skin, 
                             myData->myDisplay.myColors, p31, p32, p33, p34,
                             myData->myDisplay.myColorMinValue, myData->myDisplay.myColorMaxValue,
                             xmin, xmax, ymin, ymax, zmin, zmax);
    }
    // Recursive Octree Boolean boxes
	else if (myData->myROctBoolVoxels &&
             myData->myROctBoolVoxels->GetNbX() &&
             myData->myROctBoolVoxels->GetNbY() &&
             myData->myROctBoolVoxels->GetNbZ())
    {
        // Compute size
        Standard_Real dx = myData->myROctBoolVoxels->GetXLen() / (Standard_Real) myData->myROctBoolVoxels->GetNbX();
        Standard_Real dy = myData->myROctBoolVoxels->GetYLen() / (Standard_Real) myData->myROctBoolVoxels->GetNbY();
        Standard_Real dz = myData->myROctBoolVoxels->GetZLen() / (Standard_Real) myData->myROctBoolVoxels->GetNbZ();
        Standard_Real d  = 0.01 * (Standard_Real) myData->myDisplay.myQuadrangleSize;
        dx *= d;
        dy *= d;
        dz *= d;

        // Translatethe quadrangles to the side of the voxel
        gp_Pnt p11, p12, p13, p14, p21, p22, p23, p24, p31, p32, p33, p34;
        setPlaneNormal(vdir1, dx, dy, dz, plane1, p11, p12, p13, p14);
        setPlaneNormal(vdir2, dx, dy, dz, plane2, p21, p22, p23, p24);
        setPlaneNormal(vdir3, dx, dy, dz, plane3, p31, p32, p33, p34);

        // Display
        Standard_Boolean skin = nearest || myData->myDisplay.myDegenerateMode;
        drawROctBoolQuadrangles(myData->myROctBoolVoxels, vdir1, plane1.Axis().Direction(), 
                                skin, p11, p12, p13, p14,
                                xmin, xmax, ymin, ymax, zmin, zmax);
        drawROctBoolQuadrangles(myData->myROctBoolVoxels, vdir2, plane2.Axis().Direction(), 
                                skin, p21, p22, p23, p24,
                                xmin, xmax, ymin, ymax, zmin, zmax);
        drawROctBoolQuadrangles(myData->myROctBoolVoxels, vdir3, plane3.Axis().Direction(), 
                                skin, p31, p32, p33, p34,
                                xmin, xmax, ymin, ymax, zmin, zmax);
    }
}

/**************************************************************************/
void VoxelClient_VisDrawer::DisplayTriangulation(const Standard_Boolean theHighlight)
{
    if(!myData || myData->myTriangulation.IsNull())
        return;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    const TColgp_Array1OfPnt& nodes = myData->myTriangulation->Nodes();
    const Poly_Array1OfTriangle& triangles = myData->myTriangulation->Triangles();
    Standard_Integer itriangle = triangles.Lower(), nb_triangles = triangles.Upper();

    Standard_Boolean compute_normals = Standard_False;
    if (myData->myNormalsOfNodes.IsNull())
    {
        compute_normals = Standard_True;
        myData->myNormalsOfNodes = new TColgp_HArray1OfDir(itriangle, nb_triangles);

        // Release the GL list
        if (myData->myDisplay.myTriangulationList > 0)
        {
            glDeleteLists(myData->myDisplay.myTriangulationList, 1);
            myData->myDisplay.myTriangulationList = -1;
        }

        // Generate a new GL list
        if (myData->myDisplay.myUsageOfGLlists)
        {
            genListIndex(myData->myDisplay.myTriangulationList);
            glNewList(myData->myDisplay.myTriangulationList, GL_COMPILE);
        }
    }
    TColgp_Array1OfDir& normals = myData->myNormalsOfNodes->ChangeArray1();

    if (!myData->myDisplay.myUsageOfGLlists || compute_normals)
    {

        glBegin(GL_TRIANGLES);

        Standard_Integer n1, n2, n3;
        Standard_Real x, y, z;
        for (; itriangle <= nb_triangles; itriangle++)
        {
            const Poly_Triangle& t = triangles.Value(itriangle);
            t.Get(n1, n2, n3);

            const gp_Pnt& p1 = nodes.Value(n1);
            const gp_Pnt& p2 = nodes.Value(n2);
            const gp_Pnt& p3 = nodes.Value(n3);

            // Make the normal:
            if (compute_normals)
            {
                gp_Vec v1(p1, p2), v2(p1, p3);
                v1.Cross(v2);
                if (v1.SquareMagnitude() > 1.e-14)
                    v1.Normalize();
                else
                    v1.SetCoord(0.0, 0.0, 1.0);
                normals.SetValue(itriangle, v1);
                v1.Coord(x, y, z);
            }
            else
            {
                normals.Value(itriangle).Coord(x, y, z);
            }
            glNormal3d(x, y, z);

            // P1
            p1.Coord(x, y, z);
            glVertex3d(x, y, z);

            // P2
            p2.Coord(x, y, z);
            glVertex3d(x, y, z);

            // P3
            p3.Coord(x, y, z);
            glVertex3d(x, y, z);
        }

        glEnd();

        if (myData->myDisplay.myUsageOfGLlists)
            glEndList();
    }

    if (myData->myDisplay.myUsageOfGLlists)
        glCallList(myData->myDisplay.myTriangulationList);
}

void VoxelClient_VisDrawer::HighlightVoxel()
{
    if (myData &&
        myData->myDisplay.myHighlightx >= 0 &&
        myData->myDisplay.myHighlighty >= 0 &&
        myData->myDisplay.myHighlightz >= 0)
    {
        Standard_Integer nbx, nby, nbz;
        Standard_Real xlen, ylen, zlen, xc, yc, zc;
        Voxel_DS* ds = (Voxel_DS*) myData->myBoolVoxels;
        if (myData->myColorVoxels)
            ds = (Voxel_DS*) myData->myColorVoxels;
        if (myData->myROctBoolVoxels)
            ds = (Voxel_DS*) myData->myROctBoolVoxels;
        nbx = ds->GetNbX();
        nby = ds->GetNbY();
        nbz = ds->GetNbZ();
        xlen = ds->GetXLen();
        ylen = ds->GetYLen();
        zlen = ds->GetZLen();
        ds->GetCenter(myData->myDisplay.myHighlightx, 
                      myData->myDisplay.myHighlighty, 
                      myData->myDisplay.myHighlightz, 
                      xc, yc, zc);

        Standard_Real half_voxelx = xlen / Standard_Real(nbx) / 2.0;
        Standard_Real half_voxely = ylen / Standard_Real(nby) / 2.0;
        Standard_Real half_voxelz = zlen / Standard_Real(nbz) / 2.0;
        Standard_Real x1 = xc - half_voxelx, y1 = yc - half_voxely, z1 = zc - half_voxelz;
        Standard_Real x2 = xc + half_voxelx, y2 = yc + half_voxely, z2 = zc + half_voxelz;

        setColor(Quantity_NOC_BLUE1, Standard_True);
        setTypeOfLine(Aspect_TOL_SOLID);
        setWidthOfLine(3);

        glBegin(GL_LINES);

        glVertex3d(x1, y1, z1);
        glVertex3d(x1, y2, z1);

        glVertex3d(x1, y1, z1);
        glVertex3d(x2, y1, z1);

        glVertex3d(x1, y1, z1);
        glVertex3d(x1, y1, z2);

        glVertex3d(x1, y2, z1);
        glVertex3d(x2, y2, z1);

        glVertex3d(x2, y1, z1);
        glVertex3d(x2, y2, z1);

        glVertex3d(x2, y2, z1);
        glVertex3d(x2, y2, z2);

        glVertex3d(x1, y1, z2);
        glVertex3d(x1, y2, z2);

        glVertex3d(x1, y1, z2);
        glVertex3d(x2, y1, z2);

        glVertex3d(x2, y1, z2);
        glVertex3d(x2, y2, z2);

        glVertex3d(x2, y2, z2);
        glVertex3d(x1, y2, z2);

        glVertex3d(x1, y2, z2);
        glVertex3d(x1, y2, z1);

        glVertex3d(x2, y1, z1);
        glVertex3d(x2, y1, z2);

        glEnd();
    }
}
