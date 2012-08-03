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
 * the Free Software Foundation; either version 2 of the License, or
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
	GtkPlotLinear *plt;
	gdouble iv;
	gint sz4;
	gchar *str;

	if ((fgs&PROC_ZDT)!=0)
	{
		plt=GTK_PLOT_LINEAR(pt2);
		sz4=g_array_index((plt->sizes), gint, 0);
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
	GtkPlotLinear *plt;
	gdouble iv;
	gint sz4;
	gchar *str;

	if ((fgs&PROC_ZDT)!=0)
	{
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(dta));
		plt=GTK_PLOT_LINEAR(pt2);
		sz4=g_array_index((plt->sizes), gint, 0);
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
	GtkPlotLinear *plt;
	gint sz4;
	gchar *str;

	if ((fgs&PROC_ZDT)!=0)
	{
		plt=GTK_PLOT_LINEAR(pt2);
		sz4=g_array_index((plt->sizes), gint, 0);
		//fgs|=PROC_LDT;
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
	gchar *str;
	gdouble h, dx, iv, iv2, cm, mxy, mny;
	gdouble *dpr;
	gint j, m, sz4, nx4, sp, st, zd, zd2;
	gint *ipr;
	GtkPlotLinear *plt;

	if ((fgs&PROC_LDT)!=0)
	{
		plt=GTK_PLOT_LINEAR(pt1);
		sz4=g_array_index((plt->sizes), gint, 0);
		nx4=g_array_index((plt->ind), gint, 1);
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
		{g_array_free(z, TRUE); g_array_free(kp, TRUE); g_array_free(sz2, TRUE); g_array_free(nx2, TRUE);}
		z=g_array_new(FALSE, FALSE, sizeof(gdouble));
		kp=g_array_new(FALSE, FALSE, sizeof(gdouble));
		sz2=g_array_new(FALSE, FALSE, sizeof(gint));
		nx2=g_array_new(FALSE, FALSE, sizeof(gint));
		h=1/((gdouble)zd);
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
		mxy=dx*sqrt((cue[0]*cue[0])+(cue[1]*cue[1]));
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
			iv=dx*sqrt((cue[0]*cue[0])+(cue[1]*cue[1]));
			*dpr=iv;
			if (iv>mxy) mxy=iv;
			else if (iv<mny) mny=iv;
			dpr=&g_array_index(kp, gdouble, zd2+m);
			iv=atan2(cue[1],cue[0]);
			*dpr=iv;
			if (iv>mxy) mxy=iv;
			else if (iv<mny) mny=iv;
		}
		{fftw_free(ir); fftw_free(ym); fftw_free(zm); fftw_free(tm);}
		h*=1/dx;
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
		{(plt->sizes)=sz2; (plt->ind)=nx2; (plt->xdata)=z; (plt->ydata)=kp;}
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
