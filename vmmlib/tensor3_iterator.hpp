/*
 * Copyright (c) 2006-2014, Visualization and Multimedia Lab,
 *                          University of Zurich <http://vmml.ifi.uzh.ch>,
 *                          Eyescale Software GmbH,
 *                          Blue Brain Project, EPFL
 *
 * This file is part of VMMLib <https://github.com/VMML/vmmlib/>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.  Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions and
 * the following disclaimer in the documentation and/or other materials provided
 * with the distribution.  Neither the name of the Visualization and Multimedia
 * Lab, University of Zurich nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __VMML__TENSOR3_ITERATOR__HPP__
#define __VMML__TENSOR3_ITERATOR__HPP__

#include <vmmlib/matrix.hpp>

namespace vmml
{
	

template< typename T >
class tensor3_iterator
{
public:
	
	typedef typename T::value_type              value_type;
	typedef typename T::pointer                 pointer;
	typedef typename T::reference               reference;
    typedef typename std::forward_iterator_tag  iterator_category;
    typedef size_t                              difference_type;
    
	typedef typename matrix< T::ROWS, T::COLS, typename T::value_type >::iterator matrix_iterator;
    
    typedef typename T::front_slice_type  slice_type;
	
	tensor3_iterator() : _tensor3( 0 ), _matrix_index( 0 ) {};
	
	tensor3_iterator( T& t_, bool begin_ ) : _tensor3( &t_ ), _matrix_index( 0 )
    {
        if ( begin_ )
        {
            _matrix_index       = 0;
            slice_type& slice_  = _tensor3->get_frontal_slice_fwd( _matrix_index );
            
            _matrix_it          = slice_.begin();
            _matrix_it_end      = slice_.end();
        }
        else
        {
            _matrix_index       = T::SLICES - 1;
            slice_type& slice_  = _tensor3->get_frontal_slice_fwd( _matrix_index );

            _matrix_it          = slice_.end();
            _matrix_it_end      = slice_.begin();
        }
    }
	
	value_type& operator*()
	{
		return *_matrix_it;
	}
	
	const value_type& operator*() const
	{
		return *_matrix_it;
	}
    
    bool operator==( const tensor3_iterator& it_ ) const
    {
        return it_._tensor3 == _tensor3
            // && it_._matrix_index == _matrix_index 
            && it_._matrix_it == _matrix_it;
        
    }
    
    bool operator!=( const tensor3_iterator& it_ ) const
    {
        return ! operator==(it_);
    }
    
	
	void operator++() 
	{
        if ( _tensor3 == 0 )
        {
            VMMLIB_ERROR( "attempt to increase singular iterator", VMMLIB_HERE );
        }
        
        if ( _matrix_it != _matrix_it_end )
            ++_matrix_it;

		if ( _matrix_it == _matrix_it_end && _matrix_index + 1 < T::SLICES )
		{
            ++_matrix_index;
            //slice_type& slice_  = _tensor3->get_frontal_slice( _matrix_index );

            _matrix_it = _tensor3->get_frontal_slice_fwd( _matrix_index ).begin();
            _matrix_it_end	= _tensor3->get_frontal_slice_fwd( _matrix_index ).end();
		}
	}
	
	
protected:
	matrix_iterator		_matrix_it;
	matrix_iterator		_matrix_it_end;
	T*					_tensor3;
	size_t				_matrix_index;

}; //end tensor3_iterator class




template< typename T >
class tensor3_const_iterator
{
public:
	
	typedef typename T::value_type              value_type;
	typedef typename T::pointer                 pointer;
	typedef typename T::reference               reference;
    typedef typename std::forward_iterator_tag  iterator_category;
    typedef size_t                              difference_type;
    
	typedef typename matrix< T::ROWS, T::COLS, typename T::value_type >::const_iterator matrix_iterator;
    
    typedef typename T::front_slice_type  slice_type;
	
	tensor3_const_iterator() : _tensor3( 0 ), _matrix_index( 0 ) {};
	
	tensor3_const_iterator( const T& t_, bool begin_ ) : _tensor3( &t_ ), _matrix_index( 0 )
    {
        if ( begin_ )
        {
            _matrix_index       = 0;
            const slice_type& slice_  = _tensor3->get_frontal_slice_fwd( _matrix_index );
            
            _matrix_it          = slice_.begin();
            _matrix_it_end      = slice_.end();
        }
        else
        {
            _matrix_index       = T::SLICES - 1;
            const slice_type& slice_  = _tensor3->get_frontal_slice_fwd( _matrix_index );

            _matrix_it          = slice_.end();
            _matrix_it_end      = slice_.begin();
        }
    }
	
	const value_type& operator*() const
	{
		return *_matrix_it;
	}
    
    bool operator==( const tensor3_const_iterator& it_ ) const
    {
        return it_._matrix_it == _matrix_it;
        
    }
    
    bool operator!=( const tensor3_const_iterator& it_ ) const
    {
        return it_._matrix_it != _matrix_it;
    }
    
	
	void operator++() 
	{
        if ( _tensor3 == 0 )
        {
            VMMLIB_ERROR( "attempt to increase singular iterator", VMMLIB_HERE );
        }
        
        if ( _matrix_it != _matrix_it_end )
            ++_matrix_it;

		if ( _matrix_it == _matrix_it_end && _matrix_index + 1 < T::SLICES )
		{
            ++_matrix_index;
            //const slice_type& slice_  = _tensor3->get_frontal_slice( _matrix_index );

            _matrix_it = _tensor3->get_frontal_slice_fwd( _matrix_index ).begin();
            _matrix_it_end	= _tensor3->get_frontal_slice_fwd( _matrix_index ).end();
		}
	}
	
	
protected:
	matrix_iterator		_matrix_it;
	matrix_iterator		_matrix_it_end;
	const T*            _tensor3;
	size_t				_matrix_index;

}; //end tensor3_const_iterator class





}// end vmml namespace

#endif

