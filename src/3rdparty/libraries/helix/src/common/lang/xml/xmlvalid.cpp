/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: xmlvalid.cpp,v 1.9 2007/07/25 06:23:35 anshuman Exp $
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

#include "hxtypes.h"
#include "hxcom.h"
#include "hxassert.h"

#ifdef XXXJHUG
#include "hlxclib/stdio.h"
#endif /* XXXJHUG */
#include "hlxclib/stdlib.h"
#include "xmlvalid.h"
#include "hlxclib/string.h"

#if defined(_WINDOWS) && defined(WIN32_PLATFORM_PSPC)
void * __cdecl __helix_bsearch (
        const void *key,
        const void *base,
        size_t num,
        size_t width,
        int (__cdecl *compare)(const void *, const void *)
        )
{
        char *lo = (char *)base;
        char *hi = (char *)base + (num - 1) * width;
        char *mid;
        unsigned int half;
        int result;

        while (lo <= hi)
                if (half = num / 2)
                {
                        mid = lo + (num & 1 ? half : (half - 1)) * width;
                        if (!(result = (*compare)(key,mid)))
                                return(mid);
                        else if (result < 0)
                        {
                                hi = mid - width;
                                num = num & 1 ? half : half-1;
                        }
                        else    {
                                lo = mid + width;
                                num = half;
                        }
                }
                else if (num)
                        return((*compare)(key,lo) ? NULL : lo);
                else
                        break;

        return(NULL);
}
#endif //_WINDOWS && WIN32_PLATFORM_PSPC

HXBOOL ISO8859Valid::IsNameValid(const BYTE* p, UINT32 len)
{
    HXBOOL bValid = IsLetter(*p) || IsDigit(*p) || *p == '_' || *p == ':';

    for (const BYTE* end = p + len; bValid && *p && p < end; ++p )
    {
	bValid = IsLetter(*p) || IsDigit(*p) || *p == '.' ||
	    *p == '-' || *p == '_' || *p == ':' || 
	    IsCombiningChar(*p);
    }

    return bValid;
}

HXBOOL ISO8859Valid::IsNmtokenValid(const BYTE* p, UINT32 len)
{
    HXBOOL bValid = TRUE;
    
    for (const BYTE* end = p + len; bValid && *p && p < end; ++p )
    {
	bValid = IsLetter(*p) || IsDigit(*p) || *p == '.' ||
	    *p == '-' || *p == '_' || *p == ':' || 
	    IsCombiningChar(*p);
    }

    return bValid;
}


// assume that string has allready been parsed for ' and "
//::=  '"' ([^%&"] | PEReference | Reference)* '"'  
//   |  "'" ([^%&'] | PEReference | Reference)* "'" 
HXBOOL ISO8859Valid::IsEntityValueValid(const BYTE* p, UINT32 len)
{
    HXBOOL bValid = TRUE;
    const BYTE* pBeginRef = NULL;
    
    for (const BYTE* end = p + len; bValid && *p && p < end; ++p )
    {
	if ( *p == '&' || *p == '%' )
	{
	    pBeginRef = p;
	}
	else if (pBeginRef && *p == ';')
	{
	    bValid = IsRefValid(pBeginRef + 1, p - pBeginRef - 1);
	    pBeginRef = NULL;
	}
    }
    return bValid;
}

HXBOOL ISO8859Valid::IsRefValid(const BYTE *p, UINT32 len)
{
    HXBOOL bValid = FALSE;
    HX_ASSERT(*(p -1) == '&');
    // ref - 0 - nothing, '&' - beginning of Refference
    //	     'n' - beginning of number
    //	     'h' - hex number
    //	     'd' - decimal number
    //	     'e' - entity reference
    BYTE ref = '&';
    const BYTE* pBeginRef = NULL;
    
    for (const BYTE* end = p + len; bValid && *p && p < end; ++p )
    {
	switch ( ref )
	{
	case '&':
	    if ( *p == '#' )
	    {
		ref = 'n';
	    }
	    else
	    {
		ref = 'e';
		pBeginRef = p;
	    }
	    break;
	case 'n':
	    if (*p == 'x' || *p == 'X')
	    {
		ref = 'h';
	    }
	    else
	    {
		ref = 'd';
		bValid = IsDigit(*p);
	    }
	    break;
	case 'd':
	    if ( *p == ';' )
	    {
		ref = '\0';
	    }
	    else 
	    {
		bValid = IsDigit(*p);
	    }
	    break;
	case 'h':
	    if ( *p == ';' )
	    {
		ref = '\0';
	    }
	    else
	    {
		bValid = IsDigit(*p) || BETWEEN(*p, 'a', 'f') 
		    || BETWEEN(*p, 'A', 'F');
	    }
	    break;
	case 'e':
	    if ( *p == ';' )
	    {
		// this could be checked as we go in a single pass.
		bValid = IsNameValid(pBeginRef, p - pBeginRef);
		pBeginRef = NULL;
		ref = '\0';
	    }
	    break;
	case '\0':
	    return bValid;
	}
    }
    return bValid;
}


// assume that string has allready been parsed for ' and "
// AttValue ::=  '"' ([^<&"] | Reference)* '"'  
//   |  "'" ([^<&'] | Reference)* "'" 
HXBOOL ISO8859Valid::IsAttValueValid(const BYTE* p, UINT32 len)
{
    HXBOOL bValid = TRUE;
    const BYTE* pBeginRef = NULL;
    
    for (const BYTE* end = p + len; bValid && *p && p < end; ++p )
    {
	if ( *p == '&' )
	{
	    bValid = FALSE;
	    pBeginRef = p;
	}
	else if (*p == '<')
	{
	    bValid = FALSE;
	}
	else if (pBeginRef && *p == ';')
	{
	    bValid = IsRefValid(pBeginRef + 1, p - pBeginRef - 1);
	    pBeginRef = NULL;
	}
    }
    return bValid;
}

//SystemLiteral ::=  ('"' [^"]* '"') | ("'" [^']* "'")  
// assuming that the deliminators have allready been parsed.
// there fore system literals should allways be valid.
HXBOOL ISO8859Valid::IsSystemLiteralValid(const BYTE* p, UINT32 len)
{
    return TRUE;
}

// Public ID Literal
//[12]  PubidLiteral ::=  '"' PubidChar* '"' | "'" (PubidChar - "'")* "'" 
//[13]  PubidChar ::=  #x20 | #xD | #xA | [a-zA-Z0-9] | [] 
HXBOOL ISO8859Valid::IsPubidLiteralValid(const BYTE* p, UINT32 len)
{
    HXBOOL bValid = TRUE;
    for (const BYTE* end = p + len; bValid && *p && p < end; ++p )
    {
	bValid = *p == 0x20 || *p == 0xD || *p == 0xA || BETWEEN(*p, 'a', 'z') || 
	    BETWEEN(*p, 'A', 'Z') || BETWEEN(*p, '0', '9') || strchr("-()+,./:=?;!*#@$_%", *p);
    }

    return bValid;
}


inline HXBOOL ISO8859Valid::IsLetter(const BYTE c)
{
    return IsBaseChar(c) || IsIdeographic(c);
}

inline HXBOOL ISO8859Valid::IsBaseChar(const BYTE c)
{
    return BETWEEN(c, 0x41, 0x5A) || BETWEEN(c, 0x61, 0x7A) || 
	   BETWEEN(c, 0xC0, 0xD6) || BETWEEN(c, 0xD8, 0xF6) ||
	   BETWEEN(c, 0xF8, 0xFF);
}

inline HXBOOL ISO8859Valid::IsIdeographic(const BYTE c)
{
    return FALSE;
}

inline HXBOOL ISO8859Valid::IsCombiningChar(const BYTE c)
{
    return FALSE;
}

inline HXBOOL ISO8859Valid::IsDigit(const BYTE c)
{
    return BETWEEN(c, 0x30, 0x39);
}

inline HXBOOL ISO8859Valid::IsExtender(const BYTE c)
{
    return c == 0xB7;
}



HXBOOL UTF16Valid::IsNameValid(const UINT16* p, UINT32 len)
{
    HXBOOL bValid = IsLetter(*p) || IsDigit(*p) || *p == '_' || *p == ':';

    for (const UINT16* end = p + len; bValid && *p && p < end; ++p )
    {
	bValid = IsLetter(*p) || IsDigit(*p) || *p == '.' ||
	    *p == '-' || *p == '_' || *p == ':' || 
	    IsCombiningChar(*p);
    }

    return bValid;
}

HXBOOL UTF16Valid::IsNmtokenValid(const UINT16* p, UINT32 len)
{
    HXBOOL bValid = TRUE;
    
    for (const UINT16* end = p + len; bValid && *p && p < end; ++p )
    {
	bValid = IsLetter(*p) || IsDigit(*p) || *p == '.' ||
	    *p == '-' || *p == '_' || *p == ':' || 
	    IsCombiningChar(*p);
    }

    return bValid;
}

// these four functions assume that the ' or " deliminators have
// allready been parsed.
HXBOOL UTF16Valid::IsEntityValueValid(const UINT16* p, UINT32 len)
{
    // copy UTF8 after it has been tested.
    return FALSE;
}

HXBOOL UTF16Valid::IsAttValueValid(const UINT16* p, UINT32 len)
{
    // copy UTF8 after it has been tested.
    return FALSE;
}

HXBOOL UTF16Valid::IsSystemLiteralValid(const UINT16* p, UINT32 len)
{
    return TRUE;
}

HXBOOL UTF16Valid::IsPubidLiteralValid(const UINT16* p, UINT32 len)
{
    HXBOOL bValid = TRUE;
    for (const UINT16* end = p + len; bValid && *p && p < end; ++p )
    {
	bValid = *p == 0x20 || *p == 0xD || *p == 0xA || BETWEEN(*p, 'a', 'z') || 
	    BETWEEN(*p, 'A', 'Z') || BETWEEN(*p, '0', '9') || strchr("-()+,./:=?;!*#@$_%", *p);
    }

    return bValid;
}

struct Node
{
    UINT16 Begin;
    UINT16 End;
};

static const Node pBaseChar[] = {
        { 0x0041, 0x005A }, { 0x0061, 0x007A }, { 0x00C0, 0x00D6 }, { 0x00D8, 0x00F6 },
        { 0x00F8, 0x00FF }, { 0x0100, 0x0131 }, { 0x0134, 0x013E }, { 0x0141, 0x0148 },
        { 0x014A, 0x017E }, { 0x0180, 0x01C3 }, { 0x01CD, 0x01F0 }, { 0x01F4, 0x01F5 },
        { 0x01FA, 0x0217 }, { 0x0250, 0x02A8 }, { 0x02BB, 0x02C1 }, { 0x0386, 0x0386 },
        { 0x0388, 0x038A }, { 0x038C, 0x038C }, { 0x038E, 0x03A1 }, { 0x03A3, 0x03CE },
        { 0x03D0, 0x03D6 }, { 0x03DA, 0x03DA }, { 0x03DC, 0x03DC }, { 0x03DE, 0x03DE },
        { 0x03E0, 0x03E0 }, { 0x03E2, 0x03F3 }, { 0x0401, 0x040C }, { 0x040E, 0x044F },
        { 0x0451, 0x045C }, { 0x045E, 0x0481 }, { 0x0490, 0x04C4 }, { 0x04C7, 0x04C8 },
        { 0x04CB, 0x04CC }, { 0x04D0, 0x04EB }, { 0x04EE, 0x04F5 }, { 0x04F8, 0x04F9 },
        { 0x0531, 0x0556 }, { 0x0559, 0x0559 }, { 0x0561, 0x0586 }, { 0x05D0, 0x05EA },
        { 0x05F0, 0x05F2 }, { 0x0621, 0x063A }, { 0x0641, 0x064A }, { 0x0671, 0x06B7 },
        { 0x06BA, 0x06BE }, { 0x06C0, 0x06CE }, { 0x06D0, 0x06D3 }, { 0x06D5, 0x06D5 },
        { 0x06E5, 0x06E6 }, { 0x0905, 0x0939 }, { 0x093D, 0x093D }, { 0x0958, 0x0961 },
        { 0x0985, 0x098C }, { 0x098F, 0x0990 }, { 0x0993, 0x09A8 }, { 0x09AA, 0x09B0 },
        { 0x09B2, 0x09B2 }, { 0x09B6, 0x09B9 }, { 0x09DC, 0x09DD }, { 0x09DF, 0x09E1 },
        { 0x09F0, 0x09F1 }, { 0x0A05, 0x0A0A }, { 0x0A0F, 0x0A10 }, { 0x0A13, 0x0A28 },
        { 0x0A2A, 0x0A30 }, { 0x0A32, 0x0A33 }, { 0x0A35, 0x0A36 }, { 0x0A38, 0x0A39 },
        { 0x0A59, 0x0A5C }, { 0x0A5E, 0x0A5E }, { 0x0A72, 0x0A74 }, { 0x0A85, 0x0A8B },
        { 0x0A8D, 0x0A8D }, { 0x0A8F, 0x0A91 }, { 0x0A93, 0x0AA8 }, { 0x0AAA, 0x0AB0 },
        { 0x0AB2, 0x0AB3 }, { 0x0AB5, 0x0AB9 }, { 0x0ABD, 0x0ABD }, { 0x0AE0, 0x0AE0 },
        { 0x0B05, 0x0B0C }, { 0x0B0F, 0x0B10 }, { 0x0B13, 0x0B28 }, { 0x0B2A, 0x0B30 },
        { 0x0B32, 0x0B33 }, { 0x0B36, 0x0B39 }, { 0x0B3D, 0x0B3D }, { 0x0B5C, 0x0B5D },
        { 0x0B5F, 0x0B61 }, { 0x0B85, 0x0B8A }, { 0x0B8E, 0x0B90 }, { 0x0B92, 0x0B95 },
        { 0x0B99, 0x0B9A }, { 0x0B9C, 0x0B9C }, { 0x0B9E, 0x0B9F }, { 0x0BA3, 0x0BA4 },
        { 0x0BA8, 0x0BAA }, { 0x0BAE, 0x0BB5 }, { 0x0BB7, 0x0BB9 }, { 0x0C05, 0x0C0C },
        { 0x0C0E, 0x0C10 }, { 0x0C12, 0x0C28 }, { 0x0C2A, 0x0C33 }, { 0x0C35, 0x0C39 },
        { 0x0C60, 0x0C61 }, { 0x0C85, 0x0C8C }, { 0x0C8E, 0x0C90 }, { 0x0C92, 0x0CA8 },
        { 0x0CAA, 0x0CB3 }, { 0x0CB5, 0x0CB9 }, { 0x0CDE, 0x0CDE }, { 0x0CE0, 0x0CE1 },
        { 0x0D05, 0x0D0C }, { 0x0D0E, 0x0D10 }, { 0x0D12, 0x0D28 }, { 0x0D2A, 0x0D39 },
        { 0x0D60, 0x0D61 }, { 0x0E01, 0x0E2E }, { 0x0E30, 0x0E30 }, { 0x0E32, 0x0E33 },
        { 0x0E40, 0x0E45 }, { 0x0E81, 0x0E82 }, { 0x0E84, 0x0E84 }, { 0x0E87, 0x0E88 },
        { 0x0E8A, 0x0E8A }, { 0x0E8D, 0x0E8D }, { 0x0E94, 0x0E97 }, { 0x0E99, 0x0E9F },
        { 0x0EA1, 0x0EA3 }, { 0x0EA5, 0x0EA5 }, { 0x0EA7, 0x0EA7 }, { 0x0EAA, 0x0EAB },
        { 0x0EAD, 0x0EAE }, { 0x0EB0, 0x0EB0 }, { 0x0EB2, 0x0EB3 }, { 0x0EBD, 0x0EBD },
        { 0x0EC0, 0x0EC4 }, { 0x0F40, 0x0F47 }, { 0x0F49, 0x0F69 }, { 0x10A0, 0x10C5 },
        { 0x10D0, 0x10F6 }, { 0x1100, 0x1100 }, { 0x1102, 0x1103 }, { 0x1105, 0x1107 },
        { 0x1109, 0x1109 }, { 0x110B, 0x110C }, { 0x110E, 0x1112 }, { 0x113C, 0x113C },
        { 0x113E, 0x113E }, { 0x1140, 0x1140 }, { 0x114C, 0x114C }, { 0x114E, 0x114E },
        { 0x1150, 0x1150 }, { 0x1154, 0x1155 }, { 0x1159, 0x1159 }, { 0x115F, 0x1161 },
        { 0x1163, 0x1163 }, { 0x1165, 0x1165 }, { 0x1167, 0x1167 }, { 0x1169, 0x1169 },
        { 0x116D, 0x116E }, { 0x1172, 0x1173 }, { 0x1175, 0x1175 }, { 0x119E, 0x119E },
        { 0x11A8, 0x11A8 }, { 0x11AB, 0x11AB }, { 0x11AE, 0x11AF }, { 0x11B7, 0x11B8 },
        { 0x11BA, 0x11BA }, { 0x11BC, 0x11C2 }, { 0x11EB, 0x11EB }, { 0x11F0, 0x11F0 },
        { 0x11F9, 0x11F9 }, { 0x1E00, 0x1E9B }, { 0x1EA0, 0x1EF9 }, { 0x1F00, 0x1F15 },
        { 0x1F18, 0x1F1D }, { 0x1F20, 0x1F45 }, { 0x1F48, 0x1F4D }, { 0x1F50, 0x1F57 },
        { 0x1F59, 0x1F59 }, { 0x1F5B, 0x1F5B }, { 0x1F5D, 0x1F5D }, { 0x1F5F, 0x1F7D },
        { 0x1F80, 0x1FB4 }, { 0x1FB6, 0x1FBC }, { 0x1FBE, 0x1FBE }, { 0x1FC2, 0x1FC4 },
        { 0x1FC6, 0x1FCC }, { 0x1FD0, 0x1FD3 }, { 0x1FD6, 0x1FDB }, { 0x1FE0, 0x1FEC },
        { 0x1FF2, 0x1FF4 }, { 0x1FF6, 0x1FFC }, { 0x2126, 0x2126 }, { 0x212A, 0x212B },
        { 0x212E, 0x212E }, { 0x2180, 0x2182 }, { 0x3041, 0x3094 }, { 0x30A1, 0x30FA },
        { 0x3105, 0x312C }, { 0xAC00, 0xD7A3 }
};

int compare(const void* elem1, const void* elem2)
{
    if ( *((const UINT16*)elem1) < ((Node*)elem2)->Begin )
    {
	return -1;
    }
    else if ( *((const UINT16*)elem1) > ((Node*)elem2)->End )
    {
	return 1;
    }
    else
    {
	 return 0;
    }
}

inline HXBOOL UTF16Valid::IsLetter(const UINT16 c)
{
    return IsBaseChar(c) || IsIdeographic(c);
}

inline HXBOOL UTF16Valid::IsBaseChar(const UINT16 c)
{
    return (bsearch(&c, pBaseChar, sizeof(pBaseChar)/sizeof(Node), sizeof(Node), compare) ? TRUE : FALSE );
}


inline HXBOOL UTF16Valid::IsIdeographic(const UINT16 c)
{
    return BETWEEN(c, 0x4E00, 0x9FA5) || c == 0x3007 || BETWEEN(c, 0x3021, 0x3029);
}


static const Node pCombiningChar[] = {
        { 0x0300, 0x0345 }, { 0x0360, 0x0361 }, { 0x0483, 0x0486 }, { 0x0591, 0x05A1 },
        { 0x05A3, 0x05B9 }, { 0x05BB, 0x05BD }, { 0x05BF, 0x05BF }, { 0x05C1, 0x05C2 },
        { 0x05C4, 0x05C4 }, { 0x064B, 0x0652 }, { 0x0670, 0x0670 }, { 0x06D6, 0x06DC },
        { 0x06DD, 0x06DF }, { 0x06E0, 0x06E4 }, { 0x06E7, 0x06E8 }, { 0x06EA, 0x06ED },
        { 0x0901, 0x0903 }, { 0x093C, 0x093C }, { 0x093E, 0x094C }, { 0x094D, 0x094D },
        { 0x0951, 0x0954 }, { 0x0962, 0x0963 }, { 0x0981, 0x0983 }, { 0x09BC, 0x09BC },
        { 0x09BE, 0x09BE }, { 0x09BF, 0x09BF }, { 0x09C0, 0x09C4 }, { 0x09C7, 0x09C8 },
        { 0x09CB, 0x09CD }, { 0x09D7, 0x09D7 }, { 0x09E2, 0x09E3 }, { 0x0A02, 0x0A02 },
        { 0x0A3C, 0x0A3C }, { 0x0A3E, 0x0A3E }, { 0x0A3F, 0x0A3F }, { 0x0A40, 0x0A42 },
        { 0x0A47, 0x0A48 }, { 0x0A4B, 0x0A4D }, { 0x0A70, 0x0A71 }, { 0x0A81, 0x0A83 },
        { 0x0ABC, 0x0ABC }, { 0x0ABE, 0x0AC5 }, { 0x0AC7, 0x0AC9 }, { 0x0ACB, 0x0ACD },
        { 0x0B01, 0x0B03 }, { 0x0B3C, 0x0B3C }, { 0x0B3E, 0x0B43 }, { 0x0B47, 0x0B48 },
        { 0x0B4B, 0x0B4D }, { 0x0B56, 0x0B57 }, { 0x0B82, 0x0B83 }, { 0x0BBE, 0x0BC2 },
        { 0x0BC6, 0x0BC8 }, { 0x0BCA, 0x0BCD }, { 0x0BD7, 0x0BD7 }, { 0x0C01, 0x0C03 },
        { 0x0C3E, 0x0C44 }, { 0x0C46, 0x0C48 }, { 0x0C4A, 0x0C4D }, { 0x0C55, 0x0C56 },
        { 0x0C82, 0x0C83 }, { 0x0CBE, 0x0CC4 }, { 0x0CC6, 0x0CC8 }, { 0x0CCA, 0x0CCD },
        { 0x0CD5, 0x0CD6 }, { 0x0D02, 0x0D03 }, { 0x0D3E, 0x0D43 }, { 0x0D46, 0x0D48 },
        { 0x0D4A, 0x0D4D }, { 0x0D57, 0x0D57 }, { 0x0E31, 0x0E31 }, { 0x0E34, 0x0E3A },
        { 0x0E47, 0x0E4E }, { 0x0EB1, 0x0EB1 }, { 0x0EB4, 0x0EB9 }, { 0x0EBB, 0x0EBC },
        { 0x0EC8, 0x0ECD }, { 0x0F18, 0x0F19 }, { 0x0F35, 0x0F35 }, { 0x0F37, 0x0F37 },
        { 0x0F39, 0x0F39 }, { 0x0F3E, 0x0F3E }, { 0x0F3F, 0x0F3F }, { 0x0F71, 0x0F84 },
        { 0x0F86, 0x0F8B }, { 0x0F90, 0x0F95 }, { 0x0F97, 0x0F97 }, { 0x0F99, 0x0FAD },
        { 0x0FB1, 0x0FB7 }, { 0x0FB9, 0x0FB9 }, { 0x20D0, 0x20DC }, { 0x20E1, 0x20E1 },
        { 0x302A, 0x302F }, { 0x3099, 0x3099 }, { 0x309A, 0x309A }
};
static const UINT32 ulCombiningCharCount = 95;


inline HXBOOL UTF16Valid::IsCombiningChar(const UINT16 c)
{
    return (bsearch(&c, pCombiningChar, sizeof(pCombiningChar)/sizeof(Node), sizeof(Node), compare) ? TRUE : FALSE );
}

static const Node pDigitChar[] = {
        { 0x0030, 0x0039 }, { 0x0660, 0x0669 }, { 0x06F0, 0x06F9 }, { 0x0966, 0x096F },
        { 0x09E6, 0x09EF }, { 0x0A66, 0x0A6F }, { 0x0AE6, 0x0AEF }, { 0x0B66, 0x0B6F },
        { 0x0BE7, 0x0BEF }, { 0x0C66, 0x0C6F }, { 0x0CE6, 0x0CEF }, { 0x0D66, 0x0D6F },
        { 0x0E50, 0x0E59 }, { 0x0ED0, 0x0ED9 }, { 0x0F20, 0x0F29 }
};

// XXXNH: this doesn't appear to be used anywhere!
//UINT32 ulDigitCharCount = 15;

inline HXBOOL UTF16Valid::IsDigit(const UINT16 c)
{
    return (bsearch(&c, pDigitChar, sizeof(pDigitChar)/sizeof(Node), sizeof(Node), compare) ? TRUE : FALSE );
}

inline HXBOOL UTF16Valid::IsExtender(const UINT16 c)
{
    return c == 0xB7 || c == 0x02D0 || c == 0x02D1 
    || c == 0x0387 || c == 0x0640 || c == 0x0E46 
    || c == 0x0EC6 || 0x3005 || BETWEEN(c, 0x3031, 0x3035)
    || BETWEEN(c, 0x309D, 0x309E) || BETWEEN(c, 0x30FC, 0x30FE);
}

#ifdef XXXJHUG

void OutputTree(char* name, char* array)
{
    printf("\n\n");
    printf("static const Node p%s[] = { \n\t", name);
    int num = -1;
    char* seps = "[] |";
    char* token = strtok( array, seps);
    int count = 0;
    while( token != NULL )
    {
	count++;
	if ( num == 3 )
	{
	    printf("\n\t");
	    num = 0;
	}
	else
	{
	    num++;
	}

	if ( *(token + 6) == '-' )
	{
	    // range
	    token[6] = '\0';
	    token++;
	    printf("{ 0%s, 0%s }, ", token, token+7);
	}
	else
	{
	    // single value.
	    token++;
	    printf("{ 0%s, 0%s }, ", token, token);
	}
	/* Get next token: */
	token = strtok( NULL, seps );
    }
    printf("\n};\n");

    printf("UINT32 ul%sCount = %d;\n", name, count);
}

void main()
{
    // output static binary trees..
    char baseChar[] = "[#x0041-#x005A] | [#x0061-#x007A] | [#x00C0-#x00D6] | [#x00D8-#x00F6] | [#x00F8-#x00FF] | [#x0100-#x0131] | [#x0134-#x013E] | [#x0141-#x0148] | [#x014A-#x017E] | [#x0180-#x01C3] | [#x01CD-#x01F0] | [#x01F4-#x01F5] | [#x01FA-#x0217] | [#x0250-#x02A8] | [#x02BB-#x02C1] | #x0386 | [#x0388-#x038A] | #x038C | [#x038E-#x03A1] | [#x03A3-#x03CE] | [#x03D0-#x03D6] | #x03DA | #x03DC | #x03DE | #x03E0 | [#x03E2-#x03F3] | [#x0401-#x040C] | [#x040E-#x044F] | [#x0451-#x045C] | [#x045E-#x0481] | [#x0490-#x04C4] | [#x04C7-#x04C8] | [#x04CB-#x04CC] | [#x04D0-#x04EB] | [#x04EE-#x04F5] | [#x04F8-#x04F9] | [#x0531-#x0556] | #x0559 | [#x0561-#x0586] | [#x05D0-#x05EA] | [#x05F0-#x05F2] | [#x0621-#x063A] | [#x0641-#x064A] | [#x0671-#x06B7] | [#x06BA-#x06BE] | [#x06C0-#x06CE] | [#x06D0-#x06D3] | #x06D5 | [#x06E5-#x06E6] | [#x0905-#x0939] | #x093D | [#x0958-#x0961] | [#x0985-#x098C] | [#x098F-#x0990] | [#x0993-#x09A8] | [#x09AA-#x09B0] | #x09B2 | [#x09B6-#x09B9] | [#x09DC-#x09DD] | [#x09DF-#x09E1] | [#x09F0-#x09F1] | [#x0A05-#x0A0A] | [#x0A0F-#x0A10] | [#x0A13-#x0A28] | [#x0A2A-#x0A30] | [#x0A32-#x0A33] | [#x0A35-#x0A36] | [#x0A38-#x0A39] | [#x0A59-#x0A5C] | #x0A5E | [#x0A72-#x0A74] | [#x0A85-#x0A8B] | #x0A8D | [#x0A8F-#x0A91] | [#x0A93-#x0AA8] | [#x0AAA-#x0AB0] | [#x0AB2-#x0AB3] | [#x0AB5-#x0AB9] | #x0ABD | #x0AE0 | "
			"[#x0B05-#x0B0C] | [#x0B0F-#x0B10] | [#x0B13-#x0B28] | [#x0B2A-#x0B30] | [#x0B32-#x0B33] | [#x0B36-#x0B39] | #x0B3D | [#x0B5C-#x0B5D] | [#x0B5F-#x0B61] | [#x0B85-#x0B8A] | [#x0B8E-#x0B90] | [#x0B92-#x0B95] | [#x0B99-#x0B9A] | #x0B9C | [#x0B9E-#x0B9F] | [#x0BA3-#x0BA4] | [#x0BA8-#x0BAA] | [#x0BAE-#x0BB5] | [#x0BB7-#x0BB9] | [#x0C05-#x0C0C] | [#x0C0E-#x0C10] | [#x0C12-#x0C28] | [#x0C2A-#x0C33] | [#x0C35-#x0C39] | [#x0C60-#x0C61] | [#x0C85-#x0C8C] | [#x0C8E-#x0C90] | [#x0C92-#x0CA8] | [#x0CAA-#x0CB3] | [#x0CB5-#x0CB9] | #x0CDE | [#x0CE0-#x0CE1] | [#x0D05-#x0D0C] | [#x0D0E-#x0D10] | [#x0D12-#x0D28] | [#x0D2A-#x0D39] | [#x0D60-#x0D61] | [#x0E01-#x0E2E] | #x0E30 | [#x0E32-#x0E33] | [#x0E40-#x0E45] | [#x0E81-#x0E82] | #x0E84 | [#x0E87-#x0E88] | #x0E8A | #x0E8D | [#x0E94-#x0E97] | [#x0E99-#x0E9F] | [#x0EA1-#x0EA3] | #x0EA5 | #x0EA7 | [#x0EAA-#x0EAB] | [#x0EAD-#x0EAE] | #x0EB0 | [#x0EB2-#x0EB3] | #x0EBD | [#x0EC0-#x0EC4] | [#x0F40-#x0F47] | [#x0F49-#x0F69] | [#x10A0-#x10C5] | [#x10D0-#x10F6] | #x1100 | [#x1102-#x1103] | [#x1105-#x1107] | #x1109 | [#x110B-#x110C] | [#x110E-#x1112] | #x113C | #x113E | #x1140 | #x114C | #x114E | #x1150 | [#x1154-#x1155] | #x1159 | [#x115F-#x1161] | #x1163 | #x1165 | #x1167 | #x1169 | [#x116D-#x116E] | [#x1172-#x1173] | #x1175 | #x119E | #x11A8 | #x11AB | [#x11AE-#x11AF] | [#x11B7-#x11B8] | #x11BA | [#x11BC-#x11C2] | #x11EB | #x11F0 | #x11F9 | [#x1E00-#x1E9B] | [#x1EA0-#x1EF9] | [#x1F00-#x1F15] | [#x1F18-#x1F1D] | [#x1F20-#x1F45] | [#x1F48-#x1F4D] | [#x1F50-#x1F57] | #x1F59 | #x1F5B | #x1F5D | [#x1F5F-#x1F7D] | [#x1F80-#x1FB4] | [#x1FB6-#x1FBC] | #x1FBE | [#x1FC2-#x1FC4] | [#x1FC6-#x1FCC] | [#x1FD0-#x1FD3] | [#x1FD6-#x1FDB] | [#x1FE0-#x1FEC] | [#x1FF2-#x1FF4] | [#x1FF6-#x1FFC] | #x2126 | [#x212A-#x212B] | #x212E | [#x2180-#x2182] | [#x3041-#x3094] | [#x30A1-#x30FA] | [#x3105-#x312C] | [#xAC00-#xD7A3]";
    
    char combiningChar[] = "[#x0300-#x0345] | [#x0360-#x0361] | [#x0483-#x0486] | [#x0591-#x05A1] | [#x05A3-#x05B9] | [#x05BB-#x05BD] | #x05BF | [#x05C1-#x05C2] | #x05C4 | [#x064B-#x0652] | #x0670 | [#x06D6-#x06DC] | [#x06DD-#x06DF] | [#x06E0-#x06E4] | [#x06E7-#x06E8] | [#x06EA-#x06ED] | [#x0901-#x0903] | #x093C | [#x093E-#x094C] | #x094D | [#x0951-#x0954] | [#x0962-#x0963] | [#x0981-#x0983] | #x09BC | #x09BE | #x09BF | [#x09C0-#x09C4] | [#x09C7-#x09C8] | [#x09CB-#x09CD] | #x09D7 | [#x09E2-#x09E3] | #x0A02 | #x0A3C | #x0A3E | #x0A3F | [#x0A40-#x0A42] | [#x0A47-#x0A48] | [#x0A4B-#x0A4D] | [#x0A70-#x0A71] | [#x0A81-#x0A83] | #x0ABC | [#x0ABE-#x0AC5] | [#x0AC7-#x0AC9] | [#x0ACB-#x0ACD] | [#x0B01-#x0B03] | #x0B3C | [#x0B3E-#x0B43] | [#x0B47-#x0B48] | [#x0B4B-#x0B4D] | [#x0B56-#x0B57] | [#x0B82-#x0B83] | [#x0BBE-#x0BC2] | [#x0BC6-#x0BC8] | [#x0BCA-#x0BCD] | #x0BD7 | [#x0C01-#x0C03] | [#x0C3E-#x0C44] | [#x0C46-#x0C48] | [#x0C4A-#x0C4D] | [#x0C55-#x0C56] | [#x0C82-#x0C83] | [#x0CBE-#x0CC4] | [#x0CC6-#x0CC8] | [#x0CCA-#x0CCD] | [#x0CD5-#x0CD6] | [#x0D02-#x0D03] | [#x0D3E-#x0D43] | [#x0D46-#x0D48] | [#x0D4A-#x0D4D] | #x0D57 | #x0E31 | [#x0E34-#x0E3A] | [#x0E47-#x0E4E] | #x0EB1 | [#x0EB4-#x0EB9] | [#x0EBB-#x0EBC] | [#x0EC8-#x0ECD] | [#x0F18-#x0F19] | #x0F35 | #x0F37 | #x0F39 | #x0F3E | #x0F3F | [#x0F71-#x0F84] | [#x0F86-#x0F8B] | [#x0F90-#x0F95] | #x0F97 | [#x0F99-#x0FAD] | [#x0FB1-#x0FB7] | #x0FB9 | [#x20D0-#x20DC] | #x20E1 | [#x302A-#x302F] | #x3099 | #x309A";
    char digitChar[] = "[#x0030-#x0039] | [#x0660-#x0669] | [#x06F0-#x06F9] | [#x0966-#x096F] | [#x09E6-#x09EF] | [#x0A66-#x0A6F] | [#x0AE6-#x0AEF] | [#x0B66-#x0B6F] | [#x0BE7-#x0BEF] | [#x0C66-#x0C6F] | [#x0CE6-#x0CEF] | [#x0D66-#x0D6F] | [#x0E50-#x0E59] | [#x0ED0-#x0ED9] | [#x0F20-#x0F29]";
    
    OutputTree("BaseChar", baseChar);
    OutputTree("CombiningChar", combiningChar);
    OutputTree("DigitChar", digitChar);

    HXBOOL inRange = FALSE;
    int count = 0;
    for ( const UINT16 i = 0; i < 0xFFFF; ++i )
    {
	HXBOOL b = UTF16Valid::IsBaseChar(i);
	if ( ( b && !inRange) || ( !b && inRange ) )
	{
	    printf("0x%X, ", i);
	    if ( count == 8 )
	    {
		printf("\n");
		count = 0;
	    }
	    count++;
	}
	inRange = b;
    }
    printf("\n\n");
    inRange = FALSE;
    for ( i = 0; i < 0xFFFF; ++i )
    {
	HXBOOL b = UTF16Valid::IsCombiningChar(i);
	if ( ( b && !inRange) || ( !b && inRange ) )
	{
	    printf("0x%X, ", i);
	    if ( count == 8 )
	    {
		printf("\n");
		count = 0;
	    }
	    count++;
	}
	inRange = b;
    }
    printf("\n\n");
    inRange = FALSE;
    for ( i = 0; i < 0xFFFF; ++i )
    {
	HXBOOL b = UTF16Valid::IsDigit(i);
	if ( ( b && !inRange) || ( !b && inRange ) )
	{
	    printf("0x%X, ", i);
	    if ( count == 8 )
	    {
		printf("\n");
		count = 0;
	    }
	    count++;
	}
	inRange = b;
    }
    printf("\n\n");
}

#endif
