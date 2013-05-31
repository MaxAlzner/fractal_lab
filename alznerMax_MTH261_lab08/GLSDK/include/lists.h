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

namespace idr_lists {

#ifndef NULL
# define NULL 0
#endif

class single_list;
class single_circular_list;


/**
 * Simple singly-linked list node
 */
class single_node {
public:
	void set_data(void *d)
	{
		data = d;
	}


	void *get_data(void)
	{
		return data;
	}

private:
	single_node *next;
	void *data;

	friend class single_list;
	friend class single_circular_list;
	friend class single_node_iterator;
};


/**
 * Forward iterator for singly-linked lists
 */
class single_node_iterator {
public:
	single_node_iterator(single_node *start, bool it)
		: iterated(it), curr(start)
	{
	}


	/**
	 * Return the list node referenced by the current iterator position.
	 */
	single_node *operator*(void)
	{
		return curr;
	}


	/** Determine whether two iterators are the same. */
	bool operator==(const single_node_iterator &b) const
	{
		return (iterated == b.iterated) && (curr == b.curr);
	}


	/** Determine whether two iterators are different. */
	bool operator!=(const single_node_iterator &b) const
	{
		return (iterated != b.iterated) || (curr != b.curr);
	}


	/** Iterate to the next node. */
	single_node_iterator &operator++(void)
	{
		if (curr != NULL) {
			curr = curr->next;
		}

		iterated = true;
		return *this;
	}

private:
	/**
	 * Has the iterator been advanced at least once?
	 */
	bool iterated;


	/**
	 * Current node structure
	 */
	single_node *curr;
};


/**
 * Simple singly-linked list
 */
class single_list {
public:
	single_list(void) : head(NULL), tail(NULL), end_iter(NULL, true)
	{
	}


	/**
	 * Add a node to the end (tail) of the list.
	 */
	void push_back(single_node *n)
	{
		if (head == NULL) {
			head = n;
		}

		if (tail != NULL) {
			tail->next = n;
		}

		tail = n;
		n->next = NULL;
	}


	/**
	 * Get a list node iterator starting with the first node in the list.
	 */
	single_node_iterator &begin(void)
	{
		return *(new single_node_iterator(head, true));
	}


	/**
	 * Get a list node iterator starting aftet the last node of the list.
	 */
	const single_node_iterator &end(void) const
	{
		return end_iter;
	}

private:
	single_node *head;
	single_node *tail;

	single_node_iterator end_iter;
};


/**
 * Simple singly-linked circular list
 *
 * \note
 * This code is not used internally by the library, and has received \b no
 * testing.  Caveat emptor.
 */
class single_circular_list {
	single_circular_list(void) : head(NULL)
	{
	}


	void push_back(single_node *n)
	{
		if (head == NULL) {
			head = n;
		}

		if (tail != NULL) {
			tail->next = n;
		}

		tail = n;
		n->next = head;
	}

private:
	single_node *head;
	single_node *tail;
};

};
