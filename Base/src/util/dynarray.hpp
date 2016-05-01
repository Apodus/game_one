
#pragma once

#include <iterator>
#include <cstring>
#include <limits>
#include "util/logging.hpp"
#include "util/platform.hpp"

#ifdef max
#undef max
#endif

#if PLATFORM_WIN
#pragma warning(disable:4996)
#endif

namespace std {
template< class T >
struct dynarray
{
    // types:
    typedef       T                               value_type;
    typedef       T&                              reference;
    typedef const T&                              const_reference;
    typedef       T*                              iterator;
    typedef const T*                              const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;

private:
    T*        store;
    size_type count;
	size_type currentCount;

    void check(size_type n) {
		ASSERT(n < count, "dynarray index out of bounds");
	}

    T* alloc(size_type n) {
		ASSERT(n < std::numeric_limits<size_type>::max()/sizeof(T), "dynarray alloc request too big");
		return reinterpret_cast<T*>(new char[n*sizeof(T)]);
	}

	dynarray();
public:
    explicit dynarray(size_type c) : store( alloc( c ) ), count( c ), currentCount(0)
    {
		size_type i;
        try {
            for ( size_type i = 0; i < count; ++i )
                new (store+i) T;
        } catch ( ... ) {
            for ( ; i > 0; --i )
                (store+(i-1))->~T();
            throw;
        }
	}

	const dynarray& operator = (const dynarray& other) {
		this->currentCount = other.currentCount;
		this->count = other.count;
		delete[] this->store;
		this->store = alloc(max_size());
		std::memcpy(store, other.store, sizeof(T) * count);
		return *this;
	}

    dynarray(const dynarray& d)
		: store( alloc( d.count ) ), count( d.count ), currentCount( d.currentCount )
        { 
			try {
				std::memcpy( store, d.store, sizeof(T) * count );
			}
			catch ( ... ) {
				delete store;
				ASSERT(false, "something failed bad");
			}
		}

    ~dynarray()
        { for ( size_type i = 0; i < count; ++i )
              (store+i)->~T();
          delete[] store; }

    // iterators:
    iterator       begin()        { return store; }
    const_iterator begin()  const { return store; }
    const_iterator cbegin() const { return store; }
    iterator       end()          { return store + count; }
    const_iterator end()    const { return store + count; }
    const_iterator cend()   const { return store + count; }

    reverse_iterator       rbegin()       
        { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const
        { return reverse_iterator(end()); }
    reverse_iterator       rend()         
        { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const
        { return reverse_iterator(begin()); }

    // capacity:
    size_type size()     const { return currentCount; }
    size_type max_size() const { return count; }
    bool      empty()    const { return count == 0; }

    // element access:
    reference       operator[](size_type n)       { return store[n]; }
    const_reference operator[](size_type n) const { return store[n]; }

    reference       front()       { return store[0]; }
    const_reference front() const { return store[0]; }
    reference       back()        { return store[currentCount-1]; }
    const_reference back()  const { return store[currentCount-1]; }

    const_reference at(size_type n) const { check(n); return store[n]; }
    reference       at(size_type n)       { check(n); return store[n]; }


	void push_back(const T& t) {store[currentCount] = t; ++currentCount;}
	void clear() { currentCount = 0; }

    // data access:
    T*       data()       { return store; }
    const T* data() const { return store; }
};

} // namespace std


#if PLATFORM_WIN
#pragma warning(default:4996)
#endif
