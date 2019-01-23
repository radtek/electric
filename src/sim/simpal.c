/* -*- tab-width: 4 -*-
 *
 * Electric(tm) VLSI Design System
 *
 * File: simpal.c
 * Simulation tool: deck generator for ABEL PAL system, from Data I/O
 * Written by: Steven M. Rubin, Static Free Software
 *
 * Copyright (c) 2000 Static Free Software.
 *
 * Electric(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Electric(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Electric(tm); see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, Mass 02111-1307, USA.
 *
 * Static Free Software
 * 4119 Alpine Road
 * Portola Valley, California 94028
 * info@staticfreesoft.com
 */

#include "config.h"
#if SIMTOOL

#include "global.h"
#include "sim.h"
#include "efunction.h"
#include "network.h"

/* prototypes for local routines */
static CHAR *sim_palnetname(PNET*);

/*********************** ABEL PAL FILE GENERATION ***********************/

/*
 * routine to write a ".pal" file from the cell "np"
 */
void sim_writepalnetlist(NODEPROTO *np)
{
	CHAR name[100], *fun, *truename;
	INTBIG power, ground, components, nets;
	REGISTER INTBIG j, a, y;
	REGISTER PCOMP *p;
	REGISTER PNET *pn;
	REGISTER PCOMP *pcompused;
	PNET *pnetused;
	REGISTER FILE *palfile;

	/* make sure network tool is on */
	if ((net_tool->toolstate&TOOLON) == 0)
	{
		ttyputerr(_("Network tool must be running...turning it on"));
		toolturnon(net_tool);
		ttyputerr(_("...now reissue the simulation command"));
		return;
	}

	/* first write the "pal" file */
	(void)estrcpy(name, np->protoname);
	(void)estrcat(name, x_(".pal"));
	palfile = xcreate(name, sim_filetypepal, _("PAL File"), &truename);
	if (palfile == NULL)
	{
		if (truename != 0) ttyputerr(_("Cannot write %s"), truename);
		return;
	}

	/* write header information */
	xprintf(palfile, x_("module %s\n"), np->protoname);
	xprintf(palfile, x_("title 'generated by Electric'\n"));

	/* build flattened representation of circuit */
	net_initnetflattening();
	pcompused = net_makepseudo(np, &components, &nets, &power, &ground, &pnetused,
		TRUE, TRUE, FALSE, FALSE, FALSE);
	if (pcompused == NOPCOMP) return;

	/* mark the used ports */
	for(pn = pnetused; pn != NOPNET; pn = pn->nextpnet) pn->flags = 0;
	for(p = pcompused; p != NOPCOMP; p = p->nextpcomp)
	{
		switch (p->function)
		{
			case NPGATEAND:
			case NPGATEOR:
			case NPGATEXOR:
			case NPBUFFER:
				for(y=0; y<p->wirecount; y++)
					if (namesame(p->portlist[y]->protoname, x_("y")) == 0) break;
				if (y >= p->wirecount) break;
				p->netnumbers[y]->flags++;
				for(a=0; a<p->wirecount; a++)
					if (namesame(p->portlist[a]->protoname, x_("a")) == 0) p->netnumbers[a]->flags++;
				break;
		}
	}

	/* number all ports */
	j = 1;
	for(pn = pnetused; pn != NOPNET; pn = pn->nextpnet)
		if (pn->realportcount != 0 && pn->flags != 0) pn->flags = (INTSML)j++;
	for(pn = pnetused; pn != NOPNET; pn = pn->nextpnet)
		if (pn->realportcount == 0 && pn->flags != 0) pn->flags = (INTSML)j++;

	/* print exports */
	for(pn = pnetused; pn != NOPNET; pn = pn->nextpnet)
		if (pn->realportcount != 0 && pn->flags != 0)
			xprintf(palfile, x_("    %s pin %d;\n"), sim_palnetname(pn), pn->flags);

	/* print internal points */
	for(pn = pnetused; pn != NOPNET; pn = pn->nextpnet)
		if (pn->realportcount == 0 && pn->flags != 0)
			xprintf(palfile, x_("    %s = 0,1;\n"), sim_palnetname(pn));

	xprintf(palfile, x_("\nequations\n"));
	for(p = pcompused; p != NOPCOMP; p = p->nextpcomp)
	{
		switch (p->function)
		{
			case NPGATEAND:
			case NPGATEOR:
			case NPGATEXOR:
			case NPBUFFER:
				if (p->function == NPGATEAND) fun = x_("&"); else
					if (p->function == NPGATEOR) fun = x_("#"); else
						if (p->function == NPGATEXOR) fun = x_("$"); else
							fun = x_("");
				for(y=0; y<p->wirecount; y++)
					if (namesame(p->portlist[y]->protoname, x_("y")) == 0) break;
				if (y >= p->wirecount)
				{
					ttyputmsg(_("Cannot find output port on %s"), describenodeinst((NODEINST *)p->actuallist));
					break;
				}
				xprintf(palfile, x_("   "));
				if ((p->state[y]&NEGATEDPORT) != 0) xprintf(palfile, x_("!"));
				xprintf(palfile, x_("%s ="), sim_palnetname(p->netnumbers[y]));
				j = 0;
				for(a=0; a<p->wirecount; a++)
					if (namesame(p->portlist[a]->protoname, x_("a")) == 0)
				{
					if (j != 0) xprintf(palfile, x_(" %s"), fun);
					xprintf(palfile, x_(" "));
					if ((p->state[a]&NEGATEDPORT) != 0) xprintf(palfile, x_("!"));
					xprintf(palfile, x_("%s"), sim_palnetname(p->netnumbers[a]));
					j++;
				}
				xprintf(palfile, x_(";\n"));
				break;

			default:
				ttyputmsg(_("Don't know how to handle %s objects"), describenodeinst((NODEINST *)p->actuallist));
				break;
		}
	}

	/* clean up */
	xprintf(palfile, x_("\nend %s\n"), describenodeproto(np));
	xclose(palfile);
	ttyputmsg(_("%s written"), truename);

	/* free all PNETs and PCOMPs */
	net_freeallpnet(pnetused);
	net_freeallpcomp(pcompused);
}

CHAR *sim_palnetname(PNET *pn)
{
	static CHAR line[20];
	REGISTER PORTPROTO *pp;

	if (pn->realportcount != 0)
	{
		if (pn->realportcount == 1) pp = (PORTPROTO *)pn->realportlist; else
			pp = ((PORTPROTO **)pn->realportlist)[0];
		return(pp->protoname);
	}
	(void)esnprintf(line, 20, x_("I%d"), pn->flags);
	return(line);
}

#endif  /* SIMTOOL - at top */