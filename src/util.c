/***************************************************************************
 *            util.c
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

#include "util.h"

void hlp(GtkWidget *wgt, gpointer dta)
{
	gchar *uri, *str;
	GError *Err=NULL;

	if (dta) uri=g_strdup_printf("ghelp:InvScatt-help?%s", (gchar *) dta);
	else uri=g_strdup("ghelp:InvScatt-help");
	gtk_show_uri(NULL, uri, gtk_get_current_event_time(), &Err);
	g_free(uri);
	if (Err)
	{
		str=g_strdup(_("Could not load help files."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
		g_error_free(Err);
	}
}

void abt(GtkWidget *wgt, gpointer dta)
{
	GtkWidget *hwn;

	hwn=gtk_about_dialog_new();
	g_signal_connect_swapped(G_OBJECT(hwn), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(hwn));
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(hwn), "0.1.0");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(hwn), "(c) Paul Childs, 2011");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(hwn), _("ISTIB is a program for performing inverse scattering for 1D optical media using the Toplitz Inner Bordering method."));
	gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(hwn), _("translator-credits"));
	gtk_widget_show(hwn);
	g_signal_connect_swapped(G_OBJECT(hwn), "response", G_CALLBACK(gtk_widget_destroy), G_OBJECT(hwn));
}

void pmv(GtkPlotLinear *plt, gpointer dta)
{
	gchar *str;

	str=g_strdup_printf(_("x: %f, y: %f"), (plt->xps), (plt->yps));
	gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
	g_free(str);
}

