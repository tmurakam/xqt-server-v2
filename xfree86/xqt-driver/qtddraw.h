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
 * �ʲ�������� RR_Rotate_* ��Ʊ���ͤǤʤ���Фʤ�ʤ���
 */
#define RR_0	1
#define RR_90	2
#define RR_180	4
#define RR_270	8


/*
 * Shadow buffer �δ�����Ԥ����饹
 * �ʲ��ν�����Ԥ�
 *
 *  - Shadow buffer �δ���
 *  - ��ɸ�Ѵ�
 *  - ����(�ե졼��Хåե��ؤ�ž��)����
 */
class qtShadow
{
private:
	/* frame buffer �ѥ�᡼�� */
	unsigned char *fbBase;		/* �١������ɥ쥹 */
	int	fbBpl;			/* bytes/line */
	int 	fbWidth, fbHeight;	/* width/height */

	/* shadow bufer �ѥ�᡼�� */
	unsigned char	*shaBase;	/* �١������ɥ쥹 */
	int	shaBpl;			/* bytes/line */
	int	shaBpp;			/* bits/pixel */
	int	shaRandr;		/* ��ž�� */

	int	scrRandr;		/* �����꡼���ž�� */

public:
	qtShadow();

	/* �����꡼���ɸ�β�ž�ѥ�᡼�������� */
	void setScrRandr(int angle) { scrRandr = angle; }

	/* �ե졼��Хåե��Υѥ�᡼������ */
	void setFbParam(void *base, int bpl);
	void setFbSize(int w, int h);

	/* shadow buffer �Υѥ�᡼������ */
	void setShaParam(void *base, int bpl, int bpp, int randr);

	/*
	 * ��ɸ�Ѵ�����
	 *
	 * ��ɸ�Ϥϰʲ��Σ��̤꤬���롣
	 *  1) shadow buffer ��κ�ɸ
	 *     �桼��/X���饤����Ȥ˸������ɸ��
	 *
	 *  2) frame buffer ��κ�ɸ
	 *     frame buffer ��κ�ɸ�ϡ�QDirectPainter �� QImage �����褹��Ȥ��˻Ȥ���
	 *
	 *  3) screen ��ɸ
	 *     Qt ��κ�ɸ��
	 *
	 * �̾�� 1) �� 3) ��Ʊ��������xrandr �ǲ�ž�����Ƥ�����Ϥ���롣
	 * SL-C[7/8]?? �Ϥξ�硢2) �� View Style ���κ�ɸ�Ϥ˸��ꤵ��Ƥ��롣
	 */
	void sha2fb(int *x, int *y, int *w = 0, int *h = 0);
	void fb2sha(int *x, int *y, int *w = 0, int *h = 0);

	void fb2scr(int *x, int *y, int *w = 0, int *h = 0);
	void scr2fb(int *x, int *y, int *w = 0, int *h = 0);

	/* �������� */
#ifdef QWS
	void transfer(QDirectPainter *p);
#endif
	void transfer(int x, int y, int w, int h);

private:
	/* �����᥽�å� */
	void rot_x2fb(int rot, int *x, int *y, int *w = 0, int *h = 0);
	void rot_fb2x(int rot, int *x, int *y, int *w = 0, int *h = 0);

	void transfer0(int x, int y, int w, int h);
	void transfer3(int x, int y, int w, int h);

};
