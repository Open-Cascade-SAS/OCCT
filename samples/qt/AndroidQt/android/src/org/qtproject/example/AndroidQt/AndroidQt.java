package org.qtproject.example.AndroidQt;

import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;

import java.util.List;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class AndroidQt extends org.qtproject.qt5.android.bindings.QtActivity
{
  @Override public void onCreate(Bundle theBundle)
  {
    super.onCreate(theBundle);

    // copy OCCT resources
    String aResFolder = getFilesDir().getAbsolutePath();
    copyAssetFolder (getAssets(), "opencascade", aResFolder + "/opencascade");

  }

  //! Copy folder from assets
  private boolean copyAssetFolder (AssetManager theAssetMgr,
                                   String       theAssetFolder,
                                   String       theFolderPathTo)
  {
    try
    {
      String[] aFiles  = theAssetMgr.list (theAssetFolder);
      File     aFolder = new File (theFolderPathTo);
      aFolder.mkdirs();
      boolean isOk = true;
      for (String aFileIter : aFiles)
      {
        if (aFileIter.contains ("."))
        {
          isOk &= copyAsset (theAssetMgr,
                             theAssetFolder  + "/" + aFileIter,
                             theFolderPathTo + "/" + aFileIter);
        }
        else
        {
          isOk &= copyAssetFolder (theAssetMgr,
                                   theAssetFolder  + "/" + aFileIter,
                                   theFolderPathTo + "/" + aFileIter);
        }
      }
      return isOk;
    }
    catch (Exception theError)
    {
      theError.printStackTrace();
      return false;
    }
  }

  //! Copy single file from assets
  private boolean copyAsset (AssetManager theAssetMgr,
                             String       thePathFrom,
                             String       thePathTo)
  {
    try
    {
      InputStream aStreamIn = theAssetMgr.open (thePathFrom);
      File aFileTo = new File (thePathTo);
      aFileTo.createNewFile();
      OutputStream aStreamOut = new FileOutputStream (thePathTo);
      copyStreamContent (aStreamIn, aStreamOut);
      aStreamIn.close();
      aStreamIn = null;
      aStreamOut.flush();
      aStreamOut.close();
      aStreamOut = null;
      return true;
    }
    catch (Exception theError)
    {
      theError.printStackTrace();
      return false;
    }
  }

  //! Copy single file
  private static void copyStreamContent (InputStream  theIn,
                                         OutputStream theOut) throws IOException
  {
    byte[] aBuffer = new byte[1024];
    int aNbReadBytes = 0;
    while ((aNbReadBytes = theIn.read (aBuffer)) != -1)
    {
      theOut.write (aBuffer, 0, aNbReadBytes);
    }
  }
}
