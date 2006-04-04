/* Register names and numbers for S/390 DWARF.
   Copyright (C) 2006 Red Hat, Inc.
   This file is part of Red Hat elfutils.

   Red Hat elfutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 2 of the License.

   Red Hat elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with Red Hat elfutils; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.

   Red Hat elfutils is an included package of the Open Invention Network.
   An included package of the Open Invention Network is a package for which
   Open Invention Network licensees cross-license their patents.  No patent
   license is granted, either expressly or impliedly, by designation as an
   included package.  Should you wish to participate in the Open Invention
   Network licensing program, please visit www.openinventionnetwork.com
   <http://www.openinventionnetwork.com>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>

#define BACKEND s390_
#include "libebl_CPU.h"


/*
zseries (64)

0-15	gpr0-gpr15	x
16-19	fpr[0246]
20-24	fpr[13578]
25-27	fpr1[024]
28	fpr9
29-31	fpr1[135]
32-47	cr0-cr15	x
48-63	ar0-ar15	x
64	psw_mask
65	psw_address
*/


ssize_t
s390_register_name (Ebl *ebl __attribute__ ((unused)),
		    int regno, char *name, size_t namelen,
		    const char **prefix, const char **setname)
{
  if (name == NULL)
    return 66;

  if (regno < 0 || regno > 65 || namelen < 7)
    return -1;

  *prefix = "%";

  if (regno < 16)
    *setname = "integer";
  else if (regno < 32)
    *setname = "FPU";
  else if (regno < 48 || regno > 63)
    *setname = "control";
  else
    *setname = "access";

  switch (regno)
    {
    case 0 ... 9:
      name[0] = 'r';
      name[1] = regno + '0';
      namelen = 2;
      break;

    case 10 ... 15:
      name[0] = 'r';
      name[1] = '1';
      name[2] = regno - 10 + '0';
      namelen = 3;
      break;

    case 16 ... 31:
      name[0] = 'f';
      regno = (regno & 8) | ((regno & 4) >> 2) | ((regno & 3) << 1);
      namelen = 1;
      if (regno >= 10)
	{
	  regno -= 10;
	  name[namelen++] = '1';
	}
      name[namelen++] = regno + '0';
      break;

    case 32 + 0 ... 32 + 9:
    case 48 + 0 ... 48 + 9:
      name[0] = regno < 48 ? 'c' : 'a';
      name[1] = (regno & 15) + '0';
      namelen = 2;
      break;

    case 32 + 10 ... 32 + 15:
    case 48 + 10 ... 48 + 15:
      name[0] = regno < 48 ? 'c' : 'a';
      name[1] = '1';
      name[2] = (regno & 15) - 10 + '0';
      namelen = 3;
      break;

    case 64:
      return stpcpy (name, "pswm") - name;
    case 65:
      return stpcpy (name, "pswa") - name;

    default:
      *setname = NULL;
      return 0;
    }

  name[namelen++] = '\0';
  return namelen;
}