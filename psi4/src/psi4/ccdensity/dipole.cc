/*
 * @BEGIN LICENSE
 *
 * Psi4: an open-source quantum chemistry software package
 *
 * Copyright (c) 2007-2016 The Psi4 Developers.
 *
 * The copyrights for code used from other parties are included in
 * the corresponding files.
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
 * @END LICENSE
 */

/*! \file
    \ingroup CCDENSITY
    \brief Enter brief description of file here
*/
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "psi4/libciomr/libciomr.h"
#include "psi4/libiwl/iwl.h"
#include "psi4/libdpd/dpd.h"
#include "psi4/libqt/qt.h"
#include "psi4/psifiles.h"
#include "psi4/libmints/matrix.h"
#include "psi4/libmints/vector.h"
#include "psi4/libmints/wavefunction.h"
#include "psi4/libmints/molecule.h"
#include "psi4/libmints/oeprop.h"
#include "psi4/libmints/writer.h"
#include "MOInfo.h"
#include "Params.h"
#include "Frozen.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace ccdensity {
#include "psi4/physconst.h"

#define IOFF_MAX 32641
#define INDEX(i,j) ((i>j) ? (ioff[(i)]+(j)) : (ioff[(j)]+(i)))

void dipole(std::shared_ptr<Wavefunction> wfn)
{
    /* Call OEProp here for each root opdm */
    std::shared_ptr<OEProp> oe(new OEProp(wfn));
    std::shared_ptr<Matrix> Ca = wfn->Ca();
    std::shared_ptr<Matrix> Cb = wfn->Cb();

    Dimension nmopi = wfn->nmopi();
    Dimension frzvpi = wfn->frzvpi();

    SharedMatrix Pa(new Matrix("P alpha", Ca->colspi(), Ca->colspi()));
    SharedMatrix Pb(new Matrix("P beta", Cb->colspi(), Cb->colspi()));
    int mo_offset = 0;
    for (int h = 0; h < Ca->nirrep(); h++) {
      int nmo = nmopi[h];
      int nfv = frzvpi[h];
      int nmor = nmo - nfv;
      if (!nmo || !nmor) continue;

      // Loop over QT, convert to Pitzer
      double **Pap = Pa->pointer(h);
      double **Pbp = Pb->pointer(h);
      for (int i=0; i<nmor; i++) {
        for (int j=0; j<nmor; j++) {
          int I = moinfo.pitzer2qt[i+mo_offset];
          int J = moinfo.pitzer2qt[j+mo_offset];
          if(wfn->same_a_b_dens())
            Pap[i][j] = moinfo.opdm[I][J];
          else {
            Pap[i][j] = moinfo.opdm_a[I][J];
            Pbp[i][j] = moinfo.opdm_b[I][J];
          }
        }
      }
      mo_offset += nmo;
    }

    if(wfn->same_a_b_dens()) Pa->scale(0.5);
    oe->set_Da_mo(Pa);
    if(!wfn->same_a_b_dens()) oe->set_Db_mo(Pb);

    oe->add("DIPOLE");
    oe->add("QUADRUPOLE");
    oe->add("MULLIKEN_CHARGES");
    oe->add("NO_OCCUPATIONS");

    // TODO: This section needs work to duplicate the generic CC DIPOLE X, etc.
    //  into the exact method and/or root, like in DETCI
    outfile->Printf( "\nCC Density OPDM\n\n");
    oe->set_title("CC");

    oe->compute();

    if(params.write_nos) {
      MoldenWriter nowriter(wfn);
      std::string mol_name = wfn->molecule()->name();

      if(wfn->same_a_b_dens()) {
        SharedMatrix Na(new Matrix("Alpha Natural Orbitals", Pa->colspi(), Pa->colspi()));
        SharedVector Oa(new Vector("Alpha NO Occupations", Pa->colspi()));
        // Pa->scale(0.5);
        Pa->diagonalize(Na, Oa, descending);
        nowriter.writeNO(mol_name+"NO.molden",Na,Na,Oa,Oa);
      }
      else{
        SharedMatrix Na(new Matrix("Alpha Natural Orbitals", Pa->colspi(), Pa->colspi()));
        SharedVector Oa(new Vector("Alpha NO Occupations", Pa->colspi()));
        SharedMatrix Nb(new Matrix("Beta Natural Orbitals", Pb->colspi(), Pb->colspi()));
        SharedVector Ob(new Vector("Beta NO Occupations", Pb->colspi()));
        Pa->diagonalize(Na, Oa, descending);
        Pb->diagonalize(Nb, Ob, descending);

        SharedMatrix Pt(Pa);
        Pt->set_name("Total Density");
        Pt->add(Pb);
        SharedMatrix Nt(new Matrix("Total Naural Orbitals", Pa->colspi(), Pa->colspi()));
        SharedVector Ot(new Vector("Total NO Occupations", Pa->colspi()));
        Pt->diagonalize(Nt,Ot, descending);

        nowriter.writeNO(mol_name+"NO.molden",Na,Nb,Oa,Ob);
      }
    }

//  Comments so that autodoc utility will find these PSI variables
//
//  Process::environment.globals["CC DIPOLE X"] =
//  Process::environment.globals["CC DIPOLE Y"] =
//  Process::environment.globals["CC DIPOLE Z"] =
//  Process::environment.globals["CC QUADRUPOLE XX"] =
//  Process::environment.globals["CC QUADRUPOLE XY"] =
//  Process::environment.globals["CC QUADRUPOLE XZ"] =
//  Process::environment.globals["CC QUADRUPOLE YY"] =
//  Process::environment.globals["CC QUADRUPOLE YZ"] =
//  Process::environment.globals["CC QUADRUPOLE ZZ"] =

}

}}
