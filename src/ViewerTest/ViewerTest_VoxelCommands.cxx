// Created on: 2008-09-17
// Created by: Vlad Romashko
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <ViewerTest.hxx>

#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>

#include <Voxel_BoolDS.hxx>
#include <Voxel_ColorDS.hxx>
#include <Voxel_FloatDS.hxx>
#include <Voxel_OctBoolDS.hxx>
#include <Voxel_ROctBoolDS.hxx>
#include <Voxel_BooleanOperation.hxx>
#include <Voxel_FastConverter.hxx>

#include <BRepPrimAPI_MakeSphere.hxx>

static Standard_Integer VOXELBOOLDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;

  // 1. BoolDS:

  Voxel_BoolDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds.Set(ix, iy, iz, Standard_False);
	else
	  ds.Set(ix, iy, iz, Standard_True);
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Boolean value = ds.Get(ix, iy, iz) == Standard_True;
	if (ix & 0x01)
	{
	  if (value != Standard_False)
	  {
	    di<<"error : voxelboolds";
	    return 1; // error
	  }
	}
	else
        {
	  if (value != Standard_True)
	  {
	    di<<"error : voxelboolds";
	    return 1; // error
	  }
	}
      }
    }
  }

  return 0; // Sehr gut
}

static Standard_Integer VOXELCOLORDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;
  
  // 1. ColorDS:
  
  Voxel_ColorDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
  
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds.Set(ix, iy, iz, 8);
	else
	  ds.Set(ix, iy, iz, 7);
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Byte value = ds.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (value != 8)
	  {
	    di<<"error : voxelcolords";
	    return 1;
	  }
	}
	else
	{
	  if (value != 7)
	  {
	    di<<"error : voxelcolords";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELFLOATDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;

  // 1. FloatDS:

  Voxel_FloatDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds.Set(ix, iy, iz, 8.8f);
	else
	  ds.Set(ix, iy, iz, 7.7f);
      }
    }
  }

  /*
     // Write the voxels
     Voxel_Writer writer;
     writer.SetFormat(Voxel_VFF_BINARY);
     writer.SetVoxels(ds);
     if (!writer.Write("W:\\OCC621\\samples\\standard\\voxeldemo\\f.vx"))
     {
     statusBar()->message( "Storage failed... sorry", 2000 );
     return;
     }
     
     // Read the voxels
     Voxel_Reader reader;
     if (!reader.Read("W:\\OCC621\\samples\\standard\\voxeldemo\\f.vx"))
     {
     statusBar()->message( "Open failed... sorry", 2000 );
     return;
     }
  */

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	float value = ds.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (value != 8.8f)
	  {
	    di<<"error : voxelfloatds";
	    return 1;
	  }
	}
	else
	{
	  if (value != 7.7f)
	  {
	    di<<"error : voxelfloatds";
	    return 1;
	  }
	}
      }
    }
  }
  
  return 0;
}

static Standard_Integer VOXELOCTBOOLDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 30, nby = 30, nbz = 30;

  // 1. OctBoolDS:

  Voxel_OctBoolDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	{
	  ds.Set(ix, iy, iz, Standard_True);
	}
	else
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    if (i & 0x01)
	      ds.Set(ix, iy, iz, i, Standard_True);
	    else
	      ds.Set(ix, iy, iz, i, Standard_False);
	  }
	}
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
        {
	  Standard_Boolean value = ds.Get(ix, iy, iz) == Standard_True;
	  if (value != Standard_True)
	    cout<<"Wrong value!"<<endl;
	}
	else
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    if (i & 0x01)
	    {
	      Standard_Boolean value = ds.Get(ix, iy, iz, i) == Standard_True;
	      if (value != Standard_True)
	      {
		di<<"error : voxeloctboolds";
		return 1;
	      }
	    }
	    else
	    {
	      Standard_Boolean value = ds.Get(ix, iy, iz, i) == Standard_True;
	      if (value != Standard_False)
	      {
		di<<"error : voxeloctboolds";
		return 1;
	      }
	    }
	  }
	}
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    ds.Set(ix, iy, iz, i, Standard_True);
	  }
	}
	else
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    ds.Set(ix, iy, iz, i, Standard_False);
	  }
	}
      }
    }
  }

  ds.OptimizeMemory();

  return 0;
}

static Standard_Integer VOXELROCTBOOLDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz, i, j;
  Standard_Integer nbx = 30, nby = 30, nbz = 30;

  // 1. ROctBoolDS:
  
  Voxel_ROctBoolDS ds(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds.Set(ix, iy, iz, Standard_True);
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	for (i = 0; i < 8; i++)
	{
	  for (j = 0; j < 8; j++)
	  {
	    ds.Set(ix, iy, iz, i, j, Standard_True);
	  }
	}
      }
    }
  }

  ds.OptimizeMemory();

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ds.Deepness(ix, iy, iz) == 0)
	{
	  Standard_Boolean value = ds.Get(ix, iy, iz);
	  if (value != Standard_True)
	  {
	    di<<"error : voxelroctboolds";
	    return 1;
	  }
	}
	if (ds.Deepness(ix, iy, iz) == 1)
	{
	  for (i = 0; i < 8; i++)
	  {
	    Standard_Boolean value = ds.Get(ix, iy, iz, i);
	    if (value != Standard_True)
	    {
	      di<<"error : voxelroctboolds";
	      return 1;
	    }
	  }
	}
	if (ds.Deepness(ix, iy, iz) == 2)
	{
	  for (i = 0; i < 8; i++)
	  {
	    for (j = 0; j < 8; j++)
	    {
	      Standard_Boolean value = ds.Get(ix, iy, iz, i, j);
	      if (value != Standard_True)
	      {
		di<<"error : voxelroctboolds";
		return 1;
	      }
	    }
	  }
	}
      }
    }
  }

  // Test converter
  Standard_Integer progress = 0;
  TopoDS_Shape S = BRepPrimAPI_MakeSphere(100.0).Shape();
  Voxel_ROctBoolDS* ds2 = new Voxel_ROctBoolDS;
  Voxel_FastConverter converter(S, *ds2, 0.1, nbx, nby, nbz, 1);
  converter.Convert(progress);
  ds2->OptimizeMemory();
  delete ds2;

  return 0;
}

static Standard_Integer VOXELFUSEBOOLDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;

  // 1. Set two BoolDS:

  Voxel_BoolDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
  Voxel_BoolDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, Standard_False);
	else
	  ds2.Set(ix, iy, iz, Standard_True);
      }
    }
  }

  // 2. Fuse them

  Voxel_BooleanOperation fuser;
  if (!fuser.Fuse(ds1, ds2))
  {
    di<<"error : voxelfuseboolds";
    return 1;
  }

  // 3. Check result
  
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Boolean value = ds1.Get(ix, iy, iz) == Standard_True;
	if (ix & 0x01)
	{
	  if (value != Standard_False)
	  {
	    di<<"error : voxelfuseboolds";
	    return 1;
	  }
	}
	else
	{
	  if (value != Standard_True)
	  {
	    di<<"error : voxelfuseboolds";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELFUSECOLORDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;

  // 1. Set two ColorDS:

  Voxel_ColorDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
  Voxel_ColorDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
  
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, 11);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, 3);
	else
	  ds2.Set(ix, iy, iz, 5);
      }
    }
  }
  
  // 2. Fuse them
  
  Voxel_BooleanOperation fuser;
  if (!fuser.Fuse(ds1, ds2))
  {
    di<<"error : voxelfusecolords";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Byte value = ds1.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (value != 14)
	  {
	    di<<"error : voxelfusecolords";
	    return 1;
	  }
	}
	else
	{
	  if (value != 15)
	  {
	    di<<"error : voxelfusecolords";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELFUSEFLOATDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;
  
  // 1. Set two FloatDS:

  Voxel_FloatDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
  Voxel_FloatDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, 11.1f);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, 3.3f);
	else
	  ds2.Set(ix, iy, iz, 5.5f);
      }
    }
  }

  // 2. Fuse them
  
  Voxel_BooleanOperation fuser;
  if (!fuser.Fuse(ds1, ds2))
  {
    di<<"error : voxelfusefloatds";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	float value = ds1.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (fabs(value - 14.4f) > 0.001)
	  {
	    di<<"error : voxelfusefloatds";
	    return 1;
	  }
	}
	else
	{
	  if (fabs(value - 16.6f) > 0.001)
	  {
	    di<<"error : voxelfusefloatds";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELCUTBOOLDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;

  // 1. Set two BoolDS:

  Voxel_BoolDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
  Voxel_BoolDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, Standard_True);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, Standard_False);
	else
	  ds2.Set(ix, iy, iz, Standard_True);
      }
    }
  }
  
  // 2. Cut them

  Voxel_BooleanOperation cutter;
  if (!cutter.Cut(ds1, ds2))
  {
    di<<"error : voxelcutboolds";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Boolean value = ds1.Get(ix, iy, iz) == Standard_True;
	if (ix & 0x01)
	{
	  if (value != Standard_True)
	  {
	    di<<"error : voxelcutboolds";
	    return 1;
	  }
	}
	else
	{
	  if (value != Standard_False)
	  {
	    di<<"error : voxelcutboolds";
	    return 1;
	  }
	}
      }
    }
  }
  
  return 0;
}

static Standard_Integer VOXELCUTCOLORDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;

  // 1. Set two ColorDS:

  Voxel_ColorDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
  Voxel_ColorDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, 11);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, 3);
	else
	  ds2.Set(ix, iy, iz, 5);
      }
    }
  }

  // 2. Cut them
  
  Voxel_BooleanOperation cutter;
  if (!cutter.Cut(ds1, ds2))
  {
    di<<"error : voxelcutcolords";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Byte value = ds1.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (value != 8)
	  {
	    di<<"error : voxelcutcolords";
	    return 1;
	  }
	}
	else
	{
	  if (value != 6)
	  {
	    di<<"error : voxelcutcolords";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELCUTFLOATDS(Draw_Interpretor& di, Standard_Integer, const char**)
{
  Standard_Integer ix, iy, iz;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;

  // 1. Set two FloatDS:

  Voxel_FloatDS ds1(0, 0, 0, 1, 1, 1, nbx, nby, nbz);
  Voxel_FloatDS ds2(0, 0, 0, 1, 1, 1, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, 11.1f);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, 3.3f);
	else
	  ds2.Set(ix, iy, iz, 5.5f);
      }
    }
  }

  // 2. Cut them
  
  Voxel_BooleanOperation cutter;
  if (!cutter.Cut(ds1, ds2))
  {
    di<<"error : voxelcutcolords";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	float value = ds1.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (fabs(value - 7.8f) > 0.001)
	  {
	    di<<"error : voxelcutcolords";
	    return 1;
	  }
	}
	else
	{
	  if (fabs(value - 5.6f) > 0.001)
	  {
	    di<<"error : voxelcutcolords";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

void  ViewerTest::VoxelCommands(Draw_Interpretor& theCommands)
{
  const char* g = "Viewer Voxel non-regression test-commands";
  theCommands.Add("voxelboolds","voxelboolds; returns 0 if success",__FILE__,VOXELBOOLDS,g);
  theCommands.Add("voxelcolords","voxelcolords; returns 0 if success",__FILE__,VOXELCOLORDS,g);
  theCommands.Add("voxelfloatds","voxelfloatds; returns 0 if success",__FILE__,VOXELFLOATDS,g);
  theCommands.Add("voxeloctboolds","voxeloctboolds; returns 0 if success",__FILE__,VOXELOCTBOOLDS,g);
  theCommands.Add("voxelroctboolds","voxelroctboolds; returns 0 if success",__FILE__,VOXELROCTBOOLDS,g);
  theCommands.Add("voxelfuseboolds","voxelfuseboolds; returns 0 if success",__FILE__,VOXELFUSEBOOLDS,g);
  theCommands.Add("voxelfusecolords","voxelfusecolords; returns 0 if success",__FILE__,VOXELFUSECOLORDS,g);
  theCommands.Add("voxelfusefloatds","voxelfusefloatds; returns 0 if success",__FILE__,VOXELFUSEFLOATDS,g);
  theCommands.Add("voxelcutboolds","voxelcutboolds; returns 0 if success",__FILE__,VOXELCUTBOOLDS,g);
  theCommands.Add("voxelcutcolords","voxelcutcolords; returns 0 if success",__FILE__,VOXELCUTCOLORDS,g);
  theCommands.Add("voxelcutfloatds","voxelcutfloatds; returns 0 if success",__FILE__,VOXELCUTFLOATDS,g);
}
