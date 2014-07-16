/***************************************************************************
 *            proc.h
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

#ifndef __PROC_H__
#	define __PROC_H__
#	include "main.h"
#	include <gtk/gtk.h>
	extern gint fgs;
	extern GtkWidget *cl, *crm, *crp, *dBs, *dl, *fst, *kms, *nbk, *neg, *nl, *nz, *pst, *psp, *pt1, *pt2, *sbr, *tx, *wdw, *wsp, *wst, *zpd;
	void coe(GtkWidget*, gpointer);
	void con(GtkWidget*, gpointer);
	void prs(GtkWidget*, gpointer);
	void trs(GtkWidget*, gpointer);
#endif
