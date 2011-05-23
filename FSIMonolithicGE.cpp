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
    @brief A short description of the file content

    @author Paolo Crosetto <crosetto@iacspc70.epfl.ch>
    @date 26 Jul 2010

    A more detailed description of the file (if necessary)
 */

#include <life/lifecore/LifeV.hpp>

#include <FSIMonolithicGE.hpp>

namespace LifeV
{

// ===================================================
// Constructors & Destructor
// ===================================================


void FSIMonolithicGE::setupFluidSolid( UInt const fluxes )
{
    super_Type::setupFluidSolid( fluxes );

    M_meshMotion.reset(new FSIOperator::meshMotion_Type(*M_mmFESpace,
                                                        M_epetraComm));

    M_fluid.reset(new FSIOperator::fluid_Type(M_data->dataFluid(),
                                              *M_uFESpace,
                                              *M_pFESpace,
                                              M_epetraComm,
                                              *M_monolithicMap,
                                              fluxes));

    //             if (isLinearFluid())// to be implemented
    //                 M_fluidLin.reset(new FSIOperator::fluidlin_raw_type(dataFluid(),
    //                                                                    *M_uFESpace,
    //                                                                    *M_pFESpace,
    //                                                                    *M_epetraComm));
    M_un.reset (new vector_Type(*this->M_monolithicMap));
    M_rhs.reset(new vector_Type(*this->M_monolithicMap));
    M_rhsFull.reset(new vector_Type(*this->M_monolithicMap));
    M_beta.reset  (new vector_Type(M_uFESpace->map()));

    M_solid.reset(solid_Type::StructureSolverFactory::instance().createObject( M_data->dataSolid()->getSolidType() ));

    M_solid->setup(M_data->dataSolid(),
                   M_dFESpace,
                   M_epetraComm,
                   M_monolithicMap,
                   M_offset
                  );

    //             if (isLinearSolid())// to be implemented with the offset
    //                 M_solidLin.reset(new FSIOperator::solidlin_raw_type(dataSolid(),
    //                                                                    *M_dFESpace,
    //
    //                                                      *M_epetraComm));
}




void FSIMonolithicGE::setupDOF()
{
    M_bcvStructureDispToHarmonicExtension.reset( new  BCVectorInterface );
    super_Type::setupDOF();
}

void
FSIMonolithicGE::setupSystem( )
{
    super_Type::setupSystem();
    M_meshMotion->setUp( M_dataFile );
}

void
FSIMonolithicGE::updateSystem()
{
    super_Type::updateSystem();
}


void
FSIMonolithicGE::evalResidual( vector_Type&       res,
                            const vector_Type& disp,
                            const UInt          iter )
{

    if ((iter==0)|| !this->M_data->dataFluid()->isSemiImplicit())
    {
        // Solve HE
        iterateMesh(disp);

        // Update displacement
        M_meshMotion->updateDispDiff();

        M_beta.reset(new vector_Type(M_uFESpace->map()));
        vector_Type meshDispDiff( M_meshMotion->disp(), Repeated );

        this->moveMesh(meshDispDiff);//initialize the mesh position with the total displacement

        meshDispDiff=M_meshMotion->dispDiff();//repeating the mesh dispDiff
        this->interpolateVelocity(meshDispDiff, *this->M_beta);

        *this->M_beta /= -M_data->dataFluid()->dataTime()->timeStep(); //mesh velocity w

        vectorPtr_Type fluid(new vector_Type(this->M_uFESpace->map()));
        fluid->subset(*M_un, (UInt)0);
        *this->M_beta += *fluid/*M_un*/;//relative velocity beta=un-w

        //M_monolithicMatrix.reset(new matrix_Type(*M_monolithicMap));

        assembleSolidBlock(iter, M_un);
        assembleFluidBlock(iter, M_un);
        *M_rhsFull = *M_rhs;

        applyBoundaryConditions();
    }
    super_Type::evalResidual( disp,  M_rhsFull, res, M_diagonalScale);
}

void
FSIMonolithicGE::iterateMesh(const vector_Type& disp)
{
    vector_Type lambdaFluid(*M_interfaceMap, Unique);

    monolithicToInterface(lambdaFluid, disp);

    lambdaFluid *= (M_data->dataFluid()->dataTime()->timeStep()*M_solid->getRescaleFactor());//(M_data->dataSolid()->rescaleFactor()));

    this->setLambdaFluid(lambdaFluid); // it must be _disp restricted to the interface

    M_meshMotion->iterate(*M_BCh_mesh);

}

void FSIMonolithicGE::applyBoundaryConditions( )
{

         if ( !M_BCh_u->bcUpdateDone() )
             M_BCh_u->bcUpdate( *M_uFESpace->mesh(), M_uFESpace->feBd(), M_uFESpace->dof() );
         M_BCh_d->setOffset(M_offset);
         if ( !M_BCh_d->bcUpdateDone() )
             M_BCh_d->bcUpdate( *M_dFESpace->mesh(), M_dFESpace->feBd(), M_dFESpace->dof() );

         M_monolithicMatrix->setRobin( M_robinCoupling, M_rhsFull );
         M_precPtr->setRobin(M_robinCoupling, M_rhsFull);

         if(!this->M_monolithicMatrix->set())
         {
             M_BChs.push_back(M_BCh_d);
             M_BChs.push_back(M_BCh_u);
             M_FESpaces.push_back(M_dFESpace);
             M_FESpaces.push_back(M_uFESpace);

             M_monolithicMatrix->push_back_matrix(M_solidBlockPrec, false);
             M_monolithicMatrix->push_back_matrix(M_fluidBlock, true);
             M_monolithicMatrix->setConditions(M_BChs);
             M_monolithicMatrix->setSpaces(M_FESpaces);
             M_monolithicMatrix->setOffsets(2, M_offset, 0);
             M_monolithicMatrix->coupler(M_monolithicMap, M_dofStructureToHarmonicExtension->localDofMap(), M_numerationInterface, M_data->dataFluid()->dataTime()->timeStep());
         }
         else
         {
             M_monolithicMatrix->replace_matrix(M_fluidBlock, 1);
             M_monolithicMatrix->replace_matrix(M_solidBlockPrec, 0);
         }

         super_Type::checkIfChangedFluxBC( M_monolithicMatrix );

         M_monolithicMatrix->blockAssembling();
         M_monolithicMatrix->applyBoundaryConditions(dataFluid()->dataTime()->time(), M_rhsFull);

         M_monolithicMatrix->GlobalAssemble();
         //M_monolithicMatrix->matrix()->spy("M");
}



// ===================================================
//! Products registration
// ===================================================

bool FSIMonolithicGE::reg = FSIFactory_Type::instance().registerProduct( "monolithicGE", &FSIMonolithicGE::createM )  &&
    BlockPrecFactory::instance().registerProduct("ComposedDNND"  , &MonolithicBlockComposedDNND::createComposedDNND) &&
    BlockPrecFactory::instance().registerProduct("AdditiveSchwarz"  , &MonolithicBlockMatrix::createAdditiveSchwarz) &&
    MonolithicBlockMatrix::Factory_Type::instance().registerProduct("AdditiveSchwarz"  , &MonolithicBlockMatrix::createAdditiveSchwarz ) &&
    BlockPrecFactory::instance().registerProduct("AdditiveSchwarzRN"  , &MonolithicBlockMatrixRN::createAdditiveSchwarzRN ) &&
    MonolithicBlockMatrix::Factory_Type::instance().registerProduct("AdditiveSchwarzRN"  , &MonolithicBlockMatrixRN::createAdditiveSchwarzRN ) &&
    BlockPrecFactory::instance().registerProduct("ComposedDN"  , &MonolithicBlockComposedDN::createComposedDN ) &&
    BlockPrecFactory::instance().registerProduct("ComposedDN2"  , &MonolithicBlockComposedDN::createComposedDN2 );

} // Namespace LifeV
