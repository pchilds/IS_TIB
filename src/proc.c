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

void prs(GtkWidget *wgt, gpointer dta)
{
	PlotLinear *plt;
	gint sz;
	gchar *str;

	if ((fgs&PROC_TRS)!=0)
	{
		plt=PLOT_LINEAR(pt2);
		sz=g_array_index((plt->sizes), gint, 0);
		fgs|=PROC_PRS;
		gtk_notebook_set_current_page(GTK_NOTEBOOK(nbk), 0);
	}
	else
	{
		str=g_strdup(_("No spatial profile for transformation exists yet."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}

void trs(GtkWidget *wgt, gpointer dta) /* need to incorporate case for inversion to 2pi/x */
{
	GArray *k, *r;
	gchar *str;
	gdouble dx, iv, iv2, yx;
	gdouble *dpr;
	gint j, lc, sp, st;
	gint *ipr;
	PlotLinear *plt;

	if ((fgs&PROC_OPN)!=0)
	{
		plt=PLOT_LINEAR(pt1);
		lc=g_array_index((plt->sizes), gint, 0);
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(wst));
		j=0;
		while ((j<lc)&&(iv>g_array_index((plt->xdata), gdouble, j))) j++;
		st=j;
		iv=gtk_spin_button_get_value(GTK_SPIN_BUTTON(wsp));
		while ((j<lc)&&(iv>=g_array_index((plt->xdata), gdouble, j))) j++;
		sp=j;
		r=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), sp);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)))/* interpolate */
		{
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dBs)))
			{
				iv2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst));
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx)))
				{
					if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg))) /* -TdB0- */
					{
						/* fill array */
					}
					else /* +TdB0- */
					{
						/* fill array */
					}
				}
				else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg))) /* -RdB0- */
				{
					/* fill array */
				}
				else /* +RdB0- */
				{
					/* fill array */
				}
			}
			else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx))) /* -Tl0- +Tl0- */
			{
				iv2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst));
				/* fill array */
			}
			else /* -Rl0- +Rl0- */
			{
				iv2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst));
				/* fill array */
			}
		}
		else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dBs)))
		{
			iv2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst));
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx)))
			{
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg))) /* -TdB0o+ */
				{
					/*dx=g_array_index((plt->xdata), gdouble, sp-1)-g_array_index((plt->xdata), gdouble, st);*/
					for (j=st;j<sp;j++)
					{
						iv=1-exp(LNTOT*(iv2-g_array_index((plt->ydata), gdouble, j)));
						g_array_append_val(r, iv);
					}
				}
				else /* +TdB0o+ */
				{
					/*dx=g_array_index((plt->xdata), gdouble, sp-1)-g_array_index((plt->xdata), gdouble, st);*/
					for (j=st;j<sp;j++)
					{
						iv=1-exp(LNTOT*(g_array_index((plt->ydata), gdouble, j)-iv2));
						g_array_append_val(r, iv);
					}
				}
			}
			else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg))) /* -RdB0o+ */
			{
				/*dx=g_array_index((plt->xdata), gdouble, sp-1)-g_array_index((plt->xdata), gdouble, st);*/
				for (j=st;j<sp;j++)
				{
					iv=exp(LNTOT*(iv2-g_array_index((plt->ydata), gdouble, j)));
					g_array_append_val(r, iv);
				}
			}
			else /* +RdB0o+ */
			{
				/*dx=g_array_index((plt->xdata), gdouble, sp-1)-g_array_index((plt->xdata), gdouble, st);*/
				for (j=st;j<sp;j++)
				{
					iv=exp(LNTOT*(g_array_index((plt->ydata), gdouble, j)-iv2));
					g_array_append_val(r, iv);
				}
			}
		}
		else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx))) /* -Tl0o+ and +Tl0o+ */ 
		{
			iv2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst));
			if ((iv2<DZE)&&(iv2>NZE))
			{
				str=g_strdup(_("Offset must be nonzero for linear measurements."));
				gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
				g_free(str);
			}
			else
			{
				/*dx=g_array_index((plt->xdata), gdouble, sp-1)-g_array_index((plt->xdata), gdouble, st);*/
				for (j=st;j<sp;j++)
				{
					iv=(1-g_array_index((plt->ydata), gdouble, j))/iv2;
					g_array_append_val(r, iv);
				}
			}
		}
		else /* -Rl0o+ and +Rl0o+ */ 
		{
			iv2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst));
			if ((iv2<DZE)&&(iv2>NZE))
			{
				str=g_strdup(_("Offset must be nonzero for linear measurements."));
				gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
				g_free(str);
			}
			else
			{
				/*dx=g_array_index((plt->xdata), gdouble, sp-1)-g_array_index((plt->xdata), gdouble, st);*/
				for (j=st;j<sp;j++)
				{
					iv=g_array_index((plt->ydata), gdouble, j)/iv2;
					g_array_append_val(r, iv);
				}
			}
		}
		/*star=fftw_malloc(sizeof(double)*n);
		p=fftw_plan_many_r2r(1, &zp, jdimx, y, NULL, 1, zp, star, NULL, 1, zp, &type, FFTW_ESTIMATE);
		fftw_execute(p);
		fftw_destroy_plan(p);
		fftw_free(y);*/
		sp-=st;
		plt=PLOT_LINEAR(pt2);
		ipr=&g_array_index((plt->sizes), gint, 0);
		{g_array_set_size((plt->xdata), sp); g_array_set_size((plt->ydata),sp);}
		dpr=&g_array_index((plt->ydata), gdouble, 0);
		/*yx=*/		
		*dpr=yx;
		dpr=&g_array_index((plt->xdata), gdouble, 0);
		*dpr=0;
		for (j=1;j<sp;j++)
		{
			dpr=&g_array_index((plt->ydata), gdouble, j);
			/*iv=star[j+(k*zp)];
			iv2=star[((k+1)*zp)-j];*/
			*dpr=sqrt((iv*iv)+(iv2*iv2));
			if (yx<(*dpr)) yx=(*dpr);
			dpr=&g_array_index((plt->xdata), gdouble, j);
			*dpr=j*dx;
		}
		plot_linear_update_scale_pretty(pt2, 0, *dpr, 0, yx);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(nbk), 1);
		fgs|=PROC_TRS;
	}
	else
	{
		str=g_strdup(_("Open a file for analysis first."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}
