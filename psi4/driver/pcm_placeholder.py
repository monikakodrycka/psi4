#
# @BEGIN LICENSE
#
# Psi4: an open-source quantum chemistry software package
#
# Copyright (c) 2007-2016 The Psi4 Developers.
#
# The copyrights for code used from other parties are included in
# the corresponding files.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# @END LICENSE
#

# -*- python -*-
# -*- coding: utf-8 -*-
# vim:filetype=python:
#
# Roberto Di Remigio <roberto.d.remigio@uit.no>
# University of Tromso 2015
#

"""
This is a placeholder for the real pcmsolver.py script.
The location of the real pcmsolver.py is configured by CMake
to point to the proper install prefix.
In this way we avoid to transform inputparser.py into a file
that has to be configured by CMake
With conda, this starts to get complicated. Bottom option
works for build-in-place and build-psi-w-prebuilt-pcmsolver.
Upper option necessary when psi4metapackage is conda build
dependency. At least I think that's what's going on.

"""

if '' == 'ON':
    PCMSolver_PARSE_DIR = '/opt/anaconda1anaconda2anaconda3/bin'
else:
    PCMSolver_PARSE_DIR = '/theoryfs2/ds/richard/SrcFiles/psi4/build/stage/theoryfs2/ds/richard/SrcFiles/psi4/Install/external/pcmsolver/bin'

