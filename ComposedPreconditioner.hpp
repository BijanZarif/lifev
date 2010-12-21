/* -*- mode: c++ -*-*/
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
/**
   \file ComposedPreconditioner.hpp
   \author Simone Deparis <simone.deparis@epfl.ch>
   \contributor Paolo Crosetto
   \mantainer Paolo Crosetto
   \date 2009-05-07
 */


#ifndef _IFPACKCOMPOSEDPREC_HPP_
#define _IFPACKCOMPOSEDPREC_HPP_

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <Ifpack_ConfigDefs.h>
#include <Ifpack.h>
#include <Ifpack_Preconditioner.h>
#include <ml_MultiLevelPreconditioner.h>
#include <Ifpack_AdditiveSchwarz.h>
#include <Ifpack_Amesos.h>
#include <Ifpack_ILU.h>
#pragma GCC diagnostic warning "-Wunused-variable"
#pragma GCC diagnostic warning "-Wunused-parameter"


#include <life/lifefilters/GetPot.hpp>

#include <life/lifearray/EpetraMatrix.hpp>

#include <life/lifealg/Preconditioner.hpp>

#include <lifemc/lifealg/ComposedOperator.hpp>

namespace LifeV
{

class ComposedPreconditioner:
        public Preconditioner
{
public:

    /** @name Typedefs
     */
    //@{

    typedef Preconditioner                                             super_Type;
    typedef ComposedOperator<Preconditioner>                     prec_Type;
    typedef boost::shared_ptr<prec_Type>                               precPtr_Type;
    typedef boost::shared_ptr<Preconditioner>                    epetraPrecPtr_Type;
    typedef boost::shared_ptr<ML_Epetra::MultiLevelPreconditioner>     mlPrecPtr_Type;
    typedef super_Type::operator_raw_type                              operator_Type;
    typedef boost::shared_ptr<operator_Type>                           operatorPtr_Type;
    typedef super_Type::list_Type                                      list_Type;
    typedef boost::shared_ptr<Preconditioner>                    epetraPrec_Type;


    /** @name Constructors, destructor
     */
    //@{
    //! default constructor.
    ComposedPreconditioner( boost::shared_ptr<Epetra_Comm> comm = boost::shared_ptr<Epetra_Comm>() );

    //!Copy Constructor
    /**
       This copy constructor does not copy the matrices, but just the shared_ptrs. It calls the copy constructor of ComposedOperator.
     */
    ComposedPreconditioner( ComposedPreconditioner& P );

    //! constructor from matrix A.
    //! @param A EpetraMatrix<double> matrix upon which construct the preconditioner
    //    ComposedPreconditioner(operatorPtr_Type& A);

    //! default destructor

    ~ComposedPreconditioner();

    //@}


    //!@name  Public Methods
    //@{
    void                   setDataFromGetPot ( const GetPot&      dataFile,
                                               const std::string& section);

    void                   createList( list_Type& /*list*/, const GetPot& dataFile, const std::string& section, const std::string& subSection );
    double                 Condest ();

    int                    buildPreconditioner(operatorPtr_Type& A);
    int                    buildPreconditioner(operatorPtr_Type& A,
                                               const bool useInverse,
                                               const bool useTranspose=false);
    //! Build a preconditioner based on A and push it back in the composedPreconditioner.
    int                    push_back          (operatorPtr_Type& A,
                                               const bool useInverse=false,
                                               const bool useTranspose=false
                                              );

    //! Build a preconditioner based on A and replace it in the composedPreconditioner.
    int                    replace            (operatorPtr_Type& A,
                                               const UInt index,
                                               const bool useInverse=false,
                                               const bool useTranspose=false);

    void                   precReset();

    //! returns true if prec exists
    /*const*/
    bool                   set() const {return M_prec;}
    //@}

    //!@name Implementation of Methods from Epetra_Operator
    //@{
    const Epetra_Comm& Comm(){return getPrec()->Comm(); }

    const Epetra_Map& OperatorDomainMap() { return  M_prec->OperatorDomainMap(); }
    const Epetra_Map& OperatorRangeMap() { return  M_prec->OperatorRangeMap(); }
    std::vector<operatorPtr_Type>& getOperVector(){return M_operVector;}

    int            SetUseTranspose( bool useTranspose=false )
    {
        return M_prec->SetUseTranspose(useTranspose);
    }

    bool            UseTranspose(  ) {return M_prec->UseTranspose();}

    virtual int ApplyInverse(const Epetra_MultiVector& X, Epetra_MultiVector& Y) const
    {
        return M_prec->ApplyInverse(X, Y);
    }

    virtual int Apply(const Epetra_MultiVector& X, Epetra_MultiVector& Y) const
    {
        return M_prec->Apply(X, Y);
    }

    const Epetra_Map & OperatorRangeMap() const
    {return M_prec->OperatorRangeMap();}

    const Epetra_Map & OperatorDomainMap() const
    {return M_prec->OperatorDomainMap();}
    //@}

    //!@name Get Methods
    //@{
    super_Type::prec_raw_type*  getPrec ();

    UInt number() const {return M_prec->number();}

    super_Type::prec_type              getPrecPtr(){return M_prec;}

    std::string            precType() {return "composedPreconditioner";}
    //@}

    //!@name Static Methods
    //@{

    //! Factory method
    static Preconditioner* createComposedPreconditioner()
    {
        return new ComposedPreconditioner();
    }
    //@}

private:

    //!@name Private Methods
    //@{

    Int createPrec (operatorPtr_Type& oper,
                    boost::shared_ptr<Preconditioner>& prec);
    //@}


    //!@name Private Members
    //@{
    precPtr_Type                  M_prec;
    std::vector<operatorPtr_Type> M_operVector; // we need to keep track of all the operators.
    static bool registerComposed;
    //@}
};

} // namespace LifeV

#endif
