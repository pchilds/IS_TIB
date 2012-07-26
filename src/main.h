/***************************************************************************
 *            main.h
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

#ifndef __MAIN_H__
#	define __MAIN_H__
#	include <gtk/gtk.h>
#	include <glib/gi18n.h>
#	include <math.h>
#	include <gtkplot-2.0/gtkplotlinear.h>
	G_BEGIN_DECLS
#	define DZE 0.00001 /* divide by zero threshold */
#	define NZE -0.00001 /* negative of this */
#	define MY_2PI 6.2831853071795864769252867665590057683943387987502
#	define MY_C_2PI 4774648292.7568600730665129011754308610337893722137
#	define NMY_2PI -6.2831853071795864769252867665590057683943387987502
#	define NMY_PI -3.1415926535897932384626433832795028841971693993751
#	define LNTOT 0.23025850929940456840179914546843642076011014886288 /* (ln10)/10 */
#	define LNTOW 0.11512925464970228420089957273421821038005507443144 /* (ln10)/20 */
#	ifdef G_OS_WIN32
#		define DLMT "\r\n"
#	else
#		define DLMT "\n"
#	endif
	typedef enum
	{
		PROC_LDT=1<<0,
		PROC_ZDT=1<<1
	} ProcState;
	G_END_DECLS
#endif
