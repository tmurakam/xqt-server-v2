/*
 *Copyright (C) 2004 The X/Qt Server Project. All Rights Reserved.
 *
 *Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 *"Software"), to deal in the Software without restriction, including
 *without limitation the rights to use, copy, modify, merge, publish,
 *distribute, sublicense, and/or sell copies of the Software, and to
 *permit persons to whom the Software is furnished to do so, subject to
 *the following conditions:
 *
 *The above copyright notice and this permission notice shall be
 *included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE XFREE86 PROJECT BE LIABLE FOR
 *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 *CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *Except as contained in this notice, the name of the X/Qt Server Project
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from the XFree86 Project.
 *
 * Authors:	Takuya Murakami
 */
/*
 * Define virtual Qt keyboard map
 */

#define	MIN_KEYCODE	8	// ad hoc...

/* keyname              Qt::Key_	KeySym		KeySym(Shift) */
/* Escape     */	K(Escape,	XK_Escape,	NoSymbol)
/* 1          */	K(1,		XK_1,		XK_exclam)
/* 2          */	K(2,		XK_2,		XK_quotedbl)
/* 3          */	K(3,		XK_3,		XK_numbersign)
/* 4          */	K(4,		XK_4,		XK_dollar)
/* 5          */	K(5,		XK_5,		XK_percent)
/* 6          */	K(6,		XK_6,		XK_ampersand)
/* 7          */	K(7,		XK_7,		XK_quoteright)
/* 8          */	K(8,		XK_8,		XK_parenleft)
/* 9          */	K(9,		XK_9,		XK_parenright)
/* 0          */	K(0,		XK_0,		XK_asciitilde)
/* !          */	K(Exclam,	XK_exclam,	XK_exclam)
/* "          */	K(QuoteDbl,	XK_quotedbl,	XK_quotedbl)
/* #          */	K(NumberSign,	XK_numbersign,	XK_numbersign)
/* $          */	K(Dollar,	XK_dollar,	XK_dollar)
/* %          */	K(Percent,	XK_percent,	XK_percent)
/* &          */	K(Ampersand,	XK_ampersand,	XK_ampersand)
/* '          */	K(Apostrophe,	XK_quoteright,	XK_quoteright)
/* (          */	K(ParenLeft,	XK_parenleft,	XK_parenleft)
/* )          */	K(ParenRight,	XK_parenright,	XK_parenright)
/* ~          */	K(AsciiTilde,	XK_asciitilde,	XK_asciitilde)
/* -          */	K(Minus,	XK_minus,	XK_equal)
/* =          */	K(Equal,	XK_equal,	XK_equal)		// Fn+E
/* ^          */	K(AsciiCircum,	XK_asciicircum,	XK_asciicircum)		// Fn+W	
/* BackSpace  */	K(BackSpace,	XK_BackSpace,	NoSymbol)
/* Tab        */	K(Tab,		XK_Tab,		XK_ISO_Left_Tab)
/* Q          */	K(Q,		XK_Q,		NoSymbol)
/* W          */	K(W,		XK_W,		NoSymbol)
/* E          */	K(E,		XK_E,		NoSymbol)
/* R          */	K(R,		XK_R,		NoSymbol)
/* T          */	K(T,		XK_T,		NoSymbol)
/* Y          */	K(Y,		XK_Y,		NoSymbol)
/* U          */	K(U,		XK_U,		NoSymbol)
/* I          */	K(I,		XK_I,		NoSymbol)
/* O          */	K(O,		XK_O,		NoSymbol)
/* P          */	K(P,		XK_P,		NoSymbol)
/* @          */	K(At,		XK_at,		XK_quoteleft)		// Fn+-
/* `          */	K(QuoteLeft,	XK_quoteleft,	NoSymbol)
/* [          */	K(BracketLeft,	XK_bracketleft, XK_braceleft)		// Fn+T
/* {          */	K(BraceLeft,	XK_braceleft,	NoSymbol)		// ???
/* cent       */	K(cent,		XK_cent,	NoSymbol)		// Fn+U
/* Return     */	K(Return,	XK_Return,	NoSymbol)
/* OK         */	K(F33,		XK_Return,	NoSymbol)
/* Control    */	K(Control,	XK_Control_L,	NoSymbol)
/* A          */	K(A,		XK_A,		NoSymbol)
/* S          */	K(S,		XK_S,		NoSymbol)
/* D          */	K(D,		XK_D,		NoSymbol)
/* F          */	K(F,		XK_F,		NoSymbol)
/* G          */	K(G,		XK_G,		NoSymbol)
/* H          */	K(H,		XK_H,		NoSymbol)
/* J          */	K(J,		XK_J,		NoSymbol)
/* K          */	K(K,		XK_K,		NoSymbol)
/* L          */	K(L,		XK_L,		NoSymbol)
/* ;          */	K(Semicolon,	XK_semicolon,	XK_plus)		// Fn+G
/* +          */	K(Plus,		XK_plus,	NoSymbol)		// Fn+R
/* :          */	K(Colon,	XK_colon,	XK_asterisk)		// Fn+H
/* *          */	K(Asterisk,	XK_asterisk,	NoSymbol)		// Fn+K
/* ZenHan     */	K(F21,		XK_Zenkaku_Hankaku,	NoSymbol)
/* Shift      */	K(Shift,	XK_Shift_L,	NoSymbol)
/* ]          */	K(BracketRight,	XK_bracketright,XK_braceright)		// Fn+Y
/* }          */	K(BraceRight,	XK_braceright,	NoSymbol)
/* stering    */	K(sterling,	XK_sterling,	NoSymbol)		// Fn+I
/* Z          */	K(Z,		XK_Z,		NoSymbol)
/* X          */	K(X,		XK_X,		NoSymbol)
/* C          */	K(C,		XK_C,		NoSymbol)
/* V          */	K(V,		XK_V,		NoSymbol)
/* B          */	K(B,		XK_B,		NoSymbol)
/* N          */	K(N,		XK_N,		NoSymbol)
/* M          */	K(M,		XK_M,		NoSymbol)
/* comma      */	K(Comma,	XK_comma,	XK_comma)		// !!!
/* .          */	K(Period,	XK_period,	XK_period)		// !!!
/* /          */	K(Slash,	XK_slash,	XK_slash)		// Shift+,
/* <          */	K(Less,		XK_less,	XK_less)		// Fn+,
/* >          */	K(Greater,	XK_greater,	XK_greater)		// Fn+?
/* ?          */	K(Question,	XK_question,	XK_question)		// Shift+.
/* Alt        */	K(Alt,		XK_Alt_L,	XK_Meta_L)
/* Meta       */	K(Meta,		XK_Meta_L,	NoSymbol)
/* Space      */	K(Space,	XK_space,	NoSymbol)
/* Caps_Lock  */	K(CapsLock,	XK_Caps_Lock,	NoSymbol)
/* F1         */	K(F1,		XK_F1,		NoSymbol)
/* F2         */	K(F2,		XK_F2,		NoSymbol)
/* F3         */	K(F3,		XK_F3,		NoSymbol)
/* F4         */	K(F4,		XK_F4,		NoSymbol)
/* F5         */	K(F5,		XK_F5,		NoSymbol)
/* F6         */	K(F6,		XK_F6,		NoSymbol)
/* F7         */	K(F7,		XK_F7,		NoSymbol)
/* F8         */	K(F8,		XK_F8,		NoSymbol)
/* F9         */	K(F9,		XK_F9,		NoSymbol)
/* F10(Addr.) */	K(F10,		XK_Control_L,	NoSymbol)		// Use Addr As CTRL
/* F11        */	K(F11,		XK_F11,		NoSymbol)
/* F12(Home)  */	K(F12,		XK_Alt_L,	XK_Meta_L)		// Use Home as ALT
/* F13(Mail)  */	K(F13,		XK_F13,		NoSymbol)
/* F14        */	K(F14,		XK_F14,		NoSymbol)
/* HiraKata   */	K(F15,		XK_F15,		NoSymbol)
/* F16        */	K(F16,		XK_F16,		NoSymbol)
/* F17        */	K(F17,		XK_F17,		NoSymbol)
/* Insert     */	K(Insert,	XK_Insert,	NoSymbol)
/* Delete     */	K(Delete,	XK_Delete,	NoSymbol)
/* Pause      */	K(Pause,	66,		NoSymbol)
/* Left       */	K(Left,		XK_Left,	NoSymbol)
/* Up         */	K(Up,		XK_Up,		NoSymbol)
/* Right      */	K(Right,	XK_Right,	NoSymbol)
/* Down       */	K(Down,		XK_Down,	NoSymbol)
/* Begin      */	/*K(Begin,	XK_Begin,	NoSymbol)*/
/* End        */	K(End,		XK_End,		NoSymbol)
/* Prior      */	K(Prior,	XK_Prior,	NoSymbol)
/* Next       */	K(Next,		XK_Next,	NoSymbol)
/* NumLock    */	K(NumLock,	XK_Num_Lock,	NoSymbol)
/* ScrollLock */	K(ScrollLock,	XK_Scroll_Lock,	NoSymbol)
/* Break      */	/*K(Break,	XK_Break,	NoSymbol)*/
/* Print      */	K(Print,	XK_Print,	NoSymbol)
/* \          */	K(Backslash,	XK_backslash,	XK_backslash)		// Fn+K
/* _          */	K(Underscore,	XK_underscore,	XK_underscore)		// no key?
/* |          */	K(Bar,		XK_bar,		XK_bar)			// Fn+L

// --- Sharp SL-Cxxx series ---
/* \          */	KD(0x2022,	XK_backslash,	NoSymbol)
/* Fn+1       */	KD(0x200f,	XK_1,		NoSymbol)	
/* Fn+2       */	KD(0x2010,	XK_2,		NoSymbol)	
/* Fn+3       */	KD(0x2012,	XK_3,		NoSymbol)	
/* Fn+4       */	KD(0x2011,	XK_4,		NoSymbol)	
/* Fn+5       */	KD(0x201b,	XK_5,		NoSymbol)	
/* Fn+6       */	KD(0x200a,	XK_6,		NoSymbol)	
/* Fn+7       */	KD(0x200b,	XK_7,		NoSymbol)	
/* Fn+8       */	KD(0x200c,	XK_8,		NoSymbol)	
/* Fn+9       */	KD(0x2007,	XK_9,		NoSymbol)	
/* Fn+0       */	KD(0x2008,	XK_0,		NoSymbol)	
/* Fn+Q       */	KD(0x2014,	XK_Q,		NoSymbol)	
/* Fn+O       */	KD(0x2015,	XK_O,		NoSymbol)	
/* Fn+P       */	KD(0x2016,	XK_P,		NoSymbol)	
/* Fn+A       */	KD(0x2017,	XK_A,		NoSymbol)	
/* Fn+S       */	KD(0x2018,	XK_S,		NoSymbol)	
/* Fn+D       */	KD(0x00a5,	XK_D,		NoSymbol)	
/* Fn+N       */	KD(0x2019,	XK_N,		NoSymbol)	
/* Fn+M       */	KD(0x201a,	XK_M,		NoSymbol)
