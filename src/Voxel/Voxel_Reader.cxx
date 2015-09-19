// Created on: 2008-08-28
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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


#include <OSD_OpenFile.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Voxel_BoolDS.hxx>
#include <Voxel_ColorDS.hxx>
#include <Voxel_FloatDS.hxx>
#include <Voxel_Reader.hxx>
#include <Voxel_TypeDef.hxx>
#include <Voxel_VoxelFileFormat.hxx>

Voxel_Reader::Voxel_Reader():myBoolVoxels(0),myColorVoxels(0),myFloatVoxels(0)
{

}

Standard_Boolean Voxel_Reader::Read(const TCollection_ExtendedString& file)
{
  // Open file in ASCII mode to read header
  FILE* f = OSD_OpenFile(file, "r");
  if (!f)
    return Standard_False;

  // Read the header
  Standard_Byte type; // 0 - bool, 1 - color, 2 - float
  Voxel_VoxelFileFormat format;
  Standard_Character svoxels[9], sformat[9], stype[9];
  if (fscanf(f, "%8s %8s %8s\n", svoxels, sformat, stype) != 3)
  {
    fclose(f);
    return Standard_False;
  }
  fclose(f);

  // Take format, type of voxels.
  // Voxels
  if (strcmp(svoxels, VOXELS))
    return Standard_False;
  // Format
  if (strcmp(sformat, ASCII) == 0)
    format = Voxel_VFF_ASCII;
  else if (strcmp(sformat, BINARY) == 0)
    format = Voxel_VFF_BINARY;
  else
    return Standard_False;
  // Type of voxels
  if (strcmp(stype, BOOL) == 0)
    type = 0;
  else if (strcmp(stype, COLOR) == 0)
    type = 1;
  else if (strcmp(stype, FLOAT) == 0)
    type = 2;
  else
    return Standard_False;

  // Read the rest
  switch (format)
  {
    case Voxel_VFF_ASCII:
    {
      switch (type)
      {
	case 0:
	  return ReadBoolAsciiVoxels(file);
	case 1:
	  return ReadColorAsciiVoxels(file);
	case 2:
	  return ReadFloatAsciiVoxels(file);
      }
    }
    case Voxel_VFF_BINARY:
    {
      switch (type)
      {
	case 0:
	  return ReadBoolBinaryVoxels(file);
	case 1:
	  return ReadColorBinaryVoxels(file);
	case 2:
	  return ReadFloatBinaryVoxels(file);
      }
    }
  }

  // No voxels or no format description is found:
  return Standard_False;
}

Standard_Boolean Voxel_Reader::IsBoolVoxels() const
{
  return (myBoolVoxels != 0);
}

Standard_Boolean Voxel_Reader::IsColorVoxels() const
{
  return (myColorVoxels != 0);
}

Standard_Boolean Voxel_Reader::IsFloatVoxels() const
{
  return (myFloatVoxels != 0);
}

Standard_Address Voxel_Reader::GetBoolVoxels() const
{
  return myBoolVoxels;
}

Standard_Address Voxel_Reader::GetColorVoxels() const
{
  return myColorVoxels;
}

Standard_Address Voxel_Reader::GetFloatVoxels() const
{
  return myFloatVoxels;
}

static Standard_Boolean has_slice(const Standard_CString line)
{
  Standard_Integer i = 0, nb_spaces = 0;
  while (line[i] != '\0')
  {
    if (line[i] == ' ')
      nb_spaces++;
    i++;
  }
  return (nb_spaces == 2);
}

Standard_Boolean Voxel_Reader::ReadBoolAsciiVoxels(const TCollection_ExtendedString& file)
{
  // Open file for reading
  FILE* f = OSD_OpenFile(file, "r");
  if (!f)
    return Standard_False;
  Standard_Character line[65], sx[33], sy[33], sz[33];

  // Header: skip it
  if (fgets(line, 64, f) == NULL)
  {
    return Standard_False;
  }

  // Location, size, number of splits
  Standard_Integer nbx = 0, nby = 0, nbz = 0;
  Standard_Real x = 0.0, y = 0.0, z = 0.0, xlen = 0.0, ylen = 0.0, zlen = 0.0;
  if (fscanf(f, "%32s %32s %32s\n", sx, sy, sz) != 3)
  {
    fclose(f);
    return Standard_False;
  }
  x = Atof(sx); y = Atof(sy); z = Atof(sz);
  if (fscanf(f, "%32s %32s %32s\n", sx, sy, sz) != 3)
  {
    fclose(f);
    return Standard_False;
  }
  xlen = Atof(sx); ylen = Atof(sy); zlen = Atof(sz);
  if (fscanf(f, "%d %d %d\n", &nbx, &nby, &nbz) != 3)
  {
    fclose(f);
    return Standard_False;
  }

  // Allocate the voxels
  myBoolVoxels = (Standard_Address) new Voxel_BoolDS(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  // Data
  // Copied from Voxel_BoolDS.cxx:
  Standard_Integer nb_bytes = RealToInt(ceil(nbx * nby * nbz / 8.0));
  Standard_Integer nb_slices = RealToInt(ceil(nb_bytes / 8.0));
  // myData[0 .. nb_slices - 1][0 .. 7]
  if (nb_slices)
  {
    Standard_Integer i1 = 0, i2 = 0, value = 0;
    while (!feof(f)
         && fgets(line, 64, f) != NULL)
    {
      if (has_slice(line))
      {
	if (sscanf(line, "%d %d %d\n", &i1, &i2, &value) != 3)
        {
	  fclose(f);
	  return Standard_False;
	}
      }
      else
      {
	if (sscanf(line, "%d %d\n", &i2, &value) != 2)
        {
	  fclose(f);
	  return Standard_False;
	}
      }

      // Set value
      if (!((Standard_Byte**)((Voxel_DS*)myBoolVoxels)->myData)[i1])
      {
	((Standard_Byte**)((Voxel_DS*)myBoolVoxels)->myData)[i1] = 
	  (Standard_Byte*) calloc(8/*number of bytes in slice*/, sizeof(Standard_Byte));
      }
      (((Standard_Byte**)((Voxel_DS*)myBoolVoxels)->myData)[i1])[i2] = (Standard_Byte)value;
    }
  }

  fclose(f);
  return Standard_True;
}

Standard_Boolean Voxel_Reader::ReadColorAsciiVoxels(const TCollection_ExtendedString& file)
{
  // Open file for reading
  FILE* f = OSD_OpenFile(file, "r");
  if (!f)
    return Standard_False;
  Standard_Character line[65], sx[33], sy[33], sz[33];

  // Header: skip it
  if (fgets(line, 64, f) == NULL)
  {
    fclose(f);
    return Standard_False;
  }

  // Location, size, number of splits
  Standard_Integer nbx = 0, nby = 0, nbz = 0;
  Standard_Real x = 0.0, y = 0.0, z = 0.0, xlen = 0.0, ylen = 0.0, zlen = 0.0;
  if (fscanf(f, "%32s %32s %32s\n", sx, sy, sz) != 3)
  {
    fclose(f);
    return Standard_False;
  }
  x = Atof(sx); y = Atof(sy); z = Atof(sz);
  if (fscanf(f, "%32s %32s %32s\n", sx, sy, sz) != 3)
  {
    fclose(f);
    return Standard_False;
  }
  xlen = Atof(sx); ylen = Atof(sy); zlen = Atof(sz);
  if (fscanf(f, "%d %d %d\n", &nbx, &nby, &nbz) != 3)
  {
    fclose(f);
    return Standard_False;
  }

  // Allocate the voxels
  myColorVoxels = (Standard_Address) new Voxel_ColorDS(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  // Data
  // Copied from Voxel_ColorDS.cxx:
  Standard_Integer nb_bytes = RealToInt(ceil(nbx * nby * nbz / 2.0));
  Standard_Integer nb_slices = RealToInt(ceil(nb_bytes / 32.0));
  // myData[0 .. nb_slices - 1][0 .. 31]
  if (nb_slices)
  {
    Standard_Integer i1 = 0, i2 = 0, value = 0;
    while (!feof(f)
         && fgets(line, 64, f) != NULL)
    {
      if (has_slice(line))
      {
	if (sscanf(line, "%d %d %d\n", &i1, &i2, &value) != 3)
        {
	  fclose(f);
	  return Standard_False;
	}
      }
      else
      {
	if (sscanf(line, "%d %d\n", &i2, &value) != 2)
        {
	  fclose(f);
	  return Standard_False;
	}
      }

      // Set value
      if (!((Standard_Byte**)((Voxel_DS*)myColorVoxels)->myData)[i1])
      {
	((Standard_Byte**)((Voxel_DS*)myColorVoxels)->myData)[i1] = 
	  (Standard_Byte*) calloc(32/*number of bytes in slice*/, sizeof(Standard_Byte));
      }
      (((Standard_Byte**)((Voxel_DS*)myColorVoxels)->myData)[i1])[i2] = (Standard_Byte)value;
    }
  }

  fclose(f);
  return Standard_True;
}

Standard_Boolean Voxel_Reader::ReadFloatAsciiVoxels(const TCollection_ExtendedString& file)
{
  // Open file for reading
  FILE* f = OSD_OpenFile(file, "r");
  if (!f)
    return Standard_False;
  Standard_Character line[65], sx[33], sy[33], sz[33];

  // Header: skip it
  if (fgets(line, 64, f) == NULL)
  {
    fclose(f);
    return Standard_False;
  }

  // Location, size, number of splits
  Standard_Integer nbx = 0, nby = 0, nbz = 0;
  Standard_Real x = 0.0, y = 0.0, z = 0.0, xlen = 0.0, ylen = 0.0, zlen = 0.0;
  if (fscanf(f, "%32s %32s %32s\n", sx, sy, sz) != 3)
  {
    fclose(f);
    return Standard_False;
  }
  x = Atof(sx); y = Atof(sy); z = Atof(sz);
  if (fscanf(f, "%32s %32s %32s\n", sx, sy, sz) != 3)
  {
    fclose(f);
    return Standard_False;
  }
  xlen = Atof(sx); ylen = Atof(sy); zlen = Atof(sz);
  if (fscanf(f, "%d %d %d\n", &nbx, &nby, &nbz) != 3)
  {
    fclose(f);
    return Standard_False;
  }

  // Allocate the voxels
  myFloatVoxels = (Standard_Address) new Voxel_FloatDS(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  // Data
  // Copied from Voxel_FloatDS.cxx:
  Standard_Integer nb_floats = nbx * nby * nbz;
  Standard_Integer nb_slices = RealToInt(ceil(nb_floats / 32.0)); // 32 values in 1 slice
  // myData[0 .. nb_slices - 1][0 .. 31]
  if (nb_slices)
  {
    Standard_Integer i1 = 0, i2 = 0;
    Standard_ShortReal value = 0.0;
    while (!feof(f)
         && fgets(line, 64, f) != NULL)
    {
      if (has_slice(line))
      {
	if (sscanf(line, "%d %d %64s\n", &i1, &i2, line) != 3)
        {
	  fclose(f);
	  return Standard_False;
	}
      }
      else
      {
	if (sscanf(line, "%d %64s\n", &i2, line) != 2)
        {
	  fclose(f);
	  return Standard_False;
	}
      }
      value = (Standard_ShortReal)Atof(line);

      // Set value
      if (!((Standard_ShortReal**)((Voxel_DS*)myFloatVoxels)->myData)[i1])
      {
	((Standard_ShortReal**)((Voxel_DS*)myFloatVoxels)->myData)[i1] = 
	  (Standard_ShortReal*) calloc(32/*number of floats in slice*/, sizeof(Standard_ShortReal));
      }
      (((Standard_ShortReal**)((Voxel_DS*)myFloatVoxels)->myData)[i1])[i2] = value;
    }
  }

  fclose(f);
  return Standard_True;
}

Standard_Boolean Voxel_Reader::ReadBoolBinaryVoxels(const TCollection_ExtendedString& file)
{
  // Open file for reading
  FILE* f = OSD_OpenFile(file, "r");
  if (!f)
    return Standard_False;

  // Header: skip it
  Standard_Character line[65];
  if (fgets(line, 64, f) == NULL)
  {
    fclose(f);
    return Standard_False;
  }

  // Location, size, number of splits
  Standard_Integer nbx = 0, nby = 0, nbz = 0;
  Standard_Real x = 0.0, y = 0.0, z = 0.0, xlen = 0.0, ylen = 0.0, zlen = 0.0;
  if (fread(&x,    sizeof(Standard_Real),    1, f) != 1
   || fread(&y,    sizeof(Standard_Real),    1, f) != 1
   || fread(&z,    sizeof(Standard_Real),    1, f) != 1
   || fread(&xlen, sizeof(Standard_Real),    1, f) != 1
   || fread(&ylen, sizeof(Standard_Real),    1, f) != 1
   || fread(&zlen, sizeof(Standard_Real),    1, f) != 1
   || fread(&nbx,  sizeof(Standard_Integer), 1, f) != 1
   || fread(&nby,  sizeof(Standard_Integer), 1, f) != 1
   || fread(&nbz,  sizeof(Standard_Integer), 1, f) != 1)
  {
    fclose(f);
    return Standard_False;
  }

  // Allocate the voxels
  myBoolVoxels = (Standard_Address) new Voxel_BoolDS(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  // Data
  // Copied from Voxel_BoolDS.cxx:
  Standard_Integer nb_bytes = RealToInt(ceil(nbx * nby * nbz / 8.0));
  Standard_Integer nb_slices = RealToInt(ceil(nb_bytes / 8.0));
  // myData[0 .. nb_slices - 1][0 .. 7]
  if (nb_slices)
  {
    Standard_Integer i1 = 0, i2 = 0, value = 0;
    while (!feof(f)
         && fread(&i1, sizeof(Standard_Integer), 1, f) == 1
         && fread(&i2, sizeof(Standard_Integer), 1, f) == 1
         && fread(&value, sizeof(Standard_Byte), 1, f) == 1)
    {
      // Set value
      if (!((Standard_Byte**)((Voxel_DS*)myBoolVoxels)->myData)[i1])
      {
	((Standard_Byte**)((Voxel_DS*)myBoolVoxels)->myData)[i1] =
	  (Standard_Byte*) calloc(8/*number of bytes in slice*/, sizeof(Standard_Byte));
      }
      (((Standard_Byte**)((Voxel_DS*)myBoolVoxels)->myData)[i1])[i2] = (Standard_Byte)value;
    }
  }

  fclose(f);
  return Standard_True;
}

Standard_Boolean Voxel_Reader::ReadColorBinaryVoxels(const TCollection_ExtendedString& file)
{
  // Open file for reading
  FILE* f = OSD_OpenFile(file, "r");
  if (!f)
    return Standard_False;

  // Header: skip it
  Standard_Character line[65];
  if (fgets(line, 64, f) == NULL)
  {
    fclose(f);
    return Standard_False;
  }

  // Location, size, number of splits
  Standard_Integer nbx = 0, nby = 0, nbz = 0;
  Standard_Real x = 0.0, y = 0.0, z = 0.0, xlen = 0.0, ylen = 0.0, zlen = 0.0;
  if (fread(&x,    sizeof(Standard_Real),    1, f) != 1
   || fread(&y,    sizeof(Standard_Real),    1, f) != 1
   || fread(&z,    sizeof(Standard_Real),    1, f) != 1
   || fread(&xlen, sizeof(Standard_Real),    1, f) != 1
   || fread(&ylen, sizeof(Standard_Real),    1, f) != 1
   || fread(&zlen, sizeof(Standard_Real),    1, f) != 1
   || fread(&nbx,  sizeof(Standard_Integer), 1, f) != 1
   || fread(&nby,  sizeof(Standard_Integer), 1, f) != 1
   || fread(&nbz,  sizeof(Standard_Integer), 1, f) != 1)
  {
    fclose(f);
    return Standard_False;
  }

  // Allocate the voxels
  myColorVoxels = (Standard_Address) new Voxel_ColorDS(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  // Data
  // Copied from Voxel_ColorDS.cxx:
  Standard_Integer nb_bytes = RealToInt(ceil(nbx * nby * nbz / 2.0));
  Standard_Integer nb_slices = RealToInt(ceil(nb_bytes / 32.0));
  // myData[0 .. nb_slices - 1][0 .. 31]
  if (nb_slices)
  {
    Standard_Integer i1 = 0, i2 = 0, value = 0;
    while (!feof(f)
         && fread(&i1, sizeof(Standard_Integer), 1, f) == 1
         && fread(&i2, sizeof(Standard_Integer), 1, f) == 1
         && fread(&value, sizeof(Standard_Byte), 1, f) == 1)
    {
      // Set value
      if (!((Standard_Byte**)((Voxel_DS*)myColorVoxels)->myData)[i1])
      {
	((Standard_Byte**)((Voxel_DS*)myColorVoxels)->myData)[i1] = 
	  (Standard_Byte*) calloc(32/*number of bytes in slice*/, sizeof(Standard_Byte));
      }
      (((Standard_Byte**)((Voxel_DS*)myColorVoxels)->myData)[i1])[i2] = (Standard_Byte)value;
    }
  }

  fclose(f);
  return Standard_True;
}

Standard_Boolean Voxel_Reader::ReadFloatBinaryVoxels(const TCollection_ExtendedString& file)
{
  // Open file for reading
  FILE* f = OSD_OpenFile(file, "r");
  if (!f)
    return Standard_False;

  // Header: skip it
  Standard_Character line[65];
  if (fgets(line, 64, f) == NULL)
  {
    fclose(f);
    return Standard_False;
  }

  // Location, size, number of splits
  Standard_Integer nbx = 0, nby = 0, nbz = 0;
  Standard_Real x = 0.0, y = 0.0, z = 0.0, xlen = 0.0, ylen = 0.0, zlen = 0.0;
  if (fread(&x,    sizeof(Standard_Real),    1, f) != 1
   || fread(&y,    sizeof(Standard_Real),    1, f) != 1
   || fread(&z,    sizeof(Standard_Real),    1, f) != 1
   || fread(&xlen, sizeof(Standard_Real),    1, f) != 1
   || fread(&ylen, sizeof(Standard_Real),    1, f) != 1
   || fread(&zlen, sizeof(Standard_Real),    1, f) != 1
   || fread(&nbx,  sizeof(Standard_Integer), 1, f) != 1
   || fread(&nby,  sizeof(Standard_Integer), 1, f) != 1
   || fread(&nbz,  sizeof(Standard_Integer), 1, f) != 1)
  {
    fclose(f);
    return Standard_False;
  }

  // Allocate the voxels
  myFloatVoxels = (Standard_Address) new Voxel_FloatDS(x, y, z, xlen, ylen, zlen, nbx, nby, nbz);

  // Data
  // Copied from Voxel_FloatDS.cxx:
  Standard_Integer nb_floats = nbx * nby * nbz;
  Standard_Integer nb_slices = RealToInt(ceil(nb_floats / 32.0)); // 32 values in 1 slice
  // myData[0 .. nb_slices - 1][0 .. 31]
  if (nb_slices)
  {
    Standard_Integer i1 = 0, i2 = 0;
    Standard_ShortReal value = 0.0;
    while (!feof(f)
         && fread(&i1,    sizeof(Standard_Integer),   1, f) == 1
         && fread(&i2,    sizeof(Standard_Integer),   1, f) == 1
         && fread(&value, sizeof(Standard_ShortReal), 1, f) == 1)
    {
      // Set value
      if (!((Standard_ShortReal**)((Voxel_DS*)myFloatVoxels)->myData)[i1])
      {
	((Standard_ShortReal**)((Voxel_DS*)myFloatVoxels)->myData)[i1] = 
	  (Standard_ShortReal*) calloc(32/*number of floats in slice*/, sizeof(Standard_ShortReal));
      }
      (((Standard_ShortReal**)((Voxel_DS*)myFloatVoxels)->myData)[i1])[i2] = value;
    }
  }

  fclose(f);
  return Standard_True;
}
