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

/*! \file cart.cc
    \ingroup optking
    \brief cart class definition ; for xyz coordinate on an atom
*/

#include "cart.h"
#include <sstream>
#include "opt_except.h"

#include "v3d.h"
#include "physconst.h"
#include "psi4-dec.h"
namespace opt {

using namespace v3d;
using std::ostringstream;

// constructor - makes sure A<B ; default freeze_in =false
CART::CART(int A_in, int xyz_in, bool freeze_in) : SIMPLE(cart_type, 1, freeze_in) {
  s_atom[0] = A_in;
  xyz = xyz_in;
}

double CART::value(GeomType geom) const {
  return geom[s_atom[0]][xyz];
}

// compute and return array of first derivative (B marix elements)
// return order is:
// atom_i  cartesian
// 0       x y z
// 1       x y z ..
double ** CART::DqDx(GeomType geom) const {
  double **dqdx = init_matrix(1,3);
  dqdx[0][xyz] = 1.0;
  return dqdx;
}

// compute and return array of second derivative (B' matrix elements)
double ** CART::Dq2Dx2(GeomType geom) const {
  double **dq2dx2 = init_matrix(3,3);
  return dq2dx2;
}

// print cart and value
void CART::print(std::string OutFileRMR, GeomType geom, int off) const {
   boost::shared_ptr<psi::PsiOutStream> printer(OutFileRMR=="outfile"? psi::outfile:
         boost::shared_ptr<psi::OutFile>(new psi::OutFile(OutFileRMR,psi::APPEND)));
   ostringstream iss(ostringstream::out); // create stream; allow output to it
  iss << get_definition_string(off);

  double val = value(geom);
  if (!s_frozen)
    printer->Printf("\t %-15s  =  %15.6lf\t%15.6lf\n", iss.str().c_str(), val, val*_bohr2angstroms);
  else
    printer->Printf("\t*%-15s  =  %15.6lf\t%15.6lf\n", iss.str().c_str(), val, val*_bohr2angstroms);

}

// function to return string of coordinate definition
std::string CART::get_definition_string(int off) const {
  ostringstream iss(ostringstream::out); // create stream; allow output to it
  iss << "R(" << s_atom[0]+1+off << ",";

  if (xyz == 0)      iss << "X";
  else if (xyz == 1) iss << "Y";
  else if (xyz == 2) iss << "Z";
  
  iss << ")" << std::flush ;
  return iss.str();
}

void CART::print_intco_dat(std::string OutFileRMR, int off) const {
   boost::shared_ptr<psi::PsiOutStream> printer(OutFileRMR=="outfile"? psi::outfile:
         boost::shared_ptr<psi::OutFile>(new psi::OutFile(OutFileRMR,psi::APPEND)));
   printer->Printf("X");

  if (s_frozen) 
    printer->Printf("*");
  else
    printer->Printf(" ");

  printer->Printf( "%6d", s_atom[0]+1+off);

  if      (xyz == 0)  printer->Printf( "     X");
  else if (xyz == 1)  printer->Printf( "     Y");
  else if (xyz == 2)  printer->Printf( "     Z");

  if (s_has_fixed_eq_val)
    printer->Printf( "%10.5lf", s_fixed_eq_val);

  printer->Printf( "\n");
}

// print displacement
void CART::print_disp(std::string OutFileRMR, const double q_orig, const double f_q,
    const double dq, const double new_q, int atom_offset) const {
   boost::shared_ptr<psi::PsiOutStream> printer(OutFileRMR=="outfile"? psi::outfile:
         boost::shared_ptr<psi::OutFile>(new psi::OutFile(OutFileRMR,psi::APPEND)));
   ostringstream iss(ostringstream::out);
  if (s_frozen) iss << "*";
  iss << "R(" << s_atom[0]+atom_offset+1 << ",";

  if      (xyz == 0)  iss << "X";
  else if (xyz == 1)  iss << "Y";
  else if (xyz == 2)  iss << "Z";

  iss << ")" << std::flush ;

  printer->Printf("%-15s = %13.6lf%13.6lf%13.6lf%13.6lf\n",
    iss.str().c_str(), q_orig*_bohr2angstroms, f_q*_hartree2aJ/_bohr2angstroms,
    dq*_bohr2angstroms, new_q*_bohr2angstroms);
}


// print s vectors
void CART::print_s(std::string OutFileRMR, GeomType geom) const {
   boost::shared_ptr<psi::PsiOutStream> printer(OutFileRMR=="outfile"? psi::outfile:
         boost::shared_ptr<psi::OutFile>(new psi::OutFile(OutFileRMR,psi::APPEND)));
   printer->Printf("S vector for cart R(%d %d): \n",
    s_atom[0]+1, s_atom[1]+1);
  double **dqdx = DqDx(geom);
  printer->Printf("Atom 1: %12.8f %12.8f,%12.8f\n", dqdx[0][0],dqdx[0][1],dqdx[0][2]);
  free_matrix(dqdx);
}

bool CART::operator==(const SIMPLE & s2) const {
  if (cart_type != s2.g_type())
    return false;

  if ((this->s_atom[0] == s2.g_atom(0)) && (this->xyz == s2.g_xyz()))
    return true;
  else
    return false;
}

}
