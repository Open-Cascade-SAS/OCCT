// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <OSD_Disk.hxx>

#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <OSD_WhoAmI.hxx>
#include <Standard_ProgramError.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_String.hxx>
#include <TCollection_ExtendedString.hxx>

#ifdef _WIN32
  #include <windows.h>

  void _osd_wnt_set_error (OSD_Error&, OSD_WhoAmI, ... );

  static TCollection_AsciiString _osd_wnt_set_disk_name (const OSD_Path& thePath)
  {
    {
      TCollection_AsciiString aDisk = thePath.Disk();
      if (aDisk.UsefullLength() != 0)
      {
        return aDisk + '/';
      }
    }

    TCollection_AsciiString aDir = thePath.Trek();
    const int j = aDir.UsefullLength();
    if (j < 3
    || aDir.Value (1) != '|'
    || aDir.Value (2) != '|')
    {
      throw Standard_ProgramError ("OSD_Disk: bad disk name");
    }

    aDir.SetValue (1, '\\');
    aDir.SetValue (2, '\\');
    int k = 0;
    for (int i = 3; i <= j; ++i)
    {
      if (aDir.Value (i) == '|')
      {
        if (k == 0)
        {
          aDir.SetValue (i, '\\');
          ++k;
          continue;
        }

        aDir.SetValue (i, '\\');
        break;
      }
    }

    if (k == 0)
    {
      if (thePath.Name().UsefullLength() == 0
       && thePath.Extension().UsefullLength() == 0)
      {
        throw Standard_ProgramError ("OSD_Disk: bad disk name");
      }
      else
      {
        aDir += '\\';
        aDir += thePath.Name();
        aDir += thePath.Extension();
      }
    }

    if (aDir.Value (aDir.UsefullLength()) != '\\')
    {
      aDir += '\\';
    }
    return aDir;
  }

#else
  const OSD_WhoAmI Iam = OSD_WDisk;
  extern "C" {
  #if defined(__ANDROID__)
    #include <sys/vfs.h>
    #define statvfs  statfs
    #define fstatvfs fstatfs
  #else
    #include <sys/statvfs.h>
  #endif
  }
  #include <errno.h>
#endif

// =======================================================================
// function : OSD_Disk
// purpose  :
// =======================================================================
OSD_Disk::OSD_Disk()
{
#ifdef _WIN32
  const DWORD aBuffLen = GetCurrentDirectoryW (0, NULL);
  NCollection_Array1<wchar_t> aBuff (0, aBuffLen);
  GetCurrentDirectoryW (aBuffLen, &aBuff.ChangeFirst());
  aBuff.ChangeValue (aBuffLen - 1) = (aBuff.Value (aBuffLen - 2) == L'\\') ? L'\0' : L'\\';
  aBuff.ChangeLast() = L'\0';
  if (aBuffLen > 3 && aBuff.First() != L'\\')
  {
    aBuff.ChangeValue (3) = L'\0';
    myDiskName = TCollection_AsciiString (&aBuff.ChangeFirst());
  }
#endif
}

// =======================================================================
// function : OSD_Disk
// purpose  :
// =======================================================================
OSD_Disk::OSD_Disk (const OSD_Path& theName)
: myDiskName (theName.Disk())
{
#ifdef _WIN32
  myDiskName = _osd_wnt_set_disk_name (theName);
#endif
}

OSD_Disk::OSD_Disk (const Standard_CString theName)
: myDiskName (theName)
{
#ifdef _WIN32
  OSD_Path aPath (theName);
  myDiskName = _osd_wnt_set_disk_name (aPath);
#endif
}

// =======================================================================
// function : SetName
// purpose  :
// =======================================================================
void OSD_Disk::SetName (const OSD_Path& theName)
{
  myDiskName = theName.Disk();
}

// =======================================================================
// function : Name
// purpose  :
// =======================================================================
OSD_Path OSD_Disk::Name() const
{
#ifdef _WIN32
  return myDiskName;
#else
  OSD_Path aPath;
  aPath.SetDisk (myDiskName);
  return aPath;
#endif
}

// =======================================================================
// function : DiskSize
// purpose  :
// =======================================================================
Standard_Integer OSD_Disk::DiskSize()
{
#ifdef _WIN32
  ULARGE_INTEGER aNbFreeAvailableBytes, aNbTotalBytes, aNbTotalFreeBytes;
  const TCollection_ExtendedString aDiskNameW (myDiskName);
  if (!GetDiskFreeSpaceExW (aDiskNameW.ToWideString(),
                            &aNbFreeAvailableBytes,
                            &aNbTotalBytes,
                            &aNbTotalFreeBytes))
  {
    _osd_wnt_set_error (myError, OSD_WDisk);
    return 0;
  }

  ULONGLONG aSize = aNbTotalBytes.QuadPart / 512;
  return (Standard_Integer )aSize; // may be an overflow
#else
  struct statvfs buffer;
  if (statvfs (myDiskName.ToCString(), &buffer) == 0)
  {
    int BSize512 = buffer.f_frsize / 512;
    return buffer.f_blocks * BSize512;
  }
  myError.SetValue (errno, Iam, "OSD_Disk: statvfs failed.");
  return 0;
#endif
}

// =======================================================================
// function : DiskFree
// purpose  :
// =======================================================================
Standard_Integer OSD_Disk::DiskFree()
{
#ifdef _WIN32
  ULARGE_INTEGER aNbFreeAvailableBytes, aNbTotalBytes, aNbTotalFreeBytes;
  const TCollection_ExtendedString aDiskNameW (myDiskName);
  if (!GetDiskFreeSpaceExW (aDiskNameW.ToWideString(),
                            &aNbFreeAvailableBytes,
                            &aNbTotalBytes,
                            &aNbTotalFreeBytes))
  {
    _osd_wnt_set_error (myError, OSD_WDisk);
    return 0;
  }

  ULONGLONG aSize = aNbFreeAvailableBytes.QuadPart / 512;
  return (Standard_Integer )aSize; // may be an overflow
#else
  struct statvfs buffer;
  if (statvfs (myDiskName.ToCString(), &buffer) == 0)
  {
    int BSize512 = buffer.f_frsize / 512;
    return buffer.f_bavail * BSize512;
  }
  myError.SetValue (errno, Iam, "OSD_Disk: statvfs failed.");
  return 0;
#endif
}
