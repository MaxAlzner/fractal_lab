/*
 * (C) Copyright Ian D. Romanick 2007, 2008
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

#ifndef MODEL_H
#define MODEL_H

#include <GL/gl.h>

struct attribute_info;
struct element_info;
class model_data_factory;


/**
 * Semantic associated with a particular vertex attribute
 */
enum attribute_semantic {
	invalid_semantic,      /**< Invalid or unknown semantic */
	position_semantic,     /**< Vertex position */
	color_semantic,        /**< Vertex color */
	normal_semantic,       /**< Vertex or surface normal */
	tangent_semantic,      /**< Vertex or surface tangent */
	binormal_semantic,     /**< Vertex or surface binormal */
	texcoord_semantic,     /**< Vertex texture coordinate */
};

enum data_type {
	float_type,
	half_type,
	normalized_ubyte_type,
	normalized_ushort_type,
	normalized_uint_type,
	normalized_byte_type,
	normalized_short_type,
	normalized_int_type,
	ubyte_type,
	ushort_type,
	uint_type,
	byte_type,
	short_type,
	int_type
};


/**
 * Information describing a single attribute array
 */
struct attribute_info {
	/**
	 * Semantic of this attribute.
	 */
	enum attribute_semantic semantic;

	/**
	 * Specifies the number of components per generic vertex attribute
	 *
	 * \note
	 * Must be 1, 2, 3, or 4.
	 */
	GLint size;


	/**
	 * Specifies the data type of each component in the array
	 */
	enum data_type type;

	/**
	 * Specifies the byte offset between consecutive generic vertex
	 * attributes.
	 * 
	 * \note
	 * While 0 is a valid value for use with \c glVertexAttribPointer,
	 * it must not be used here.  Use the actual stride.
	 */
	GLsizei stride;


	/**
	 * Specifies a pointer to the first component of the first generic
	 * vertex attribute in the array.
	 */
	GLvoid *pointer;


	/**
	 * Get a pointer to an array item at a specific index.
	 */
	void *element_pointer(unsigned idx)
	{
		return ((char *) pointer + (stride * idx));
	}


	/**
	 * Get a pointer to an array item at a specific index.
	 */
	const void *element_pointer(unsigned idx) const
	{
		return ((const char *) pointer + (stride * idx));
	}
};


/**
 * Information describing vertex array indices
 */
struct element_info {
	/**
	 * Raw pointer to array indices
	 * 
	 * Pass this pointer to \c glDrawElements.  All other uses should
	 * probably use either \c index_16 or \c index_32.
	 *
	 * By default, the \c elements list is a list of triangles.  There
	 * are 3 elements per triangle, and (\c total_elements / 3) triangles.
	 *
	 * \sa set_index, get_index
	 */
	void *data;

	enum data_type type;

	/**
	 * Get the vertex index of the ith element in the element array
	 */
	unsigned get_index(unsigned i) const
	{
		if (this->type == uint_type) {
			const GLuint *const p = (GLuint *) data;
			return p[i];
		} else if (this->type == ushort_type) {
			const GLushort *const p = (GLushort *) data;
			return p[i];
		} else {
			return 0;
		}
	}


	/**
	 * Set the vertex index of the ith element in the element array
	 * 
	 * \param i  Entry in the element array to be set
	 * \param v  Value to set
	 */
	void set_index(unsigned i, unsigned v)
	{
		if (this->type == uint_type) {
			GLuint *const p = (GLuint *) data;
			p[i] = v;
		} else if (this->type == ushort_type) {
			GLushort *const p = (GLushort *) data;
			p[i] = v;
		} else {
			/* do nothing */
		}
	}
};

/**
 * Virtual base class used for all model types.
 */
class model {
public:
	virtual ~model();

	/**
	 * Enable automatic generation of per-vertex normal vectors.
	 *
	 * When called after \c preload and before \c load, this will enable
	 * automatic generation of per-vertex normals.  If the model already
	 * has per-vertex normals, this data will be discarded in favor of the
	 * generated data.
	 *
	 * \note
	 * Calling this function may invalidate the data previously returned
	 * by \c model::get_attribute_count and \c model::get_attribute.
	 */
	void enable_normal_generation()
	{
		this->autogenerate_normals = true;
	}

	/**
	 * Enable automatic generation of per-vertex tangent vectors.
	 *
	 * When called after \c preload and before \c load, this will enable
	 * automatic generation of per-vertex tangents.  If the model already
	 * has per-vertex tangents, this data will be discarded in favor of
	 * the generated data.
	 *
	 * \note
	 * Automatic generation of tangent vectors requires the availability
	 * of texture coordinates.  If the model lacks textures coordinates,
	 * this function will return \c false, and tangent vectors will not be
	 * generated.
	 */
	bool enable_tangent_generation()
	{
		if (this->get_attribute(texcoord_semantic)) {
			this->autogenerate_tangents = true;
			return true;
		}

		return false;
	}

	unsigned get_vertex_count() const;

	unsigned get_attribute_count() const;

	const attribute_info *get_attribute(unsigned idx) const;

	const attribute_info *get_attribute(enum attribute_semantic sem) const;

	virtual bool load(model_data_factory *factory) = 0;

//protected:
	/**
	 * Generic base class constructor.
	 *
	 * \note
	 * Making constructor protected forces derived classes to be used for
	 * object creation.
	 */
	model(void)
		: single_mode(true), count(NULL), base(NULL), attr(NULL),
		  autogenerate_normals(false), autogenerate_tangents(false)
	{
	}


	void initialize_draw_data(unsigned _prim_count, bool _single_mode);


	/**
	 * Generate normals from model data
	 */
	void generate_normals(float cut_off_angle);


	/**
	 * Find the attribute with the specified semantic
	 */
	struct attribute_info *get_attribute(enum attribute_semantic sem);


	/**
	 * Number of draw calls for the object.
	 */
	GLsizei prim_count;


	/**
	 * Is there a single mode for all draw calls or a separate mode for each?
	 */
	bool single_mode;

	union {
		GLenum *ptr;	/**< Array of per-draw call modes. */
		GLenum value;	/**< Mode used for all draw calls. */
	} mode;


	/**
	 * Per-draw call vertex counts.
	 */
	GLsizei *count;


	/**
	 * Per-draw call element data
	 *
	 * For element based models this is an array of indices into the
	 * element array.  For DrawArrays-like models, this is an array of
	 * base element indices.
	 */
	GLint *base;

	unsigned attr_count;
	struct attribute_info *attr;

	struct element_info elements;

	bool autogenerate_normals;

	bool autogenerate_tangents;

private:

};


class model_data_factory {
public:
	virtual bool get_attribute_memory(struct attribute_info *attributes,
					  unsigned num_attributes,
					  unsigned num_verts) = 0;

	virtual bool get_element_memory(struct element_info *element,
					unsigned num_elements,
					unsigned num_verts) = 0;
};

#endif /* MODEL_H */
