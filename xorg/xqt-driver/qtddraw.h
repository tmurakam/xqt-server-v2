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

#ifdef QWS
#include <qdirectpainter_qws.h>
#endif

/*
 * 以下の定義は RR_Rotate_* と同じ値でなければならない！
 */
#define RR_0	1
#define RR_90	2
#define RR_180	4
#define RR_270	8


/*
 * Shadow buffer の管理を行うクラス
 * 以下の処理を行う
 *
 *  - Shadow buffer の管理
 *  - 座標変換
 *  - 描画(フレームバッファへの転送)処理
 */
class qtShadow
{
private:
	/* frame buffer パラメータ */
	unsigned char *fbBase;		/* ベースアドレス */
	int	fbBpl;			/* bytes/line */
	int 	fbWidth, fbHeight;	/* width/height */

	/* shadow bufer パラメータ */
	unsigned char	*shaBase;	/* ベースアドレス */
	int	shaBpl;			/* bytes/line */
	int	shaBpp;			/* bits/pixel */
	int	shaRandr;		/* 回転角 */

	int	scrRandr;		/* スクリーン回転角 */

public:
	qtShadow();

	/* スクリーン座標の回転パラメータの設定 */
	void setScrRandr(int angle) { scrRandr = angle; }

	/* フレームバッファのパラメータ設定 */
	void setFbParam(void *base, int bpl);
	void setFbSize(int w, int h);

	/* shadow buffer のパラメータ設定 */
	void setShaParam(void *base, int bpl, int bpp, int randr);

	/*
	 * 座標変換処理
	 *
	 * 座標系は以下の３通りがある。
	 *  1) shadow buffer 上の座標
	 *     ユーザ/Xクライアントに見える座標系
	 *
	 *  2) frame buffer 上の座標
	 *     frame buffer 上の座標系。QDirectPainter や QImage に描画するときに使う。
	 *
	 *  3) screen 座標
	 *     Qt 上の座標系
	 *
	 * 通常は 1) と 3) は同じだが、xrandr で回転させている場合はずれる。
	 * SL-C[7/8]?? 系の場合、2) は View Style 時の座標系に固定されている。
	 */
	void sha2fb(int *x, int *y, int *w = 0, int *h = 0);
	void fb2sha(int *x, int *y, int *w = 0, int *h = 0);

	void fb2scr(int *x, int *y, int *w = 0, int *h = 0);
	void scr2fb(int *x, int *y, int *w = 0, int *h = 0);

	/* 画面描画 */
#ifdef QWS
	void transfer(QDirectPainter *p);
#endif
	void transfer(int x, int y, int w, int h);

private:
	/* 内部メソッド */
	void rot_x2fb(int rot, int *x, int *y, int *w = 0, int *h = 0);
	void rot_fb2x(int rot, int *x, int *y, int *w = 0, int *h = 0);

	void transfer0(int x, int y, int w, int h);
	void transfer3(int x, int y, int w, int h);

};
