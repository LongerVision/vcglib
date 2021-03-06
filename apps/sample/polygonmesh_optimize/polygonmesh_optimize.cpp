/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2016                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include <vcg/complex/complex.h>

/*include the algorithms for updating: */
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/create/platonic.h>

#include <wrap/io_trimesh/export_obj.h>
#include <wrap/io_trimesh/import_ply.h>

#include <vcg/complex/algorithms/dual_meshing.h>

#include <vcg/complex/algorithms/polygon_support.h>

#include <vcg/complex/algorithms/polygonal_algorithms.h>

#include <vcg/complex/algorithms/update/quality.h>

using namespace vcg;
using namespace std;

// forward declarations
class TFace;
class TVertex;

struct TUsedTypes: public vcg::UsedTypes< vcg::Use<TVertex>::AsVertexType, vcg::Use<TFace>::AsFaceType >{};

/* Definition of a mesh of triangles
*/
class TVertex : public Vertex< TUsedTypes,
    vertex::BitFlags,
    vertex::Coord3f,
    vertex::Normal3f,
    vertex::Mark >{};

class TFace   : public Face<   TUsedTypes,
    face::VertexRef,	// three pointers to vertices
    face::Normal3f,		// normal
    face::BitFlags,		// flags
    face::FFAdj        // three pointers to adjacent faces
> {};

/* the mesh is a container of vertices and a container of faces */
class TMesh   : public vcg::tri::TriMesh< vector<TVertex>, vector<TFace> > {};


/* Definition of a mesh of polygons that also supports half-edges
*/
class PFace;
class PVertex;

struct PUsedTypes: public vcg::UsedTypes<vcg::Use<PVertex>  ::AsVertexType,
                                          vcg::Use<PFace>	::AsFaceType>{};

class PVertex:public vcg::Vertex<	PUsedTypes,
                                    vcg::vertex::Coord3f,
                                    vcg::vertex::Normal3f,
                                    vcg::vertex::Mark,
                                    vcg::vertex::BitFlags>{} ;

class PFace:public vcg::Face<
     PUsedTypes
    ,vcg::face::PolyInfo // this is necessary  if you use component in vcg/simplex/face/component_polygon.h
                         // It says "this class is a polygon and the memory for its components (e.g. pointer to its vertices
                         // will be allocated dynamically")
    ,vcg::face::PFVAdj	 // Pointer to the vertices (just like FVAdj )
    ,vcg::face::PFVAdj
    ,vcg::face::PFFAdj	 // Pointer to edge-adjacent face (just like FFAdj )
    ,vcg::face::BitFlags // bit flags
    ,vcg::face::Normal3f // normal
    ,face::Qualityd      // face quality
> {};

class PMesh: public
    vcg::tri::TriMesh<
    std::vector<PVertex>,	// the vector of vertices
    std::vector<PFace >     // the vector of faces
    >{};

TMesh primalT;
PMesh primal,dual,dual_opt,dual_flat;

int	main(int argc, char *argv[])
{
    assert(argc>1);

    std::cout<<"Opening file " << argv[0] << std::endl;

    int res=vcg::tri::io::ImporterPLY<TMesh>::Open(primalT,argv[1]);
    assert(res==vcg::ply::E_NOERROR);
    vcg::tri::PolygonSupport<TMesh,PMesh>::ImportFromTriMesh(primal,primalT);

    vcg::tri::DualMeshing<PMesh>::MakeDual(primal,dual);
    vcg::tri::io::ExporterOBJ<PMesh>::Save(dual,"./dual_no_optimized.obj",vcg::tri::io::Mask::IOM_BITPOLYGONAL);

    //copy the mesh
    vcg::tri::Append<PMesh,PMesh>::Mesh(dual_opt,dual);
    vcg::tri::Append<PMesh,PMesh>::Mesh(dual_flat,dual);

    vcg::PolygonalAlgorithm<PMesh>::SmoothReprojectPCA(dual_opt);

    vcg::tri::io::ExporterOBJ<PMesh>::Save(dual_opt,"./dual_optimized.obj",vcg::tri::io::Mask::IOM_BITPOLYGONAL);

    vcg::PolygonalAlgorithm<PMesh>::FlattenFaces(dual_flat);
    vcg::tri::io::ExporterOBJ<PMesh>::Save(dual_flat,"./dual_flattened.obj",vcg::tri::io::Mask::IOM_BITPOLYGONAL);

    //update the quality as template
    std::pair<typename PMesh::ScalarType,typename PMesh::ScalarType> minmax_dual,minmax_dual_opt,minmax_dual_flat;
    typename PMesh::ScalarType Avg_dual,Avg_dual_opt,Avg_dual_flat;

    vcg::PolygonalAlgorithm<PMesh>::UpdateQuality(dual,PolygonalAlgorithm<PMesh>::QTemplate);
    minmax_dual=tri::Stat<PMesh>::ComputePerFaceQualityMinMax(dual);
    Avg_dual=tri::Stat<PMesh>::ComputePerFaceQualityAvg(dual);

    vcg::PolygonalAlgorithm<PMesh>::UpdateQuality(dual_opt,PolygonalAlgorithm<PMesh>::QTemplate);
    minmax_dual_opt=tri::Stat<PMesh>::ComputePerFaceQualityMinMax(dual_opt);
    Avg_dual_opt=tri::Stat<PMesh>::ComputePerFaceQualityAvg(dual_opt);

    vcg::PolygonalAlgorithm<PMesh>::UpdateQuality(dual_flat,PolygonalAlgorithm<PMesh>::QTemplate);
    minmax_dual_flat=tri::Stat<PMesh>::ComputePerFaceQualityMinMax(dual_flat);
    Avg_dual_flat=tri::Stat<PMesh>::ComputePerFaceQualityAvg(dual_flat);

    std::cout<<std::endl<<std::endl<<"Template Quality No Optimized min / max " <<minmax_dual.first<<" / "<<minmax_dual.second<<std::endl;
    std::cout<<"Template Quality Optimized min / max " <<minmax_dual_opt.first<<" / "<<minmax_dual_opt.second<<std::endl;
    std::cout<<"Template Quality Flattened min / max " <<minmax_dual_flat.first<<" / "<<minmax_dual_flat.second<<std::endl<<std::endl;

    std::cout<<"Template Quality No Optimized Average " <<Avg_dual<<std::endl;
    std::cout<<"Template Quality Optimized Average " <<Avg_dual_opt<<std::endl;
    std::cout<<"Template Quality Flattened Average " <<Avg_dual_flat<<std::endl<<std::endl<<std::endl<<std::endl;


    vcg::PolygonalAlgorithm<PMesh>::UpdateQuality(dual,PolygonalAlgorithm<PMesh>::QPlanar);
    minmax_dual=tri::Stat<PMesh>::ComputePerFaceQualityMinMax(dual);
    Avg_dual=tri::Stat<PMesh>::ComputePerFaceQualityAvg(dual);

    vcg::PolygonalAlgorithm<PMesh>::UpdateQuality(dual_opt,PolygonalAlgorithm<PMesh>::QPlanar);
    minmax_dual_opt=tri::Stat<PMesh>::ComputePerFaceQualityMinMax(dual_opt);
    Avg_dual_opt=tri::Stat<PMesh>::ComputePerFaceQualityAvg(dual_opt);

    vcg::PolygonalAlgorithm<PMesh>::UpdateQuality(dual_flat,PolygonalAlgorithm<PMesh>::QPlanar);
    minmax_dual_flat=tri::Stat<PMesh>::ComputePerFaceQualityMinMax(dual_flat);
    Avg_dual_flat=tri::Stat<PMesh>::ComputePerFaceQualityAvg(dual_flat);

    std::cout<<"Flatness Quality No Optimized min / max " <<minmax_dual.first<<" / "<<minmax_dual.second<<std::endl;
    std::cout<<"Flatness Quality Optimized min / max " <<minmax_dual_opt.first<<" / "<<minmax_dual_opt.second<<std::endl;
    std::cout<<"Flatness Quality Flattened min / max " <<minmax_dual_flat.first<<" / "<<minmax_dual_flat.second<<std::endl<<std::endl;

    std::cout<<"Flatness Quality No Optimized Average " <<Avg_dual<<std::endl;
    std::cout<<"Flatness Quality Optimized Average " <<Avg_dual_opt<<std::endl;
    std::cout<<"Flatness Quality Flattened Average " <<Avg_dual_flat<<std::endl;

}



