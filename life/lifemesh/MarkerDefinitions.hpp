//@HEADER
/*
*******************************************************************************

    Copyright (C) 2004, 2005, 2007 EPFL, Politecnico di Milano, INRIA
    Copyright (C) 2010 EPFL, Politecnico di Milano, Emory University

    This file is part of LifeV.

    LifeV is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LifeV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LifeV.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************
*/
//@HEADER

/*!
    @file
    @brief A simple implementations of Markers

    @date 00-00-0000
    @author Luca Formaggia
    @contributor Luca Bertagna <lbertag@emody.edu>

    This is the simplest implementation of the markers, which just adopts the
    basis marker classes defined in marker_base.h.

    Specialised markers can be implemented using this "template" as
    reference.
*/

#ifndef MARKERDEFINITIONS_H
#define MARKERDEFINITIONS_H 1

#include <life/lifemesh/Marker.hpp>

namespace LifeV
{

//! The simples MarkerCommon: uses all defaults
typedef MarkerCommon<MarkerTraits> defaultMarkerCommon_Type;

//!Expose entityFlag_Type
typedef MarkerTraits::entityFlag_Type entityFlag_Type;

//!Expose NULLFLAG
static const entityFlag_Type S_NULLFLAG = MarkerTraits::S_NULLFLAG;

// Old static variable to delete after the make check
//static const entityFlag_Type NULLFLAG __attribute__ ((deprecated)) = MarkerTraits::S_NULLFLAG;

// Old typedefs to delete
typedef MarkerCommon<MarkerTraits> DefMarkerCommon __attribute__ ((deprecated));
typedef MarkerTraits::EntityFlag EntityFlag __attribute__ ((deprecated));

} // Namespace LifeV

#endif /* MARKERDEFINITIONS_H */
