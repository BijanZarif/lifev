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
 *  @brief File containing a base class for non linear 1D model flux function.
 *
 *  @version 1.0
 *  @author Vincent Martin
 *
 *  @version 2.0
 *  @date 15-04-2010
 *  @author Cristiano Malossi <cristiano.malossi@epfl.ch>
 *
 *  @contributors Simone Rossi <simone.rossi@epfl.ch>, Ricardo Ruiz-Baier <ricardo.ruiz@epfl.ch>
 *  @mantainer Cristiano Malossi <cristiano.malossi@epfl.ch>
 */

#include <OneDimensionalFluxNonLinear.hpp>

namespace LifeV
{
// ===================================================
// Methods
// ===================================================
Real
OneDimensionalFluxNonLinear::flux( const Real& A, const Real& Q, const ID& ii,  const UInt& iNode ) const
{
    if ( ii == 1 ) // F1
    {
        return Q;
    }

    if ( ii == 2 ) // F2
    {
        return ( M_physics->data()->alpha( iNode ) * Q * Q / A +
                 M_physics->data()->beta0( iNode ) * M_physics->data()->beta1( iNode ) *
                 M_physics->data()->area0( iNode ) / ( ( M_physics->data()->beta1( iNode ) + 1 ) *
                 M_physics->data()->densityRho() ) * ( OneDimensional::pow15( A / M_physics->data()->area0( iNode ), M_physics->data()->beta1( iNode ) + 1 ) - 1 ) ) *
                 M_physics->data()->robertsonCorrection();
    }

    ERROR_MSG("The flux function has only 2 components.");

    return -1.;
}

Real
OneDimensionalFluxNonLinear::dFdU( const Real& A, const Real& Q, const ID& ii, const ID& jj, const UInt& iNode ) const
{
    if ( ii == 1 && jj == 1 ) // dF1/dA
    {
        return 0.;
    }

    if ( ii == 1 && jj == 2 ) // dF1/dQ
    {
        return 1.;
    }

    if ( ii == 2 && jj == 1 ) // dF2/dA
    {
        return ( M_physics->data()->beta0( iNode ) *
                 M_physics->data()->beta1( iNode ) /
                 M_physics->data()->densityRho() * OneDimensional::pow05( A / M_physics->data()->area0( iNode ), M_physics->data()->beta1( iNode ) ) -
                 M_physics->data()->alpha( iNode ) * Q * Q / A / A ) *
                 M_physics->data()->robertsonCorrection();
    }
    if ( ii == 2 && jj == 2 ) // dF2/dQ
    {
        return M_physics->data()->robertsonCorrection() * 2 * M_physics->data()->alpha( iNode ) * Q / A;
    }

    ERROR_MSG("Flux's differential function has only 4 components.");

    return -1.;
}

//Real
//OneDimensionalFluxNonLinear::diff2( const Real& A, const Real& Q,
//                                           const ID& ii,   const ID& jj, const ID& kk,
//                                           const UInt& iNode ) const
//{
//    // diff second of F1 is always 0.
//    if( ii == 1 ) // d2F1/dUjdUk = 0.
//    {
//        if( ( jj == 1 || jj == 2 ) && ( kk == 1 || kk == 2 ) )
//            return 0.;
//    }
//    if( ii == 2 )
//    {
//        if( jj == 1 && kk == 1 )  // d2F2/dA2
//        {
//            return M_physics->data()->robertsonCorrection()
//                       * M_physics->data()->beta0( iNode ) * M_physics->data()->beta1( iNode ) * M_physics->data()->beta1( iNode )
//                       / ( M_physics->data()->densityRho() * M_physics->data()->area0( iNode ) )
//                       * std::pow( A / M_physics->data()->area0( iNode ), M_physics->data()->beta1( iNode ) - 1 );
//        }
//        // cross terms (equal)
//        if( (jj == 1 && kk == 2) || (jj == 2 && kk == 1) ) // d2F2/dAdQ=d2F2/dQdA
//        {
//            return -M_physics->data()->robertsonCorrection() * M_physics->data()->alpha( iNode ) * Q / ( A * A );
//        }
//        if( jj == 2 && kk == 2 ) // d2F2/dQ2
//        {
//            return M_physics->data()->robertsonCorrection() * 2 * M_physics->data()->alpha( iNode ) / A;
//        }
//    }
//    ERROR_MSG("Flux's second differential function has only 8 components.");
//
//    return -1.;
//}

void
OneDimensionalFluxNonLinear::eigenValuesEigenVectors( const Real& A,
                                                      const Real& Q,
                                                      container2D_Type& eigenvalues,
                                                      container2D_Type& leftEigenvector1,
                                                      container2D_Type& leftEigenvector2,
                                                      const UInt& iNode ) const
{
#ifdef HAVE_LIFEV_DEBUG
    Debug(6312) << "[OneDimensionalModel_Flux_NonLinear]::jabocian_EigenValues_Vectors\n";
#endif

    Real celerity;
    celerity       = std::sqrt( M_physics->data()->alpha( iNode ) * (
                                M_physics->data()->alpha( iNode ) - 1) * Q * Q / ( A * A ) +
                                M_physics->data()->beta0( iNode ) *
                                M_physics->data()->beta1( iNode ) /
                                M_physics->data()->densityRho() * OneDimensional::pow05( A / M_physics->data()->area0( iNode ), M_physics->data()->beta1( iNode ) ) );

    eigenvalues[0] = M_physics->data()->alpha( iNode ) * Q / A + celerity;
    eigenvalues[1] = M_physics->data()->alpha( iNode ) * Q / A - celerity;

    leftEigenvector1[0] = - eigenvalues[1] / A;
    leftEigenvector1[1] = 1. / A;
    leftEigenvector2[0] = - eigenvalues[0] / A;
    leftEigenvector2[1] = 1. / A;
}

void
OneDimensionalFluxNonLinear::deltaEigenValuesEigenVectors( const Real& A,
                                                           const Real& Q,
                                                           container2D_Type& deltaEigenvalues,
                                                           container2D_Type& deltaLeftEigenvector1,
                                                           container2D_Type& deltaLeftEigenvector2,
                                                           const UInt& iNode ) const
{
    Real deltaCelerity;

    Real AoverA0( A / M_physics->data()->area0( iNode ) );
    Real C ( OneDimensional::pow05(  AoverA0, M_physics->data()->beta1( iNode ) ) / M_physics->data()->densityRho() );

    deltaCelerity  = 0.5 / std::sqrt( M_physics->data()->alpha( iNode ) * (
                     M_physics->data()->alpha( iNode ) - 1) * Q * Q / ( A * A ) +
                     M_physics->data()->beta0( iNode ) *
                     M_physics->data()->beta1( iNode ) * C ) * ( C * (
                     M_physics->data()->beta1( iNode ) *
                     M_physics->data()->dBeta0dz( iNode ) -
                     M_physics->data()->beta0( iNode ) *
                     M_physics->data()->beta1( iNode ) *
                     M_physics->data()->beta1( iNode ) /
                     M_physics->data()->area0( iNode ) *
                     M_physics->data()->dArea0dz( iNode ) +
                     M_physics->data()->beta0( iNode ) * ( 1 +
                     M_physics->data()->beta0( iNode ) * std::log( AoverA0 ) ) *
                     M_physics->data()->dBeta1dz( iNode ) ) + ( 2 *
                     M_physics->data()->alpha( iNode ) - 1 ) * Q * Q / ( A * A ) *
                     M_physics->data()->dAlphadz( iNode ) );

    deltaEigenvalues[0] = M_physics->data()->dAlphadz( iNode ) * Q / A + deltaCelerity;
    deltaEigenvalues[1] = M_physics->data()->dAlphadz( iNode ) * Q / A - deltaCelerity;

    deltaLeftEigenvector1[0] = - deltaEigenvalues[1] / A;
    deltaLeftEigenvector1[1] = 0;
    deltaLeftEigenvector2[0] = - deltaEigenvalues[0] / A;
    deltaLeftEigenvector2[1] = 0;
}

}
