/*
 * Copyright 2012-2014 @marbocub <marbocub@gmail.com>
 * All rights reserved.
 * Distributed under the terms of the MIT license.
 */


#include "PanedView.h"

#include <View.h> 
#include <Cursor.h>


#undef CONTROL_DEBUG

#ifdef CONTROL_DEBUG
# include <cstdio>
# define DEBUG_PRINT(x) {printf("%s:%s:", Name(),__FUNCTION__); printf x;}
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif


uchar cursor_h_paned[] = {
	16, 1, 7, 7,

	0x00, 0x00, // 00000000 00000000
	0x00, 0x00, // 00000000 00000000
	0x04, 0x20, // 00000.00 00.00000
	0x0c, 0x30, // 0000..00 00..0000
	0x14, 0x28, // 000.0.00 00.0.000
	0x27, 0xe4, // 00.00... ...00.00
	0x40, 0x02, // 0.000000 000000.0
	0x80, 0x01, // .0000000 0000000.
	0x40, 0x02, // 0.000000 000000.0
	0x27, 0xe4, // 00.00... ...00.00
	0x14, 0x28, // 000.0.00 00.0.000
	0x0c, 0x30, // 0000..00 00..0000
	0x04, 0x20, // 00000.00 00.00000
	0x00, 0x00, // 00000000 00000000
	0x00, 0x00, // 00000000 00000000
	0x00, 0x00, // 00000000 00000000

	0x00, 0x00, // 00000000 00000000
	0x00, 0x00, // 00000000 00000000
	0x04, 0x20, // 00000.00 00.00000
	0x0c, 0x30, // 0000..00 00..0000
	0x1c, 0x38, // 000...00 00...000
	0x3f, 0xfc, // 00...... ......00
	0x7f, 0xfe, // 0....... .......0
	0xff, 0xff, // ........ ........
	0x7f, 0xfe, // 0....... .......0
	0x3f, 0xfc, // 00...... ......00
	0x1c, 0x38, // 000...00 00...000
	0x0c, 0x30, // 0000..00 00..0000
	0x04, 0x20, // 00000.00 00.00000
	0x00, 0x00, // 00000000 00000000
	0x00, 0x00, // 00000000 00000000
	0x00, 0x00, // 00000000 00000000
	
	0
};


uchar cursor_v_paned[] = {
	16, 1, 7, 7,

	0x01, 0x00, // 0000000. 00000000
	0x02, 0x80, // 000000.0 .0000000
	0x04, 0x40, // 00000.00 0.000000
	0x08, 0x20, // 0000.000 00.00000
	0x10, 0x10, // 000.0000 000.0000
	0x3c, 0x78, // 00....00 0....000
	0x04, 0x40, // 00000.00 0.000000
	0x04, 0x40, // 00000.00 0.000000
	0x04, 0x40, // 00000.00 0.000000
	0x04, 0x40, // 00000.00 0.000000
	0x3c, 0x78, // 00....00 0....000
	0x10, 0x10, // 000.0000 000.0000
	0x08, 0x20, // 0000.000 00.00000
	0x04, 0x40, // 00000.00 0.000000
	0x02, 0x80, // 000000.0 .0000000
	0x01, 0x00, // 0000000. 00000000

	0x01, 0x00, // 0000000. 00000000
	0x03, 0x80, // 000000.. .0000000
	0x07, 0xc0, // 00000... ..000000
	0x0f, 0xe0, // 0000.... ...00000
	0x1f, 0xf0, // 000..... ....0000
	0x3f, 0xf8, // 00...... .....000
	0x07, 0xc0, // 00000... ..000000
	0x07, 0xc0, // 00000... ..000000
	0x07, 0xc0, // 00000... ..000000
	0x07, 0xc0, // 00000... ..000000
	0x3f, 0xf8, // 00...... .....000
	0x1f, 0xf0, // 000..... ....0000
	0x0f, 0xe0, // 0000.... ...00000
	0x07, 0xc0, // 00000... ..000000
	0x03, 0x80, // 000000.. .0000000
	0x01, 0x00, // 0000000. 00000000
	
	0
};


PanedView::PanedView(BRect frame, const char* name, uint32 orientation,
		uint32 paned_follow_mode, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags),
	  mView1(new BView(BRect(0,0,0,0),NULL,B_FOLLOW_NONE,B_WILL_DRAW)),
	  mView2(new BView(BRect(0,0,0,0),NULL,B_FOLLOW_NONE,B_WILL_DRAW)),
	  mChild1(NULL),
	  mChild2(NULL),
	  mPositionLeftTop(-1),
	  mPositionRightBottom(-1),
	  mTopBorder(0),
	  mLeftBorder(0),
	  mRightBorder(0),
	  mBottomBorder(0),
	  mOrientation(orientation),
	  mDragMode(false),
	  mDragStartPoint(0,0),
	  mPanedFollowMode(paned_follow_mode)
{
	AddChild(mView1);
	AddChild(mView2);
}


PanedView::~PanedView()
{
}


void
PanedView::AttachedToWindow()
{
	if (Parent()) {
		SetViewColor(Parent()->ViewColor());
		SetHighColor(Parent()->HighColor());
		SetLowColor (Parent()->LowColor() );
	}
	BView::AttachedToWindow();
	FrameResized(Bounds().Width(), Bounds().Height());
}


void
PanedView::SetViewColor(rgb_color color)
{
	BView::SetViewColor(color);
	mView1->SetViewColor(color);
	mView2->SetViewColor(color);
}


void
PanedView::SetHighColor(rgb_color color)
{
	BView::SetHighColor(color);
	mView1->SetHighColor(color);
	mView2->SetHighColor(color);
}


void
PanedView::SetLowColor(rgb_color color)
{
	BView::SetLowColor(color);
	mView1->SetLowColor(color);
	mView2->SetLowColor(color);
}


void
PanedView::FrameResized(float width, float height)
{
	float pos = 0;

	if (mPositionLeftTop < 0 || mPanedFollowMode == PANED_FOLLOW_HALF) {
		if (mOrientation == H_PANED) {
			pos = width / 2.0;
		} else {
			pos = height / 2.0;
		}
	} else {
		switch (mPanedFollowMode) {
		case PANED_FOLLOW_LEFTTOP:
			pos = mPositionLeftTop;
			break;
		case PANED_FOLLOW_RIGHTBOTTOM:
			if (mOrientation == H_PANED) {
				pos = width - mPositionRightBottom;
			} else {
				pos = height - mPositionRightBottom;
			}
			break;
		default:
			float len;
			if (mOrientation == H_PANED) {
				len = width;
			} else {
				len = height;
			}
			pos = (mPositionLeftTop/(mPositionLeftTop+mPositionRightBottom)) * len;
			break;
		}
	}
	SetSplitPosition(pos);

	BView::FrameResized(width, height);
}


void
PanedView::MouseDown(BPoint point)
{
	mDragMode = true;
	SetMouseEventMask(B_POINTER_EVENTS);
}


void
PanedView::MouseMoved(BPoint point, uint32 transit, const BMessage* message)
{
	if (transit ==  B_ENTERED_VIEW) {
		if (mOrientation == H_PANED) {
			BCursor cursor = BCursor(cursor_h_paned);
			SetViewCursor(&cursor);
		} else {
			BCursor cursor = BCursor(cursor_v_paned);
			SetViewCursor(&cursor);
		}
	}

	if (mDragMode) {
		if (mOrientation == H_PANED) {
			SetSplitPosition(point.x);
		} else {
			SetSplitPosition(point.y);
		}
	}
}


void
PanedView::MouseUp(BPoint point)
{
	if (mDragMode) {
		mDragMode = false;
	}
}


void
PanedView::SetSplitPosition(float s)
{
	BRect	rect0 = Bounds();
	BRect	rect1, rect2;
	float	len = 0;

	if (mOrientation == H_PANED) {
		if (s < rect0.left + SEPARATION_SIZE/2.0)
			s = rect0.left + SEPARATION_SIZE/2.0;
		if (s > rect0.right - SEPARATION_SIZE/2.0)
			s = rect0.right - SEPARATION_SIZE/2.0;

		rect1.top    = rect0.top    + mTopBorder;
		rect1.left   = rect0.left   + mLeftBorder;
		rect1.right  = (int)(s - (SEPARATION_SIZE/2.0));
		rect1.bottom = rect0.bottom - mBottomBorder;

		rect2.top    = rect0.top    + mTopBorder;
		rect2.left   = rect1.right  + 1 + SEPARATION_SIZE;
		rect2.right  = rect0.right  - mRightBorder;
		rect2.bottom = rect0.bottom - mBottomBorder;
	} else {
		if (s < rect0.top + SEPARATION_SIZE/2.0)
			s = rect0.top + SEPARATION_SIZE/2.0;
		if (s > rect0.bottom - SEPARATION_SIZE/2.0)
			s = rect0.bottom - SEPARATION_SIZE/2.0;

		rect1.top    = rect0.top    + mTopBorder;
		rect1.left   = rect0.left   + mLeftBorder;
		rect1.right  = rect0.right  - mRightBorder;
		rect1.bottom = (int)(s - (SEPARATION_SIZE/2.0));

		rect2.top    = rect1.bottom + 1 + SEPARATION_SIZE;
		rect2.left   = rect0.left   + mLeftBorder;
		rect2.right  = rect0.right  - mRightBorder;
		rect2.bottom = rect0.bottom - mBottomBorder;
	}

	if (rect1.top > rect1.bottom)
		rect1.bottom = rect1.top;
	if (rect1.left > rect1.right)
		rect1.right = rect1.left;

	if (rect2.top > rect2.bottom)
		rect2.bottom = rect2.top;
	if (rect2.left > rect2.right)
		rect2.right = rect2.left;

	DEBUG_PRINT(("PanedView::SetSplitPosition: %f\n", s));
	DEBUG_PRINT(("rect0: "));
	DEBUG_PRINT(("(%f,%f)-(%f,%f)\n", rect0.left, rect0.top, rect0.right, rect0.bottom));
	DEBUG_PRINT(("rect1: "));
	DEBUG_PRINT(("(%f,%f)-(%f,%f), w:%f, h:%f\n", rect1.left, rect1.top, rect1.right, rect1.bottom, rect1.Width(), rect1.Height()));
	DEBUG_PRINT(("rect2: "));
	DEBUG_PRINT(("(%f,%f)-(%f,%f), w:%f, h:%f\n", rect2.left, rect2.top, rect2.right, rect2.bottom, rect2.Width(), rect2.Height()));

	mView1->ResizeTo(rect1.Width(), rect1.Height());
	mView1->MoveTo(rect1.left, rect1.top);

	mView2->ResizeTo(rect2.Width(), rect2.Height());
	mView2->MoveTo(rect2.left, rect2.top);

	mPositionLeftTop = s;
	if (mOrientation == H_PANED) {
		mPositionRightBottom = rect0.Width() - s;
	} else {
		mPositionRightBottom = rect0.Height() - s;
	}
}


void
PanedView::SetBorderWidth(float border)
{
	mTopBorder = border;
	mLeftBorder = border;
	mRightBorder = border;
	mBottomBorder = border;
	
	SetSplitPosition(mPositionLeftTop);
}


void
PanedView::SetRightBorder(float border)
{
	mRightBorder = border;
}


void
PanedView::SetChild1(BView* child)
{
	if (mChild1 != NULL) {
		mView1->RemoveChild(mChild1);
		mChild1 = NULL;
	}
	mChild1 = child;
	mView1->AddChild(mChild1);

	SetSplitPosition(mPositionLeftTop);
}


void
PanedView::SetChild2(BView* child)
{
	if (mChild2 != NULL) {
		mView2->RemoveChild(mChild2);
		mChild2 = NULL;
	}
	mChild2 = child;
	mView2->AddChild(mChild2);

	SetSplitPosition(mPositionLeftTop);
}


BView*
PanedView::GetChild1(void)
{
	return mChild1;
}


BView*
PanedView::GetChild2(void)
{
	return mChild2;
}


BRect
PanedView::Bounds1(void)
{
	return mView1->Bounds();
}


BRect
PanedView::Bounds2(void)
{
	return mView2->Bounds();
}
