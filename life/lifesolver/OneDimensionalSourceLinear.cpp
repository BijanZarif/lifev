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
 *  @file
 *  @brief File containing a class for the linear source function B of the 1D hyperbolic problem
 *
 *  @version 1.0
 *  @author Vincent Martin
 *
 *  @version 2.0
 *  @date 15-04-2010
 *  @author Cristiano Malossi <cristiano.malossi@epfl.ch>
 *
 *  @contributor Simone Rossi <simone.rossi@epfl.ch>
 *  @mantainer Cristiano Malossi <cristiano.malossi@epfl.ch>
 */

#include <OneDimensionalSourceLinear.hpp>

namespace LifeV
{
// ===================================================
// Methods
// ===================================================
Real
OneDimensionalSourceLinear::source( const Real& U1, const Real& U2, const ID& ii, const UInt& iNode ) const
{
    if ( ii == 0 ) // S1
    {
        return M_physics->data()->source10( iNode ) +
               M_physics->data()->source11( iNode ) * U1 +
               M_physics->data()->source12( iNode ) * U2;
    }
    if ( ii == 1 ) // S2
    {
        return M_physics->data()->source20( iNode ) +
               M_physics->data()->source21( iNode ) * U1 +
               M_physics->data()->source22( iNode ) * U2;
    }
    ERROR_MSG("The flux function has only 2 components.");
    return -1.;
}

Real
OneDimensionalSourceLinear::dSdU( const Real& /*_U1*/, const Real& /*_U2*/, const ID& ii, const ID& jj, const UInt& iNode ) const
{
    if ( ii == 0 && jj == 0) // dS1/dU1 = 0
    {
        return M_physics->data()->source11( iNode );
    }
    if ( ii == 0 && jj == 1) // dS1/dU2 = 0
    {
        return M_physics->data()->source12( iNode );
    }
    if ( ii == 1 && jj == 0 ) // dS2/dU1
    {
        return M_physics->data()->source21( iNode );
    }
    if ( ii == 1 && jj == 1 ) // dS2/dU2
    {
        return M_physics->data()->source22( iNode );
    }
    ERROR_MSG("Source's differential function has only 4 components.");
    return -1.;
}

//Real
//OneDimensionalSourceLinear::diff2( const Real& /*_U1*/, const Real& /*_U2*/,
//                                          const ID& ii,        const ID& jj, const ID& kk,
//                                          const UInt& /*iNode*/ ) const
//{
//    if( (0 < ii && ii < 3) && (0 < jj && jj < 3) && (0 < kk && kk < 3) )
//    {
//        return 0.;
//    }
//    ERROR_MSG("Source's second differential function has only 8 components.");
//    return -1.;
//}

Real
OneDimensionalSourceLinear::interpolatedQuasiLinearSource( const Real& U1, const Real& U2,
                                                           const ID& ii,   const container2D_Type& bcNodes, const Real& /*cfl*/ ) const
{
    //TODO Implement the interpolation as done for the non-linear case
    return this->source(U1, U2, ii, bcNodes[0]);
}

}
