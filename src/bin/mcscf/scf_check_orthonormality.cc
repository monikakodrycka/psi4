/*
 *@BEGIN LICENSE
 *
 * PSI4: an ab initio quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *@END LICENSE
 */

#include <iostream>
#include <cmath>
#include <cstdio>

#include <libchkpt/chkpt.hpp>

#include "scf.h"

extern FILE* outfile;

namespace psi{ namespace mcscf{

void SCF::check_orthonormality()
{
  SBlockMatrix CSC("CSC",nirreps,sopi,sopi);
  transform(S,CSC,C);

  double    diagonal = 0.0;
  for(int h = 0; h < nirreps; ++h)
    for(int i = 0; i < sopi[h]; ++i)
      diagonal += fabs(CSC->get(h,i,i));

  double offdiagonal = 0.0;
  for(int h = 0; h < nirreps; ++h)
    for(int i = 0; i < sopi[h]; ++i)
      for(int j = i + 1; j < sopi[h]; ++j)
        offdiagonal += fabs(CSC->get(h,i,j));

  if((offdiagonal > 1.0e-8) || ((diagonal-double(nso)) > 1.0e-8)){
    psi::fprintf(outfile,"\n\n  Warning: CSC has an orthonormality index of %lf",offdiagonal);
    psi::fprintf(outfile,"\n  Trace(CSC) - nso = %lf",diagonal-nso);
    psi::fprintf(outfile,"      Sum_i>j (CSC)ij  = %lf",offdiagonal);
  }else{
    psi::fprintf(outfile,"\n  MOs orthonormality check passed.");
  }
}

}} // End namespace
