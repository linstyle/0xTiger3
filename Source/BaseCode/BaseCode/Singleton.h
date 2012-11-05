#pragma once

#include <assert.h>

#define initialiseSingleton( type ) \
  template <> type * Singleton < type > :: mSingleton = 0

template < class type > class  Singleton {
public:
  
  Singleton( ) {
	
	assert( this->mSingleton == 0 );
	this->mSingleton = static_cast<type *>(this);
  }
  
  virtual ~Singleton( ) {
	this->mSingleton = 0;
  }

  static type & getSingleton( ) { assert( mSingleton ); return *mSingleton; }
  static type * getSingletonPtr( ) { return mSingleton; }

protected:
  
  static type * mSingleton;
};