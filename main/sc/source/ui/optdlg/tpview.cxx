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
#include "precompiled_scui.hxx"


//------------------------------------------------------------------

#include "scitems.hxx"
#include "tpview.hxx"
#include "global.hxx"
#include "viewopti.hxx"
#include "tabvwsh.hxx"
#include "uiitems.hxx"
#include "scresid.hxx"
#include "docsh.hxx"
#include "sc.hrc"       // -> Slot-IDs
#include "optdlg.hrc"
#include "globstr.hrc"
#include <appoptio.hxx>
#include <scmod.hxx>
#include <svx/dlgutil.hxx>
#include <svx/drawitem.hxx>
#include <svx/xtable.hxx>


// STATIC DATA -----------------------------------------------------------



/*-----------------11.01.97 10.52-------------------
	Optionen Inhalte
--------------------------------------------------*/

ScTpContentOptions::ScTpContentOptions( Window*			pParent,
							 const SfxItemSet&	rArgSet ) :
	SfxTabPage(pParent, ScResId( RID_SCPAGE_CONTENT ), rArgSet),

    aLinesGB( this,         ScResId(GB_LINES          )),
	aGridCB( this, 			ScResId(CB_GRID			)),
	aColorFT( this, 		ScResId(FT_COLOR			)),
	aColorLB( this, 		ScResId(LB_COLOR			)),
	aBreakCB( this,			ScResId(CB_PAGEBREAKS		)),
	aGuideLineCB( this, 	ScResId(CB_GUIDELINE		)),
	aBigHandleCB( this,	 	ScResId(CB_BIGHANDLES		)),

    aSeparator1FL    (this, ScResId(FL_SEPARATOR1 )),
    aDisplayGB( this,   ScResId(GB_DISPLAY)),
	aFormulaCB( this, 	ScResId(CB_FORMULA)),
	aNilCB( this, 		ScResId(CB_NIL	)),
	aAnnotCB( this, 	ScResId(CB_ANNOT	)),
	aValueCB( this, 	ScResId(CB_VALUE	)),
	aAnchorCB( this, 	ScResId(CB_ANCHOR	)),
	aClipMarkCB( this, 	ScResId(CB_CLIP	)),
    aRangeFindCB( this,     ScResId( CB_RFIND     )),

    aObjectGB( this,    ScResId(GB_OBJECT )),
	aObjGrfFT( this, 	ScResId(FT_OBJGRF	)),
	aObjGrfLB( this, 	ScResId(LB_OBJGRF	)),
    aDiagramFT( this,   ScResId(FT_DIAGRAM)),
	aDiagramLB( this, 	ScResId(LB_DIAGRAM)),
	aDrawFT( this, 		ScResId(FT_DRAW	)),
	aDrawLB( this, 		ScResId(LB_DRAW	)),

    aZoomGB( this,      ScResId(GB_ZOOM) ),
    aSyncZoomCB( this,  ScResId(CB_SYNCZOOM) ),

    aSeparator2FL    (this, ScResId(FL_SEPARATOR2)),
    aWindowGB( this,        ScResId(GB_WINDOW         )),
	aRowColHeaderCB(this,	ScResId(CB_ROWCOLHEADER	)),
	aHScrollCB( this, 		ScResId(CB_HSCROLL		)),
	aVScrollCB( this, 		ScResId(CB_VSCROLL		)),
	aTblRegCB( this, 		ScResId(CB_TBLREG			)),
	aOutlineCB( this, 		ScResId(CB_OUTLINE		)),
    pLocalOptions(0)
{
	FreeResource();
    aSeparator1FL.SetStyle( aSeparator1FL.GetStyle() | WB_VERT );
    aSeparator2FL.SetStyle( aSeparator2FL.GetStyle() | WB_VERT );
    SetExchangeSupport();
    Link aSelObjHdl(LINK( this, ScTpContentOptions, SelLbObjHdl ) );
    aObjGrfLB.  SetSelectHdl(aSelObjHdl);
    aDiagramLB. SetSelectHdl(aSelObjHdl);
    aDrawLB.    SetSelectHdl(aSelObjHdl);

    Link aCBHdl(LINK( this, ScTpContentOptions, CBHdl ) );
    aFormulaCB  .SetClickHdl(aCBHdl);
    aNilCB      .SetClickHdl(aCBHdl);
    aAnnotCB    .SetClickHdl(aCBHdl);
    aValueCB    .SetClickHdl(aCBHdl);
    aAnchorCB   .SetClickHdl(aCBHdl);
    aClipMarkCB .SetClickHdl(aCBHdl);

    aVScrollCB  .SetClickHdl(aCBHdl);
    aHScrollCB  .SetClickHdl(aCBHdl);
    aTblRegCB   .SetClickHdl(aCBHdl);
    aOutlineCB  .SetClickHdl(aCBHdl);
    aBreakCB    .SetClickHdl(aCBHdl);
    aGuideLineCB.SetClickHdl(aCBHdl);
    aBigHandleCB.SetClickHdl(aCBHdl);
    aRowColHeaderCB.SetClickHdl(aCBHdl);

    aGridCB     .SetClickHdl( LINK( this, ScTpContentOptions, GridHdl ) );
}
/*-----------------11.01.97 10.52-------------------

--------------------------------------------------*/

ScTpContentOptions::~ScTpContentOptions()
{
	delete pLocalOptions;
}
/*-----------------11.01.97 10.52-------------------

--------------------------------------------------*/

SfxTabPage*	ScTpContentOptions::Create( Window*		pParent,
							  const SfxItemSet&		rCoreSet )
{
	return new ScTpContentOptions(pParent, rCoreSet);
}
/*-----------------11.01.97 10.52-------------------

--------------------------------------------------*/
sal_Bool	ScTpContentOptions::FillItemSet( SfxItemSet& rCoreSet )
{
	sal_Bool bRet = sal_False;
	if(	aFormulaCB	.GetSavedValue() != aFormulaCB	.IsChecked() ||
		aNilCB		.GetSavedValue() != aNilCB		.IsChecked() ||
		aAnnotCB  	.GetSavedValue() != aAnnotCB	.IsChecked() ||
		aValueCB  	.GetSavedValue() != aValueCB  	.IsChecked() ||
		aAnchorCB 	.GetSavedValue() != aAnchorCB 	.IsChecked() ||
		aClipMarkCB	.GetSavedValue() != aClipMarkCB	.IsChecked() ||
		aObjGrfLB   .GetSavedValue() != aObjGrfLB  	.GetSelectEntryPos() ||
		aDiagramLB  .GetSavedValue() != aDiagramLB 	.GetSelectEntryPos() ||
        aDrawLB     .GetSavedValue() != aDrawLB     .GetSelectEntryPos() ||
        aGridCB         .GetSavedValue() != aGridCB.IsChecked() ||
		aRowColHeaderCB	.GetSavedValue() != aRowColHeaderCB.IsChecked() ||
		aHScrollCB      .GetSavedValue() != aHScrollCB     .IsChecked() ||
		aVScrollCB      .GetSavedValue() != aVScrollCB     .IsChecked() ||
		aTblRegCB       .GetSavedValue() != aTblRegCB      .IsChecked() ||
		aOutlineCB      .GetSavedValue() != aOutlineCB     .IsChecked() ||
		aColorLB        .GetSavedValue() != aColorLB       .GetSelectEntryPos() ||
		aBreakCB		.GetSavedValue() != aBreakCB	   .IsChecked() ||
		aGuideLineCB    .GetSavedValue() != aGuideLineCB   .IsChecked() ||
        aBigHandleCB    .GetSavedValue() != aBigHandleCB   .IsChecked())
	{
        pLocalOptions->SetGridColor( aColorLB.GetSelectEntryColor(),
									 aColorLB.GetSelectEntry() );
        rCoreSet.Put(ScTpViewItem(SID_SCVIEWOPTIONS, *pLocalOptions));
		bRet = sal_True;
	}
    if(aRangeFindCB.GetSavedValue() != aRangeFindCB.IsChecked())
    {
        rCoreSet.Put(SfxBoolItem(SID_SC_INPUT_RANGEFINDER, aRangeFindCB.IsChecked()));
        bRet = sal_True;
    }
    if(aSyncZoomCB.GetSavedValue() != aSyncZoomCB.IsChecked())
    {
        rCoreSet.Put(SfxBoolItem(SID_SC_OPT_SYNCZOOM, aSyncZoomCB.IsChecked()));
        bRet = sal_True;
    }


    return bRet;
}
/*-----------------11.01.97 10.53-------------------

--------------------------------------------------*/

void	ScTpContentOptions::Reset( const SfxItemSet& rCoreSet )
{
	const SfxPoolItem* pItem;
	if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SCVIEWOPTIONS, sal_False , &pItem))
		pLocalOptions  = new ScViewOptions(
							((const ScTpViewItem*)pItem)->GetViewOptions() );
	else
		pLocalOptions = new ScViewOptions;
	aFormulaCB	.Check(pLocalOptions->GetOption(VOPT_FORMULAS));
	aNilCB		.Check(pLocalOptions->GetOption(VOPT_NULLVALS));
	aAnnotCB  	.Check(pLocalOptions->GetOption(VOPT_NOTES));
	aValueCB  	.Check(pLocalOptions->GetOption(VOPT_SYNTAX));
	aAnchorCB 	.Check(pLocalOptions->GetOption(VOPT_ANCHOR));
	aClipMarkCB	.Check(pLocalOptions->GetOption(VOPT_CLIPMARKS));

	aObjGrfLB 	.SelectEntryPos( (sal_uInt16)pLocalOptions->GetObjMode(VOBJ_TYPE_OLE) );
	aDiagramLB  .SelectEntryPos( (sal_uInt16)pLocalOptions->GetObjMode(VOBJ_TYPE_CHART) );
	aDrawLB     .SelectEntryPos( (sal_uInt16)pLocalOptions->GetObjMode(VOBJ_TYPE_DRAW) );

	aRowColHeaderCB.Check( pLocalOptions->GetOption(VOPT_HEADER) );
	aHScrollCB .Check( pLocalOptions->GetOption(VOPT_HSCROLL) );
	aVScrollCB .Check( pLocalOptions->GetOption(VOPT_VSCROLL) );
	aTblRegCB  .Check( pLocalOptions->GetOption(VOPT_TABCONTROLS) );
	aOutlineCB .Check( pLocalOptions->GetOption(VOPT_OUTLINER) );

	InitGridOpt();

	aBreakCB.Check( pLocalOptions->GetOption(VOPT_PAGEBREAKS) );
	aGuideLineCB.Check( pLocalOptions->GetOption(VOPT_HELPLINES) );
	aBigHandleCB.Check( pLocalOptions->GetOption(VOPT_BIGHANDLES) );

    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_INPUT_RANGEFINDER, sal_False, &pItem))
		aRangeFindCB.Check(((const SfxBoolItem*)pItem)->GetValue());
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_OPT_SYNCZOOM, sal_False, &pItem))
		aSyncZoomCB.Check(((const SfxBoolItem*)pItem)->GetValue());

    aRangeFindCB.SaveValue();
    aSyncZoomCB.SaveValue();

    aFormulaCB  .SaveValue();
	aNilCB		.SaveValue();
	aAnnotCB  	.SaveValue();
	aValueCB  	.SaveValue();
	aAnchorCB 	.SaveValue();
	aClipMarkCB	.SaveValue();
	aObjGrfLB   .SaveValue();
	aDiagramLB  .SaveValue();
	aDrawLB     .SaveValue();
    aRowColHeaderCB .SaveValue();
	aHScrollCB      .SaveValue();
	aVScrollCB      .SaveValue();
	aTblRegCB       .SaveValue();
	aOutlineCB      .SaveValue();
	aGridCB         .SaveValue();
	aColorLB        .SaveValue();
	aBreakCB		.SaveValue();
	aGuideLineCB    .SaveValue();
	aBigHandleCB	.SaveValue();
}
/*-----------------11.01.97 12.45-------------------

--------------------------------------------------*/

void ScTpContentOptions::ActivatePage( const SfxItemSet& rSet)
{
	const SfxPoolItem* pItem;
	if(SFX_ITEM_SET == rSet.GetItemState(SID_SCVIEWOPTIONS, sal_False , &pItem))
		*pLocalOptions = ((const ScTpViewItem*)pItem)->GetViewOptions();
}
/*-----------------11.01.97 12.45-------------------

--------------------------------------------------*/

int	ScTpContentOptions::DeactivatePage( SfxItemSet* pSetP )
{
    if(pSetP)
        FillItemSet(*pSetP);
	return SfxTabPage::LEAVE_PAGE;
}
/*-----------------11.01.97 13.43-------------------

--------------------------------------------------*/

IMPL_LINK( ScTpContentOptions, SelLbObjHdl, ListBox*, pLb )
{
	sal_uInt16		nSelPos = pLb->GetSelectEntryPos();
	ScVObjMode	eMode	= ScVObjMode(nSelPos);
	ScVObjType	eType	= VOBJ_TYPE_OLE;

	if ( pLb == &aDiagramLB )
		eType = VOBJ_TYPE_CHART;
	else if ( pLb == &aDrawLB )
		eType = VOBJ_TYPE_DRAW;

	pLocalOptions->SetObjMode( eType, eMode );

	return 0;
}

/*-----------------11.01.97 14.25-------------------

--------------------------------------------------*/

IMPL_LINK( ScTpContentOptions, CBHdl, CheckBox*, pBtn )
{
    ScViewOption eOption = VOPT_FORMULAS;
	sal_Bool		 bChecked = pBtn->IsChecked();

	if ( 	  &aFormulaCB 	== pBtn )	eOption = VOPT_FORMULAS;
	else if ( &aNilCB 		== pBtn )	eOption = VOPT_NULLVALS;
	else if ( &aAnnotCB		== pBtn )	eOption = VOPT_NOTES;
	else if ( &aValueCB		== pBtn )	eOption = VOPT_SYNTAX;
	else if ( &aAnchorCB	== pBtn )   eOption = VOPT_ANCHOR;
	else if ( &aClipMarkCB	== pBtn )   eOption = VOPT_CLIPMARKS;
    else if ( &aVScrollCB       == pBtn )   eOption = VOPT_VSCROLL;
	else if ( &aHScrollCB		== pBtn )	eOption = VOPT_HSCROLL;
	else if ( &aTblRegCB		== pBtn )	eOption = VOPT_TABCONTROLS;
	else if ( &aOutlineCB 		== pBtn )	eOption = VOPT_OUTLINER;
	else if ( &aBreakCB			== pBtn )	eOption = VOPT_PAGEBREAKS;
	else if ( &aGuideLineCB 	== pBtn )	eOption = VOPT_HELPLINES;
	else if ( &aBigHandleCB	 	== pBtn )	eOption = VOPT_BIGHANDLES;
	else if ( &aRowColHeaderCB 	== pBtn )	eOption = VOPT_HEADER;

	pLocalOptions->SetOption( eOption, bChecked );

	return 0;
}
/*-----------------11.01.97 13.13-------------------

--------------------------------------------------*/

void ScTpContentOptions::InitGridOpt()
{
	sal_Bool bGrid = pLocalOptions->GetOption( VOPT_GRID );

	aGridCB.Check( bGrid );

	if ( bGrid )
		aColorFT.Enable(), aColorLB.Enable();
	else
		aColorFT.Disable(), aColorLB.Disable();

	if ( aColorLB.GetEntryCount() == 0 )
	{
		SfxObjectShell*	pDocSh = SfxObjectShell::Current();
		// hier koennte auch eine andere DocShell kommen!
		pDocSh = PTR_CAST(ScDocShell, pDocSh);

		XColorListSharedPtr aColorTable;

		if ( pDocSh  )
		{
            const SfxPoolItem* pItem = pDocSh->GetItem( SID_COLOR_TABLE );

			if ( pItem )
				aColorTable = static_cast< const SvxColorTableItem* >(pItem)->GetColorTable();
		}
		else
			aColorTable = XColorList::GetStdColorList();

		if ( !aColorTable.get() )
			return;

		//------------------------------------------------------

		aColorLB.SetUpdateMode( sal_False );

		//	Eintraege aus der Colortable

		long nCount = aColorTable->Count();
		for ( long n=0; n<nCount; n++ )
		{
			XColorEntry* pEntry = aColorTable->GetColor(n);
			aColorLB.InsertEntry( pEntry->GetColor(), pEntry->GetName() );
		}

		//	Standard-Gitterfarbe

		Color aStdCol( SC_STD_GRIDCOLOR );			// wie Default in ScViewOptions
		if ( LISTBOX_ENTRY_NOTFOUND ==
				aColorLB.GetEntryPos( aStdCol ) )
			aColorLB.InsertEntry( aStdCol, ScGlobal::GetRscString( STR_GRIDCOLOR ) );

		aColorLB.SetUpdateMode( sal_True );

		Invalidate();
	}

	//	#79720# also select grid color entry on subsequent calls

	String	aName;
	Color	aCol	= pLocalOptions->GetGridColor( &aName );
	sal_uInt16	nSelPos	= aColorLB.GetEntryPos( aCol );

	if ( LISTBOX_ENTRY_NOTFOUND != nSelPos )
		aColorLB.SelectEntryPos( nSelPos );
	else
		aColorLB.SelectEntryPos( aColorLB.InsertEntry( aCol, aName ) );
}
/*-----------------11.01.97 13.40-------------------

--------------------------------------------------*/

IMPL_LINK( ScTpContentOptions, GridHdl, CheckBox*, pBox )
{
	sal_Bool bChecked = pBox->IsChecked();
	aColorFT.Enable(bChecked);
	aColorLB.Enable(bChecked);
	pLocalOptions->SetOption( VOPT_GRID, bChecked );
	return 0;
}
/*-----------------11.01.97 10.53-------------------

--------------------------------------------------*/

ScTpLayoutOptions::ScTpLayoutOptions( 	Window*	pParent,
										const SfxItemSet&	rArgSet ) :
	SfxTabPage(pParent, ScResId( RID_SCPAGE_LAYOUT ), rArgSet),
    aUnitGB( this,          ScResId(GB_UNIT           )),
    aUnitFT( this,          ScResId(FT_UNIT           )),
    aUnitLB( this,          ScResId(LB_UNIT           )),
    aTabFT( this,           ScResId( FT_TAB           )),
    aTabMF( this,           ScResId( MF_TAB           )),
    aSeparatorFL( this,     ScResId( FL_SEPARATOR         )),
	aLinkGB		(this, ScResId(GB_LINK	)),
    aLinkFT(this, ScResId(FT_UPDATE_LINKS )),
	aAlwaysRB	(this, ScResId(RB_ALWAYS	)),
	aRequestRB	(this, ScResId(RB_REQUEST	)),
	aNeverRB	(this, ScResId(RB_NEVER	)),

    aOptionsGB( this,       ScResId( GB_OPTIONS   )),
	aAlignCB  ( this, 		ScResId( CB_ALIGN		)),
	aAlignLB  ( this, 		ScResId( LB_ALIGN		)),
	aEditModeCB( this, 		ScResId( CB_EDITMODE	)),
	aFormatCB( this, 		ScResId( CB_FORMAT	)),
    aExpRefCB( this,        ScResId( CB_EXPREF    )),
    aMarkHdrCB( this,       ScResId( CB_MARKHDR   )),
	aTextFmtCB( this,       ScResId( CB_TEXTFMT   )),
    aReplWarnCB( this,      ScResId( CB_REPLWARN  )),
    aUnitArr(               ScResId(ST_UNIT           )),
    pDoc(NULL)
{
	FreeResource();
    aSeparatorFL.SetStyle( aSeparatorFL.GetStyle() | WB_VERT );
    SetExchangeSupport();

    aUnitLB.    SetSelectHdl( LINK( this, ScTpLayoutOptions, MetricHdl ) );

    aAlignCB.SetClickHdl(LINK(this, ScTpLayoutOptions, AlignHdl));


	for ( sal_uInt16 i = 0; i < aUnitArr.Count(); ++i )
	{
		String sMetric = aUnitArr.GetStringByPos( i );
		FieldUnit eFUnit = (FieldUnit)aUnitArr.GetValue( i );

		switch ( eFUnit )
		{
			case FUNIT_MM:
			case FUNIT_CM:
			case FUNIT_POINT:
			case FUNIT_PICA:
			case FUNIT_INCH:
			{
				// nur diese Metriken benutzen
				sal_uInt16 nPos = aUnitLB.InsertEntry( sMetric );
				aUnitLB.SetEntryData( nPos, (void*)(long)eFUnit );
			}
            break;
            default:
            {
                // added to avoid warnings
            }
		}
	}

}
/*-----------------11.01.97 10.53-------------------

--------------------------------------------------*/

ScTpLayoutOptions::~ScTpLayoutOptions()
{
}
/*-----------------11.01.97 10.53-------------------

--------------------------------------------------*/

SfxTabPage*	ScTpLayoutOptions::Create( Window*			pParent,
									const SfxItemSet&	rCoreSet )
{
    ScTpLayoutOptions* pNew = new ScTpLayoutOptions(pParent, rCoreSet);
    ScDocShell* pDocSh = PTR_CAST(ScDocShell,SfxObjectShell::Current());

	if(pDocSh!=NULL)
		pNew->SetDocument(pDocSh->GetDocument());
    return pNew;
}
/*-----------------11.01.97 10.53-------------------

--------------------------------------------------*/

sal_Bool	ScTpLayoutOptions::FillItemSet( SfxItemSet& rCoreSet )
{
	sal_Bool bRet = sal_True;
    const sal_uInt16 nMPos = aUnitLB.GetSelectEntryPos();
	if ( nMPos != aUnitLB.GetSavedValue() )
	{
		sal_uInt16 nFieldUnit = (sal_uInt16)(long)aUnitLB.GetEntryData( nMPos );
		rCoreSet.Put( SfxUInt16Item( SID_ATTR_METRIC,
									 (sal_uInt16)nFieldUnit ) );
		bRet = sal_True;
	}

	if(aTabMF.GetText() != aTabMF.GetSavedValue())
	{
		rCoreSet.Put(SfxUInt16Item(SID_ATTR_DEFTABSTOP,
                    sal::static_int_cast<sal_uInt16>( aTabMF.Denormalize(aTabMF.GetValue(FUNIT_TWIP)) )));
		bRet = sal_True;
	}

    ScLkUpdMode nSet=LM_ALWAYS;

	if(aRequestRB.IsChecked())
	{
		nSet=LM_ON_DEMAND;
	}
	else if(aNeverRB.IsChecked())
	{
		nSet=LM_NEVER;
	}

    if(aRequestRB.IsChecked() != aRequestRB.GetSavedValue() ||
            aNeverRB.IsChecked() != aNeverRB.GetSavedValue() )
	{
        if(pDoc)
            pDoc->SetLinkMode(nSet);
        ScAppOptions aAppOptions=SC_MOD()->GetAppOptions();
		aAppOptions.SetLinkMode(nSet );
		SC_MOD()->SetAppOptions(aAppOptions);
        bRet = sal_True;
    }
    if(aAlignCB.GetSavedValue() != aAlignCB.IsChecked())
    {
		rCoreSet.Put(SfxBoolItem(SID_SC_INPUT_SELECTION, aAlignCB.IsChecked()));
        bRet = sal_True;
    }

	if(aAlignLB.GetSavedValue() != aAlignLB.GetSelectEntryPos())
    {
        rCoreSet.Put(SfxUInt16Item(SID_SC_INPUT_SELECTIONPOS, aAlignLB.GetSelectEntryPos()));
        bRet = sal_True;
    }

	if(aEditModeCB.GetSavedValue() != aEditModeCB.IsChecked())
    {
        rCoreSet.Put(SfxBoolItem(SID_SC_INPUT_EDITMODE, aEditModeCB.IsChecked()));
        bRet = sal_True;
    }

	if(aFormatCB.GetSavedValue() != aFormatCB.IsChecked())
    {
        rCoreSet.Put(SfxBoolItem(SID_SC_INPUT_FMT_EXPAND, aFormatCB.IsChecked()));
        bRet = sal_True;
    }


    if(aExpRefCB.GetSavedValue() != aExpRefCB.IsChecked())
    {
        rCoreSet.Put(SfxBoolItem(SID_SC_INPUT_REF_EXPAND, aExpRefCB.IsChecked()));
        bRet = sal_True;
    }

	if(aMarkHdrCB.GetSavedValue() != aMarkHdrCB.IsChecked())
    {
        rCoreSet.Put(SfxBoolItem(SID_SC_INPUT_MARK_HEADER, aMarkHdrCB.IsChecked()));
        bRet = sal_True;
    }

	if(aTextFmtCB.GetSavedValue() != aTextFmtCB.IsChecked())
    {
        rCoreSet.Put(SfxBoolItem(SID_SC_INPUT_TEXTWYSIWYG, aTextFmtCB.IsChecked()));
        bRet = sal_True;
    }

    if( aReplWarnCB.GetSavedValue() != aReplWarnCB.IsChecked() )
    {
        rCoreSet.Put( SfxBoolItem( SID_SC_INPUT_REPLCELLSWARN, aReplWarnCB.IsChecked() ) );
        bRet = sal_True;
    }

    return bRet;
}
/*-----------------11.01.97 10.53-------------------

--------------------------------------------------*/

void	ScTpLayoutOptions::Reset( const SfxItemSet& rCoreSet )
{
	aUnitLB.SetNoSelection();
	if ( rCoreSet.GetItemState( SID_ATTR_METRIC ) >= SFX_ITEM_AVAILABLE )
	{
		const SfxUInt16Item& rItem = (SfxUInt16Item&)rCoreSet.Get( SID_ATTR_METRIC );
		FieldUnit eFieldUnit = (FieldUnit)rItem.GetValue();

		for ( sal_uInt16 i = 0; i < aUnitLB.GetEntryCount(); ++i )
		{
			if ( (FieldUnit)(long)aUnitLB.GetEntryData( i ) == eFieldUnit )
			{
				aUnitLB.SelectEntryPos( i );
				break;
			}
		}
		::SetFieldUnit(aTabMF, eFieldUnit);
	}
	aUnitLB.SaveValue();

    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_ATTR_DEFTABSTOP, sal_False, &pItem))
		aTabMF.SetValue(aTabMF.Normalize(((SfxUInt16Item*)pItem)->GetValue()), FUNIT_TWIP);
	aTabMF.SaveValue();

    aUnitLB         .SaveValue();
	aTabMF			.SaveValue();

    ScLkUpdMode nSet=LM_UNKNOWN;

	if(pDoc!=NULL)
	{
		nSet=pDoc->GetLinkMode();
	}

	if(nSet==LM_UNKNOWN)
	{
		ScAppOptions aAppOptions=SC_MOD()->GetAppOptions();
		nSet=aAppOptions.GetLinkMode();
	}

	switch(nSet)
	{
		case LM_ALWAYS:		aAlwaysRB.	Check();	break;
		case LM_NEVER:		aNeverRB.	Check();	break;
		case LM_ON_DEMAND:	aRequestRB.	Check();	break;
        default:
        {
            // added to avoid warnings
        }
	}
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_INPUT_SELECTION, sal_False, &pItem))
		aAlignCB.Check(((const SfxBoolItem*)pItem)->GetValue());

	if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_INPUT_SELECTIONPOS, sal_False, &pItem))
		aAlignLB.SelectEntryPos(((const SfxUInt16Item*)pItem)->GetValue());

	if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_INPUT_EDITMODE, sal_False, &pItem))
		aEditModeCB.Check(((const SfxBoolItem*)pItem)->GetValue());

	if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_INPUT_FMT_EXPAND, sal_False, &pItem))
		aFormatCB.Check(((const SfxBoolItem*)pItem)->GetValue());


    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_INPUT_REF_EXPAND, sal_False, &pItem))
		aExpRefCB.Check(((const SfxBoolItem*)pItem)->GetValue());

	if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_INPUT_MARK_HEADER, sal_False, &pItem))
		aMarkHdrCB.Check(((const SfxBoolItem*)pItem)->GetValue());

	if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SC_INPUT_TEXTWYSIWYG, sal_False, &pItem))
		aTextFmtCB.Check(((const SfxBoolItem*)pItem)->GetValue());

    if( SFX_ITEM_SET == rCoreSet.GetItemState( SID_SC_INPUT_REPLCELLSWARN, sal_False, &pItem ) )
        aReplWarnCB.Check( ( (const SfxBoolItem*)pItem)->GetValue() );

    aAlignCB    .SaveValue();
	aAlignLB  	.SaveValue();
	aEditModeCB	.SaveValue();
	aFormatCB	.SaveValue();

    aExpRefCB   .SaveValue();
	aMarkHdrCB	.SaveValue();
	aTextFmtCB	.SaveValue();
    aReplWarnCB .SaveValue();
	AlignHdl(&aAlignCB);

    aAlwaysRB.SaveValue();
    aNeverRB.SaveValue();
    aRequestRB.SaveValue();
}

/*-----------------11.01.97 12.46-------------------

--------------------------------------------------*/

void	ScTpLayoutOptions::ActivatePage( const SfxItemSet& /* rCoreSet */ )
{
}
/*-----------------11.01.97 12.46-------------------

--------------------------------------------------*/

int	ScTpLayoutOptions::DeactivatePage( SfxItemSet* pSetP )
{
    if(pSetP)
        FillItemSet(*pSetP);
	return SfxTabPage::LEAVE_PAGE;
}


/*-----------------13.01.97 14.44-------------------
	Metric des Deftabstops umschalten
--------------------------------------------------*/

IMPL_LINK(ScTpLayoutOptions, MetricHdl, ListBox*, EMPTYARG)
{
	const sal_uInt16 nMPos = aUnitLB.GetSelectEntryPos();
	if(nMPos != USHRT_MAX)
	{
		FieldUnit eFieldUnit = (FieldUnit)(long)aUnitLB.GetEntryData( nMPos );
        sal_Int64 nVal =
			aTabMF.Denormalize( aTabMF.GetValue( FUNIT_TWIP ) );
		::SetFieldUnit( aTabMF, eFieldUnit );
		aTabMF.SetValue( aTabMF.Normalize( nVal ), FUNIT_TWIP );
	}

	return 0;
}
/*-----------------11.01.97 15.30-------------------

--------------------------------------------------*/
IMPL_LINK( ScTpLayoutOptions, AlignHdl, CheckBox*, pBox )
{
	aAlignLB.Enable(pBox->IsChecked());
	return 0;
}
