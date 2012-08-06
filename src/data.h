/***************************************************************************
 *            data.h
 *
 *  Sat Dec  4 17:18:14 2010
 *  Copyright  2010  Paul Childs
 *  <pchilds@physics.org>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#ifndef __DATA_H__
#	define __DATA_H__
#	include "main.h"
#	include "util.h"
	extern GArray *l, *R, *z, *kp, *sz1, *nx1, *sz2, *nx2;
	extern GtkWidget *dBs, *kms, *nbk, *neg, *pt1, *pt2, *r0, *ri, *rp, *sbr, *tx, *wk, *wsp, *wst;
	extern GSList *grp;
	extern gint fgs, lc;
	extern gchar *fld, *flr;
	void opd(GtkWidget*, gpointer);
	void prf(GtkPrintOperation*, GtkPrintContext*, int);
	void prg(GtkWidget*, gpointer);
	void prt(GtkWidget*, gpointer);
	void sav(GtkWidget*, gpointer);
	void ssr(GtkWidget*, gpointer);
	void sss(GtkWidget*, gpointer);
#endif
