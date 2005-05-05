/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
/****************************************************************************
  History

$Log: not supported by cvs2svn $
Revision 1.2  2004/03/10 21:38:39  cignoni
Written some documentation and added to the space module

Revision 1.1  2004/02/15 23:34:04  cignoni
Initial commit


****************************************************************************/

#ifndef __VCGLIB_BOX2
#define __VCGLIB_BOX2

#include <assert.h>
#include <vcg/math/base.h>
#include <vcg/space/point2.h>
namespace vcg {

/** \addtogroup space */
/*@{*/

/**
	Templated class for a 2D bounding box. It is stored just as two Point2	
	@param BoxScalarType (Template Parameter) Specifies the scalar field.
*/
template <class BoxScalarType>
class Box2
{
public:
		/// The scalar type
	typedef BoxScalarType ScalarType;

		/// min coordinate point
    Point2<BoxScalarType> min;
		/// max coordinate point
    Point2<BoxScalarType> max;
		/// Standard constructor
	inline  Box2() { min.x()= 1; max.x()= -1; min.y()= 1; max.y()= -1; }
		/// Copy constructor
	inline  Box2( const Box2 & b ) { min=b.min; max=b.max; }
		/// Distructor
	inline ~Box2() { }
		/// Operator to compare two bounding box
	inline bool operator == ( Box2 const & p ) const
	{
		return min==p.min && max==p.max;
	}
			/// Initializing the bounding box with a point
	void Set( const Point2<BoxScalarType> & p )
	{
		min = max = p;
	}
		// Initializing with the values
	inline void Set( BoxScalarType minx, BoxScalarType miny, BoxScalarType maxx, BoxScalarType maxy )
	{
		min[0] = minx;
		min[1] = miny;
		max[0] = maxx;
		max[1] = maxy;
	}
		/// Set the bounding box to a null value
	void SetNull()
	{
		 min.x()= 1; max.x()= -1; min.y()= 1; max.y()= -1;
	}
		/** Function to add two bounding box
			@param b Il bounding box che si vuole aggiungere
		*/
	void Add( Box2 const & b )
	{
		if(IsNull())
		{
			min=b.min;
			max=b.max;
		}
		esle
		{
			if(min.v[0] > b.min.v[0]) min.v[0] = b.min.v[0];
			if(min.v[1] > b.min.v[1]) min.v[1] = b.min.v[1];

			if(max.v[0] < b.max.v[0]) max.v[0] = b.max.v[0];
			if(max.v[1] < b.max.v[1]) max.v[1] = b.max.v[1];
		}
	}
		/** Funzione per aggiungere un punto al bounding box. Il bounding box viene modificato se il punto
			cade fuori da esso.
			@param p The point 2D
		*/
	void Add( const Point2<BoxScalarType> & p )
	{
		if(IsNull()) Set(p);
		else 
		{
			if(min.v[0] > p.v[0]) min.v[0] = p.v[0];
			if(min.v[1] > p.v[1]) min.v[1] = p.v[1];

			if(max.v[0] < p.v[0]) max.v[0] = p.v[0];
			if(max.v[1] < p.v[1]) max.v[1] = p.v[1];
		}
	}
		/** Calcola l'intersezione tra due bounding box. Al bounding box viene assegnato il valore risultante.
			@param b Il bounding box con il quale si vuole effettuare l'intersezione
		*/
	void Intersect( const Box2 & b )
	{
		if(min.v[0] < b.min.v[0]) min.v[0] = b.min.v[0];
		if(min.v[1] < b.min.v[1]) min.v[1] = b.min.v[1];

		if(max.v[0] > b.max.v[0]) max.v[0] = b.max.v[0];
		if(max.v[1] > b.max.v[1]) max.v[1] = b.max.v[1];

		if(min.v[0]>max.v[0] || min.v[1]>max.v[1]) SetNull();
	}

		/** Trasla il bounding box di un valore definito dal parametro.
			@param p Il bounding box trasla sulla x e sulla y in base alle coordinate del parametro
		*/
	void Translate( const Point2<BoxScalarType> & p )
	{
		min += p;
		max += p;
	}
		/** Verifica se un punto appartiene ad un bounding box.
			@param p The point 2D
			@return True se p appartiene al bounding box, false altrimenti
		*/
	bool IsIn( Point2<BoxScalarType> const & p ) const
	{
		return (
			min.v[0] <= p.v[0] && p.v[0] <= max.v[0] &&
			min.v[1] <= p.v[1] && p.v[1] <= max.v[1]
		);
	}
		/** Verifica se un punto appartiene ad un bounding box aperto sul max.
			@param p The point 2D
			@return True se p appartiene al bounding box, false altrimenti
		*/
	bool IsInEx( Point2<BoxScalarType> const & p ) const
	{
		return  (
			min.v[0] <= p.v[0] && p.v[0] < max.v[0] &&
			min.v[1] <= p.v[1] && p.v[1] < max.v[1] 
		);
	}
		/** Verifica se due bounding box collidono cioe' se hanno una intersezione non vuota. Per esempio
			due bounding box adiacenti non collidono.
			@param b A bounding box
			@return True se collidoo, false altrimenti
		*/
	bool Collide( Box2 const &b )
	{
		Box2f bb=*this;
		bb.Intersect(b);
		return bb.IsValid();
	}
		/** Controlla se il bounding box e' nullo.
			@return True se il bounding box e' nullo, false altrimenti
		*/
	inline bool IsNull() const { return min.v[0]>max.v[0] || min.v[1]>max.v[1]; }
		/** Controlla se il bounding box e' consistente.
			@return True se il bounding box e' consistente, false altrimenti
		*/
	inline bool IsValid() const { return min.v[0]<max.v[0] && min.v[1]<max.v[1]; }
		/** Controlla se il bounding box e' vuoto.
			@return True se il bounding box e' vuoto, false altrimenti
		*/
	inline bool IsEmpty() const { return min==max; }
		/// Restituisce la lunghezza della diagonale del bounding box.
	BoxScalarType Diag() const
	{
		return Distance(min,max);
	}
		/// Calcola il centro del bounding box.
	Point2<BoxScalarType> Center() const
	{
		return (min+max)/2;
	}
		/// Calcola l'area del Bounding box.
	inline BoxScalarType Area() const
	{
		return (max.v[0]-min.v[0])*(max.v[1]-min.v[1]);
	}
		/// Calcola la dimensione del bounding box sulla x.
	inline BoxScalarType DimX() const { return max.v[0]-min.v[0]; }
	/// Calcola la dimensione del bounding box sulla y.
	inline BoxScalarType DimY() const { return max.v[1]-min.v[1]; }

	inline void Normalize( Point2<BoxScalarType> & p )
	{
		p -= min;
		p[0] /= max[0]-min[0];
		p[1] /= max[1]-min[1];
	}
}; // end class definition


	/// Specificazione di box of short
typedef Box2<short>  Box2s;
	/// Specificazione di box of int
typedef Box2<int>	 Box2i;
	/// Specificazione di box of float
typedef Box2<float>  Box2f;
	/// Specificazione di box of double
typedef Box2<double> Box2d;

/*@}*/
} // end namespace


#endif
