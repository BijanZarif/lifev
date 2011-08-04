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

#ifndef OneDimensionalSourceLinear_H
#define OneDimensionalSourceLinear_H

// LIFEV - MATHCARD
#include <lifemc/lifesolver/OneDimensionalSource.hpp>

namespace LifeV
{

//! OneDimensionalSourceLinear - Class for the linear source function S of the 1D hyperbolic problem.
/*!
 *  @author Vincent Martin, Cristiano Malossi
 *
 *  dU/dt + dF(U)/dz + S(U) = 0
 *  with U=[U1,U2]^T
 */
class OneDimensionalSourceLinear : public OneDimensionalSource
{

public:

    //! @name Type definitions and Enumerators
    //@{

    typedef OneDimensionalSource         super;

    //@}


    //! @name Constructors & Destructor
    //@{

    //! Constructor
    explicit OneDimensionalSourceLinear() : super() {}

    explicit OneDimensionalSourceLinear( const physicsPtr_Type physics ) : super( physics ) {}

    //! Do nothing destructor
    virtual ~OneDimensionalSourceLinear() {}

    //@}


    //! @name Methods
    //@{

    //! S = [S1, S2]^T
    /*!
     *
     * S1 = S10 + S11 U1 + S12 U2
     * S2 = S20 + S21 U1 + S22 U2
     *
     * \param iNode : is the index position for the parameter
     */
    Real source( const Real& U1, const Real& U2, const ID& ii, const UInt& iNode ) const ;

    //! Jacobian matrix dSi/dxj
    Real dSdU( const Real& U1, const Real& U2, const ID& ii, const ID& jj, const UInt& iNode ) const;

    //! Second derivative tensor d2Si/(dxj dxk)
//    Real diff2( const Real& _U1, const Real& _U2,
//                const ID& ii,    const ID& jj, const ID& kk,
//                const UInt& iNode = 0 ) const;

    //! Sql = [Sql1, Sql2]^T
    /*!
     *  Sql source term of the equation under its quasi-linear formulation:
     *
     *  dU/dt + H(U) dU/dz + Sql(U) = 0
     *
     *  Here H is constant w.r. to U. And Sql = S(U), because there is no variation of
     *  the coefficients.
     */
    Real interpolatedQuasiLinearSource( const Real& U1, const Real& U2,
                                        const ID& ii,   const container2D_Type& bcNodes, const Real& cfl ) const ;

    //@}
private:

    //! @name Unimplemented Methods
    //@{

    OneDimensionalSourceLinear& operator=( const physicsPtr_Type physics );

    //@}

};

//! Factory create function
inline OneDimensionalSource* createOneDimensionalSourceLinear()
{
    return new OneDimensionalSourceLinear();
}

}

#endif // OneDimensionalSourceLinear_H
