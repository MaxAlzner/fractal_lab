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

#ifndef IDR_PLY_MODEL_H
#define IDR_PLY_MODEL_H

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# define ZLIB_WINAPI
# include <windows.h>
#endif

#include <GL/glew.h>
#include <zlib.h>

#include <model.h>


/**
 * Model subclass used to for PLY formatted models loaded from disk
 */
class idr_ply_model : public model {
public:
	idr_ply_model()
	{
		/* empty */
	}

	virtual ~idr_ply_model(void);

	bool set_file(const char *filename);

	virtual bool load(model_data_factory *factory);

private:
	gzFile f;

	int nfaces;
	struct Face **flist;
};

#endif /* IDR_PLY_MODEL_H */
