/*
 * (C) Copyright Ian D. Romanick 2008
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * AUTHORS, COPYRIGHT HOLDERS, AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <lists.h>


namespace idr_winged_edge {

class polygon;
class mesh;
class half_edge;
class vertex;

/**
 * Edge data structure
 */
class half_edge {
public:
	half_edge(vertex *start);


	/**
	 * Gets a pointer to the polygon associated with this edge
	 */
	polygon *get_polygon(void)
	{
		return poly;
	}


	/**
	 * Sets the polygon associated with this edge
	 */
	void set_polygon(polygon *p)
	{
		poly = p;
	}


	/**
	 * Gets a pointer to the next counter-clockwise edge
	 *
	 * \sa get_prev_ccw
	 */
	half_edge *get_next_ccw(void)
	{
		return next_ccw;
	}


	/**
	 * Gets a pointer to the previous counter-clockwise edge
	 *
	 * \sa get_next_ccw
	 */
	half_edge *get_prev_ccw(void);


	/**
	 * Gets a pointer to the edge's sibling
	 *
	 * \returns
	 * A pointer to the edge's silbing.  If the edge has no sibling,
	 * \c NULL is returned.
	 *
	 * \sa set_sibling()
	 */
	half_edge *get_sibling(void)
	{
		return sibling;
	}


	/**
	 * Get a pointer to the starting vertex of the directed half-edge
	 *
	 * The starting vertex is clockwise relative to the ending vertex
	 * of an edge.  A half-edge is considered to be "leaving" its
	 * starting vertex.
	 *
	 * \sa get_end_vertex
	 */
	vertex *get_start_vertex(void)
	{
		return vert;
	}


	/**
	 * Get a pointer to the ending vertex of the directed half-edge
	 *
	 * The ending vertex is counter-clockwise relative to the starting
	 * vertex of an edge.  A half-edge is considered to be "entering" its
	 * ending vertex.
	 *
	 * \sa get_start_vertex
	 */
	vertex *get_end_vertex(void)
	{
		return sibling->vert;
	}


	/**
	 * Connects two half-edges as siblings
	 *
	 * The two \c half_edge objects are connected as siblings.  The
	 * connection is both ways.  It is not necessary to call
	 * \c set_sibling for the other object.
	 *
	 * \param e  \c half_edge to be connected to the calling object as
	 *           a sibling
	 *
	 * \sa get_sibling()
	 */
	void set_sibling(half_edge *e)
	{
		sibling = e;
		next_ccw = e;

		e->sibling = this;
		e->next_ccw = this;
	}


	/**
	 * Get a pointer to the next half-edge leaving the half-edge's
	 * starting vertex.
	 *
	 * \sa get_next_edge_entering_vertex, get_starting_vertex
	 */
	half_edge *get_next_edge_leaving_vertex(void)
	{
		return sibling->next_ccw;
	}


	/**
	 * Get a pointer to the next half-edge entering the half-edge's
	 * ending vertex.
	 *
	 * \sa get_next_edge_leaving_vertex, get_ending_vertex
	 */
	half_edge *get_next_edge_entering_vertex(void)
	{
		return next_ccw->sibling;
	}


	/**
	 * Link two edges in the mesh such that the current half-edge's next
	 * counter-clockwise edge is \c out
	 * 
	 * \param out  Half-edge that will be the next counter-clockwise edge
	 *             from \c this.
	 */
	bool make_adjacent(half_edge *out);

private:
	/**
	 * Directed half-edge in the opposite direction
	 */
	half_edge *sibling;


	/**
	 * Next edge linked in counter-clockwise order
	 *
	 * \note
	 * The current half-edge and its \c next_ccw pointer must have the
	 * same \c poly pointer.
	 */
	half_edge *next_ccw;


	/**
	 * Linkage information for the mesh's list of edges
	 */
	idr_lists::single_node mesh_node;


	/**
	 * Pointer to the polygon associated with this edge.
	 *
	 * \sa get_polygon()
	 */
	polygon *poly;


	/**
	 * Pointer to the half-edge's starting vertex. 
	 */
	vertex *vert;

	friend class mesh;
};


/**
 * Vertex data storage for edges in the mesh
 */
class vertex {
public:
	vertex(void *_v) : edge(NULL), v(_v)
	{
		/* empty */
	}


	vertex(void) : edge(NULL), v(NULL)
	{
		/* empty */
	}


	/**
	 * Get a pointer to a half-edge leaving the vertex
	 *
	 * \sa half_edge::get_next_edge_leaving_vertex, get_entering_edge
	 */
	half_edge *get_leaving_edge(void)
	{
		return edge;
	}


	/**
	 * Get a pointer to a half-edge entering the vertex
	 *
	 * \sa half_edge::get_next_edge_entering_vertex, get_leaving_edge
	 */
	half_edge *get_entering_edge(void)
	{
		return edge->get_sibling();
	}


	void set_edge(half_edge * e)
	{
		edge = e;
	}


	/**
	 * Determine whether a vertex is isolated.
	 *
	 * A vertex is considered to be isolated if there are no half-edges
	 * entering or leaving the vertex.
	 */
	bool isolated(void) const
	{
		return (edge == NULL);
	}


	/**
	 * Get a pointer to the user data associated with the vertex.
	 *
	 * \sa set_data
	 */
	void *get_data(void)
	{
		return v;
	}


	/**
	 * Stores a pointer to the user's data with the vertex
	 *
	 * \sa get_data
	 */
	void set_data(void *_v)
	{
		v = _v;
	}

private:
	/** Pointer to any half-edge leaving the vertex */
	half_edge *edge;

	/** Pointer to the vertex data. */
	void *v;
};


/**
 * Base class for polygons in the winged-edge data structure
 */
class polygon {
public:
	polygon(void) : edge(NULL)
	{
	}


	void set_edge(half_edge *e)
	{
		edge = e;
	}

	
	/**
	 * Get a pointer to the first edge added to the polygon
	 */
	half_edge *get_edge(void)
	{
		return edge;
	}

private:
	half_edge *edge;
};


/**
 * Factory class used to create \c polygon objects
 */
class polygon_factory {
public:
	/**
	 * Create a \c polygon object
	 */
	virtual polygon *create(void);


	/**
	 * Destructor for \c polygon_factory objects
	 *
	 * Typically the destructor is not needed, but since this class
	 * contains virtual functions a virtual destructor is required.
	 */
	virtual ~polygon_factory();
};


/**
 * Mesh class for all winged-edge structures
 */
class mesh {
public:
	/**
	 * \name Constructors for mesh class
	 *
	 * If supplied, the \c polygon_factory object is used to create
	 * \c polygon objects as they are added to the mesh.
	 */
	/*@{@*/
	mesh()
	{
		poly_factory = new polygon_factory;
	}

	mesh(polygon_factory *pf) : poly_factory(pf)
	{
	}
	/*@}@*/


	/**
	 * Add an edge to the mesh
	 */
	half_edge *add_edge(vertex *start, vertex *end);


	/**
	 * Add a polygon to the mesh from a list of edges
	 */
	polygon *add_polygon(half_edge **edges, unsigned num_edges);


	/**
	 * Iterator for the first edge in the mesh
	 */
	idr_lists::single_node_iterator &begin_edge(void)
	{
		return edge_list.begin();
	}


	/**
	 * Iterator for the edge after the last edge in the mesh
	 *
	 * This iterator is used as the terminating condition for edge
	 * iteration loops.
	 *
	 * \sa begin_edge()
	 */
	const idr_lists::single_node_iterator &end_edge(void) const
	{
		return edge_list.end();
	}

private:
	/**
	 * Linked list of half-edge pairs associated with this mesh
	 *
	 * \note
	 * Only \b one half-edge in each half-edge pair is stored in this
	 * list.
	 */
	idr_lists::single_list edge_list;


	/**
	 * Polygon factory used to create polygons for this mesh
	 *
	 * \sa polygon_factory::create
	 */
	polygon_factory *poly_factory;
};

};
