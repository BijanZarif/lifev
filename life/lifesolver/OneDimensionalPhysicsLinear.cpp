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
 *  @brief File containing a class providing linear physical operations for the 1D model data.
 *
 *  @version 1.0
 *  @date 01-07-2004
 *  @author Vincent Martin
 *
 *  @version 2.0
 *  @date 13-04-2010
 *  @author Cristiano Malossi <cristiano.malossi@epfl.ch>
 *
 *  @contributor Simone Rossi <simone.rossi@epfl.ch>
 *  @mantainer  Cristiano Malossi <cristiano.malossi@epfl.ch>
 */

#include <lifemc/lifesolver/OneDimensionalPhysicsLinear.hpp>

namespace LifeV
{

// ===================================================
// Conversion Methods
// ===================================================
void
OneDimensionalPhysicsLinear::fromUToW( Real& W1, Real& W2, const Real& U1, const Real& U2, const UInt& iNode ) const
{
    W1 = U2 + celerity0( iNode ) * ( U1 - M_data->area0( iNode ) );

    W2 = U2 - celerity0( iNode ) * ( U1 - M_data->area0( iNode ) );

#ifdef HAVE_LIFEV_DEBUG
    Debug( 6320 ) << "[OneDimensionalModel_Physics_Linear::fromUToW] Q " << U2 << "\n";
    Debug( 6320 ) << "[OneDimensionalModel_Physics_Linear::fromUToW] W1 " << W1 << "\n";
    Debug( 6320 ) << "[OneDimensionalModel_Physics_Linear::fromUToW] W2 " << W2 << "\n";
    Debug( 6320 ) << "[OneDimensionalModel_Physics_Linear::fromUToW] celerity " << celerity0( iNode ) << "\n";
    Debug( 6320 ) << "[OneDimensionalModel_Physics_Linear::fromUToW] ( _U1 - area0( iNode ) ) " << ( U1 - M_data->area0( iNode ) ) << "\n";
#endif
}

void
OneDimensionalPhysicsLinear::fromWToU( Real& U1, Real& U2, const Real& W1, const Real& W2, const UInt& iNode ) const
{
    U1 = M_data -> area0( iNode ) + ( W1 - W2) / ( 2 * celerity0( iNode ) );

    U2 = ( W1 + W2 ) / 2;
}

Real
OneDimensionalPhysicsLinear::fromWToP( const Real& W1, const Real& W2, const UInt& iNode ) const
{
    return ( M_data -> beta0( iNode )
             * (   OneDimensional::pow05( 1 / M_data->area0( iNode ), M_data -> beta1( iNode ) )
                 * OneDimensional::pow05( (W1 - W2 ) / ( 2 * celerity0( iNode ) ) + M_data -> area0( iNode ), M_data -> beta1( iNode ) )
                 - 1 )
           );
}

Real
OneDimensionalPhysicsLinear::fromPToW( const Real& P, const Real& W, const ID& i, const UInt& iNode ) const
{
    Real add( 2 * celerity0( iNode ) * M_data -> area0( iNode ) * ( OneDimensional::pow20( P / M_data -> beta0( iNode ) + 1, 1 / M_data -> beta1( iNode ) ) - 1 ) );

#ifdef HAVE_LIFEV_DEBUG
    Debug(6320) << "[fromPToW] "
    << "2 * celerity0( iNode ) * area0( iNode ) = " << 2 * celerity0( iNode ) * M_data -> area0( iNode )
    << ", pow( ( P / beta0( iNode ) + 1 ), 1 / beta1( iNode ) ) = "
    << OneDimensional::pow20( P / M_data -> beta0( iNode ) + 1 , 1 / M_data -> beta1( iNode ) ) << "\n";
    Debug(6320) << "[fromPToW] add term = " << add << "\n";
#endif

    if ( i == 0 )
        return W - add;
    if ( i == 1 )
        return W + add;

    ERROR_MSG("You can only find W1 or W2 as function of P");
    return -1.;
}

Real
OneDimensionalPhysicsLinear::fromQToW( const Real& Q, const Real& /*W_n*/, const Real& W, const ID& i, const UInt& /*iNode*/ ) const
{
    Real add( 2 * Q );

    if ( i == 0 ) // W1 given
        return add - W;

    if ( i == 1 ) // W2 given
        return add - W;

    ERROR_MSG("You can only find W1 or W2 as function of Q");
    return -1.;
}

// ===================================================
// Derivatives Methods
// ===================================================
Real
OneDimensionalPhysicsLinear::dPdW( const Real& W1, const Real& W2, const ID& i, const UInt& iNode ) const
{
    Real beta0beta1overA0beta1 ( M_data->beta0( iNode ) * M_data -> beta1( iNode ) / OneDimensional::pow05( M_data -> area0( iNode ), M_data -> beta1( iNode ) ) );

    Real oneover2celerity( 1 / ( 2 * celerity0( iNode ) ) );

    Real result( beta0beta1overA0beta1 * oneover2celerity );
    result *= ( ( W1 - W2 ) * oneover2celerity + M_data -> area0( iNode ) );

    if ( i == 0 ) //! dP/dW1
        return result;

    if ( i == 1 ) //! dP/dW2
        return -result;

    ERROR_MSG("P(W1,W2)'s differential function has only 2 components.");
    return -1.;
}

}
