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

#ifndef OneDimensionalPhysicsLinear_H
#define OneDimensionalPhysicsLinear_H

// LIFEV - MATHCARD
#include <lifemc/lifesolver/OneDimensionalPhysics.hpp>

namespace LifeV
{

//! OneDimensionalPhysicsLinear - Class providing linear physical operations for the 1D model data.
/*!
 *  @author Vincent Martin, Cristiano Malossi
 *
 *  Parameters:
 *  Area0, alpha, beta0, beta1, Kr, rho.
 *
 *  Euler equations:
 *  dA/dt + dQ/dz = 0
 *  dQ/dt + A/rho * dP/dz + Kr * Q/A_0 = 0
 *
 *  with
 *  P - P_ext = beta0 [ ( A / Area0 )^{beta1} - 1 ]
 *
 *  which means
 *  dP / dz = beta0 beta1 ( A / Area0 )^{beta1 - 1} dA / dz
 *  \simeq beta0 beta1 dA / dz
 *
 *  The linearization of Euler model yields
 *
 *  F = [ Q; A * (c_L)^2];
 *  B = [ 0; k_R / A0];
 *  c_L = sqrt( beta0 * beta1 / rho );
 */
class OneDimensionalPhysicsLinear : public OneDimensionalPhysics
{
public:

    //! @name Type definitions and Enumerators
    //@{

    typedef OneDimensionalPhysics           super;

    //@}


    //! @name Constructors & Destructor
    //@{

    //! Constructor
    explicit OneDimensionalPhysicsLinear() : super() {}

    explicit OneDimensionalPhysicsLinear( const dataPtr_Type data ) : super( data ) {}

    //! Destructor
    virtual ~OneDimensionalPhysicsLinear() {}

    //@}


    //! @name Conversion methods
    //@{

    //! Compute W from U
    /*!
     *  Riemann Invariants corresponding to data (Q, A) at node iNode
     *  W1,2 = Q +- celerity * ( A - A0 )
     */
    void fromUToW( Real& W1, Real& W2, const Real& U1, const Real& U2, const UInt& iNode ) const;

    //! Compute U from W
    /*!
     *  Physical variables corresponding to (W1, W2) at node iNode
     *  A = A0 + (W1 - W2) / (2 * celerity)
     *  Q = (W1 + W2) / 2
     */
    void fromWToU( Real& U1, Real& U2, const Real& W1, const Real& W2, const UInt& iNode ) const;

    //! Compute the pressure as a function of W1, W2:
    /*!
     *  P = beta0 * ( ( 1 / Area0 )^(beta1) * ( (W1 - W2) / (2 * celerity0) + Area0 )^(beta1) - 1 )
     */
    Real fromWToP( const Real& W1, const Real& W2, const UInt& iNode ) const;

    //! Compute W1 or W2 given the pressure:
    /*!
     *  W1 - W2 = (2 * celerity * A0) * ( ( P / beta0 + 1 )^(1/beta1) - 1 )
     *  W1 - W2 = 4 * sqrt( beta0 / (beta1 * rho ) ) * ( sqrt( P / beta0 + 1 ) - 1 )
     */
    Real fromPToW( const Real& P, const Real& W, const ID& i, const UInt& iNode ) const;

    //! Compute W1 or W2 given the flux
    /*!
     *  W1 + W2 = 2 * Q
     */
    Real fromQToW( const Real& Q, const Real& W_n, const Real& W, const ID& i, const UInt& iNode ) const;

    //@}


    //! @name Derivatives methods
    //@{

    //! Compute the derivative of pressure with respect to W1 and W2
    /*!
     *  dP(W1,W2)/dW_1 = beta0 * beta1 / ( 2 * celerity0 * Area0^(beta1) ) * ( (W1 - W2) / ( 2 * celerity0 ) + Area0 )^(beta1-1)
     *  dP(W1,W2)/dW_2 = - dP(W1,W2)/dW_1
     */
    Real dPdW( const Real& W1, const Real&_W2, const ID& i, const UInt& iNode ) const;

    //@}

private:

    //! @name Unimplemented Methods
    //@{

    OneDimensionalPhysicsLinear& operator=( const dataPtr_Type data );

    //@}
};

//! Factory create function
inline OneDimensionalPhysics* createOneDimensionalPhysicsLinear()
{
    return new OneDimensionalPhysicsLinear();
}

}

#endif //OneDimensionalPhysicsLinear_H
