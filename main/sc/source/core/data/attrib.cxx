/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"



// INCLUDE ---------------------------------------------------------------


#include <com/sun/star/util/CellProtection.hpp>
#include <com/sun/star/util/XProtectable.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include "scitems.hxx"
#include <editeng/eeitem.hxx>

#include <editeng/boxitem.hxx>
#include <editeng/editdata.hxx>
#include <editeng/editeng.hxx>
#include <editeng/editobj.hxx>
#include <editeng/flditem.hxx>

#include "attrib.hxx"
#include "global.hxx"
#include "editutil.hxx"
#include "sc.hrc"
#include "globstr.hrc"

#include "textuno.hxx"	// ScHeaderFooterContentObj

using namespace com::sun::star;

//------------------------------------------------------------------------

TYPEINIT1(ScMergeAttr,		 	SfxPoolItem);
TYPEINIT1_AUTOFACTORY(ScProtectionAttr,     SfxPoolItem);
TYPEINIT1(ScRangeItem,		 	SfxPoolItem);
TYPEINIT1(ScTableListItem,		SfxPoolItem);
TYPEINIT1(ScPageHFItem, 	 	SfxPoolItem);
TYPEINIT1(ScViewObjectModeItem, SfxEnumItem);
TYPEINIT1(ScDoubleItem, 		SfxPoolItem);
TYPEINIT1(ScPageScaleToItem,    SfxPoolItem);

//------------------------------------------------------------------------

//
//		allgemeine Hilfsfunktionen
//

sal_Bool ScHasPriority( const SvxBorderLine* pThis, const SvxBorderLine* pOther )
{
//	  DBG_ASSERT( pThis || pOther, "LineAttr == 0" );

	if (!pThis)
		return sal_False;
	if (!pOther)
		return sal_True;

	sal_uInt16 nThisSize = pThis->GetOutWidth() + pThis->GetDistance() + pThis->GetInWidth();
	sal_uInt16 nOtherSize = pOther->GetOutWidth() + pOther->GetDistance() + pOther->GetInWidth();

	if (nThisSize > nOtherSize)
		return sal_True;
	else if (nThisSize < nOtherSize)
		return sal_False;
	else
	{
		if ( pOther->GetInWidth() && !pThis->GetInWidth() )
			return sal_True;
		else if ( pThis->GetInWidth() && !pOther->GetInWidth() )
			return sal_False;
		else
		{
			return sal_True;			//! ???
		}
	}
}


//
//		Item - Implementierungen
//

//------------------------------------------------------------------------
// Merge
//------------------------------------------------------------------------

ScMergeAttr::ScMergeAttr():
	SfxPoolItem(ATTR_MERGE),
	nColMerge(0),
	nRowMerge(0)
{}

//------------------------------------------------------------------------

ScMergeAttr::ScMergeAttr( SCsCOL nCol, SCsROW nRow):
	SfxPoolItem(ATTR_MERGE),
	nColMerge(nCol),
	nRowMerge(nRow)
{}

//------------------------------------------------------------------------

ScMergeAttr::ScMergeAttr(const ScMergeAttr& rItem):
	SfxPoolItem(ATTR_MERGE)
{
	nColMerge = rItem.nColMerge;
	nRowMerge = rItem.nRowMerge;
}

ScMergeAttr::~ScMergeAttr()
{
}

//------------------------------------------------------------------------

String ScMergeAttr::GetValueText() const
{
	String aString( '(' );
	aString += String::CreateFromInt32( nColMerge );
	aString += ',';
	aString += String::CreateFromInt32( nRowMerge );
	aString += ')';
	return aString;
}

//------------------------------------------------------------------------

int ScMergeAttr::operator==( const SfxPoolItem& rItem ) const
{
	DBG_ASSERT( Which() != rItem.Which() || Type() == rItem.Type(), "which ==, type !=" );
	return (Which() == rItem.Which())
			 && (nColMerge == ((ScMergeAttr&)rItem).nColMerge)
			 && (nRowMerge == ((ScMergeAttr&)rItem).nRowMerge);
}

//------------------------------------------------------------------------

SfxPoolItem* ScMergeAttr::Clone( SfxItemPool * ) const
{
	return new ScMergeAttr(*this);
}

//------------------------------------------------------------------------

SfxPoolItem* ScMergeAttr::Create( SvStream& rStream, sal_uInt16 /* nVer */ ) const
{
	sal_Int16	nCol;
	sal_Int16	nRow;
	rStream >> nCol;
	rStream >> nRow;
	return new ScMergeAttr(static_cast<SCCOL>(nCol),static_cast<SCROW>(nRow));
}

//------------------------------------------------------------------------
// MergeFlag
//------------------------------------------------------------------------

ScMergeFlagAttr::ScMergeFlagAttr():
	SfxInt16Item(ATTR_MERGE_FLAG, 0)
{
}

//------------------------------------------------------------------------

ScMergeFlagAttr::ScMergeFlagAttr(sal_Int16 nFlags):
	SfxInt16Item(ATTR_MERGE_FLAG, nFlags)
{
}

ScMergeFlagAttr::~ScMergeFlagAttr()
{
}

//------------------------------------------------------------------------
// Protection
//------------------------------------------------------------------------

ScProtectionAttr::ScProtectionAttr():
	SfxPoolItem(ATTR_PROTECTION),
	bProtection(sal_True),
	bHideFormula(sal_False),
	bHideCell(sal_False),
	bHidePrint(sal_False)
{
}

//------------------------------------------------------------------------

ScProtectionAttr::ScProtectionAttr( sal_Bool bProtect, sal_Bool bHFormula,
									sal_Bool bHCell, sal_Bool bHPrint):
	SfxPoolItem(ATTR_PROTECTION),
	bProtection(bProtect),
	bHideFormula(bHFormula),
	bHideCell(bHCell),
	bHidePrint(bHPrint)
{
}

//------------------------------------------------------------------------

ScProtectionAttr::ScProtectionAttr(const ScProtectionAttr& rItem):
	SfxPoolItem(ATTR_PROTECTION)
{
	bProtection  = rItem.bProtection;
	bHideFormula = rItem.bHideFormula;
	bHideCell	 = rItem.bHideCell;
	bHidePrint	 = rItem.bHidePrint;
}

ScProtectionAttr::~ScProtectionAttr()
{
}

//------------------------------------------------------------------------

sal_Bool ScProtectionAttr::QueryValue( uno::Any& rVal, sal_uInt8 nMemberId ) const
{
	nMemberId &= ~CONVERT_TWIPS;
    switch ( nMemberId  )
    {
        case 0 :
        {
            util::CellProtection aProtection;
            aProtection.IsLocked        = bProtection;
            aProtection.IsFormulaHidden = bHideFormula;
            aProtection.IsHidden        = bHideCell;
            aProtection.IsPrintHidden   = bHidePrint;
            rVal <<= aProtection;
            break;
        }
        case MID_1 :
            rVal <<= (sal_Bool ) bProtection; break;
        case MID_2 :
            rVal <<= (sal_Bool ) bHideFormula; break;
        case MID_3 :
            rVal <<= (sal_Bool ) bHideCell; break;
        case MID_4 :
            rVal <<= (sal_Bool ) bHidePrint; break;
        default:
            DBG_ERROR("Wrong MemberID!");
            return sal_False;
    }

	return sal_True;
}

sal_Bool ScProtectionAttr::PutValue( const uno::Any& rVal, sal_uInt8 nMemberId )
{
	sal_Bool bRet = sal_False;
    sal_Bool bVal = sal_Bool();
	nMemberId &= ~CONVERT_TWIPS;
    switch ( nMemberId )
    {
        case 0 :
        {
            util::CellProtection aProtection;
            if ( rVal >>= aProtection )
            {
                bProtection  = aProtection.IsLocked;
                bHideFormula = aProtection.IsFormulaHidden;
                bHideCell    = aProtection.IsHidden;
                bHidePrint   = aProtection.IsPrintHidden;
                bRet = sal_True;
            }
            else
            {
                DBG_ERROR("exception - wrong argument");
            }
            break;
        }
        case MID_1 :
            bRet = (rVal >>= bVal); if (bRet) bProtection=bVal; break;
        case MID_2 :
            bRet = (rVal >>= bVal); if (bRet) bHideFormula=bVal; break;
        case MID_3 :
            bRet = (rVal >>= bVal); if (bRet) bHideCell=bVal; break;
        case MID_4 :
            bRet = (rVal >>= bVal); if (bRet) bHidePrint=bVal; break;
        default:
            DBG_ERROR("Wrong MemberID!");
    }

	return bRet;
}

//------------------------------------------------------------------------

String ScProtectionAttr::GetValueText() const
{
	String aValue;
	String aStrYes ( ScGlobal::GetRscString(STR_YES) );
	String aStrNo  ( ScGlobal::GetRscString(STR_NO) );
	sal_Unicode cDelim = ',';

	aValue	= '(';
	aValue += (bProtection	? aStrYes : aStrNo);	aValue += cDelim;
	aValue += (bHideFormula ? aStrYes : aStrNo);	aValue += cDelim;
	aValue += (bHideCell	? aStrYes : aStrNo);	aValue += cDelim;
	aValue += (bHidePrint	? aStrYes : aStrNo);
	aValue += ')';

	return aValue;
}

//------------------------------------------------------------------------

SfxItemPresentation ScProtectionAttr::GetPresentation
	(
		SfxItemPresentation ePres,
        SfxMapUnit /* eCoreMetric */,
        SfxMapUnit /* ePresMetric */,
		String& rText,
        const IntlWrapper* /* pIntl */
	) const
{
	String aStrYes	( ScGlobal::GetRscString(STR_YES) );
	String aStrNo	( ScGlobal::GetRscString(STR_NO) );
	String aStrSep	 = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM( ": " ));
	String aStrDelim = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM( ", " ));

	switch ( ePres )
	{
		case SFX_ITEM_PRESENTATION_NONE:
			rText.Erase();
			break;

		case SFX_ITEM_PRESENTATION_NAMELESS:
			rText = GetValueText();
			break;

		case SFX_ITEM_PRESENTATION_COMPLETE:
			rText  = ScGlobal::GetRscString(STR_PROTECTION); rText += aStrSep;
			rText += (bProtection ? aStrYes : aStrNo);		 rText += aStrDelim;
			rText += ScGlobal::GetRscString(STR_FORMULAS);	 rText += aStrSep;
			rText += (!bHideFormula ? aStrYes : aStrNo);	 rText += aStrDelim;
			rText += ScGlobal::GetRscString(STR_HIDE);		 rText += aStrSep;
			rText += (bHideCell ? aStrYes : aStrNo);		 rText += aStrDelim;
			rText += ScGlobal::GetRscString(STR_PRINT); 	 rText += aStrSep;
			rText += (!bHidePrint ? aStrYes : aStrNo);
			break;

		default:
			ePres = SFX_ITEM_PRESENTATION_NONE;
	}

	return ePres;
}

//------------------------------------------------------------------------

int ScProtectionAttr::operator==( const SfxPoolItem& rItem ) const
{
	DBG_ASSERT( Which() != rItem.Which() || Type() == rItem.Type(), "which ==, type !=" );
	return (Which() == rItem.Which())
			 && (bProtection == ((ScProtectionAttr&)rItem).bProtection)
			 && (bHideFormula == ((ScProtectionAttr&)rItem).bHideFormula)
			 && (bHideCell == ((ScProtectionAttr&)rItem).bHideCell)
			 && (bHidePrint == ((ScProtectionAttr&)rItem).bHidePrint);
}

//------------------------------------------------------------------------

SfxPoolItem* ScProtectionAttr::Clone( SfxItemPool * ) const
{
	return new ScProtectionAttr(*this);
}

//------------------------------------------------------------------------

SfxPoolItem* ScProtectionAttr::Create( SvStream& rStream, sal_uInt16 /* n */ ) const
{
	sal_Bool bProtect;
	sal_Bool bHFormula;
	sal_Bool bHCell;
	sal_Bool bHPrint;

	rStream >> bProtect;
	rStream >> bHFormula;
	rStream >> bHCell;
	rStream >> bHPrint;

	return new ScProtectionAttr(bProtect,bHFormula,bHCell,bHPrint);
}

//------------------------------------------------------------------------

sal_Bool ScProtectionAttr::SetProtection( sal_Bool bProtect)
{
	bProtection =  bProtect;
	return sal_True;
}

//------------------------------------------------------------------------

sal_Bool ScProtectionAttr::SetHideFormula( sal_Bool bHFormula)
{
	bHideFormula = bHFormula;
	return sal_True;
}

//------------------------------------------------------------------------

sal_Bool ScProtectionAttr::SetHideCell( sal_Bool bHCell)
{
	bHideCell = bHCell;
	return sal_True;
}

//------------------------------------------------------------------------

sal_Bool ScProtectionAttr::SetHidePrint( sal_Bool bHPrint)
{
	bHidePrint = bHPrint;
	return sal_True;
}

// -----------------------------------------------------------------------
//		ScRangeItem - Tabellenbereich
// -----------------------------------------------------------------------

int ScRangeItem::operator==( const SfxPoolItem& rAttr ) const
{
	DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );

	return ( aRange == ( (ScRangeItem&)rAttr ).aRange );
}

// -----------------------------------------------------------------------

SfxPoolItem* ScRangeItem::Clone( SfxItemPool* ) const
{
	return new ScRangeItem( *this );
}

//------------------------------------------------------------------------

SfxItemPresentation ScRangeItem::GetPresentation
	(
		SfxItemPresentation ePres,
        SfxMapUnit          /* eCoreUnit */,
        SfxMapUnit          /* ePresUnit */,
		String& 			rText,
        const IntlWrapper* /* pIntl */
	) const
{
	rText.Erase();

	switch ( ePres )
	{
		case SFX_ITEM_PRESENTATION_COMPLETE:
		rText  = ScGlobal::GetRscString(STR_AREA);
		rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ": " ));
//		break;// Durchfallen !!!

		case SFX_ITEM_PRESENTATION_NAMELESS:
		{
			String aText;
			/* Always use OOo:A1 format */
			aRange.Format( aText );
			rText += aText;
		}
		break;

        default:
        {
            // added to avoid warnings
        }
	}

	return ePres;
}

// -----------------------------------------------------------------------
//		ScTableListItem - Liste von Tabellen(-nummern)
// -----------------------------------------------------------------------

ScTableListItem::ScTableListItem( const ScTableListItem& rCpy )
	:	SfxPoolItem ( rCpy.Which() ),
		nCount		( rCpy.nCount )
{
	if ( nCount > 0 )
	{
		pTabArr = new SCTAB [nCount];

		for ( sal_uInt16 i=0; i<nCount; i++ )
			pTabArr[i] = rCpy.pTabArr[i];
	}
	else
		pTabArr = NULL;
}

// -----------------------------------------------------------------------

//UNUSED2008-05  ScTableListItem::ScTableListItem( const sal_uInt16 nWhichP, const List& rList )
//UNUSED2008-05      :   SfxPoolItem ( nWhichP ),
//UNUSED2008-05          nCount      ( 0 ),
//UNUSED2008-05          pTabArr     ( NULL )
//UNUSED2008-05  {
//UNUSED2008-05      SetTableList( rList );
//UNUSED2008-05  }

// -----------------------------------------------------------------------

ScTableListItem::~ScTableListItem()
{
	delete [] pTabArr;
}

// -----------------------------------------------------------------------

ScTableListItem& ScTableListItem::operator=( const ScTableListItem& rCpy )
{
	delete [] pTabArr;

	if ( rCpy.nCount > 0 )
	{
		pTabArr = new SCTAB [rCpy.nCount];
		for ( sal_uInt16 i=0; i<rCpy.nCount; i++ )
			pTabArr[i] = rCpy.pTabArr[i];
	}
	else
		pTabArr = NULL;

	nCount = rCpy.nCount;

	return *this;
}

// -----------------------------------------------------------------------

int ScTableListItem::operator==( const SfxPoolItem& rAttr ) const
{
	DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );

	ScTableListItem&	rCmp   = (ScTableListItem&)rAttr;
	sal_Bool				bEqual = (nCount == rCmp.nCount);

	if ( nCount > 0 )
	{
		sal_uInt16	i=0;

		bEqual = ( pTabArr && rCmp.pTabArr );

		while ( bEqual && i<nCount )
		{
			bEqual = ( pTabArr[i] == rCmp.pTabArr[i] );
			i++;
		}
	}
	return bEqual;
}

// -----------------------------------------------------------------------

SfxPoolItem* ScTableListItem::Clone( SfxItemPool* ) const
{
	return new ScTableListItem( *this );
}

//------------------------------------------------------------------------

SfxItemPresentation ScTableListItem::GetPresentation
	(
		SfxItemPresentation ePres,
        SfxMapUnit			/* eCoreUnit */,
        SfxMapUnit			/* ePresUnit */,
		String& 			rText,
        const IntlWrapper* /* pIntl */
	) const
{
	const sal_Unicode cDelim = ',';

	switch ( ePres )
	{
		case SFX_ITEM_PRESENTATION_NONE:
			rText.Erase();
			return ePres;

		case SFX_ITEM_PRESENTATION_NAMELESS:
			{
			rText  = '(';
			if ( nCount>0 && pTabArr )
				for ( sal_uInt16 i=0; i<nCount; i++ )
				{
					rText += String::CreateFromInt32( pTabArr[i] );
					if ( i<(nCount-1) )
						rText += cDelim;
				}
			rText += ')';
			}
			return ePres;

		case SFX_ITEM_PRESENTATION_COMPLETE:
			rText.Erase();
			return SFX_ITEM_PRESENTATION_NONE;

        default:
        {
            // added to avoid warnings
        }
	}

	return SFX_ITEM_PRESENTATION_NONE;
}

// -----------------------------------------------------------------------

//UNUSED2009-05 sal_Bool ScTableListItem::GetTableList( List& aList ) const
//UNUSED2009-05 {
//UNUSED2009-05     for ( sal_uInt16 i=0; i<nCount; i++ )
//UNUSED2009-05         aList.Insert( new SCTAB( pTabArr[i] ) );
//UNUSED2009-05 
//UNUSED2009-05     return ( nCount > 0 );
//UNUSED2009-05 }

// -----------------------------------------------------------------------

//UNUSED2009-05 void ScTableListItem::SetTableList( const List& rList )
//UNUSED2009-05 {
//UNUSED2009-05     nCount = (sal_uInt16)rList.Count();
//UNUSED2009-05 
//UNUSED2009-05     delete [] pTabArr;
//UNUSED2009-05 
//UNUSED2009-05     if ( nCount > 0 )
//UNUSED2009-05     {
//UNUSED2009-05         pTabArr = new SCTAB [nCount];
//UNUSED2009-05 
//UNUSED2009-05         for ( sal_uInt16 i=0; i<nCount; i++ )
//UNUSED2009-05             pTabArr[i] = *( (SCTAB*)rList.GetObject( i ) );
//UNUSED2009-05     }
//UNUSED2009-05     else
//UNUSED2009-05         pTabArr = NULL;
//UNUSED2009-05 }


// -----------------------------------------------------------------------
//      ScPageHFItem - Daten der Kopf-/Fusszeilen
// -----------------------------------------------------------------------

ScPageHFItem::ScPageHFItem( sal_uInt16 nWhichP )
    :   SfxPoolItem ( nWhichP ),
		pLeftArea	( NULL ),
		pCenterArea ( NULL ),
		pRightArea	( NULL )
{
}

//------------------------------------------------------------------------

ScPageHFItem::ScPageHFItem( const ScPageHFItem& rItem )
	:	SfxPoolItem ( rItem ),
		pLeftArea	( NULL ),
		pCenterArea ( NULL ),
		pRightArea	( NULL )
{
	if ( rItem.pLeftArea )
		pLeftArea = rItem.pLeftArea->Clone();
	if ( rItem.pCenterArea )
		pCenterArea = rItem.pCenterArea->Clone();
	if ( rItem.pRightArea )
		pRightArea = rItem.pRightArea->Clone();
}

//------------------------------------------------------------------------

ScPageHFItem::~ScPageHFItem()
{
	delete pLeftArea;
	delete pCenterArea;
	delete pRightArea;
}

//------------------------------------------------------------------------

sal_Bool ScPageHFItem::QueryValue( uno::Any& rVal, sal_uInt8 /* nMemberId */ ) const
{
	uno::Reference<sheet::XHeaderFooterContent> xContent =
		new ScHeaderFooterContentObj( pLeftArea, pCenterArea, pRightArea );

	rVal <<= xContent;
	return sal_True;
}

sal_Bool ScPageHFItem::PutValue( const uno::Any& rVal, sal_uInt8 /* nMemberId */ )
{
	sal_Bool bRet = sal_False;
	uno::Reference<sheet::XHeaderFooterContent> xContent;
	if ( rVal >>= xContent )
	{
		if ( xContent.is() )
		{
			ScHeaderFooterContentObj* pImp =
					ScHeaderFooterContentObj::getImplementation( xContent );
			if (pImp)
			{
				const EditTextObject* pImpLeft = pImp->GetLeftEditObject();
				delete pLeftArea;
				pLeftArea = pImpLeft ? pImpLeft->Clone() : NULL;

				const EditTextObject* pImpCenter = pImp->GetCenterEditObject();
				delete pCenterArea;
				pCenterArea = pImpCenter ? pImpCenter->Clone() : NULL;

				const EditTextObject* pImpRight = pImp->GetRightEditObject();
				delete pRightArea;
				pRightArea = pImpRight ? pImpRight->Clone() : NULL;

				if ( !pLeftArea || !pCenterArea || !pRightArea )
				{
					// keine Texte auf NULL stehen lassen
					ScEditEngineDefaulter aEngine( EditEngine::CreatePool(), sal_True );
					if (!pLeftArea)
						pLeftArea = aEngine.CreateTextObject();
					if (!pCenterArea)
						pCenterArea = aEngine.CreateTextObject();
					if (!pRightArea)
						pRightArea = aEngine.CreateTextObject();
				}

				bRet = sal_True;
			}
		}
	}

	if (!bRet)
	{
		DBG_ERROR("exception - wrong argument");
	}

	return bRet;
}

//------------------------------------------------------------------------

String ScPageHFItem::GetValueText() const
{
	return String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("ScPageHFItem"));
}

//------------------------------------------------------------------------

int ScPageHFItem::operator==( const SfxPoolItem& rItem ) const
{
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal Which or Type" );

	const ScPageHFItem&	r = (const ScPageHFItem&)rItem;

	return    ScGlobal::EETextObjEqual(pLeftArea,   r.pLeftArea)
		   && ScGlobal::EETextObjEqual(pCenterArea, r.pCenterArea)
		   && ScGlobal::EETextObjEqual(pRightArea,  r.pRightArea);
}

//------------------------------------------------------------------------

SfxPoolItem* ScPageHFItem::Clone( SfxItemPool* ) const
{
	return new ScPageHFItem( *this );
}

//------------------------------------------------------------------------

void lcl_SetSpace( String& rStr, const ESelection& rSel )
{
	// Text durch ein Leerzeichen ersetzen, damit Positionen stimmen:

	xub_StrLen nLen = rSel.nEndPos-rSel.nStartPos;
	rStr.Erase( rSel.nStartPos, nLen-1 );
	rStr.SetChar( rSel.nStartPos, ' ' );
}

sal_Bool lcl_ConvertFields(EditEngine& rEng, const String* pCommands)
{
	sal_Bool bChange = sal_False;
	sal_uInt32 nParCnt = rEng.GetParagraphCount();
	for (sal_uInt32 nPar = 0; nPar<nParCnt; nPar++)
	{
		String aStr = rEng.GetText( nPar );
		xub_StrLen nPos;

		while ((nPos = aStr.Search(pCommands[0])) != STRING_NOTFOUND)
		{
			ESelection aSel( nPar,nPos, nPar,nPos+pCommands[0].Len() );
            rEng.QuickInsertField( SvxFieldItem(SvxPageField(), EE_FEATURE_FIELD), aSel );
			lcl_SetSpace(aStr, aSel ); bChange = sal_True;
		}
		while ((nPos = aStr.Search(pCommands[1])) != STRING_NOTFOUND)
		{
			ESelection aSel( nPar,nPos, nPar,nPos+pCommands[1].Len() );
            rEng.QuickInsertField( SvxFieldItem(SvxPagesField(), EE_FEATURE_FIELD), aSel );
			lcl_SetSpace(aStr, aSel ); bChange = sal_True;
		}
		while ((nPos = aStr.Search(pCommands[2])) != STRING_NOTFOUND)
		{
			ESelection aSel( nPar,nPos, nPar,nPos+pCommands[2].Len() );
            rEng.QuickInsertField( SvxFieldItem(SvxDateField(Date(),SVXDATETYPE_VAR), EE_FEATURE_FIELD), aSel );
			lcl_SetSpace(aStr, aSel ); bChange = sal_True;
		}
		while ((nPos = aStr.Search(pCommands[3])) != STRING_NOTFOUND)
		{
			ESelection aSel( nPar,nPos, nPar,nPos+pCommands[3].Len() );
            rEng.QuickInsertField( SvxFieldItem(SvxTimeField(), EE_FEATURE_FIELD ), aSel );
			lcl_SetSpace(aStr, aSel ); bChange = sal_True;
		}
		while ((nPos = aStr.Search(pCommands[4])) != STRING_NOTFOUND)
		{
			ESelection aSel( nPar,nPos, nPar,nPos+pCommands[4].Len() );
            rEng.QuickInsertField( SvxFieldItem(SvxFileField(), EE_FEATURE_FIELD), aSel );
			lcl_SetSpace(aStr, aSel ); bChange = sal_True;
		}
		while ((nPos = aStr.Search(pCommands[5])) != STRING_NOTFOUND)
		{
			ESelection aSel( nPar,nPos, nPar,nPos+pCommands[5].Len() );
            rEng.QuickInsertField( SvxFieldItem(SvxTableField(), EE_FEATURE_FIELD), aSel );
			lcl_SetSpace(aStr, aSel ); bChange = sal_True;
		}
	}
	return bChange;
}

#define SC_FIELD_COUNT	6

SfxPoolItem* ScPageHFItem::Create( SvStream& rStream, sal_uInt16 nVer ) const
{
	EditTextObject* pLeft	= EditTextObject::Create(rStream);
	EditTextObject* pCenter = EditTextObject::Create(rStream);
	EditTextObject* pRight	= EditTextObject::Create(rStream);

	DBG_ASSERT( pLeft && pCenter && pRight, "Error reading ScPageHFItem" );

	if ( pLeft == NULL   || pLeft->GetParagraphCount() == 0 ||
		 pCenter == NULL || pCenter->GetParagraphCount() == 0 ||
		 pRight == NULL  || pRight->GetParagraphCount() == 0 )
	{
		//	If successfully loaded, each object contains at least one paragraph.
		//	Excel import in 5.1 created broken TextObjects (#67442#) that are
		//	corrected here to avoid saving wrong files again (#90487#).

		ScEditEngineDefaulter aEngine( EditEngine::CreatePool(), sal_True );
		if ( pLeft == NULL || pLeft->GetParagraphCount() == 0 )
		{
			delete pLeft;
			pLeft = aEngine.CreateTextObject();
		}
		if ( pCenter == NULL || pCenter->GetParagraphCount() == 0 )
		{
			delete pCenter;
			pCenter = aEngine.CreateTextObject();
		}
		if ( pRight == NULL || pRight->GetParagraphCount() == 0 )
		{
			delete pRight;
			pRight = aEngine.CreateTextObject();
		}
	}

	if ( nVer < 1 )				// alte Feldbefehle umsetzen
	{
		sal_uInt16 i;
		const String& rDel = ScGlobal::GetRscString( STR_HFCMD_DELIMITER );
		String aCommands[SC_FIELD_COUNT];
		for (i=0; i<SC_FIELD_COUNT; i++)
			aCommands[i] = rDel;
		aCommands[0] += ScGlobal::GetRscString(STR_HFCMD_PAGE);
		aCommands[1] += ScGlobal::GetRscString(STR_HFCMD_PAGES);
		aCommands[2] += ScGlobal::GetRscString(STR_HFCMD_DATE);
		aCommands[3] += ScGlobal::GetRscString(STR_HFCMD_TIME);
		aCommands[4] += ScGlobal::GetRscString(STR_HFCMD_FILE);
		aCommands[5] += ScGlobal::GetRscString(STR_HFCMD_TABLE);
		for (i=0; i<SC_FIELD_COUNT; i++)
			aCommands[i] += rDel;

		ScEditEngineDefaulter aEngine( EditEngine::CreatePool(), sal_True );
		aEngine.SetText(*pLeft);
		if (lcl_ConvertFields(aEngine,aCommands))
		{
			delete pLeft;
			pLeft = aEngine.CreateTextObject();
		}
		aEngine.SetText(*pCenter);
		if (lcl_ConvertFields(aEngine,aCommands))
		{
			delete pCenter;
			pCenter = aEngine.CreateTextObject();
		}
		aEngine.SetText(*pRight);
		if (lcl_ConvertFields(aEngine,aCommands))
		{
			delete pRight;
			pRight = aEngine.CreateTextObject();
		}
	}
	else if ( nVer < 2 )
	{	// nichts tun, SvxFileField nicht gegen SvxExtFileField austauschen
	}

	ScPageHFItem* pItem = new ScPageHFItem( Which() );
	pItem->SetArea( pLeft,	  SC_HF_LEFTAREA   );
	pItem->SetArea( pCenter, SC_HF_CENTERAREA );
	pItem->SetArea( pRight,  SC_HF_RIGHTAREA  );

	return pItem;
}

//------------------------------------------------------------------------

//UNUSED2009-05 class ScFieldChangerEditEngine : public ScEditEngineDefaulter
//UNUSED2009-05 {
//UNUSED2009-05     TypeId      aExtFileId;
//UNUSED2009-05     sal_uInt32      nConvPara;
//UNUSED2009-05     xub_StrLen  nConvPos;
//UNUSED2009-05     sal_Bool        bConvert;
//UNUSED2009-05 
//UNUSED2009-05 public:
//UNUSED2009-05     ScFieldChangerEditEngine( SfxItemPool* pEnginePool, sal_Bool bDeleteEnginePool );
//UNUSED2009-05     virtual     ~ScFieldChangerEditEngine() {}
//UNUSED2009-05 
//UNUSED2009-05     virtual String  CalcFieldValue( const SvxFieldItem& rField, sal_uInt32 nPara,
//UNUSED2009-05                                     sal_uInt16 nPos, Color*& rTxtColor,
//UNUSED2009-05                                     Color*& rFldColor );
//UNUSED2009-05 
//UNUSED2009-05     sal_Bool            ConvertFields();
//UNUSED2009-05 };
//UNUSED2009-05 
//UNUSED2009-05 ScFieldChangerEditEngine::ScFieldChangerEditEngine( SfxItemPool* pEnginePoolP,
//UNUSED2009-05             sal_Bool bDeleteEnginePoolP ) :
//UNUSED2009-05         ScEditEngineDefaulter( pEnginePoolP, bDeleteEnginePoolP ),
//UNUSED2009-05         aExtFileId( TYPE( SvxExtFileField ) ),
//UNUSED2009-05         nConvPara( 0 ),
//UNUSED2009-05         nConvPos( 0 ),
//UNUSED2009-05         bConvert( sal_False )
//UNUSED2009-05 {
//UNUSED2009-05 }
//UNUSED2009-05 
//UNUSED2009-05 String ScFieldChangerEditEngine::CalcFieldValue( const SvxFieldItem& rField,
//UNUSED2009-05             sal_uInt32 nPara, sal_uInt16 nPos, Color*& /* rTxtColor */, Color*& /* rFldColor */ )
//UNUSED2009-05 {
//UNUSED2009-05     const SvxFieldData* pFieldData = rField.GetField();
//UNUSED2009-05     if ( pFieldData && pFieldData->Type() == aExtFileId )
//UNUSED2009-05     {
//UNUSED2009-05         bConvert = sal_True;
//UNUSED2009-05         nConvPara = nPara;
//UNUSED2009-05         nConvPos = nPos;
//UNUSED2009-05     }
//UNUSED2009-05     return EMPTY_STRING;
//UNUSED2009-05 }
//UNUSED2009-05 
//UNUSED2009-05 sal_Bool ScFieldChangerEditEngine::ConvertFields()
//UNUSED2009-05 {
//UNUSED2009-05     sal_Bool bConverted = sal_False;
//UNUSED2009-05     do
//UNUSED2009-05     {
//UNUSED2009-05         bConvert = sal_False;
//UNUSED2009-05         UpdateFields();
//UNUSED2009-05         if ( bConvert )
//UNUSED2009-05         {
//UNUSED2009-05             ESelection aSel( nConvPara, nConvPos, nConvPara, nConvPos+1 );
//UNUSED2009-05             QuickInsertField( SvxFieldItem( SvxFileField(), EE_FEATURE_FIELD), aSel );
//UNUSED2009-05             bConverted = sal_True;
//UNUSED2009-05         }
//UNUSED2009-05     } while ( bConvert );
//UNUSED2009-05     return bConverted;
//UNUSED2009-05 }

void ScPageHFItem::SetLeftArea( const EditTextObject& rNew )
{
	delete pLeftArea;
	pLeftArea = rNew.Clone();
}

//------------------------------------------------------------------------

void ScPageHFItem::SetCenterArea( const EditTextObject& rNew )
{
	delete pCenterArea;
	pCenterArea = rNew.Clone();
}

//------------------------------------------------------------------------

void ScPageHFItem::SetRightArea( const EditTextObject& rNew )
{
	delete pRightArea;
	pRightArea = rNew.Clone();
}

void ScPageHFItem::SetArea( EditTextObject *pNew, int nArea )
{
	switch ( nArea )
	{
		case SC_HF_LEFTAREA:	delete pLeftArea;	pLeftArea   = pNew; break;
		case SC_HF_CENTERAREA:  delete pCenterArea; pCenterArea = pNew; break;
		case SC_HF_RIGHTAREA:	delete pRightArea;  pRightArea  = pNew; break;
		default:
			DBG_ERROR( "New Area?" );
	}
}

//-----------------------------------------------------------------------
//	ScViewObjectModeItem - Darstellungsmodus von ViewObjekten
//-----------------------------------------------------------------------

ScViewObjectModeItem::ScViewObjectModeItem( sal_uInt16 nWhichP )
    : SfxEnumItem( nWhichP, VOBJ_MODE_SHOW )
{
}

//------------------------------------------------------------------------

ScViewObjectModeItem::ScViewObjectModeItem( sal_uInt16 nWhichP, ScVObjMode eMode )
    : SfxEnumItem( nWhichP, sal::static_int_cast<sal_uInt16>(eMode) )
{
}

//------------------------------------------------------------------------

ScViewObjectModeItem::~ScViewObjectModeItem()
{
}

//------------------------------------------------------------------------

SfxItemPresentation ScViewObjectModeItem::GetPresentation
(
	SfxItemPresentation ePres,
    SfxMapUnit          /* eCoreUnit */,
    SfxMapUnit          /* ePresUnit */,
	String&				rText,
    const IntlWrapper* /* pIntl */
)	const
{
	String	aDel = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM(": "));
	rText.Erase();

	switch ( ePres )
	{
		case SFX_ITEM_PRESENTATION_COMPLETE:
		switch( Which() )
		{
			case SID_SCATTR_PAGE_CHARTS:
			rText  = ScGlobal::GetRscString(STR_VOBJ_CHART);
			rText += aDel;
			break;

			case SID_SCATTR_PAGE_OBJECTS:
			rText  = ScGlobal::GetRscString(STR_VOBJ_OBJECT);
			rText += aDel;
			break;

			case SID_SCATTR_PAGE_DRAWINGS:
			rText  = ScGlobal::GetRscString(STR_VOBJ_DRAWINGS);
			rText += aDel;
			break;

			default:
			ePres = SFX_ITEM_PRESENTATION_NAMELESS;//das geht immer!
			break;
		}
//		break; // DURCHFALLEN!!!

		case SFX_ITEM_PRESENTATION_NAMELESS:
		rText += ScGlobal::GetRscString(STR_VOBJ_MODE_SHOW+GetValue());
		break;

        default:
        {
            // added to avoid warnings
        }
	}

	return ePres;
}

//------------------------------------------------------------------------

String ScViewObjectModeItem::GetValueText( sal_uInt16 nVal ) const
{
	DBG_ASSERT( nVal <= VOBJ_MODE_HIDE, "enum overflow!" );

	return ScGlobal::GetRscString( STR_VOBJ_MODE_SHOW + (nVal % 2));
}

//------------------------------------------------------------------------

sal_uInt16 ScViewObjectModeItem::GetValueCount() const
{
	return 2;
}

//------------------------------------------------------------------------

SfxPoolItem* ScViewObjectModeItem::Clone( SfxItemPool* ) const
{
	return new ScViewObjectModeItem( *this );
}

//------------------------------------------------------------------------

sal_uInt16 ScViewObjectModeItem::GetVersion( sal_uInt16 /* nFileVersion */ ) const
{
	return 1;
}

//------------------------------------------------------------------------

SfxPoolItem* ScViewObjectModeItem::Create(
									SvStream&	rStream,
									sal_uInt16		nVersion ) const
{
	if ( nVersion == 0 )
	{
		// alte Version mit AllEnumItem -> mit Mode "Show" erzeugen
		return new ScViewObjectModeItem( Which() );
	}
	else
	{
		sal_uInt16 nVal;
		rStream >> nVal;

		//#i80528# adapt to new range eventually
		if((sal_uInt16)VOBJ_MODE_HIDE < nVal) nVal = (sal_uInt16)VOBJ_MODE_SHOW;

		return new ScViewObjectModeItem( Which(), (ScVObjMode)nVal);
	}
}

// -----------------------------------------------------------------------
//		double
// -----------------------------------------------------------------------

ScDoubleItem::ScDoubleItem( sal_uInt16 nWhichP, double nVal )
    :   SfxPoolItem ( nWhichP ),
		nValue	( nVal )
{
}

//------------------------------------------------------------------------

ScDoubleItem::ScDoubleItem( const ScDoubleItem& rItem )
	:	SfxPoolItem ( rItem )
{
		nValue = rItem.nValue;
}

//------------------------------------------------------------------------

String ScDoubleItem::GetValueText() const
{
	return String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("ScDoubleItem"));
}

//------------------------------------------------------------------------

int ScDoubleItem::operator==( const SfxPoolItem& rItem ) const
{
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal Which or Type" );
    const ScDoubleItem& _rItem = (const ScDoubleItem&)rItem;
	return int(nValue == _rItem.nValue);
        //int(nValue == ((const ScDoubleItem&)rItem).nValue);
}

//------------------------------------------------------------------------

SfxPoolItem* ScDoubleItem::Clone( SfxItemPool* ) const
{
	return new ScDoubleItem( *this );
}

//------------------------------------------------------------------------

SfxPoolItem* ScDoubleItem::Create( SvStream& rStream, sal_uInt16 /* nVer */ ) const
{
	double nTmp=0;
	rStream >> nTmp;

	ScDoubleItem* pItem = new ScDoubleItem( Which(), nTmp );

	return pItem;
}

//------------------------------------------------------------------------

ScDoubleItem::~ScDoubleItem()
{
}


// ============================================================================

ScPageScaleToItem::ScPageScaleToItem() :
    SfxPoolItem( ATTR_PAGE_SCALETO ),
    mnWidth( 0 ),
    mnHeight( 0 )
{
}

ScPageScaleToItem::ScPageScaleToItem( sal_uInt16 nWidth, sal_uInt16 nHeight ) :
    SfxPoolItem( ATTR_PAGE_SCALETO ),
    mnWidth( nWidth ),
    mnHeight( nHeight )
{
}

ScPageScaleToItem::~ScPageScaleToItem()
{
}

ScPageScaleToItem* ScPageScaleToItem::Clone( SfxItemPool* ) const
{
    return new ScPageScaleToItem( *this );
}

int ScPageScaleToItem::operator==( const SfxPoolItem& rCmp ) const
{
    DBG_ASSERT( SfxPoolItem::operator==( rCmp ), "ScPageScaleToItem::operator== - unequal wid or type" );
    const ScPageScaleToItem& rPageCmp = static_cast< const ScPageScaleToItem& >( rCmp );
    return ((mnWidth == rPageCmp.mnWidth) && (mnHeight == rPageCmp.mnHeight)) ? 1 : 0;
}

namespace {
void lclAppendScalePageCount( String& rText, sal_uInt16 nPages )
{
    rText.AppendAscii( ": " );
    if( nPages )
    {
        String aPages( ScGlobal::GetRscString( STR_SCATTR_PAGE_SCALE_PAGES ) );
        aPages.SearchAndReplaceAscii( "%1", String::CreateFromInt32( nPages ) );
        rText.Append( aPages );
    }
    else
        rText.Append( ScGlobal::GetRscString( STR_SCATTR_PAGE_SCALE_AUTO ) );
}
} // namespace

SfxItemPresentation ScPageScaleToItem::GetPresentation(
        SfxItemPresentation ePres, SfxMapUnit, SfxMapUnit, XubString& rText, const IntlWrapper* ) const
{
    rText.Erase();
    if( !IsValid() || (ePres == SFX_ITEM_PRESENTATION_NONE) )
        return SFX_ITEM_PRESENTATION_NONE;

    String aName( ScGlobal::GetRscString( STR_SCATTR_PAGE_SCALETO ) );
    String aValue( ScGlobal::GetRscString( STR_SCATTR_PAGE_SCALE_WIDTH ) );
    lclAppendScalePageCount( aValue, mnWidth );
    aValue.AppendAscii( ", " ).Append( ScGlobal::GetRscString( STR_SCATTR_PAGE_SCALE_HEIGHT ) );
    lclAppendScalePageCount( aValue, mnHeight );

    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
        break;

        case SFX_ITEM_PRESENTATION_NAMEONLY:
            rText = aName;
        break;

        case SFX_ITEM_PRESENTATION_NAMELESS:
            rText = aValue;
        break;

        case SFX_ITEM_PRESENTATION_COMPLETE:
            rText.Assign( aName ).AppendAscii( " (" ).Append( aValue ).Append( ')' );
        break;

        default:
            DBG_ERRORFILE( "ScPageScaleToItem::GetPresentation - unknown presentation mode" );
            ePres = SFX_ITEM_PRESENTATION_NONE;
    }
    return ePres;
}

sal_Bool ScPageScaleToItem::QueryValue( uno::Any& rAny, sal_uInt8 nMemberId ) const
{
    sal_Bool bRet = sal_True;
    switch( nMemberId )
    {
        case SC_MID_PAGE_SCALETO_WIDTH:     rAny <<= mnWidth;   break;
        case SC_MID_PAGE_SCALETO_HEIGHT:    rAny <<= mnHeight;  break;
        default:
            DBG_ERRORFILE( "ScPageScaleToItem::QueryValue - unknown member ID" );
            bRet = sal_False;
    }
    return bRet;
}

sal_Bool ScPageScaleToItem::PutValue( const uno::Any& rAny, sal_uInt8 nMemberId )
{
    sal_Bool bRet = sal_False;
    switch( nMemberId )
    {
        case SC_MID_PAGE_SCALETO_WIDTH:     bRet = rAny >>= mnWidth;    break;
        case SC_MID_PAGE_SCALETO_HEIGHT:    bRet = rAny >>= mnHeight;   break;
        default:
            DBG_ERRORFILE( "ScPageScaleToItem::PutValue - unknown member ID" );
    }
    return bRet;
}

// ============================================================================


