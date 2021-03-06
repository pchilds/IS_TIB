/***************************************************************************
 *            proc.c
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

#include "proc.h"

void coe(GtkWidget *wgt, gpointer dta)
{
	GtkPlot *pt;
	gdouble iv;
	gint sz4;
	gchar *str;

	if ((fgs&PROC_ZDT)!=0)
	{
		pt=GTK_PLOT(pt2);
		sz4=g_array_index((pt->sizes), gint, 0);
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(crm));
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(crp));
	}
	else
	{
		str=g_strdup(_("No spatial profile for modification exists yet."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}

void con(GtkWidget *wgt, gpointer dta)
{
	GtkPlot *pt;
	gdouble iv;
	gint sz4;
	gchar *str;

	if ((fgs&PROC_ZDT)!=0)
	{
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(dta));
		pt=GTK_PLOT(pt2);
		sz4=g_array_index((pt->sizes), gint, 0);
	}
	else
	{
		str=g_strdup(_("No spatial profile for modification exists yet."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}

void prs(GtkWidget *wgt, gpointer dta)
{
	fftw_complex A, B, C;
	GArray *l, *nx1, *R, *st1, *sz1;
	gdouble *cb, *ch, *csh, *dpr, *sb, *ssh;
	GtkPlot *pt;
	GtkPlotLinear *plt;
	gdouble del, dlm, iv, l0, mny, mxy;
	gint j, k, nx4, st, sp, sz4, zd2;
	gchar *str;

	if ((fgs&PROC_ZDT)!=0)
	{
		plt=GTK_PLOT_LINEAR(pt2);
		pt=GTK_PLOT(pt2);
		sz4=g_array_index((pt->sizes), gint, 0);
		nx4=g_array_index((pt->ind), gint, 1);
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(pst));
		j=0;
		while ((j<sz4)&&(iv>g_array_index((plt->xdata), gdouble, j))) j++;
		st=j;
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(psp));
		while ((j<sz4)&&(iv>=g_array_index((plt->xdata), gdouble, j))) j++;
		sp=j-st;
		zd2=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(nl));
		del=(g_array_index((plt->xdata), gdouble, st+sp-1)-g_array_index((plt->xdata), gdouble, st))/(sp-1);
		dlm=gtk_spin_button_get_value(GTK_SPIN_BUTTON(dl));
		iv=dlm/((gdouble) zd2-1);
		dlm=dlm/2;
		l0=gtk_spin_button_get_value(GTK_SPIN_BUTTON(cl))-dlm;
		l=g_array_new(FALSE, FALSE, sizeof(gdouble));
		R=g_array_new(FALSE, FALSE, sizeof(gdouble));
		st1=g_array_sized_new(FALSE, FALSE, sizeof(gint), 2);
		sz1=g_array_sized_new(FALSE, FALSE, sizeof(gint), 2);
		nx1=g_array_sized_new(FALSE, FALSE, sizeof(gint), 2);
		k=1;
		g_array_append_val(st1, k);
		g_array_append_val(st1, k);
		{k=0; mny=0; mxy=0;}
		g_array_append_val(sz1, zd2);
		g_array_append_val(sz1, zd2);
		g_array_append_val(nx1, k);
		g_array_append_val(nx1, zd2);
		{g_array_append_val(l, l0); g_array_append_val(R, k);}
		for (j=1;j<zd2;j++)
		{
			l0+=iv;
			g_array_append_val(l, l0);
			g_array_append_val(R, k);
		}
		l0-=2*dlm;
		{g_array_append_val(l, l0); g_array_append_val(R, k);}
		for (j=1;j<zd2;j++)
		{
			l0+=iv;
			g_array_append_val(l, l0);
			g_array_append_val(R, k);
		}
		l0-=dlm;
		{cb=(gdouble*) g_malloc(sizeof(gdouble)*zd2); sb=(gdouble*) g_malloc(sizeof(gdouble)*zd2); ch=(gdouble*) g_malloc(sizeof(gdouble)*sp); csh=(gdouble*) g_malloc(sizeof(gdouble)*sp); ssh=(gdouble*) g_malloc(sizeof(gdouble)*sp);}
		for (k=0;k<sp;k++)//set up q arrays
		{
			ch[k]=cosh(del*g_array_index((plt->ydata), gdouble, st+k));
			ssh[k]=sinh(del*g_array_index((plt->ydata), gdouble, st+k));
			csh[k]=ssh[k]*cos(g_array_index((plt->ydata), gdouble, nx4+st+k));
			ssh[k]*=sin(g_array_index((plt->ydata), gdouble, nx4+st+k));
		}
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)))
		{
			for (j=0;j<zd2;j++)
			{
				cb[j]=100*del*dlm*(1-(2*j/((gdouble) zd2-1)));
				sb[j]=sin(cb[j]);
				cb[j]=cos(cb[j]);
			}
		}
		else
		{
			for (j=0;j<zd2;j++)
			{
				cb[j]=1.0e9*MY_2PI*del*((1/l0)-(1/(l0+(dlm*((2*j/((gdouble) zd2-1))-1)))));
				sb[j]=sin(cb[j]);
				cb[j]=cos(cb[j]);
			}
		}
		for (j=0;j<zd2;j++)//run T matrix
		{
			{A[0]=1; A[1]=0; B[0]=0; B[1]=0;}
			for (k=0;k<sp;k++)
			{
				{C[0]=(A[0]*csh[k])+(A[1]*ssh[k]); C[1]=(A[0]*ssh[k])-(A[1]*csh[k]);}
				{A[0]*=ch[k]; A[1]*=ch[k];}
				{A[0]+=(B[0]*csh[k])+(B[1]*ssh[k]); A[1]+=(B[0]*ssh[k])-(B[1]*csh[k]);}
				{B[0]*=ch[k]; B[1]*=ch[k];}
				{B[0]+=C[0]; B[1]+=C[1];}
				{C[0]=A[0]*cb[j]-A[1]*sb[j]; C[1]=B[0]*cb[j]-B[1]*sb[j];}
				{A[1]=A[1]*cb[j]+A[0]*sb[j]; B[1]=B[1]*cb[j]+B[0]*sb[j];}
				{A[0]=C[0]; B[0]=C[1];}
			}
			dpr=&g_array_index(R, gdouble, j);
			iv=sqrt(((B[0]*B[0])+(B[1]*B[1]))/((A[0]*A[0])+(A[1]*A[1])));
			*dpr=iv;
			if (iv>mxy) mxy=iv;
			dpr=&g_array_index(R, gdouble, zd2+j);
			iv=G_PI+atan2(B[1],B[0])-atan2(A[1],A[0]);
			if (iv>G_PI) iv-=MY_2PI;
			*dpr=iv;
			if (iv>mxy) mxy=iv;
			else if (iv<mny) mny=iv;
		}
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx)))
		{
			for (j=0;j<zd2;j++)
			{
				dpr=&g_array_index(R, gdouble, j);
				*dpr=1-*dpr;
			}
			if (mxy<1) mxy=1;
		}
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dBs)))
		{
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg)))
			{
				for (j=0;j<zd2;j++)
				{
					dpr=&g_array_index(R, gdouble, j);
					*dpr=-10*log10(*dpr);
				}
				mxy=50;
			}
			else
			{
				for (j=0;j<zd2;j++)
				{
					dpr=&g_array_index(R, gdouble, j);
					*dpr=10*log10(*dpr);
				}
				mny=-50;
			}
		}
		{g_free((gpointer) cb); g_free((gpointer) sb); g_free((gpointer) csh); g_free((gpointer) ssh); g_free((gpointer) ch);}
		plt=GTK_PLOT_LINEAR(pt1);
		gtk_plot_linear_set_data(plt, l, R, nx1, sz1, st1);
		{g_array_unref(l); g_array_unref(R); g_array_unref(nx1); g_array_unref(sz1);}
		gtk_plot_linear_update_scale_pretty(pt1, l0-dlm, l0+dlm, mny, mxy);
		fgs|=PROC_LDT;
		gtk_notebook_set_current_page(GTK_NOTEBOOK(nbk), 0);
	}
	else
	{
		str=g_strdup(_("No spatial profile for transformation exists yet."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}

void trs(GtkWidget *wgt, gpointer dta)
{
	fftw_complex *ir, *rf, *ym, *zm, *tm;
	fftw_complex beta, cue;
	fftw_plan p;
	GArray *kp, *nx2, *st2, *sz2, *z;
	gchar *str;
	gdouble cm, dx, h, iv, iv2, mxy, mny;
	gdouble *dpr;
	gint j, m, sz4, nx4, sp, st, zd, zd2;
	gint *ipr;
	GtkPlot *pt;
	GtkPlotLinear *plt;

	if ((fgs&PROC_LDT)!=0)
	{
		plt=GTK_PLOT_LINEAR(pt1);
		pt=GTK_PLOT(pt1);
		sz4=g_array_index((pt->sizes), gint, 0);
		nx4=g_array_index((pt->ind), gint, 1);
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(wst));
		j=0;
		while ((j<sz4)&&(iv>g_array_index((plt->xdata), gdouble, j))) j++;
		st=j;
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(wsp));
		while ((j<sz4)&&(iv>=g_array_index((plt->xdata), gdouble, j))) j++;
		sp=j-st;
		zd=1<<gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(zpd));
		zd2=1<<gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(nz));
		if (zd<(2*zd2)) zd2=zd/2;
		rf=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*zd);
		for (j=0;j<zd;j++) {rf[j][0]=0; rf[j][1]=0;}
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dBs)))
		{
			iv2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst));
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx)))
			{
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg)))
				{
					if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)))
					{
						dx=MY1_2PI*(g_array_index((plt->xdata), gdouble, st+sp-1)-g_array_index((plt->xdata), gdouble, st))/(sp-1);
						for (j=0;j<(sp+1)/2;j++)
						{
							iv=g_array_index((plt->ydata), gdouble, nx4+st+j+sp/2);
							iv+=j*G_PI*(zd2-2)/zd;
							{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
							iv=sqrt(1-exp(LNTOT*(iv2-g_array_index((plt->ydata), gdouble, st+j+sp/2))));
							{rf[j][0]*=iv; rf[j][1]*=iv;}
						}
						for (j=1;j<=sp/2;j++)
						{ 
							iv=g_array_index((plt->ydata), gdouble, nx4+st-j+sp/2);
							iv-=j*G_PI*(zd2-2)/zd;
							{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
							iv=sqrt(1-exp(LNTOT*(iv2-g_array_index((plt->ydata), gdouble, st-j+sp/2))));
							{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
						}
					}
					else /* -TdB0o+ */
					{
						dx=1.0e9*((1/g_array_index((plt->xdata), gdouble, st))-(1/g_array_index((plt->xdata), gdouble, st+sp-1)))/(sp-1);
						for (j=0;j<(sp+1)/2;j++)
						{
							iv=g_array_index((plt->ydata), gdouble, nx4+st-j+(sp-1)/2);
							iv+=j*G_PI*(zd2-2)/zd;
							{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
							iv=sqrt(1-exp(LNTOT*(iv2-g_array_index((plt->ydata), gdouble, st-j+(sp-1)/2))));
							{rf[j][0]*=iv; rf[j][1]*=iv;}
						}
						for (j=1;j<=sp/2;j++)
						{ 
							iv=g_array_index((plt->ydata), gdouble, nx4+st+j+(sp-1)/2);
							iv-=j*G_PI*(zd2-2)/zd;
							{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
							iv=sqrt(1-exp(LNTOT*(iv2-g_array_index((plt->ydata), gdouble, st+j+(sp-1)/2))));
							{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
						}
					}
				}
				else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)))
				{
					dx=MY1_2PI*(g_array_index((plt->xdata), gdouble, st+sp-1)-g_array_index((plt->xdata), gdouble, st))/(sp-1);
					for (j=0;j<(sp+1)/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st+j+sp/2);
						iv+=j*G_PI*(zd2-2)/zd;
						{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
						iv=sqrt(1-exp(LNTOT*(g_array_index((plt->ydata), gdouble, st+j+sp/2)-iv2)));
						{rf[j][0]*=iv; rf[j][1]*=iv;}
					}
					for (j=1;j<=sp/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st-j+sp/2);
						iv-=j*G_PI*(zd2-2)/zd;
						{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
						iv=sqrt(1-exp(LNTOT*(g_array_index((plt->ydata), gdouble, st-j+sp/2)-iv2)));
						{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
					}
				}
				else /* +TdB0o+ */
				{
					dx=1.0e9*((1/g_array_index((plt->xdata), gdouble, st))-(1/g_array_index((plt->xdata), gdouble, st+sp-1)))/(sp-1);
					for (j=0;j<(sp+1)/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st-j+(sp-1)/2);
						iv+=j*G_PI*(zd2-2)/zd;
						{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
						iv=sqrt(1-exp(LNTOT*(g_array_index((plt->ydata), gdouble, st-j+(sp-1)/2)-iv2)));
						{rf[j][0]*=iv; rf[j][1]*=iv;}
					}
					for (j=1;j<=sp/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st+j+(sp-1)/2);
						iv-=j*G_PI*(zd2-2)/zd;
						{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
						iv=sqrt(1-exp(LNTOT*(g_array_index((plt->ydata), gdouble, st+j+(sp-1)/2)-iv2)));
						{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
					}
				}
			}
			else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg)))
			{
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)))
				{
					dx=MY1_2PI*(g_array_index((plt->xdata), gdouble, st+sp-1)-g_array_index((plt->xdata), gdouble, st))/(sp-1);
					for (j=0;j<(sp+1)/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st+j+sp/2);
						iv+=j*G_PI*(zd2-2)/zd;
						{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
						iv=exp(LNTOW*(iv2-g_array_index((plt->ydata), gdouble, st+j+sp/2)));
						{rf[j][0]*=iv; rf[j][1]*=iv;}
					}
					for (j=1;j<=sp/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st-j+sp/2);
						iv-=j*G_PI*(zd2-2)/zd;
						{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
						iv=exp(LNTOW*(iv2-g_array_index((plt->ydata), gdouble, st-j+sp/2)));
						{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
					}
				}
				else /* -RdB0o+ */
				{
					dx=1.0e9*((1/g_array_index((plt->xdata), gdouble, st))-(1/g_array_index((plt->xdata), gdouble, st+sp-1)))/(sp-1);
					for (j=0;j<(sp+1)/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st-j+(sp-1)/2);
						iv+=j*G_PI*(zd2-2)/zd;
						{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
						iv=exp(LNTOW*(iv2-g_array_index((plt->ydata), gdouble, st-j+(sp-1)/2)));
						{rf[j][0]*=iv; rf[j][1]*=iv;}
					}
					for (j=1;j<=sp/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st+j+(sp-1)/2);
						iv-=j*G_PI*(zd2-2)/zd;
						{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
						iv=exp(LNTOW*(iv2-g_array_index((plt->ydata), gdouble, st+j+(sp-1)/2)));
						{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
					}
				}
			}
			else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)))
			{
				dx=MY1_2PI*(g_array_index((plt->xdata), gdouble, st+sp-1)-g_array_index((plt->xdata), gdouble, st))/(sp-1);
				for (j=0;j<(sp+1)/2;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, nx4+st+j+sp/2);
					iv+=j*G_PI*(zd2-2)/zd;
					{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
					iv=exp(LNTOW*(g_array_index((plt->ydata), gdouble, st+j+sp/2)-iv2));
					{rf[j][0]*=iv; rf[j][1]*=iv;}
				}
				for (j=1;j<=sp/2;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, nx4+st-j+sp/2);
					iv-=j*G_PI*(zd2-2)/zd;
					{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
					iv=exp(LNTOW*(g_array_index((plt->ydata), gdouble, st-j+sp/2)-iv2));
					{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
				}
			}
			else /* +RdB0o+ */
			{
				dx=1.0e9*((1/g_array_index((plt->xdata), gdouble, st))-(1/g_array_index((plt->xdata), gdouble, st+sp-1)))/(sp-1);
				for (j=0;j<(sp+1)/2;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, nx4+st-j+(sp-1)/2);
					iv+=j*G_PI*(zd2-2)/zd;
					{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
					iv=exp(LNTOW*(g_array_index((plt->ydata), gdouble, st-j+(sp-1)/2)-iv2));
					{rf[j][0]*=iv; rf[j][1]*=iv;}
				}
				for (j=1;j<=sp/2;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, nx4+st+j+(sp-1)/2);
					iv-=j*G_PI*(zd2-2)/zd;
					{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
					iv=exp(LNTOW*(g_array_index((plt->ydata), gdouble, st+j+(sp-1)/2)-iv2));
					{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
				}
			}
		}
		else
		{
			iv2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst));
			if ((iv2<DZE)&&(iv2>NZE))
			{
				str=g_strdup(_("Offset must be nonzero for linear measurements."));
				gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
				g_free(str);
			}
			else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx))) 
			{
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)))
				{
					dx=MY1_2PI*(g_array_index((plt->xdata), gdouble, st+sp-1)-g_array_index((plt->xdata), gdouble, st))/(sp-1);
					for (j=0;j<(sp+1)/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st+j+sp/2);
						iv+=j*G_PI*(zd2-2)/zd;
						{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
						iv=sqrt(1-(g_array_index((plt->ydata), gdouble, st+j+sp/2)/iv2));
						{rf[j][0]*=iv; rf[j][1]*=iv;}
					}
					for (j=1;j<=sp/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st-j+sp/2);
						iv-=j*G_PI*(zd2-2)/zd;
						{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
						iv=sqrt(1-(g_array_index((plt->ydata), gdouble, st-j+sp/2)/iv2));
						{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
					}
				}
				else /* -Tl0o+ and +Tl0o+ */
				{
					dx=1.0e9*((1/g_array_index((plt->xdata), gdouble, st))-(1/g_array_index((plt->xdata), gdouble, st+sp-1)))/(sp-1);
					for (j=0;j<(sp+1)/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st-j+(sp-1)/2);
						iv+=j*G_PI*(zd2-2)/zd;
						{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
						iv=sqrt(1-(g_array_index((plt->ydata), gdouble, st-j+(sp-1)/2)/iv2));
						{rf[j][0]*=iv; rf[j][1]*=iv;}
					}
					for (j=1;j<=sp/2;j++)
					{
						iv=g_array_index((plt->ydata), gdouble, nx4+st+j+(sp-1)/2);
						iv-=j*G_PI*(zd2-2)/zd;
						{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
						iv=sqrt(1-(g_array_index((plt->ydata), gdouble, st+j+(sp-1)/2)/iv2));
						{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
					}
				}
			}
			else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)))
			{
				dx=MY1_2PI*(g_array_index((plt->xdata), gdouble, st+sp-1)-g_array_index((plt->xdata), gdouble, st))/(sp-1);
				for (j=0;j<(sp+1)/2;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, nx4+st+j+sp/2);
					iv+=j*G_PI*(zd2-2)/zd;
					{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
					iv=sqrt(g_array_index((plt->ydata), gdouble, st+j+sp/2)/iv2);
					{rf[j][0]*=iv; rf[j][1]*=iv;}
				}
				for (j=1;j<=sp/2;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, nx4+st-j+sp/2);
					iv-=j*G_PI*(zd2-2)/zd;
					{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
					iv=sqrt(g_array_index((plt->ydata), gdouble, st-j+sp/2)/iv2);
					{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
				}
			}
			else/* -Rl0o+ and +Rl0o+ */
			{
				dx=1.0e9*((1/g_array_index((plt->xdata), gdouble, st))-(1/g_array_index((plt->xdata), gdouble, st+sp-1)))/(sp-1);
				for (j=0;j<(sp+1)/2;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, nx4+st-j+(sp-1)/2);
					iv+=j*G_PI*(zd2-2)/zd;
					{rf[j][0]=cos(iv); rf[j][1]=sin(iv);}
					iv=sqrt(g_array_index((plt->ydata), gdouble, st-j+(sp-1)/2)/iv2);
					{rf[j][0]*=iv; rf[j][1]*=iv;}
				}
				for (j=1;j<=sp/2;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, nx4+st+j+(sp-1)/2);
					iv-=j*G_PI*(zd2-2)/zd;
					{rf[zd-j][0]=cos(iv); rf[zd-j][1]=sin(iv);}
					iv=sqrt(g_array_index((plt->ydata), gdouble, st+j+(sp-1)/2)/iv2);
					{rf[zd-j][0]*=iv; rf[zd-j][1]*=iv;}
				}
			}
		}
		ir=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*zd);
		p=fftw_plan_dft_1d(zd, rf, ir, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
		fftw_free(rf);
		z=g_array_new(FALSE, FALSE, sizeof(gdouble));
		kp=g_array_new(FALSE, FALSE, sizeof(gdouble));
		st2=g_array_sized_new(FALSE, FALSE, sizeof(gint), 2);
		sz2=g_array_sized_new(FALSE, FALSE, sizeof(gint), 2);
		nx2=g_array_sized_new(FALSE, FALSE, sizeof(gint), 2);
		h=1/((gdouble)zd);
		st=1;
		g_array_append_val(st2, st);
		g_array_append_val(st2, st);
		{st=0; mny=0;}
		g_array_append_val(sz2, zd2);
		g_array_append_val(sz2, zd2);
		g_array_append_val(nx2, st);
		g_array_append_val(nx2, zd2);
		ym=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(zd2+1));
		zm=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(zd2+1));
		tm=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*zd2);
		for (j=0;j<zd2;j++) {ym[j][0]=0; ym[j][1]=0; zm[j][0]=0; zm[j][1]=0; g_array_append_val(kp, mny); g_array_append_val(kp, mny);}
		{ym[zd2][0]=0; ym[zd2][1]=0; zm[zd2][0]=0; zm[zd2][1]=0;}
		{beta[0]=-h*ir[0][0]/2; beta[1]=-h*ir[0][1]/2;}
		cm=1/(1-(beta[0]*beta[0])-(beta[1]*beta[1]));
		{cue[0]=-cm*(ir[1][0]+ir[0][0]); cue[1]=-cm*(ir[1][1]+ir[0][1]);}
		dpr=&g_array_index(kp, gdouble, 0);
		mxy=dx*sqrt((cue[0]*cue[0])+(cue[1]*cue[1]))/2;
		*dpr=mxy;
		dpr=&g_array_index(kp, gdouble, zd2);
		iv=atan2(cue[1],cue[0]);
		*dpr=iv;
		if (iv>mxy) mxy=iv;
		else if (iv<mny) mny=iv;
		{ym[0][0]=cm; zm[0][0]=cm*beta[0]; zm[0][1]=cm*beta[1];}
		{beta[0]=(ym[0][1]*ir[1][1])-(ym[0][0]*ir[1][0]); beta[1]=-(ym[0][0]*ir[1][1])-(ym[0][1]*ir[1][0]);}
		for (m=1;m<zd2;m++)
		{
			{beta[0]*=h; beta[1]*=h;}
			cm=1/(1-(beta[0]*beta[0])-(beta[1]*beta[1]));
			for (j=0;j<m;j++)//propagate ym,zm to ym+1,zm+1
			{
				{ym[j][0]*=cm; ym[j][1]*=cm; zm[j][0]*=cm; zm[j][1]*=cm;}
				{tm[j][0]=(zm[j][0]*beta[0])+(zm[j][1]*beta[1]); tm[j][1]=(zm[j][0]*beta[1])-(zm[j][1]*beta[0]);}
			}
			for (j=1;j<=m;j++) {zm[j][0]+=(ym[m-j][0]*beta[0])+(ym[m-j][1]*beta[1]); zm[j][1]+=(ym[m-j][0]*beta[1])-(ym[m-j][1]*beta[0]);}
			for (j=1;j<=m;j++) {ym[j][0]+=tm[m-j][0]; ym[j][1]+=tm[m-j][1];}
			{beta[0]=0; beta[1]=0;}//evaluate bm+1 and qm+1
			for (j=0;j<=m;j++) {beta[0]+=(ym[j][1]*ir[m+1-j][1])-(ym[j][0]*ir[m+1-j][0]); beta[1]-=(ym[j][0]*ir[m+1-j][1])+(ym[j][1]*ir[m+1-j][0]);}
			{cue[0]=beta[0]; cue[1]=beta[1];}
			for (j=0;j<=m;j++) {cue[0]+=(ym[j][1]*ir[m-j][1])-(ym[j][0]*ir[m-j][0]); cue[1]-=(ym[j][0]*ir[m-j][1])+(ym[j][1]*ir[m-j][0]);}
			dpr=&g_array_index(kp, gdouble, m);
			iv=dx*sqrt((cue[0]*cue[0])+(cue[1]*cue[1]))/2;
			*dpr=iv;
			if (iv>mxy) mxy=iv;
			dpr=&g_array_index(kp, gdouble, zd2+m);
			iv=atan2(cue[1],cue[0]);
			*dpr=iv;
			if (iv>mxy) mxy=iv;
			else if (iv<mny) mny=iv;
		}
		{fftw_free(ir); fftw_free(ym); fftw_free(zm); fftw_free(tm);}
		h*=1/(2*dx);
		iv=(1-zd2)*h/2;
		g_array_append_val(z, iv);
		for (j=1;j<zd2;j++)
		{
			iv+=h;
			g_array_append_val(z, iv);
		}
		iv=(1-zd2)*h/2;
		g_array_append_val(z, iv);
		for (j=1;j<zd2;j++)
		{
			iv+=h;
			g_array_append_val(z, iv);
		}
		plt=GTK_PLOT_LINEAR(pt2);
		gtk_plot_linear_set_data(plt, z, kp, nx2, sz2, st2);
		{g_array_unref(z); g_array_unref(kp); g_array_unref(nx2); g_array_unref(sz2);}
		gtk_plot_linear_update_scale_pretty(pt2, -iv, iv, mny, mxy);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(nbk), 1);
		fgs|=PROC_ZDT;
	}
	else
	{
		str=g_strdup(_("No spectrum for inverse transformation exists yet."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}
