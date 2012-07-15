/***************************************************************************
 *            main.c
 *
 *  A GTK+ program for performing inverse scattering using the Toplitz
 *  Inner Bordering method
 *  version 0.1.0
 *  Features:
 *            Multiple data format input
 *            Graphical display of data and transformed result
 *            Publication ready graphs of results in multiple formats
 *
 *  Thu Oct  6 19:02:14 2011
 *  Copyright  2011  Paul Childs
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

#include <gdk/gdkkeysyms.h>
#include "main.h"
#include "data.h"
#include "disp.h"
#include "proc.h"
#include "util.h"

gchar *fld=NULL, *flr=NULL;
gint fgs;
GSList *grp=NULL;
GtkWidget *dBs, *fst, *kms, *nbk, *neg, *psp, *pst, *pt1, *pt2, *r0, *ri, *rp, *sbr, *tx, *wdw, *wk, *wsp, *wst;

int main(int argc, char *argv[])
{
	AtkObject *awg, *all;
	GtkAccelGroup *acc=NULL;
	GtkAdjustment *adj;
	GtkWidget *vbx, *mnb, *mnu, *smn, *mni, *hpn, *tbl, *lbl, *btt;
	GtkPlotLinear *plt;

	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
	gtk_init(&argc, &argv);
	wdw=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wdw), _("Toplitz Inner Bordering inverse solver"));
	g_signal_connect_swapped(G_OBJECT(wdw), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	acc=gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(wdw), acc);
	vbx=gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(wdw), vbx);
	gtk_widget_show(vbx);
	mnb=gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbx), mnb, FALSE, FALSE, 2);
	gtk_widget_show(mnb);
	mnu=gtk_menu_new();
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
	gtk_widget_add_accelerator(mni, "activate", acc, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(opd), (gpointer) wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	smn=gtk_menu_new();
	mni=gtk_menu_item_new_with_label(_("Data"));
	gtk_widget_add_accelerator(mni, "activate", acc, GDK_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(sav), (gpointer) wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(smn), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_label(_("Graph"));
	gtk_widget_add_accelerator(mni, "activate", acc, GDK_g, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(prg), (gpointer) wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(smn), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), smn);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_PRINT, NULL);
	gtk_widget_add_accelerator(mni, "activate", acc, GDK_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(prt), (gpointer) wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_label(_("Restore Session"));
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(ssr), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_label(_("Save Session"));
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(sss), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, acc);
	gtk_widget_add_accelerator(mni, "activate", acc, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(gtk_main_quit), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_File"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	mnu=gtk_menu_new();
	smn=gtk_menu_new();
	wk=gtk_check_menu_item_new_with_label(_("Wavelength\n(/number)"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(wk), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(smn), wk);
	gtk_widget_show(wk);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	rp=gtk_radio_menu_item_new_with_label(grp, _("Magnitude/Phase"));
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(rp));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(rp), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(smn), rp);
	gtk_widget_show(rp);
	ri=gtk_radio_menu_item_new_with_label(grp, _("Real/Imaginary"));
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(ri));
	gtk_menu_shell_append(GTK_MENU_SHELL(smn), ri);
	gtk_widget_show(ri);
	r0=gtk_radio_menu_item_new_with_label(grp, _("Magnitude only"));
	grp=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(r0));
	gtk_menu_shell_append(GTK_MENU_SHELL(smn), r0);
	gtk_widget_show(r0);
	mni=gtk_menu_item_new_with_label(_("Data Format:"));
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), smn);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	tx=gtk_check_menu_item_new_with_label(_("Transmission\nMeasurement?"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(tx), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), tx);
	gtk_widget_show(tx);
	dBs=gtk_check_menu_item_new_with_label(_("Data in dBs?"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(dBs), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), dBs);
	gtk_widget_show(dBs);
	neg=gtk_check_menu_item_new_with_label(_("Negate?"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(neg), FALSE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), neg);
	gtk_widget_show(neg);
	kms=gtk_check_menu_item_new_with_label(_("Data over k?"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(kms), FALSE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), kms);
	gtk_widget_show(kms);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_label(_("Display Properties:"));
	gtk_widget_add_accelerator(mni, "activate", acc, GDK_F2, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(dpr), (gpointer) wdw);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_Properties"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	mnu=gtk_menu_new();
	mni=gtk_menu_item_new_with_label(_("Instructions"));
	gtk_widget_add_accelerator(mni, "activate", acc, GDK_F1, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(hlp), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(abt), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_Help"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	gtk_menu_item_right_justify(GTK_MENU_ITEM(mni));
	nbk=gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(nbk), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(vbx), nbk);
	gtk_widget_show(nbk);
	tbl=gtk_table_new(5, 2, FALSE);
	gtk_widget_show(tbl);
	lbl=gtk_label_new(_("Spectrum Start:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(0, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	wst=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), wst, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(wst);
	awg=gtk_widget_get_accessible(wst);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("Spectrum Stop:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(1, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	wsp=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), wsp, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(wsp);
	awg=gtk_widget_get_accessible(wsp);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("Offset:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(1, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	fst=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), fst, 1, 2, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(fst);
	awg=gtk_widget_get_accessible(fst);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	btt=gtk_button_new_with_label(_("Solve Inverse"));
	g_signal_connect(G_OBJECT(btt), "clicked", G_CALLBACK(trs), NULL);
	gtk_table_attach(GTK_TABLE(tbl), btt, 0, 2, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(btt);
	hpn=gtk_hpaned_new();
	gtk_widget_show(hpn);
	gtk_paned_add1(GTK_PANED(hpn), tbl);
	tbl=gtk_table_new(1, 1, FALSE);
	gtk_widget_show(tbl);
	pt1=gtk_plot_linear_new();
	g_signal_connect(pt1, "moved", G_CALLBACK(pmv), NULL);
	gtk_widget_show(pt1);
	gtk_table_attach(GTK_TABLE(tbl), pt1, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK |GTK_EXPAND, 2, 2);
	gtk_paned_add2(GTK_PANED(hpn), tbl);
	lbl=gtk_label_new(_("Spectrum"));
	gtk_notebook_append_page(GTK_NOTEBOOK(nbk), hpn, lbl);
	tbl=gtk_table_new(5, 3, FALSE);
	gtk_widget_show(tbl);
	lbl=gtk_label_new(_("Start Position:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(1, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	pst=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), pst, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(pst);
	awg=gtk_widget_get_accessible(pst);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("Stop Position:"));
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(lbl);
	adj=(GtkAdjustment*) gtk_adjustment_new(3, -G_MAXDOUBLE, G_MAXDOUBLE, 1.0, 5.0, 0.0);
	psp=gtk_spin_button_new(adj, 0.5, 3);
	gtk_table_attach(GTK_TABLE(tbl), psp, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(psp);
	awg=gtk_widget_get_accessible(psp);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	btt=gtk_button_new_with_label(_("Solve Direct"));
	g_signal_connect(G_OBJECT(btt), "clicked", G_CALLBACK(prs), NULL);
	gtk_table_attach(GTK_TABLE(tbl), btt, 0, 1, 4, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(btt);
	hpn=gtk_hpaned_new();
	gtk_widget_show(hpn);
	gtk_paned_add1(GTK_PANED(hpn), tbl);
	tbl=gtk_table_new(1, 1, FALSE);
	gtk_widget_show(tbl);
	pt2=gtk_plot_linear_new();
	g_signal_connect(pt2, "moved", G_CALLBACK(pmv), NULL);
	gtk_widget_show(pt2);
	gtk_table_attach(GTK_TABLE(tbl), pt2, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK |GTK_EXPAND, 2, 2);
	gtk_paned_add2(GTK_PANED(hpn), tbl);
	lbl=gtk_label_new(_("Spatial Structure"));
	gtk_notebook_append_page(GTK_NOTEBOOK(nbk), hpn, lbl);
	sbr=gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbx), sbr, FALSE, FALSE, 2);
	gtk_widget_show(sbr);
	{fld=g_strdup("/home"); flr=g_strdup("/home"); fgs=0;}
	plt=GTK_PLOT_LINEAR(pt1);
	{g_array_set_size((plt->sizes), 2); g_array_set_size((plt->ind), 2);}
	gtk_widget_show(wdw);
	gtk_main();
	return 0;
}
