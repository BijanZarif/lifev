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
 *  @brief File containing a base class providing physical operations for the 1D model data.
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
 *  @maintainer  Cristiano Malossi <cristiano.malossi@epfl.ch>
 */

#include <life/lifesolver/OneDimensionalPhysics.hpp>

namespace LifeV
{

// ===================================================
// Methods
// ===================================================
void
OneDimensionalPhysics::stiffenVesselLeft( const Real& xl,        const Real& xr,
                                          const Real& factor,    const Real& alpha,
                                          const Real& delta,     const Real& n,
                                          const Real& minDeltaX, const UInt& yesAdaptive )
{
    /* Stiffen Left boundary with a fifth order polynomial law
       if (alpha-delta/2) <= x < alpha
       coeff = ( (alpha + delta/2) - x )^5 * 2^4 / delta^5;

       if alpha <= x <= alpha + delta/2
       coeff = 1 - ( x - (alpha - delta/2))^5 * 2^4 / delta^5;
    */
    if (yesAdaptive)
    {
        Real ratio, n_elem_delta,n_elem_r,n_elem_l;

        UInt iz=0, alpha_iz;

        //      alpha_iz = static_cast<UInt>( alpha / (xr-xl) * static_cast<Real>( M_dataPtr -> numberOfElements()-1 ) );
        alpha_iz = static_cast<int>( std::floor( (alpha - delta / 2 ) / minDeltaX + 0.5 ) ) +
                   ( ( M_dataPtr -> numberOfElements() - 1 ) -
                     static_cast<int>( std::floor( ( xr - ( alpha + delta / 2 ) ) / minDeltaX + 0.5 ) ) -
                     static_cast<int>( std::floor( ( alpha - delta / 2 ) / minDeltaX + 0.5 ) ) ) / 2;

        //      n_elem_r = static_cast<Real>( (M_dataPtr -> numberOfElements()-1) - alpha_iz );
        n_elem_r = ( ( M_dataPtr -> numberOfElements() - 1 ) - alpha_iz ) -
                   static_cast<int>( std::floor( ( xr - ( alpha + delta / 2 ) ) / minDeltaX + 0.5 ) );

        //      n_elem_l = static_cast<Real>( alpha_iz );
        n_elem_l = alpha_iz -
                   static_cast<int>( std::floor( ( alpha - delta / 2 ) / minDeltaX + 0.5 ) );

        n_elem_delta = static_cast<Real>( M_dataPtr -> numberOfElements() - 1 ) / ( xr - xl ) * delta;

        //      n_elem_delta = n_elem_r + n_elem_l;
        Real x_current,deltax,deltax_adaptive,deltax_uniform;

        x_current = alpha;

        do
        {
            //! beta0
            // fifth order
            ratio=( ( ( alpha + delta / 2 ) - x_current ) / delta);

            M_dataPtr -> setdBeta0dz( M_dataPtr -> beta0(alpha_iz + iz) *
                                 ( factor * (- n / delta) * ( std::pow(2,(n-1)) * std::pow(ratio, (n-1)) ) ), alpha_iz + iz );
            M_dataPtr -> setdBeta0dz( M_dataPtr -> dBeta0dz(alpha_iz + iz), alpha_iz - iz );

            M_dataPtr -> setBeta0( M_dataPtr -> beta0(alpha_iz + iz) * ( 1 + factor * ( std::pow(2,(n-1)) * std::pow(ratio,n) ) ), alpha_iz + iz );
            M_dataPtr -> setBeta0( M_dataPtr -> beta0(alpha_iz + iz) / ( 1 + factor * ( std::pow(2,(n-1)) * std::pow(ratio,n) ) )
                              * ( 1 + factor * ( 1 - ( std::pow(2,(n-1)) * std::pow(ratio,n) ) ) ), alpha_iz - iz );

            // first order
            //        M_dPressbeta0dz[iz] = M_Pressbeta0[iz] * ( -factor * (n / delta) *
            //                       ( std::pow(2,(n-1)) * std::pow(ratio,(n-1)) ) );
            //M_Pressbeta0[iz] = M_Pressbeta0[iz] * ( 1 + factor * ratio );


            deltax_adaptive = ( -1 / n_elem_delta ) * ( 1 / ( ( -n / delta) * std::pow(2,(n-1) ) *
                                                            std::pow( ratio , (n-1) ) ) );

            deltax_uniform = ( ( alpha + delta / 2) - x_current ) / ( n_elem_r - iz );

            iz++;

            deltax = ( ( deltax_adaptive < deltax_uniform ) && ( iz < n_elem_r ) )
                     ? deltax_adaptive : deltax_uniform;

            //( xr - xl ) / M_edgeList.size();
            ASSERT_PRE( deltax > 0 , "The left point is on the right..." );

            x_current += deltax;

        }
        while ( ( x_current < ( alpha + delta/2 ) ) && ( ( alpha_iz - ( iz - 1 ) ) > 0) );

        if ( ( alpha_iz - ( iz - 1 ) ) > 0)
        {
            do
            {
                M_dataPtr -> setBeta0( M_dataPtr -> beta0(alpha_iz - iz) * ( 1 + factor ), alpha_iz - iz );
                iz++;
            }
            while ( (alpha_iz - ( iz - 1 ) ) > 0 );

            //      M_PressBeta0[0] = M_PressBeta0[0] *
            //  ( 1 + factor );
        }
        else
            std::cout << "[stiffenVesselRight] error! out of left boundary" << std::endl;
    }

    else
    {
        UInt iz=0;

        Real ratio, x_current=xl, deltax;

        deltax=( xr - xl ) / static_cast<Real>(M_dataPtr -> numberOfElements() - 1 );

        while ( ( x_current < ( alpha - delta / 2 ) ) && ( iz < M_dataPtr -> numberOfElements() ) )
        {
            M_dataPtr -> setBeta0( M_dataPtr->beta0(iz) * ( 1 + factor ), iz );
            iz++;
            x_current+=deltax;
        }

        while ( (x_current < alpha) && (iz < M_dataPtr -> numberOfElements()) )
        {
            ratio=(( x_current - (alpha-delta/2) ) / delta);

            M_dataPtr -> setdBeta0dz( M_dataPtr -> beta0(iz) * ( factor * (- n / delta ) * ( std::pow(2,(n-1)) * std::pow(ratio,(n-1) ) ) ), iz );

            M_dataPtr -> setBeta0( M_dataPtr -> beta0(iz) * ( 1 + factor * ( 1 - std::pow(2,(n-1)) * std::pow(ratio,n) ) ), iz );
            iz++;
            x_current+=deltax;
        }

        while ( ( x_current < ( alpha + delta / 2 ) ) && (iz < M_dataPtr -> numberOfElements()) )
        {
            ratio=( ( ( alpha + delta / 2 ) - x_current ) / delta );

            M_dataPtr -> setdBeta0dz( M_dataPtr -> beta0(iz) * ( factor * ( -n / delta) * ( std::pow(2,(n-1)) * std::pow(ratio,(n-1) ) ) ), iz );

            M_dataPtr -> setBeta0( M_dataPtr -> beta0(iz) * ( 1 + factor * ( std::pow(2,(n-1)) * std::pow(ratio,n) ) ), iz );
            iz++;
            x_current += deltax;
        }
    }
}

void
OneDimensionalPhysics::stiffenVesselRight( const Real& xl,        const Real& xr,
                                           const Real& factor,    const Real& alpha,
                                           const Real& delta,     const Real& n,
                                           const Real& minDeltaX, const UInt& yesAdaptive )
{
#ifdef HAVE_LIFEV_DEBUG
    Debug( 6320 ) << "stiffenVesselright ...\n";
#endif
    /* Stiffen Left boundary with a fifth order polynomial law

       if (alpha-delta/2) <= x < alpha
       coeff = ( x - (alpha - delta/2) )^5 * 2^4 / delta^5;

       if alpha <= x <= alpha + delta/2
       coeff = 1 + ( x - (alpha + delta/2))^5 * 2^4 / delta^5;
    */
    if ( yesAdaptive )
    {
        Real ratio, n_elem_delta,n_elem_r,n_elem_l;

        UInt iz=0, alpha_iz;

        //      alpha_iz = static_cast<UInt>( alpha / (xr-xl) * ( static_cast<Real>( M_dataPtr -> numberOfElements()-1 ) ) );
        alpha_iz = static_cast<int>( std::floor( ( alpha - delta / 2 ) / minDeltaX + 0.5 ) ) +
                   ( (M_dataPtr -> numberOfElements() - 1 ) -
                     static_cast<int>( std::floor( ( xr - ( alpha + delta / 2 ) ) / minDeltaX + 0.5 ) ) -
                     static_cast<int>( std::floor( ( alpha - delta / 2 ) / minDeltaX + 0.5 ) ) ) / 2;

        n_elem_delta = static_cast<Real>( M_dataPtr -> numberOfElements() - 1 ) / ( xr - xl ) * delta;

        //      n_elem_r = static_cast<Real>( (M_dataPtr -> numberOfElements()-1) - alpha_iz );
        n_elem_r = ( ( M_dataPtr -> numberOfElements() - 1 ) - alpha_iz ) -
                   static_cast<int>( std::floor( ( xr - ( alpha + delta / 2 ) ) / minDeltaX + 0.5 ) );

        //      n_elem_l = static_cast<Real>( alpha_iz );
        n_elem_l = alpha_iz -
                   static_cast<int>( std::floor( ( alpha - delta / 2 ) / minDeltaX + 0.5 ) );

        Real x_current,deltax,deltax_adaptive,deltax_uniform;

        x_current = alpha;

        do
        {
            //! beta0
            // fifth order
            ratio=( ( ( alpha + delta / 2 ) - x_current ) / delta );

            M_dataPtr -> setdBeta0dz( M_dataPtr -> beta0( alpha_iz + iz ) * ( factor * ( n / delta) *
                                                                  ( std::pow(2,(n-1)) * std::pow(ratio,(n-1)) ) ), alpha_iz + iz );

            M_dataPtr -> setdBeta0dz( M_dataPtr -> dBeta0dz(alpha_iz + iz), alpha_iz - iz );

            M_dataPtr -> setBeta0( M_dataPtr -> beta0(alpha_iz + iz) *
                              ( 1 + factor * ( 1 - ( std::pow(2,(n-1)) * std::pow(ratio,n) ) ) ), (alpha_iz + iz) );

            M_dataPtr -> setBeta0( M_dataPtr -> beta0(alpha_iz + iz) /
                              ( 1 + factor * ( 1 - ( std::pow(2,(n-1)) * std::pow(ratio,n) ) ) )
                              * ( 1 + factor * ( std::pow(2,(n-1)) * std::pow(ratio,n) ) ), alpha_iz - iz );

            // first order
            //        M_dataPtr -> dBeta0dz(iz) = M_dataPtr -> Beta0(iz) * ( -factor * (n / delta) *
            //                       ( std::pow(2,(n-1)) * std::pow(ratio,(n-1)) ) );
            //M_dataPtr -> Beta0(iz) = M_dataPtr -> Beta0(iz) * ( 1 + factor * ratio );

            deltax_adaptive = ( -1 / n_elem_delta ) *
                              ( 1 / ( (-n / delta ) * std::pow(2,(n-1)) *
                                      std::pow( ratio , (n-1) )
                                    )
                              );

            deltax_uniform = ( ( alpha + delta / 2) - x_current ) / ( n_elem_r - iz );

            iz++;

            deltax = ( ( deltax_adaptive < deltax_uniform ) && ( iz < n_elem_r) )
                     ? deltax_adaptive : deltax_uniform;

            //( xr - xl ) / M_edgeList.size();
            ASSERT_PRE( deltax > 0 ,
                        "The left point is on the right..." );

            x_current += deltax;

        }
        while ( x_current < ( alpha + delta / 2 ) && ( ( alpha_iz - ( iz - 1 ) ) > 0) );

        if ( ( alpha_iz + iz ) <= (M_dataPtr -> numberOfElements() -1 ) )
        {
            do
            {
                M_dataPtr -> setBeta0( M_dataPtr -> beta0(alpha_iz + iz ) * ( 1 + factor ), alpha_iz + iz );
                iz++;
            }
            while ( ( alpha_iz + iz - 1 ) < ( M_dataPtr -> numberOfElements() -1 ) );

            //      M_PressBeta0[0] = M_PressBeta0[0] *
            //  ( 1 + factor );
        }
        else
            std::cout << "\n[stiffenVesselRight] error! out of right boundary" << std::endl;
    }
    else
    {
        UInt iz = M_dataPtr -> numberOfElements()-1;

        Real ratio, x_current=xr, deltax;

        deltax = ( xr - xl ) / static_cast<Real>(M_dataPtr -> numberOfElements() - 1 );

        while ( ( x_current > ( alpha + delta / 2 ) ) && ( ( iz + 1 ) > 0 ) )
        {
            M_dataPtr -> setBeta0( M_dataPtr -> beta0(iz) * ( 1 + factor ), iz );
            iz--;
            x_current -= deltax;
        }

        while ( ( x_current > alpha ) && ( ( iz + 1 ) > 0 ) )
        {
            ratio=( ( ( alpha + delta / 2 ) - x_current ) / delta );

            M_dataPtr -> setdBeta0dz( M_dataPtr -> beta0(iz) * ( factor * ( n / delta) *  ( std::pow(2,(n-1)) * std::pow(ratio,(n-1)) ) ), iz );

            M_dataPtr -> setBeta0( M_dataPtr -> beta0(iz) * ( 1 + factor * ( 1 - std::pow(2,(n-1)) * std::pow(ratio,n) ) ), iz );
            iz--;
            x_current -= deltax;
        }

        while ( ( x_current > ( alpha - delta / 2 ) ) && ( ( iz + 1 ) > 0 ) )
        {
            ratio = ( ( x_current - ( alpha - delta / 2 ) ) / delta );

            M_dataPtr -> setdBeta0dz( M_dataPtr -> beta0(iz) * ( factor * ( n / delta) * ( std::pow(2,(n-1)) * std::pow(ratio,(n-1)) ) ), iz );

            M_dataPtr -> setBeta0( M_dataPtr -> beta0(iz) * ( 1 + factor * ( std::pow(2,(n-1)) * std::pow(ratio,n) ) ), iz );
            iz--;
            x_current -= deltax;
        }
    }
}


}
