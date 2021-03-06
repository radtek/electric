/* -*- tab-width: 4 -*-
 *
 * Electric(tm) VLSI Design System
 *
 * File: pla.h
 * Programmable Logic Array Generator
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

#if defined(__cplusplus) && !defined(ALLCPLUSPLUS)
extern "C"
{
#endif

extern TOOL     *pla_tool;
extern INTBIG    pla_lam;				/* Feature size */
extern INTBIG    pla_filetypeplatab;	/* PLA table disk file descriptor */

/* prototypes for tool interface */
void pla_init(INTBIG*, CHAR1*[], TOOL*);
void pla_done(void);
void pla_set(INTBIG, CHAR*[]);
void pla_slice(void);

/* prototypes for intratool interface */
void plac_generate(void);
void plac_dec(void);
void plac_n_generate(void);
void plac_p_generate(void);

#if defined(__cplusplus) && !defined(ALLCPLUSPLUS)
}
#endif
