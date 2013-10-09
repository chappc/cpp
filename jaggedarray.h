#ifndef JAGGEDARRAY_H_INCLUDED
#define JAGGEDARRAY_H_INCLUDED

#include <iostream>
#include <algorithm>
#include <iomanip>

template <class T> class JaggedArray {

public:


  JaggedArray () {this->fabricate();}
  JaggedArray ( size_t len, const T& object = T() ) {this->fabricate( len, object );}
  JaggedArray ( const JaggedArray& jagger ) {duplicate ( jagger );}
  JaggedArray& operator= ( const JaggedArray& jagger );
  ~JaggedArray () {this->eradicate ();}

  size_t numBins () const {return num_bins;}
  size_t numElements () const {return num_elements;}
  bool isPacked () const {return packed;}
  size_t numElementsInBin ( size_t bin ) const;
  const T& getElement ( size_t bin, size_t sub ) const;
  void print () const;

  void addElement ( size_t bin, const T& o );
  void removeElement ( size_t bin, size_t sub );
  void pack ();
  void unpack ();
  void clear ();

private:

  size_t num_elements;
  size_t num_bins;
  size_t * counts;
  T ** unpacked_values;
  size_t * offsets;
  T * packed_values;
  bool packed;

  void fabricate ();
  void fabricate ( size_t length, const T& o );
  void duplicate ( const JaggedArray& jagged );
  void eradicate ();

};

template <class T> void JaggedArray<T>::fabricate () {
  num_elements = 0;
  num_bins = 0;
  packed = false;
  counts = NULL;
  unpacked_values = NULL;
  offsets = NULL;
  packed_values = NULL;

}

template <class T> void JaggedArray<T>::fabricate ( size_t length, const T& o ) {
  unpacked_values = new T * [length];
  counts = new size_t [length];
  for ( size_t i = 0; i < length; ++i ) {
      counts [i] = 0;
    }
  num_elements = 0;
  num_bins = length;
  packed = false;
  offsets = NULL;
  packed_values = NULL;
}

template <class T> void JaggedArray<T>::duplicate ( const JaggedArray& jagged ) {
  this->num_elements = jagged.num_elements;
  this->num_bins = jagged.num_bins;
  this->packed = jagged.packed;
  size_t i;
  if ( jagged.packed ) {
      this->offsets = new size_t [num_bins];
      for ( i = 0; i < num_bins; ++i ) {
          this->offsets [i] = jagged.offsets[i];
        }
      this->packed_values = new T [num_elements];
      for ( i = 0; i < num_elements; ++i ) {
          this->packed_values [i] = jagged.packed_values [i];
        }
      this->counts = NULL;
      this->unpacked_values = NULL;
    }
  else {
      this->counts = new size_t [num_bins];
      this->unpacked_values = new T * [num_bins];
      for ( i = 0; i < num_bins; ++i ) {
          this->counts [i] = jagged.counts[i];
          if ( counts [i] > 0 ) {
              this->unpacked_values[i] = new T [counts[i]];
              for ( size_t j = 0; j < counts[i]; ++j ) {
                  this->unpacked_values [i] [j] = jagged.unpacked_values [i] [j];
                }
            }
        }
      this->offsets = NULL;
      this->packed_values = NULL;
    }
  return;
}

template <class T> JaggedArray<T>& JaggedArray<T>::operator= ( const JaggedArray<T>& jagger ) {
  if ( this != &jagger ) {
      this->eradicate();
      this->duplicate(jagger);
    }
  return *this;
}

template <class T> void JaggedArray<T>::eradicate () {
  if ( packed ) {
      delete [] offsets;
      delete [] packed_values;
    }
  else {
      for ( size_t i = 0; i < num_bins; ++i ) {
          if ( counts[i] > 0 ) delete [] unpacked_values[i];
        }
      delete [] unpacked_values;
      delete [] counts;
    }
  return;
}

template <class T> size_t JaggedArray<T>::numElementsInBin ( size_t bin ) const {
  size_t num_e;
  if ( bin >= num_bins || bin < 0 ) {
      std::cerr << "Bad bin indexing" << std::endl;
      num_e = NULL;
    }
  else if ( packed ) {
      if ( bin == num_bins -1 ) {
          num_e = num_elements - offsets [bin];
        }
      else {
          num_e = offsets [ bin + 1 ] - offsets [bin];
        }
    }
  else {
      num_e = counts [bin];
    }
  return num_e;
}

template <class T> const T& JaggedArray<T>::getElement ( size_t bin, size_t sub ) const {
  if ( bin >= num_bins || bin < 0 ) {
      std::cerr << "Bad bin indexing" << std::endl;
      return NULL;
    }
  if ( sub >= this->numElementsInBin( bin ) || sub < 0 ) {
      std::cerr << "Bad subscript indexing" << std::endl;
      return NULL;
    }
  if ( packed ) {
      return packed_values [ offsets[bin] + sub ];
    }
  else {
      return unpacked_values [bin] [sub];
    }
}

template <class T> void JaggedArray<T>::addElement ( size_t bin, const T& o ) {
  if ( packed ) {
      std::cerr << "Jagged Array can only be modified in unpacked mode" << std::endl;
    }
  else if ( bin >= num_bins || bin < 0 ) {
      std::cerr << "Bad bin indexing" << std::endl;
    }
  else {
      num_elements++;
      counts [bin] ++;
      T * oldstuff = unpacked_values [bin];
      unpacked_values [bin] = new T [ counts [bin] ];
      for ( size_t i = 0; i < counts [bin] - 1; ++i ) {
          unpacked_values [bin] [i] = oldstuff [i];
        }
      unpacked_values [bin] [ counts [bin] - 1 ] = o;
      if ( counts [bin] > 1 ) delete [] oldstuff;
    }
  return;
}

template <class T> void JaggedArray<T>::removeElement ( size_t bin, size_t sub ) {
  if ( packed ) {
    std::cerr << "Jagged Array can only be modified in unpacked mode" << std::endl;
  }
  else if ( bin >= num_bins || bin < 0 ) {
    std::cerr << "Bad bin indexing" << std::endl;
  }
  else if ( counts [bin] > sub && counts [bin] > 0 ) {
      size_t i;
      num_elements--;
      counts [bin] --;
      if ( counts [bin] > 0 ) {
          T * oldstuff = unpacked_values [bin];
          unpacked_values [bin] = new T [ counts [bin] ];
          for ( i = 0; i < sub; ++i ) {
              unpacked_values [bin] [i] = oldstuff [i];
            }
          for ( i = sub; i < counts [bin]; ++i ) {
              unpacked_values [bin] [i] = oldstuff [i+1];
            }
          delete [] oldstuff;
        }
      else {
          delete [] unpacked_values [bin];
        }
    }
  return;
}

template <class T> void JaggedArray<T>::pack () {
  if ( ! packed ) {
      packed_values = new T [num_elements];
      offsets = new size_t [num_bins];
      size_t sub = 0;
      for ( size_t i = 0; i < num_bins; ++i ) {
          offsets [i] = sub;
          for ( size_t j = 0; j < counts [i]; ++j ) {
              packed_values [sub] = unpacked_values [i] [j];
              sub++;
            }
          if ( counts [i] > 0 ) delete [] unpacked_values [i];
        }
      packed = true;
      delete [] unpacked_values;
      delete [] counts;
    }
  return;
}

template <class T> void JaggedArray<T>::unpack () {
  if ( packed ) {
      unpacked_values = new T * [num_bins];
      counts = new size_t [num_bins];
      for ( size_t i = 0; i < num_bins; ++i ) {
          counts [i] = this->numElementsInBin(i);
          if ( counts[i] > 0 ) {
              size_t num_sub = counts[i];
              unpacked_values [i] = new T [ num_sub ];
              for ( size_t j = 0; j < num_sub; ++j ) {
                  unpacked_values [i] [j] = packed_values [ offsets [i] + j ];
                }
            }
        }
      packed = false;
      delete [] packed_values;
      delete [] offsets;
    }
  return;
}

template <class T> void JaggedArray<T>::clear () {
  if ( ! packed ) {
      for ( size_t i = 0; i < num_bins; ++i ) {
          if ( counts [i] > 0 ) {
              counts [i] = 0;
              delete [] unpacked_values [i];
            }
        }
      num_elements = 0;
    }
  return;
}


template <class T> void JaggedArray<T>::print () const {

  if ( packed ) std::cout << "packed JaggedArray" << std::endl;
  else std::cout << "unpacked JaggedArray" << std::endl;

  std::cout << "  num_bins: " << num_bins << std::endl;
  std::cout << "  num_elements: " << num_elements << std::endl;
  size_t i;

  if ( packed ) {
      std::cout << "  offsets: ";

      for ( i = 0; i < num_bins; ++i ) {
          std::cout << " " << offsets[i];
        }
      std::cout << std::endl << "  values:  ";
      for ( i = 0; i < num_elements; ++i ) {
          std::cout << " " << packed_values [i];
        }

      std::cout << std::endl << std::endl;
    }
  else {
      std::cout << "  counts: ";
      for ( i = 0; i < num_bins; ++i ) {
          std::cout << " " << counts[i];
        }
      bool e_left = true;
      std::cout << std::endl << "  values: ";
      size_t j = 0;
      while ( e_left ) {
          e_left = false;
          for ( i = 0; i < num_bins; ++i ) {
              if ( numElementsInBin(i) > j ) {
                  e_left = true;
                  std::cout << " " << unpacked_values [i] [j];
                }
              else std::cout << "  ";
            }
          std::cout << std::endl << std::setw ( 10 ) << "";
          ++j;
        }
      if ( j <= 1 ) std::cout << std::endl;
      std::cout << "\r";

    }

  return;
}

#endif // JAGGEDARRAY_H_INCLUDED
