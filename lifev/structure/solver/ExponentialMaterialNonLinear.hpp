//@HEADER
/*
*******************************************************************************

    Copyright (C) 2004, 2005, 2007 EPFL, Politecnico di Milano, INRIA
    Copyright (C) 2010 EPFL, Politecnico di Milano, Emory University

    This file is part of LifeV.

    LifeV is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
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
 *  @brief This file contains the definition for the St. Venant Kirchhoff linear material
 *
 *  @version 1.0
 *  @date 29-07-2010
 *  @author Paolo Tricerri,
 *  @author Gianmarco Mengaldo
 *
 *  @maintainer  Paolo Tricerri      <paolo.tricerri@epfl.ch>
 *  @contributor Gianmarco Mengaldo  <gianmarco.mengaldo@gmail.com>
 */

#ifndef _EXPONENTIALMATERIAL_H_
#define _EXPONENTIALMATERIAL_H_

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"


#include <lifev/structure/solver/StructuralConstitutiveLaw.hpp>

namespace LifeV
{
template <typename Mesh>
class ExponentialMaterialNonLinear : public StructuralConstitutiveLaw<Mesh>
{
    //!@name Type definitions
    //@{

public:
    typedef StructuralConstitutiveLaw<Mesh>           super;

    typedef typename super::data_Type                data_Type;

    typedef typename super::vector_Type              vector_Type;
    typedef typename super::matrix_Type              matrix_Type;

    typedef typename super::matrixPtr_Type           matrixPtr_Type;
    typedef typename super::vectorPtr_Type           vectorPtr_Type;
    typedef typename super::dataPtr_Type             dataPtr_Type;
    typedef typename super::displayerPtr_Type        displayerPtr_Type;

    typedef typename super::mapMarkerVolumesPtr_Type mapMarkerVolumesPtr_Type;
    typedef typename super::mapMarkerVolumes_Type mapMarkerVolumes_Type;
    typedef typename mapMarkerVolumes_Type::const_iterator mapIterator_Type;

    typedef typename super::vectorVolumes_Type       vectorVolumes_Type;
    typedef boost::shared_ptr<vectorVolumes_Type>    vectorVolumesPtr_Type;

    typedef typename super::FESpacePtr_Type          FESpacePtr_Type;
    typedef typename super::ETFESpacePtr_Type        ETFESpacePtr_Type;

    typedef MatrixSmall<3,3>                          matrixSmall_Type;
    //@}



    //! @name Constructor &  Destructor
    //@{

    ExponentialMaterialNonLinear();

    virtual  ~ExponentialMaterialNonLinear();

    //@}



    //!@name Methods
    //@{

    //! Setup the created object of the class StructuralConstitutiveLaw
    /*!
      \param dFespace: the FiniteElement Space
      \param monolithicMap: the MapEpetra
      \param offset: the offset parameter used assembling the matrices
    */
    void setup( const FESpacePtr_Type& dFESpace,
                const ETFESpacePtr_Type& dETFESpace,
                const boost::shared_ptr<const MapEpetra>&  monolithicMap,
                const UInt offset, const dataPtr_Type& dataMaterial, const displayerPtr_Type& displayer );


    //! Compute the Stiffness matrix in StructuralSolver::buildSystem()
    /*!
      \param dataMaterial the class with Material properties data
    */
    void computeLinearStiff( dataPtr_Type& /*dataMaterial*/, const mapMarkerVolumesPtr_Type /*mapsMarkerVolumes*/ );


    //! Updates the Jacobian matrix in StructualSolver::updateJacobian
    /*!
      \param disp: solution at the k-th iteration of NonLinearRichardson Method
      \param dataMaterial: a pointer to the dataType member in StructuralSolver class to get
                           the material coefficients (e.g. Young modulus, Poisson ratio..)
      \param displayer: a pointer to the Dysplaier member in the StructuralSolver class
    */
    void updateJacobianMatrix( const vector_Type& disp,
                               const dataPtr_Type& dataMaterial,
                               const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                               const displayerPtr_Type& displayer );


    //! Updates the nonlinear terms in the Jacobian matrix in StructualSolver::updateJacobian
    /*!
      \param stiff: stiffness matrix provided from outside
      \param disp: solution at the k-th iteration of NonLinearRichardson Method
      \param dataMaterial: a pointer to the dataType member in StructuralSolver class to get
                           the material coefficients (e.g. Young modulus, Poisson ratio..)
      \param displayer: a pointer to the Dysplaier member in the StructuralSolver class
    */
    void updateNonLinearJacobianTerms( matrixPtr_Type& jacobian,
                                       const vector_Type& disp,
                                       const dataPtr_Type& dataMaterial,
                                       const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                                       const displayerPtr_Type& displayer );


    //! Interface method to compute the new Stiffness matrix in StructuralSolver::evalResidual and in
    //! StructuralSolver::updateSystem since the matrix is the expression of the matrix is the same.
    /*!
      \param sol:  the solution vector
      \param factor: scaling factor used in FSI
      \param dataMaterial: a pointer to the dataType member in StructuralSolver class to get the
                           material coefficients (e.g. Young modulus, Poisson ratio..)
      \param displayer: a pointer to the Dysplaier member in the StructuralSolver class
    */
    void computeStiffness( const vector_Type& disp, Real factor, const dataPtr_Type& dataMaterial, const mapMarkerVolumesPtr_Type mapsMarkerVolumes, const displayerPtr_Type& displayer );


    //! Computes the new Stiffness vector for Neo-Hookean and Exponential materials in StructuralSolver
    //! given a certain displacement field.
    //! This function is used both in StructuralSolver::evalResidual and in StructuralSolver::updateSystem
    //! since the matrix is the expression of the matrix is the same.
    /*!
      \param sol:  the solution vector
      \param factor: scaling factor used in FSI
      \param dataMaterial: a pointer to the dataType member in StructuralSolver class to get
                           the material coefficients (e.g. Young modulus, Poisson ratio..)
      \param displayer: a pointer to the Dysplaier member in the StructuralSolver class
    */
    // void computeVector( const vector_Type& sol,
    //                     Real factor,
    //                     const dataPtr_Type& dataMaterial,
    //                     const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
    //                     const displayerPtr_Type& displayer );

    //! Computes the deformation gradient F, the cofactor matrix Cof(F),
    //! the determinant of F (J = det(F)), the trace of right Cauchy-Green tensor tr(C)
    //! This function is used in StructuralConstitutiveLaw::computeStiffness
    /*!
      \param dk_loc: the elemental displacement
    */
    void computeKinematicsVariables( const VectorElemental& dk_loc )
    {
    }

    //! Computes the deformation Gradient F, the cofactor of F Cof(F), the determinant of F J = det(F), the trace of C Tr(C).
    /*!
      \param dk_loc: local displacement vector
    */
    //void computeStress( const vector_Type& sol );

    //! ShowMe method of the class (saved on a file the stiffness vector and the jacobian)
    void showMe( std::string const& fileNameVectStiff,
                 std::string const& fileNameJacobain );

    //! Compute the First Piola Kirchhoff Tensor
    /*!
       \param firstPiola Epetra_SerialDenseMatrix that has to be filled
       \param tensorF Epetra_SerialDenseMatrix the deformation gradient
       \param cofactorF Epetra_SerialDenseMatrix cofactor of F
       \param invariants std::vector with the invariants of C and the detF
       \param material UInt number to get the material parameteres form the VenantElasticData class
    */
    void computeLocalFirstPiolaKirchhoffTensor( Epetra_SerialDenseMatrix& firstPiola,
                                                const Epetra_SerialDenseMatrix& tensorF,
                                                const Epetra_SerialDenseMatrix& cofactorF,
                                                const std::vector<Real>& invariants,
                                                const UInt marker);

    //@}

    //! @name Get Methods
    //@{

    //! Get the Stiffness matrix
    matrixPtr_Type  const stiffMatrix() const { return super::M_jacobian; }


    //! Get the stiffness vector
    vectorPtr_Type  const stiffVector() const  {return M_stiff; }

    void apply( const vector_Type& sol, vector_Type& res,
                const mapMarkerVolumesPtr_Type mapsMarkerVolumes) ;

    //@}



protected:

    //! Vector: stiffness non-linear
    vectorPtr_Type                         M_stiff;

    //Create the indentity for F
    matrixSmall_Type                      M_identity;

};





template <typename Mesh>
ExponentialMaterialNonLinear<Mesh>::ExponentialMaterialNonLinear():
    super            ( ),
    M_stiff          ( ),
    M_identity         ( )
{
}





template <typename Mesh>
ExponentialMaterialNonLinear<Mesh>::~ExponentialMaterialNonLinear()
{}





template <typename Mesh>
void
ExponentialMaterialNonLinear<Mesh>::setup( const FESpacePtr_Type&                       dFESpace,
                                           const ETFESpacePtr_Type&                     dETFESpace,
                                           const boost::shared_ptr<const MapEpetra>&   monolithicMap,
                                           const UInt                                  offset,
                                           const dataPtr_Type&                         dataMaterial,
                                           const displayerPtr_Type&                    displayer  )
{
    this->M_displayer = displayer;
    this->M_dataMaterial  = dataMaterial;
    //    std::cout<<"I am setting up the Material"<<std::endl;

    this->M_dispFESpace                 = dFESpace;
    this->M_dispETFESpace               = dETFESpace;
    this->M_localMap                    = monolithicMap;
    this->M_offset                      = offset;

    M_stiff.reset                     (new vector_Type(*this->M_localMap));

    M_identity(0,0) = 1.0; M_identity(0,1) = 0.0; M_identity(0,2) = 0.0;
    M_identity(1,0) = 0.0; M_identity(1,1) = 1.0; M_identity(1,2) = 0.0;
    M_identity(2,0) = 0.0; M_identity(2,1) = 0.0; M_identity(2,2) = 1.0;
}

template <typename Mesh>
void ExponentialMaterialNonLinear<Mesh>::computeLinearStiff(dataPtr_Type& /*dataMaterial*/,
                                                            const mapMarkerVolumesPtr_Type /*mapsMarkerVolumes*/)
{
    //! Empty method for exponential material
}





template <typename Mesh>
void ExponentialMaterialNonLinear<Mesh>::updateJacobianMatrix( const vector_Type&       disp,
                                                               const dataPtr_Type&      dataMaterial,
                                                               const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                                                               const displayerPtr_Type& displayer )
{

    this->M_jacobian.reset(new matrix_Type(*this->M_localMap));

    displayer->leaderPrint(" \n*********************************\n  ");
    updateNonLinearJacobianTerms(this->M_jacobian, disp, dataMaterial, mapsMarkerVolumes, displayer);
    displayer->leaderPrint(" \n*********************************\n  ");
    std::cout << std::endl;

}

template <typename Mesh>
void ExponentialMaterialNonLinear<Mesh>::updateNonLinearJacobianTerms( matrixPtr_Type&         jacobian,
                                                                       const vector_Type&     disp,
                                                                       const dataPtr_Type&     dataMaterial,
                                                                       const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                                                                       const displayerPtr_Type& displayer )
{
    displayer->leaderPrint("   Non-Linear S-  updating non linear terms in the Jacobian Matrix (Exponential)");

    *(jacobian) *= 0.0;

    //! Nonlinear part of jacobian
    //! loop on volumes (i)

    mapIterator_Type it;

    vectorVolumesPtr_Type pointerListOfVolumes;

    for( it = (*mapsMarkerVolumes).begin(); it != (*mapsMarkerVolumes).end(); it++ )
    {
        //Given the marker pointed by the iterator, let's extract the material parameters
        UInt marker = it->first;

        pointerListOfVolumes.reset( new vectorVolumes_Type(it->second) );
        Real bulk = dataMaterial->bulk(marker);
        Real alpha = dataMaterial->alpha(marker);
        Real gamma = dataMaterial->gamma(marker);

        //Macros to make the assembly more readable
#define F ( grad( this->M_dispETFESpace,  disp, this->M_offset) + value(this->M_identity) )
#define J det( F )
#define F_T  minusT(F)
#define RIGHTCAUCHYGREEN transpose(F) * F
#define IC trace( RIGHTCAUCHYGREEN )
#define ICbar pow( J, (-2.0/3.0) ) * IC


        //Assembling Volumetric Part
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes  ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   this->M_dispETFESpace,
                   value( bulk / 2.0 ) * ( value(2.0)*pow(J, 2.0) - J + value(1.0) ) * dot( F_T, grad(phi_j) ) * dot( F_T, grad(phi_i) )
                   ) >> jacobian;

        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   this->M_dispETFESpace,
                   value( - bulk / 2.0 ) * ( pow(J,2.0) - J + log(J) ) * dot( F_T * transpose(grad(phi_j)) * F_T,  grad(phi_i) )
                   ) >> jacobian;

        // //Assembling the Isochoric Part
	    // //! 1. Stiffness matrix : int { - 2/3 * alpha * J^(-2/3) * exp( gamma*( Ic_iso - 3) )* ( 1. + coefExp * Ic_iso )
	    // //!                      *( F^-T : \nabla \delta ) ( F : \nabla \v ) }
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   this->M_dispETFESpace,
                   value((-2.0/3.0) * alpha) * pow(J,-2.0/3.0) * exp( value( gamma ) * ( ICbar - value(3.0) )) * ( value(1.0) + value(gamma)* ICbar) *
                   dot( F_T, grad(phi_j) ) * dot( F, grad(phi_i) )
                   ) >> jacobian;

	    //! 2. Stiffness matrix : int { 2 * alpha * gamma * J^(-4/3) * exp( gamma*( Ic_iso - 3) ) *
	    //!             ( F : \nabla \delta ) ( F : \nabla \v ) }
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   this->M_dispETFESpace,
                   value(2.0 * alpha * gamma) * pow(J, -4.0/3.0) * exp( value( gamma ) * ( ICbar - value(3.0) )) *
                   dot( F, grad(phi_j) ) * dot( F, grad(phi_i) )
                   ) >> jacobian;

	    // //! 3. Stiffness matrix : int { 2.0/9.0 *  alpha *  Ic_iso * exp( gamma*( Ic_iso - 3) )*
	    // //!                       ( 1. + gamma * Ic_iso )( F^-T : \nabla \delta ) ( F^-T : \nabla \v )}
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   this->M_dispETFESpace,
                   value((2.0/9.0) * alpha) * ICbar * exp( value(gamma) * ( ICbar - value(3.0) )) * ( value(1.0) + value(gamma)* ICbar)*
                   dot( F_T, grad(phi_j) ) * dot( F_T, grad(phi_i) )
                   ) >> jacobian;

	    // //! 4. Stiffness matrix : int { -2.0/3.0 *  alpha * J^(-2/3) * exp( gamma*( Ic_iso - 3) )
	    // //!                    * ( 1. + gamma * Ic_iso )( F : \nabla \delta ) ( F^-T : \nabla \v ) }
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ),
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   this->M_dispETFESpace,
                   value(-(2.0/3.0) * alpha ) * pow(J, -2.0/3.0) * exp( value( gamma ) * ( ICbar - value(3.0) )) * ( value(1.0) + value(gamma)* ICbar) *
                   dot( F, grad(phi_j) ) * dot( F_T, grad(phi_i) )
                   ) >> jacobian;


	    // //! 5. Stiffness matrix : int {  alpha * J^(-2/3) * exp( gamma*( Ic_iso - 3)) (\nabla \delta: \nabla \v)}
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   this->M_dispETFESpace,
                   value(alpha) * pow(J, -2.0/3.0) * exp( value( gamma ) * ( ICbar - value(3.0) )) *
                   dot( grad(phi_j), grad(phi_i) )
                   ) >> jacobian;


	    // //! 6. Stiffness matrix : int { 1.0/3.0 * alpha * Ic_iso *  exp(gamma*( Ic_iso - 3)) *
	    // //!                       (F^-T [\nabla \delta]^t F^-T ) : \nabla \v  }
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   this->M_dispETFESpace,
                   value((1.0/3.0) * alpha) * ICbar * exp( value( gamma ) * ( ICbar - value(3.0) )) *
                   dot( F_T * transpose(grad(phi_j)) * F_T , grad(phi_i) )
                   ) >> jacobian;

    }

    jacobian->globalAssemble();
}





template <typename Mesh>
void ExponentialMaterialNonLinear<Mesh>::computeStiffness( const vector_Type& disp,
                                                           Real /*factor*/,
                                                           const dataPtr_Type& dataMaterial,
                                                           const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                                                           const displayerPtr_Type& displayer )
{

    using namespace ExpressionAssembly;

    M_stiff.reset(new vector_Type(*this->M_localMap));
    *(M_stiff) *= 0.0;

    displayer->leaderPrint(" \n*********************************\n  ");
    displayer->leaderPrint(" Non-Linear S-  Computing the Exponential nonlinear stiffness vector ");
    displayer->leaderPrint(" \n*********************************\n  ");

    mapIterator_Type it;
    vectorVolumesPtr_Type pointerListOfVolumes;

    for( it = (*mapsMarkerVolumes).begin(); it != (*mapsMarkerVolumes).end(); it++ )
    {

        //Given the marker pointed by the iterator, let's extract the material parameters
        UInt marker = it->first;

        pointerListOfVolumes.reset( new vectorVolumes_Type(it->second) );

        Real bulk = dataMaterial->bulk(marker);
        Real alpha = dataMaterial->alpha(marker);
        Real gamma = dataMaterial->gamma(marker);

        //Computation of the volumetric part
        //
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   value(bulk / 2.0) * ( pow( J ,2.0) - J + log(J)) * dot(  F_T, grad(phi_i) )
                   ) >> M_stiff;

        //Computation of the isochoric part
        integrate( integrationOverSelectedVolumes( pointerListOfVolumes ) ,
                   this->M_dispFESpace->qr(),
                   this->M_dispETFESpace,
                   value(alpha) * pow(J,-2.0/3.0) * exp( value(gamma)*( ICbar  - value(3.0) ) )  *
                   (dot( F - value(1.0/3.0) * IC * F_T,grad(phi_i) ) )
                   ) >> M_stiff;
      }

    this->M_stiff->globalAssemble();
}


template <typename Mesh>
void ExponentialMaterialNonLinear<Mesh>::showMe( std::string const& fileNameStiff,
                                                 std::string const& fileNameJacobian )
{
    this->M_stiff->spy(fileNameStiff);
    this->M_jacobian->spy(fileNameJacobian);
}


template <typename Mesh>
void ExponentialMaterialNonLinear<Mesh>::apply( const vector_Type& sol,
                                                vector_Type& res, const mapMarkerVolumesPtr_Type mapsMarkerVolumes )
{
    computeStiffness(sol, 0, this->M_dataMaterial, mapsMarkerVolumes, this->M_displayer);
    res += *M_stiff;
}


template <typename Mesh>
void ExponentialMaterialNonLinear<Mesh>::computeLocalFirstPiolaKirchhoffTensor( Epetra_SerialDenseMatrix& firstPiola,
                                                                                const Epetra_SerialDenseMatrix& tensorF,
                                                                                const Epetra_SerialDenseMatrix& cofactorF,
                                                                                const std::vector<Real>& invariants,
                                                                                const UInt marker)
{

  //Get the material parameters
  Real alpha    = this->M_dataMaterial->alpha(marker);
  Real gamma    = this->M_dataMaterial->gamma(marker);
  Real bulk  	= this->M_dataMaterial->bulk(marker);


  //Computing the first term \alphaJ^{-2/3}[F-(1/3)tr(C)F^{-T}]exp(\gamma(tr(Ciso) - 3)
  Epetra_SerialDenseMatrix firstTerm(tensorF);
  Epetra_SerialDenseMatrix copyCofactorF(cofactorF);

  Real scale(0.0);
  scale = -invariants[0]/3.0;
  copyCofactorF.Scale( scale );
  firstTerm += copyCofactorF;

  //Computation trace of the isochoric C
  Real trCiso(0.0);
  trCiso = std::pow(invariants[3],-(2.0/3.0))*invariants[0];

  Real coef( 0.0 );
  coef = alpha * std::pow(invariants[3],-(2.0/3.0)) * std::exp( gamma * ( trCiso - 3 ) );
  firstTerm.Scale( coef );

  //Computing the second term (volumetric part) J*(bulk/2)(J-1+(1/J)*ln(J))F^{-T}
  Epetra_SerialDenseMatrix secondTerm(cofactorF);
  Real secCoef(0);
  secCoef = invariants[3] * (bulk/2.0) * (invariants[3] - 1 + (1.0 / invariants[3]) * std::log(invariants[3]));

  secondTerm.Scale( secCoef );

  firstPiola += firstTerm;
  firstPiola += secondTerm;

}

template <typename Mesh>
inline StructuralConstitutiveLaw<Mesh>* createExponentialMaterialNonLinear() { return new ExponentialMaterialNonLinear<Mesh >(); }

namespace
{
static bool registerEXP = StructuralConstitutiveLaw<LifeV::RegionMesh<LinearTetra> >::StructureMaterialFactory::instance().registerProduct( "exponential", &createExponentialMaterialNonLinear<LifeV::RegionMesh<LinearTetra> > );
}

} //Namespace LifeV

#endif /* __EXPONENTIALMATERIAL_H */
