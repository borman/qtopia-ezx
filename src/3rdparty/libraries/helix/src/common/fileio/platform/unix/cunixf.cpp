/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: cunixf.cpp,v 1.11 2009/01/19 23:30:46 sfu Exp $
 * 
 * Portions Copyright (c) 1995-2004 RealNetworks, Inc. All Rights Reserved.
 * 
 * The contents of this file, and the files included with this file,
 * are subject to the current version of the RealNetworks Public
 * Source License (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the current version of the RealNetworks Community
 * Source License (the "RCSL") available at
 * http://www.helixcommunity.org/content/rcsl, in which case the RCSL
 * will apply. You may also obtain the license terms directly from
 * RealNetworks.  You may not use this file except in compliance with
 * the RPSL or, if you have a valid RCSL with RealNetworks applicable
 * to this file, the RCSL.  Please see the applicable RPSL or RCSL for
 * the rights, obligations and limitations governing use of the
 * contents of the file.
 * 
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License Version 2 (the
 * "GPL") in which case the provisions of the GPL are applicable
 * instead of those above. If you wish to allow use of your version of
 * this file only under the terms of the GPL, and not to allow others
 * to use your version of this file under the terms of either the RPSL
 * or RCSL, indicate your decision by deleting the provisions above
 * and replace them with the notice and other provisions required by
 * the GPL. If you do not delete the provisions above, a recipient may
 * use your version of this file under the terms of any one of the
 * RPSL, the RCSL or the GPL.
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the
 * developer of the Original Code and owns the copyrights in the
 * portions it created.
 * 
 * This file, and the files included with this file, is distributed
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
 * ENJOYMENT OR NON-INFRINGEMENT.
 * 
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 * 
 * Contributor(s):
 * 
 * ***** END LICENSE BLOCK ***** */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "hlxclib/stdlib.h"
#include <errno.h>

#include "chxdataf.h"
#include "cunixf.h"
#include "hxheap.h"
#include "hxresult.h"

#if !defined _VXWORKS && !defined __QNXNTO__ && !defined ANDROID
extern int errno;
#endif

#define  HX_FILE_NOT_OPEN    -1000

CHXDataFile*
CHXDataFile::Construct(IUnknown* pContext, UINT32 ulFlags, IUnknown** ppCommonObj)
{
    return new CUnixFile(pContext);
}

// CHXFile should set the file reference to a value
// indicating the file is not open

CUnixFile::CUnixFile (IUnknown* pContext)
	  :CHXDataFile(pContext)
{
        // set FD to indicate file is not open
        mFD = HX_FILE_NOT_OPEN;
        mLastError = HXR_OK;
}

// ~CHXFile should close the file if it is open
CUnixFile::~CUnixFile(void)
{ 
        // close the file if it is open
        if ( mFD > 0 )
        {
           close( mFD );
        }
}

// Following comments and code of GetSize were directly taken from the 
// windows implementation ../win/cwinfile.cpp

// Simply uses stat to get the size of the file in bytes.  If the file
// is closed, it will still work.
ULONG32 CUnixFile::GetSize(void)
{
    struct stat filestats;
    if (mFD >= 0)
    {
        fstat(mFD, &filestats);
        return filestats.st_size;
    }
    return 0;
}


// Create a file with the specified mode
// Closes the previous file if it was open
// returns HXR_OK or HXR_INVALID_FILE if an error occurred

HX_RESULT CUnixFile::Create(const char *filename, UINT16 mode, HXBOOL textflag)
{
        // close previous file if necessary
        if ( mFD > 0 )
            close( mFD );

        // create file
        mLastError = HXR_OK;
        if ( ( mFD = creat( filename, mode ) ) < 0 )
        {
           mLastError = errno;
           return HXR_DOC_MISSING;
        }
        return HXR_OK;
}

// Open a file with the specified mode
// Closes the previous file if it was open
// returns HXR_OK or HXR_INVALID_FILE if an error occurred

HX_RESULT CUnixFile::Open(const char *filename, UINT16 mode, HXBOOL textflag)
{
        // close previous file if necessary
        if ( mFD > 0 )
            close( mFD );

        // open file
        mLastError = HXR_OK;
#ifndef _VXWORKS /* VXWORKS uses FAT32 */
#define OPENFLAGS (S_IREAD | S_IWRITE)
#else
#endif
        if ( ( mFD = open( filename, mode, OPENFLAGS ) ) < 0 )
        {
           mLastError = errno;
           return HXR_DOC_MISSING;
        }
        // change permissions to allow everyone to read the file and owner to write
	// only if I have to create this file
#if !defined(_VXWORKS) && !defined(_BEOS)
	if ( mode & O_CREAT )
        fchmod( mFD, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
#endif

        return HXR_OK;
}

// Close the previous file if it was open
// returns HXR_OK or HXR_INVALID_FILE if an error occurred

HX_RESULT CUnixFile::Close(void)
{
        // close previous file if necessary
        if ( mFD > 0 )
        {
           mLastError = HXR_OK;   
           if ( close( mFD ) < 0 )
           {
              mLastError = errno;
              return HXR_INVALID_FILE;
           }
         return HXR_OK;
        }
        return HXR_INVALID_FILE;
}

HX_RESULT CUnixFile::Delete(const char* pFilename)
{
    if(mFD > 0)
    {
	close(mFD);
    }
    mLastError = HXR_OK;
    if(unlink(pFilename))
    {
	if(errno == EACCES)
	{
	    mLastError = HXR_ACCESSDENIED;
	}
	else
	{
	    mLastError = HXR_DOC_MISSING;
	}
    }
    return mLastError;
}

// Seek moves the current file position to the offset from the fromWhere
// specifier returns HXR_OK or HXR_INVALID_FILE if an error occurred

HX_RESULT CUnixFile::Seek(ULONG32 offset, UINT16 fromWhere)
{
        if ( mFD > 0 )
        {
           mLastError = HXR_OK;       
           if ( lseek( mFD, offset, fromWhere ) < 0 )
           {
              mLastError = errno;
              return HXR_INVALID_FILE;
           }
         return HXR_OK;
        }
      return HXR_INVALID_FILE;
}

// Rewind sets the file position to the start of file
// returns HXR_OK or HXR_INVALID_FILE if an error occurred
HX_RESULT CUnixFile::Rewind(void)
{
        if ( mFD > 0 )
        {
           mLastError = HXR_OK;       
           if ( lseek( mFD, 0, SEEK_SET ) < 0 )
           {
              mLastError = errno;
              return HXR_INVALID_FILE;
           }
         return HXR_OK;
        }
        return HXR_INVALID_FILE;
}


// Tell returns the current file position
// returns HXR_OK or -1 if an error occurred
ULONG32 CUnixFile::Tell(void)
{   
        long offset = -1;
        if ( mFD > 0 )
        {
           mLastError = HXR_OK;       
         // the tell() function will consistently generate a warning 
         // and I haven't figured out how to fix that yet
/****
           if ((offset = tell( mFD )) < 0 )
           {
              mLastError = errno;
           }
****/
			// so we do this instead....
			if ((offset = lseek( mFD, 0, SEEK_CUR )) < 0 )
			{
				mLastError = errno;
			}
        }
        return (ULONG32)offset;
}

/*      Read reads up to count bytes of data into buf.
        returns the number of bytes read, EOF, or -1 if the read failed */

ULONG32 CUnixFile::Read (char *buf, ULONG32 count)
{
	int ncnt = -1;           // number of bytes read

	if ( mFD > 0 )
 	{ 
		mLastError = HXR_OK;
		ULONG32 tmpCheck = Tell();

		if ( ( ncnt = read( mFD, buf, count ) ) < 0 )
		{
			mLastError = errno;
		}
	}
	return (ULONG32)ncnt;
}

/*      Write writes up to count bytes of data from buf.
        returns the number of bytes written, or -1 if the write failed */

ULONG32 CUnixFile::Write(const char *buf, ULONG32 count)
{
        int ncnt = -1;           // number of bytes written

      if ( mFD > 0 )
      { mLastError = HXR_OK;

          if ( ( ncnt = write( mFD, buf, count ) ) < 0 )
          {
             mLastError = errno;
          }
      }

        return (ULONG32)ncnt;
}

HXBOOL CUnixFile::GetTemporaryFileName(const char *tag, char* name, UINT32 ulBufLen)
{
    // XXXNH: beware: mktemp is a potential security risk. see man pages.
	
	const int kBuffsize = 1024;
	char buff[kBuffsize];

	int count = snprintf(buff, kBuffsize, "/tmp/%s_XXXXXX", tag);

	if (count > 0 && count < kBuffsize)
	{
		// replace the X's with digits
		if (mktemp(buff) != NULL)
		{
			strncpy(name, buff, ulBufLen);
			return TRUE;
		}
	}
	return FALSE;
}

