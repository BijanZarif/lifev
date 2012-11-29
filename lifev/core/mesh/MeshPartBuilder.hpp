//@HEADER
/*
*******************************************************************************

Copyright (C) 2004, 2005, 2007 EPFL, Politecnico di Milano, INRIA
Copyright (C) 2010, 2011, 2012 EPFL, Politecnico di Milano, Emory University

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
  @brief Class that builds a mesh part, after the graph has been partitioned

  @date 9-11-2012
  @author Radu Popescu <radu.popescu@epfl.ch>

  @maintainer Radu Popescu <radu.popescu@epfl.ch>
*/

#ifndef MESH_PART_BUILDER_H
#define MESH_PART_BUILDER_H 1

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <boost/shared_ptr.hpp>

#pragma GCC diagnostic warning "-Wunused-variable"
#pragma GCC diagnostic warning "-Wunused-parameter"

#include<lifev/core/LifeV.hpp>

namespace LifeV
{

/*!
  @brief Class that builds a mesh part, after the graph has been partitioned
  @author Radu Popescu radu.popescu@epfl.ch

  This class is used as a component for the MeshPartitionTool class. When an
  object of class MeshPartBuilder is instantiated it holds pointer to the
  global uncut mesh.

  The only public method that this class implements is a run method, which
  takes a vector of element IDs which corespond to a mesh part and builds
  a RegionMesh object with this elements.
*/
template<typename MeshType>
class MeshPartBuilder
{
public:
    //! @name Public Types
    //@{
    typedef MeshType mesh_Type;
    typedef boost::shared_ptr<mesh_Type> meshPtr_Type;
    //@}

    //! \name Constructors & Destructors
    //@{

    //! Constructor
    /*!
     * Constructor which takes a pointer to a RegionMesh object, the uncut
     * mesh
     *
     * \param mesh - shared pointer to the global uncut mesh
    */
    MeshPartBuilder(const meshPtr_Type& mesh);

    //! Empty destructor
    ~MeshPartBuilder() {}
    //@}

    //! \name Public Methods
    //@{
    //! Run part builder
    /*!
     * This method performs all the steps for the mesh and graph partitioning
     *
     * \param meshPart - shared pointer to a RegionMesh object which will
     * 					 contain the mesh part
     * \param elementList - shared pointer to a vector of int, representing the
     * 						element IDs associated with this mesh part
     */
    void run(const meshPtr_Type& meshPart,
    		 const std::vector<Int>& elementList);
    //@}

    //! \name Get Methods
    //@{
    //@}

private:
    //! Private Methods
    //@{
    //! Construct local mesh
    /*!
      Constructs the data structures for the local mesh partition.
      Updates M_localVertices, M_localRidges, M_localFacets, M_localElements,
      M_globalToLocalVertex.
    */
    void constructLocalMesh(const std::vector<Int>& elementList);
    //! Construct nodes
    /*!
      Adds nodes to the partitioned mesh object. Updates M_nBoundaryVertices,
      M_meshPartition.
    */
    void constructVertices();
    //! Construct volumes
    /*!
      Adds volumes to the partitioned mesh object.
      Updates M_globalToLocalElement, M_meshPartition.
    */
    void constructElements();
    //! Construct edges
    /*!
      Adds edges to the partitioned mesh object. Updates M_nBoundaryRidges,
      M_meshPartition.
    */
    void constructRidges();
    //! Construct faces
    /*!
      Adds faces to the partitioned mesh object. Updates M_nBoundaryFacets,
      M_meshPartition.
    */
    void constructFacets();
    //! Final setup of local mesh
    /*!
      Updates the partitioned mesh object data members after adding the mesh
      elements (nodes, edges, faces, volumes).
      Updates M_meshPartition.
    */
    void finalSetup();
    //@}

    //! Private Data Members
    //@{
    UInt                                       M_nBoundaryVertices;
    UInt                                       M_nBoundaryRidges;
    UInt                                       M_nBoundaryFacets;
    UInt                                       M_elementVertices;
    UInt                                       M_elementFacets;
    UInt                                       M_elementRidges;
    UInt                                       M_facetVertices;
    std::vector<Int>                           M_localVertices;
    std::set<Int>                              M_localRidges;
    std::set<Int>                              M_localFacets;
    std::vector<Int>                           M_localElements;
    std::map<Int, Int>                         M_globalToLocalVertex;
    std::map<Int, Int>                         M_globalToLocalElement;
    meshPtr_Type                               M_originalMesh;
    meshPtr_Type                               M_meshPart;
    //@}
}; // class MeshPartBuilder

// IMPLEMENTATION

template<typename MeshType>
MeshPartBuilder<MeshType>::MeshPartBuilder(const meshPtr_Type& mesh)
	: M_nBoundaryVertices(0),
	  M_nBoundaryRidges(0),
	  M_nBoundaryFacets(0),
	  M_elementVertices(0),
	  M_elementFacets(0),
	  M_elementRidges(0),
	  M_facetVertices(0),
	  M_originalMesh(mesh),
	  M_meshPart()
{}

template<typename MeshType>
void MeshPartBuilder<MeshType>::run(const meshPtr_Type& meshPart,
									const std::vector<Int>& elementList)
{
	M_meshPart = meshPart;

    M_elementVertices = MeshType::elementShape_Type::S_numVertices;
    M_elementFacets = MeshType::elementShape_Type::S_numFacets;
    M_elementRidges = MeshType::elementShape_Type::S_numRidges;
    M_facetVertices    = MeshType::facetShape_Type::S_numVertices;

    constructLocalMesh(elementList);
    constructVertices();
    constructElements();
    constructRidges();
    constructFacets();

    finalSetup();
}

template<typename MeshType>
void MeshPartBuilder<MeshType>::constructLocalMesh(
		const std::vector<Int>& elementList)
{
    std::map<Int, Int>::iterator  im;
    std::set<Int>::iterator       is;

    Int count = 0;
    UInt ielem;
    UInt inode;

    count = 0;
    // cycle on local element's ID

    for (UInt jj = 0; jj < elementList.size(); ++jj)
    {
        ielem = elementList[jj];
        M_localElements.push_back(ielem);

        // cycle on element's nodes
        for (UInt ii = 0; ii < M_elementVertices; ++ii)
        {
            inode = M_originalMesh->volume(ielem).point(ii).id();
            im    = M_globalToLocalVertex.find(inode);

            // if the node is not yet present in the list of local nodes,
            // then add it
            if (im == M_globalToLocalVertex.end())
            {
                M_globalToLocalVertex.insert(std::make_pair(inode, count));
                ++count;
                // store here the global numbering of the node
                M_localVertices.push_back(
                		M_originalMesh->volume(ielem).point(ii).id());
            }
        }

        // cycle on element's edges
        for (UInt ii = 0; ii < M_elementRidges; ++ii)
        {
            // store here the global numbering of the edge
            M_localRidges.insert(M_originalMesh->localEdgeId(ielem, ii));
        }

        // cycle on element's faces
        for (UInt ii = 0; ii < M_elementFacets; ++ii)
        {
            // store here the global numbering of the face
            M_localFacets.insert(M_originalMesh->localFaceId(ielem, ii));
        }
    }
}

template<typename MeshType>
void MeshPartBuilder<MeshType>::constructVertices()
{
    UInt inode;
    std::vector<Int>::iterator it;

    M_nBoundaryVertices = 0;
    M_meshPart->pointList.reserve(M_localVertices.size());
    // guessing how many boundary points on this processor.
    M_meshPart->_bPoints.reserve(M_originalMesh->numBPoints()
    							 * M_localVertices.size()
    							 / M_originalMesh->numBPoints()
    							 );
    inode = 0;
    typename MeshType::point_Type *pp = 0;

    // loop in the list of local nodes:
    // in this loop inode is the local numbering of the points
    for (it = M_localVertices.begin();
    	 it != M_localVertices.end(); ++it, ++inode)
    {
        typename MeshType::point_Type point = 0;

        // create a boundary point in the local mesh, if needed
        bool boundary = M_originalMesh->isBoundaryPoint(*it);
        if (boundary)
        {
            ++M_nBoundaryVertices;
        }

        pp = &(M_meshPart->addPoint(boundary));
        *pp = M_originalMesh->point( *it );

        pp->setLocalId( inode );
    }
}

template<typename MeshType>
void MeshPartBuilder<MeshType>::constructElements()
{
    Int count;
    std::map<Int, Int>::iterator im;
    std::vector<Int>::iterator it;
    count = 0;
    UInt inode;

    typename MeshType::element_Type * pv = 0;

    M_meshPart->volumeList.reserve(M_localElements.size());

    // loop in the list of local elements
    // CAREFUL! in this loop inode is the global numbering of the points
    // We insert the local numbering of the nodes in the local volume list
    for (it = M_localElements.begin();
		 it != M_localElements.end(); ++it, ++count)
    {
        pv = &(M_meshPart->addVolume());
        *pv = M_originalMesh->volume( *it );
        pv->setLocalId( count );

        M_globalToLocalElement.insert(std::make_pair(pv->id(),
        											 pv->localId() ) );

        for (ID id = 0; id < M_elementVertices; ++id)
        {
            inode = M_originalMesh->volume(*it).point(id).id();
            // im is an iterator to a map element
            // im->first is the key (i. e. the global ID "inode")
            // im->second is the value (i. e. the local ID "count")
            im = M_globalToLocalVertex.find(inode);
            pv->setPoint(id, M_meshPart->pointList( (*im).second ));
        }
    }
}

template<typename MeshType>
void MeshPartBuilder<MeshType>::constructRidges()
{
    Int count;
    std::map<Int, Int>::iterator im;
    std::set<Int>::iterator is;

    typename MeshType::ridge_Type * pe;
    UInt inode;
    count = 0;

    M_nBoundaryRidges = 0;
    M_meshPart->edgeList.reserve(M_localRidges.size());

    // loop in the list of local edges
    for (is = M_localRidges.begin(); is != M_localRidges.end(); ++is, ++count)
    {
        // create a boundary edge in the local mesh, if needed
        bool boundary = (M_originalMesh->isBoundaryEdge(*is));
        if (boundary)
        {
            // create a boundary edge in the local mesh, if needed
            ++M_nBoundaryRidges;
        }

        pe = &(M_meshPart->addEdge(boundary));
        *pe = M_originalMesh->edge( *is );

        pe->setLocalId(count);

        for (ID id = 0; id < 2; ++id)
        {
            inode = M_originalMesh->edge(*is).point(id).id();
            // im is an iterator to a map element
            // im->first is the key (i. e. the global ID "inode")
            // im->second is the value (i. e. the local ID "count")
            im = M_globalToLocalVertex.find(inode);
            pe->setPoint(id, M_meshPart->pointList((*im).second));
        }
    }
}

template<typename MeshType>
void MeshPartBuilder<MeshType>::constructFacets()
{
    Int count;
    std::map<Int, Int>::iterator im;
    std::set<Int>::iterator      is;

    typename MeshType::facet_Type * pf = 0;

    UInt inode;
    count = 0;

    M_nBoundaryFacets = 0;
    M_meshPart->faceList.reserve(M_localFacets.size());

    // loop in the list of local faces
    for (is = M_localFacets.begin(); is != M_localFacets.end(); ++is, ++count)
    {
        // create a boundary face in the local mesh, if needed
        bool boundary = (M_originalMesh->isBoundaryFace(*is));
        if (boundary)
        {
            ++M_nBoundaryFacets;
        }

        Int elem1 = M_originalMesh->face(*is).firstAdjacentElementIdentity();
        Int elem2 = M_originalMesh->face(*is).secondAdjacentElementIdentity();

        // find the mesh elements adjacent to the face
        im =  M_globalToLocalElement.find(elem1);

        ID localElem1;

        if (im == M_globalToLocalElement.end())
        {
            localElem1 = NotAnId;
        }
        else
        {
            localElem1 = (*im).second;
        }

        im =  M_globalToLocalElement.find(elem2);

        ID localElem2;
        if (im == M_globalToLocalElement.end())
        {
            localElem2 = NotAnId;
        }
        else
        {
            localElem2 = (*im).second;
        }

        pf =  &(M_meshPart->addFace(boundary));
        *pf = M_originalMesh->face( *is );

        pf->setLocalId( count );

        // true if we are on a subdomain border
        if ( !boundary && ( localElem1 == NotAnId || localElem2 == NotAnId ) )
        {
            // set the flag for faces on the subdomain border
            pf->setFlag( EntityFlags::SUBDOMAIN_INTERFACE );
        }

        ASSERT((localElem1 != NotAnId)||(localElem2 != NotAnId),
			   "A hanging face in mesh partitioner!");

        if (localElem1 == NotAnId)
         {
             pf->firstAdjacentElementIdentity()  = localElem2;
             pf->firstAdjacentElementPosition()
				 = M_originalMesh->face(*is).secondAdjacentElementPosition();
             pf->secondAdjacentElementIdentity() = NotAnId;
             pf->secondAdjacentElementPosition() = NotAnId;
             pf->reversePoints();
         }
        else
        {
            pf->firstAdjacentElementIdentity()  = localElem1;
            pf->firstAdjacentElementPosition()
				= M_originalMesh->face(*is).firstAdjacentElementPosition();
            pf->secondAdjacentElementIdentity() = localElem2;
            pf->secondAdjacentElementPosition()
				= localElem2 != NotAnId
						?
					M_originalMesh->face(*is).secondAdjacentElementPosition()
						:
					NotAnId;
        }

        for (ID id = 0;
			 id < M_originalMesh->face(*is).S_numLocalVertices; ++id)
        {
            inode = pf->point(id).id();
            im = M_globalToLocalVertex.find(inode);
            pf->setPoint(id, M_meshPart->pointList((*im).second));
        }
    }
    M_meshPart->setLinkSwitch("HAS_ALL_FACETS");
    M_meshPart->setLinkSwitch("FACETS_HAVE_ADIACENCY");
}

template<typename MeshType>
void MeshPartBuilder<MeshType>::finalSetup()
{
    UInt nVolumes = M_localElements.size();
    UInt nNodes   = M_localVertices.size();
    UInt nEdges   = M_localRidges.size();
    UInt nFaces   = M_localFacets.size();

    M_meshPart->setMaxNumPoints (nNodes, true);
    M_meshPart->setMaxNumEdges  (nEdges, true);
    M_meshPart->setMaxNumFaces  (nFaces, true);
    M_meshPart->setMaxNumVolumes( nVolumes, true);

    M_meshPart->setMaxNumGlobalPoints (M_originalMesh->numPoints());
    M_meshPart->setNumGlobalVertices  (M_originalMesh->numPoints());
    M_meshPart->setMaxNumGlobalEdges  (M_originalMesh->numEdges());
    M_meshPart->setMaxNumGlobalFaces  (M_originalMesh->numFaces());

    M_meshPart->setMaxNumGlobalVolumes(M_originalMesh->numVolumes());
    M_meshPart->setNumBFaces    (M_nBoundaryFacets);

    M_meshPart->setNumBPoints   (M_nBoundaryVertices);
    M_meshPart->setNumBEdges    (M_nBoundaryRidges);

    M_meshPart->setNumVertices (nNodes );
    M_meshPart->setNumBVertices(M_nBoundaryVertices);

    M_meshPart->updateElementEdges();

    M_meshPart->updateElementFaces();
}

}// namespace LifeV

#endif // MESH_PART_BUILDER_H
