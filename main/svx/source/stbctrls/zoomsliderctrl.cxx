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
#include "precompiled_svx.hxx"

// include ---------------------------------------------------------------
#include <svx/zoomsliderctrl.hxx>
#ifndef _STATUS_HXX //autogen
#include <vcl/status.hxx>
#endif
#ifndef _MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
#include <vcl/image.hxx>
#include <svx/zoomslideritem.hxx>
#include <svx/dialmgr.hxx>
#include <svx/dialogs.hrc>

#include <set>

// -----------------------------------------------------------------------

SFX_IMPL_STATUSBAR_CONTROL( SvxZoomSliderControl, SvxZoomSliderItem );

// -----------------------------------------------------------------------

struct SvxZoomSliderControl::SvxZoomSliderControl_Impl
{
	sal_uInt16					mnCurrentZoom;
	sal_uInt16					mnMinZoom;
	sal_uInt16					mnMaxZoom;
	sal_uInt16					mnSliderCenter;
	std::vector< long >		maSnappingPointOffsets;
	std::vector< sal_uInt16 >	maSnappingPointZooms;
	Image					maSliderButton;
	Image					maIncreaseButton;
	Image					maDecreaseButton;
	bool					mbValuesSet;
	bool					mbOmitPaint;

	SvxZoomSliderControl_Impl() :
		mnCurrentZoom( 0 ),
		mnMinZoom( 0 ),
		mnMaxZoom( 0 ),
		mnSliderCenter( 0 ),
		maSnappingPointOffsets(),
		maSnappingPointZooms(),
		maSliderButton(),
		maIncreaseButton(),
		maDecreaseButton(),
		mbValuesSet( false ),
		mbOmitPaint( false ) {}
};

// -----------------------------------------------------------------------

const long nButtonWidth   = 10;
const long nButtonHeight  = 10;
const long nIncDecWidth   = 11;
const long nIncDecHeight  = 11;
const long nSliderHeight  = 2;
const long nSnappingHeight = 4;
const long nSliderXOffset = 20;
const long nSnappingEpsilon = 5; // snapping epsilon in pixels
const long nSnappingPointsMinDist = nSnappingEpsilon; // minimum distance of two adjacent snapping points

// -----------------------------------------------------------------------

// nOffset refers to the origin of the control:
// + ----------- -
sal_uInt16 SvxZoomSliderControl::Offset2Zoom( long nOffset ) const
{
	const long nControlWidth = getControlRect().GetWidth();
	sal_uInt16 nRet = 0;

	if ( nOffset < nSliderXOffset )
		return mpImpl->mnMinZoom;;

	if ( nOffset > nControlWidth - nSliderXOffset )
		return mpImpl->mnMaxZoom;

	// check for snapping points:
	sal_uInt16 nCount = 0;
	std::vector< long >::iterator aSnappingPointIter;
	for ( aSnappingPointIter = mpImpl->maSnappingPointOffsets.begin();
		  aSnappingPointIter != mpImpl->maSnappingPointOffsets.end();
		  ++aSnappingPointIter )
	{
		const long nCurrent = *aSnappingPointIter;
		if ( Abs(nCurrent - nOffset) < nSnappingEpsilon )
		{
			nOffset = nCurrent;
			nRet = mpImpl->maSnappingPointZooms[ nCount ];
			break;
		}
		++nCount;
	}

	if ( 0 == nRet )
	{
		if ( nOffset < nControlWidth / 2 )
		{
			// first half of slider
			const long nFirstHalfRange = mpImpl->mnSliderCenter - mpImpl->mnMinZoom;
			const long nHalfSliderWidth = nControlWidth/2 - nSliderXOffset;
			const long nZoomPerSliderPixel = (1000 * nFirstHalfRange) / nHalfSliderWidth;
			const long nOffsetToSliderLeft = nOffset - nSliderXOffset;
			nRet = mpImpl->mnMinZoom + sal_uInt16( nOffsetToSliderLeft * nZoomPerSliderPixel / 1000 );
		}
		else
		{
			// second half of slider
			const long nSecondHalfRange = mpImpl->mnMaxZoom - mpImpl->mnSliderCenter;
			const long nHalfSliderWidth = nControlWidth/2 - nSliderXOffset;
			const long nZoomPerSliderPixel = 1000 * nSecondHalfRange / nHalfSliderWidth;
			const long nOffsetToSliderCenter = nOffset - nControlWidth/2;
			nRet = mpImpl->mnSliderCenter + sal_uInt16( nOffsetToSliderCenter * nZoomPerSliderPixel / 1000 );
		}
	}

	if ( nRet < mpImpl->mnMinZoom )
		nRet = mpImpl->mnMinZoom;
	else if ( nRet > mpImpl->mnMaxZoom )
		nRet = mpImpl->mnMaxZoom;

	return nRet;
}

// returns the offset to the left control border
long SvxZoomSliderControl::Zoom2Offset( sal_uInt16 nCurrentZoom ) const
{
	const long nControlWidth = getControlRect().GetWidth();
	long nRet = nSliderXOffset;

	const long nHalfSliderWidth = nControlWidth/2 - nSliderXOffset;

	if ( nCurrentZoom <= mpImpl->mnSliderCenter )
	{
		nCurrentZoom = nCurrentZoom - mpImpl->mnMinZoom;
		const long nFirstHalfRange = mpImpl->mnSliderCenter - mpImpl->mnMinZoom;
		const long nSliderPixelPerZoomPercent = 1000 * nHalfSliderWidth  / nFirstHalfRange;
		const long nOffset = (nSliderPixelPerZoomPercent * nCurrentZoom) / 1000;
		nRet += nOffset;
	}
	else
	{
		nCurrentZoom = nCurrentZoom - mpImpl->mnSliderCenter;
		const long nSecondHalfRange = mpImpl->mnMaxZoom - mpImpl->mnSliderCenter;
		const long nSliderPixelPerZoomPercent = 1000 * nHalfSliderWidth  / nSecondHalfRange;
		const long nOffset = (nSliderPixelPerZoomPercent * nCurrentZoom) / 1000;
		nRet += nHalfSliderWidth + nOffset;
	}

	return nRet;
}

// -----------------------------------------------------------------------

SvxZoomSliderControl::SvxZoomSliderControl( sal_uInt16 _nSlotId,  sal_uInt16 _nId, StatusBar& _rStb ) :
	SfxStatusBarControl( _nSlotId, _nId, _rStb ),
	mpImpl( new SvxZoomSliderControl_Impl )
{
	const sal_Bool bHC = GetStatusBar().GetSettings().GetStyleSettings().GetHighContrastMode();
	mpImpl->maSliderButton   = Image( SVX_RES( bHC ? RID_SVXBMP_SLIDERBUTTON_HC : RID_SVXBMP_SLIDERBUTTON ) );
	mpImpl->maIncreaseButton = Image( SVX_RES( bHC ? RID_SVXBMP_SLIDERINCREASE_HC : RID_SVXBMP_SLIDERINCREASE ) );
	mpImpl->maDecreaseButton = Image( SVX_RES( bHC ? RID_SVXBMP_SLIDERDECREASE_HC : RID_SVXBMP_SLIDERDECREASE ) );
}

// -----------------------------------------------------------------------

SvxZoomSliderControl::~SvxZoomSliderControl()
{
	delete mpImpl;
}

// -----------------------------------------------------------------------

void SvxZoomSliderControl::StateChanged( sal_uInt16 /*nSID*/, SfxItemState eState, const SfxPoolItem* pState )
{
	if ( (SFX_ITEM_AVAILABLE != eState) || pState->ISA( SfxVoidItem ) )
	{
		GetStatusBar().SetItemText( GetId(), String() );
		mpImpl->mbValuesSet   = false;
	}
	else
	{
		OSL_ENSURE( pState->ISA( SvxZoomSliderItem ), "invalid item type: should be a SvxZoomSliderItem" );
		mpImpl->mnCurrentZoom = static_cast<const SvxZoomSliderItem*>( pState )->GetValue();
		mpImpl->mnMinZoom     = static_cast<const SvxZoomSliderItem*>( pState )->GetMinZoom();
		mpImpl->mnMaxZoom     = static_cast<const SvxZoomSliderItem*>( pState )->GetMaxZoom();
		mpImpl->mnSliderCenter= 100;
		mpImpl->mbValuesSet   = true;

		if ( mpImpl->mnSliderCenter == mpImpl->mnMaxZoom )
			mpImpl->mnSliderCenter = mpImpl->mnMinZoom + (sal_uInt16)((mpImpl->mnMaxZoom - mpImpl->mnMinZoom) * 0.5);


		DBG_ASSERT( mpImpl->mnMinZoom <= mpImpl->mnCurrentZoom &&
					mpImpl->mnMinZoom <  mpImpl->mnSliderCenter &&
					mpImpl->mnMaxZoom >= mpImpl->mnCurrentZoom &&
					mpImpl->mnMaxZoom > mpImpl->mnSliderCenter,
					"Looks like the zoom slider item is corrupted" );

		const com::sun::star::uno::Sequence < sal_Int32 > rSnappingPoints = static_cast<const SvxZoomSliderItem*>( pState )->GetSnappingPoints();
		mpImpl->maSnappingPointOffsets.clear();
		mpImpl->maSnappingPointZooms.clear();

		// get all snapping points:
		std::set< sal_uInt16 > aTmpSnappingPoints;
		for ( sal_uInt16 j = 0; j < rSnappingPoints.getLength(); ++j )
		{
			const sal_Int32 nSnappingPoint = rSnappingPoints[j];
			aTmpSnappingPoints.insert( (sal_uInt16)nSnappingPoint );
		}

		// remove snapping points that are to close to each other:
		std::set< sal_uInt16 >::iterator aSnappingPointIter;
		long nLastOffset = 0;

		for ( aSnappingPointIter = aTmpSnappingPoints.begin(); aSnappingPointIter != aTmpSnappingPoints.end(); ++aSnappingPointIter )
		{
			const sal_uInt16 nCurrent = *aSnappingPointIter;
			const long nCurrentOffset = Zoom2Offset( nCurrent );

			if ( nCurrentOffset - nLastOffset >= nSnappingPointsMinDist )
			{
				mpImpl->maSnappingPointOffsets.push_back( nCurrentOffset );
				mpImpl->maSnappingPointZooms.push_back( nCurrent );
				nLastOffset = nCurrentOffset;
			}
		}
	}

	if ( !mpImpl->mbOmitPaint && GetStatusBar().AreItemsVisible() )
		GetStatusBar().SetItemData( GetId(), 0 ); // force repaint
}

// -----------------------------------------------------------------------

void SvxZoomSliderControl::Paint( const UserDrawEvent& rUsrEvt )
{
	if ( !mpImpl->mbValuesSet || mpImpl->mbOmitPaint )
		return;

	const Rectangle		aControlRect = getControlRect();
	OutputDevice*		pDev =  rUsrEvt.GetDevice();
	Rectangle			aRect = rUsrEvt.GetRect();
	Rectangle			aSlider = aRect;

	aSlider.Top()   += (aControlRect.GetHeight() - nSliderHeight)/2 - 1;
	aSlider.Bottom() = aSlider.Top() + nSliderHeight;
	aSlider.Left()  += nSliderXOffset;
	aSlider.Right() -= nSliderXOffset;

	Color				aOldLineColor = pDev->GetLineColor();
	Color				aOldFillColor = pDev->GetFillColor();

	pDev->SetLineColor( Color( COL_GRAY ) );
	pDev->SetFillColor( Color( COL_GRAY ) );

	// draw snapping points:
	std::vector< long >::iterator aSnappingPointIter;
	for ( aSnappingPointIter = mpImpl->maSnappingPointOffsets.begin();
		  aSnappingPointIter != mpImpl->maSnappingPointOffsets.end();
		  ++aSnappingPointIter )
	{
		Rectangle aSnapping( aRect );
		aSnapping.Bottom()   = aSlider.Top();
		aSnapping.Top() = aSnapping.Bottom() - nSnappingHeight;
		aSnapping.Left() += *aSnappingPointIter;
		aSnapping.Right() = aSnapping.Left();
		pDev->DrawRect( aSnapping );

		aSnapping.Top() += nSnappingHeight + nSliderHeight;
		aSnapping.Bottom() += nSnappingHeight + nSliderHeight;
		pDev->DrawRect( aSnapping );
	}

	// draw slider
	Rectangle aFirstLine( aSlider );
	aFirstLine.Bottom() = aFirstLine.Top();

	Rectangle aSecondLine( aSlider );
	aSecondLine.Top() = aSecondLine.Bottom();

	Rectangle aLeft( aSlider );
	aLeft.Right() = aLeft.Left();

	Rectangle aRight( aSlider );
	aRight.Left() = aRight.Right();

	pDev->SetLineColor( Color ( COL_GRAY ) );
	pDev->SetFillColor( Color ( COL_GRAY ) );
	pDev->DrawRect( aSecondLine );
	pDev->DrawRect( aRight );

	pDev->SetLineColor( Color( COL_GRAY ) );
	pDev->SetFillColor( Color( COL_GRAY ) );
	pDev->DrawRect( aFirstLine );
	pDev->DrawRect( aLeft );

	// draw slider button
	Point aImagePoint = aRect.TopLeft();
	aImagePoint.X() += Zoom2Offset( mpImpl->mnCurrentZoom );
	aImagePoint.X() -= nButtonWidth/2;
	aImagePoint.Y() += (aControlRect.GetHeight() - nButtonHeight)/2;
	pDev->DrawImage( aImagePoint, mpImpl->maSliderButton );

	// draw decrease button
	aImagePoint = aRect.TopLeft();
	aImagePoint.X() += (nSliderXOffset - nIncDecWidth)/2;
	aImagePoint.Y() += (aControlRect.GetHeight() - nIncDecHeight)/2;
	pDev->DrawImage( aImagePoint, mpImpl->maDecreaseButton );

	// draw increase button
	aImagePoint.X() = aRect.TopLeft().X() + aControlRect.GetWidth() - nIncDecWidth - (nSliderXOffset - nIncDecWidth)/2;
	pDev->DrawImage( aImagePoint, mpImpl->maIncreaseButton );

	pDev->SetLineColor( aOldLineColor );
	pDev->SetFillColor( aOldFillColor );
}

// -----------------------------------------------------------------------

sal_Bool SvxZoomSliderControl::MouseButtonDown( const MouseEvent & rEvt )
{
	if ( !mpImpl->mbValuesSet )
		return sal_True;;

	const Rectangle aControlRect = getControlRect();
	const Point aPoint = rEvt.GetPosPixel();
	const sal_Int32 nXDiff = aPoint.X() - aControlRect.Left();

	const long nButtonLeftOffset = (nSliderXOffset - nIncDecWidth)/2;
	const long nButtonRightOffset = (nSliderXOffset + nIncDecWidth)/2;

	const long nOldZoom = mpImpl->mnCurrentZoom;

	// click to - button
	if ( nXDiff >= nButtonLeftOffset && nXDiff <= nButtonRightOffset )
		mpImpl->mnCurrentZoom = mpImpl->mnCurrentZoom - 5;
	// click to + button
	else if ( nXDiff >= aControlRect.GetWidth() - nSliderXOffset + nButtonLeftOffset &&
			  nXDiff <= aControlRect.GetWidth() - nSliderXOffset + nButtonRightOffset )
		mpImpl->mnCurrentZoom = mpImpl->mnCurrentZoom + 5;
	// click to slider
	else if( nXDiff >= nSliderXOffset && nXDiff <= aControlRect.GetWidth() - nSliderXOffset )
		mpImpl->mnCurrentZoom = Offset2Zoom( nXDiff );

	if ( mpImpl->mnCurrentZoom < mpImpl->mnMinZoom )
		mpImpl->mnCurrentZoom = mpImpl->mnMinZoom;
	else if ( mpImpl->mnCurrentZoom > mpImpl->mnMaxZoom )
		mpImpl->mnCurrentZoom = mpImpl->mnMaxZoom;

	if ( nOldZoom == mpImpl->mnCurrentZoom )
		return sal_True;

	if ( GetStatusBar().AreItemsVisible() )
		GetStatusBar().SetItemData( GetId(), 0 ); // force repaint

	mpImpl->mbOmitPaint = true; // optimization: paint before executing command,
								// then omit painting which is triggered by the execute function

	SvxZoomSliderItem aZoomSliderItem( mpImpl->mnCurrentZoom );

	::com::sun::star::uno::Any a;
	aZoomSliderItem.QueryValue( a );

	::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aArgs( 1 );
	aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ZoomSlider" ));
	aArgs[0].Value = a;

	execute( aArgs );

	mpImpl->mbOmitPaint = false;

	return sal_True;
}

// -----------------------------------------------------------------------

sal_Bool SvxZoomSliderControl::MouseMove( const MouseEvent & rEvt )
{
	if ( !mpImpl->mbValuesSet )
		return sal_True;;

	const short nButtons = rEvt.GetButtons();

	// check mouse move with button pressed
	if ( 1 == nButtons )
	{
		const Rectangle aControlRect = getControlRect();
		const Point aPoint = rEvt.GetPosPixel();
		const sal_Int32 nXDiff = aPoint.X() - aControlRect.Left();

		if ( nXDiff >= nSliderXOffset && nXDiff <= aControlRect.GetWidth() - nSliderXOffset )
		{
			mpImpl->mnCurrentZoom = Offset2Zoom( nXDiff );

			if ( GetStatusBar().AreItemsVisible() )
				GetStatusBar().SetItemData( GetId(), 0 ); // force repaint

			mpImpl->mbOmitPaint = true; // optimization: paint before executing command,
										// then omit painting which is triggered by the execute function

			// commit state change
			SvxZoomSliderItem aZoomSliderItem( mpImpl->mnCurrentZoom );

			::com::sun::star::uno::Any a;
			aZoomSliderItem.QueryValue( a );

			::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aArgs( 1 );
			aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ZoomSlider" ));
			aArgs[0].Value = a;

			execute( aArgs );

			mpImpl->mbOmitPaint = false;
		}
	}

	return sal_True;
}

/* vim: set noet sw=4 ts=4: */
