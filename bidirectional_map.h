#ifndef BIDIRECTIONAL_MAP_H_INCLUDED
#define BIDIRECTIONAL_MAP_H_INCLUDED

#include <cassert>
#include <iostream>
#include <iomanip>

template < class A, class B > class bimap_node {

public:
  A value;
  bimap_node <B,A> * mate;
  bimap_node <A,B> * parent;
  bimap_node <A,B> * left;
  bimap_node <A,B> * right;

  bimap_node ( const A & a ) : value(a), mate(NULL), parent(NULL), left(NULL), right(NULL) {}
};

// =========================================
// iterators

template < class A, class B >
class bimap_iterator {
public:
  bimap_iterator () : node_ptr_(NULL) {}
  bimap_iterator ( bimap_node<A,B> * np ) : node_ptr_( np ) {
    ptr_ = std::make_pair ( node_ptr_->value, node_ptr_->mate->value );
  }
  bimap_iterator ( bimap_node<B,A> * np ) : node_ptr_( np->mate ) {
    ptr_ = std::make_pair ( node_ptr_->mate->value, node_ptr_->value );
  }
  bimap_iterator ( const bimap_iterator<A,B> & other ) {
    ptr_ = std::make_pair ( other.ptr_.first, other.ptr_.second );
    node_ptr_ = other.node_ptr_;
  }

  bimap_iterator& operator= ( const bimap_iterator<A,B> & other ) {
    if ( *this == other ) return *this;
    else ptr_ = std::make_pair ( other.ptr_.first, other.ptr_.second );
    node_ptr_ = other.node_ptr_;
    return *this;
  }

  bool operator== ( const bimap_iterator<A,B> & other ) {
    return this->node_ptr_ == other.node_ptr_;
  }
  bool operator!= ( const bimap_iterator<A,B> & other ) {
    return this->node_ptr_ != other.node_ptr_;
  }

  const std::pair<A,B>& operator*() const {
    return ptr_;
  }

  bimap_iterator& operator++ () {
    bimap_node <A,B> * ptr1;
    if ( node_ptr_->right != NULL ) {
        ptr1 = node_ptr_->right;
        while ( ptr1->left != NULL ) ptr1 = ptr1->left;
        node_ptr_=ptr1;
        ptr_ = std::make_pair ( node_ptr_->value, node_ptr_->mate->value );
        return *(this);
      }
    else if ( node_ptr_->parent != NULL ) {
        ptr1 = node_ptr_;
        while ( ptr1->parent->right == ptr1 ) {
            ptr1 = ptr1->parent;
            if ( ptr1->parent == NULL ) {
                node_ptr_ = NULL;
                return *(this);
              }
          }
        node_ptr_ = ptr1->parent;
        //if ( ptr1->right != NULL ) node_ptr_ = ptr1->right;
        //else node_ptr_ = ptr1;
        ptr_ = std::make_pair ( node_ptr_->value, node_ptr_->mate->value );
        return *(this);
      }
    else {
        node_ptr_ = NULL;
        return *(this);
      }
  }

  bimap_iterator operator++ ( int ) {
    bimap_iterator temp = *(this);
    this->operator++();
    return temp;
  }

  bimap_iterator& operator-- () {
    bimap_node <A,B> * ptr1;
    if ( node_ptr_->left != NULL ) {
        ptr1 = node_ptr_->left;
        while ( ptr1->right != NULL ) ptr1 = ptr1->right;
        node_ptr_=ptr1;
        ptr_ = std::make_pair ( node_ptr_->value, node_ptr_->mate->value );
        return *(this);
      }
    else if ( node_ptr_->parent != NULL ) {
        ptr1 = node_ptr_;
        while ( ptr1->parent->left == ptr1 ) {
            ptr1 = ptr1->parent;
            if ( ptr1->parent == NULL ) {
                node_ptr_ = NULL;
                return *(this);
              }
          }
        node_ptr_ = ptr1->parent;
        ptr_ = std::make_pair ( node_ptr_->value, node_ptr_->mate->value );
        return *(this);
      }
    else {
        node_ptr_ = NULL;
        return *(this);
      }
  }

  bimap_iterator operator-- ( int ) {
    bimap_iterator temp = *(this);
    this->operator--();
    return temp;
  }

  bimap_iterator<B,A> follow_link () {
    bimap_iterator <B,A> itr ( this->node_ptr_->mate );
    return itr;
  }
  bimap_node<A,B> * node_ptr_;
protected:
  std::pair<A,B> ptr_;


};


// ========================================================
// main class

template < class A, class B >
class bidirectional_map {

public:
  typedef bimap_iterator <A,B> key_iterator;
  typedef bimap_iterator <B,A> value_iterator;
  //typedef const_bimap_iterator <A,B> const_key_iterator;
  //typedef const_bimap_iterator <B,A> const_value_iterator;
  typedef bimap_node <A,B> key_node;
  typedef bimap_node <B,A> value_node;

  // creators/destructor
  bidirectional_map () {
    this->create();
  }
  bidirectional_map ( const bidirectional_map & other ) {
    this->copy(other);
  }
  bidirectional_map & operator= ( const bidirectional_map & other ) {
    this->copy( other );
  }
  ~bidirectional_map () {
    this->destruct();
  }

  // accessors
  size_t size() const {
    //size_t foo = size__;
    return size__;
  }
  //
  key_iterator key_begin () {
    return key_iterator ( key_head );
  }
  key_iterator key_end () {
    return key_iterator ();
  }
  value_iterator value_begin () {
    return value_iterator ( value_head );
  }
  value_iterator value_end () {
    return value_iterator ();
  }
  key_iterator find ( const A& key ) {
    key_node* ptr = find_key ( key_root, key );
    if ( ptr == NULL ) return key_iterator ();
    else return key_iterator ( ptr );
  }
  //
  value_iterator find ( const B& value ) {
    value_node * ptr = find_value ( value_root, value );
    if ( ptr == NULL ) return value_iterator ();
    else return value_iterator ( ptr );
  }
  //
  const B& operator[] ( const A& key ) {
    key_node * ptr = find_key ( key_root, key );
    assert ( ptr != NULL );
    //B foo = ptr->mate->value;
    return ptr->mate->value;
  }
  const A& operator[] ( const B& value ) {
    value_node * ptr = find_value ( value_root, value );
    assert ( ptr != NULL );
    A foo = ptr->mate->value;
    return ptr->mate->value;
  }
  void print ( std::ostream& fout ) {
    print_key( key_root, 0, fout );
  }


  // editors
  std::pair <key_iterator, bool> insert ( const std::pair <A,B> & new_pair ) {
    if ( this->key_root == NULL ) {
        this->key_root = new key_node ( new_pair.first );
        this->value_root = new value_node ( new_pair.second );
        this->key_root->mate = this->value_root;
        this->value_root->mate = this->key_root;
        key_iterator itr ( key_root );
        key_head = key_root;
        value_head = value_root;
        size__++;
        return std::make_pair(itr, true);
      }
    else {
        key_node * ptr1 = this->key_root, * ptr2;
        value_node * ptr3 = this->value_root, * ptr4;
        while ( ptr1 != NULL ) {
            ptr2 = ptr1;
            if ( ptr1->value < new_pair.first ) ptr1 = ptr1->right;
            else if ( ptr1->value > new_pair.first ) ptr1 = ptr1->left;
            else {
                key_iterator itr ( ptr1 );
                return std::make_pair(itr,false);
              }
          }
        while ( ptr3 != NULL ) {
            ptr4 = ptr3;
            if ( ptr3->value < new_pair.second ) ptr3 = ptr3->right;
            else if ( ptr3->value > new_pair.second ) ptr3 = ptr3->left;
            else {
                key_iterator itr ( ptr3->mate );
                return std::make_pair(itr,false);
              }
          }

        if ( ptr2->value < new_pair.first ) {
            ptr2->right = new key_node ( new_pair.first );
            ptr1 = ptr2->right;
          }
        else if ( ptr2->value > new_pair.first ) {
            ptr2->left = new key_node ( new_pair.first );
            ptr1 = ptr2->left;
          }

        if ( ptr4->value < new_pair.second ) {
            ptr4->right = new value_node ( new_pair.second );
            ptr3 = ptr4->right;
          }
        else if ( ptr4->value > new_pair.second ) {
            ptr4->left = new value_node ( new_pair.second );
            ptr3 = ptr4->left;
          }
        ptr1->parent = ptr2;
        ptr3->parent = ptr4;
        // now hook up the mates
        ptr1->mate = ptr3;
        ptr3->mate = ptr1;
        if ( ptr1->value < key_head->value ) key_head = ptr1;
        if ( ptr3->value < value_head->value ) value_head = ptr3;
        size__++;
        key_iterator itr ( ptr1 );
        return std::make_pair( itr, true );
      }
  }
  // the big one!
  size_t erase ( const A& key ) {
    key_node * ptr = find_key( key_root, key );
    if ( ptr == NULL ) return 0;
    else if ( key_root == ptr && value_root == ptr->mate ) erase_root_key_value();
    else if ( key_root == ptr ) erase_root_key ();
    else if ( value_root == ptr->mate ) erase_root_value();
    else {
        // deal with the keys first
        key_node * parent = ptr->parent, *left = ptr->left, *right = ptr->right;
        value_node * v_ptr = ptr->mate;
        if ( left == NULL ) {
            if ( parent->left == ptr ) parent->left = right;
            else parent->right = right;
            if ( right != NULL ) right->parent = parent;
          }
        else if ( right == NULL ) {
            if ( parent->left == ptr ) parent->left = left;
            else parent->right = left;
            left->parent = parent;
          }
        else {
            key_iterator temp( ptr );
            --temp;
            key_node* lptr = temp.node_ptr_;
            if ( lptr->parent != ptr ) {
                if ( lptr->left != NULL ) {
                    lptr->parent->right = lptr->left;
                    lptr->right->parent = lptr->parent;
                  }
                lptr->left = left;
                left->parent = lptr;
                if ( lptr->parent->left == lptr ) lptr->parent->left = NULL;
                else if ( lptr->parent->right == lptr ) lptr->parent->right = NULL;
              }
            lptr->right = right;
            right->parent = lptr;
            lptr->parent = parent;
            if ( parent->right == ptr ) parent->right = lptr;
            else parent->left = lptr;
          }

        // now do the values
        value_node * v_parent = v_ptr->parent, *v_left = v_ptr->left, *v_right = v_ptr->right;
        if ( v_left == NULL ) {
            if ( v_parent->left == v_ptr ) v_parent->left = v_right;
            else v_parent->right = v_right;
            if ( v_right != NULL ) v_right->parent = v_parent;
          }
        else if ( v_right == NULL ) {
            if ( v_parent->left == v_ptr ) v_parent->left = v_left;
            else v_parent->right = v_left;
            v_left->parent = v_parent;
          }
        else {
            value_iterator v_temp( v_ptr );
            --v_temp;
            value_node* vlptr = v_temp.node_ptr_;
            if ( vlptr->parent != v_ptr ) {
                if ( vlptr->left != NULL ) {
                    vlptr->parent->right = vlptr->left;
                    vlptr->right->parent = vlptr->parent;
                  }
                vlptr->left = v_left;
                v_left->parent = vlptr;
                if ( vlptr->parent->left == vlptr ) vlptr->parent->left = NULL;
                else if ( vlptr->parent->right == vlptr ) vlptr->parent->right = NULL;
              }
            vlptr->right = v_right;
            v_right->parent = vlptr;
            vlptr->parent = v_parent;
            if ( v_parent->right == v_ptr ) v_parent->right = vlptr;
            else v_parent->left = vlptr;
          }
        // don't need these any more
        delete ptr;
        delete v_ptr;
      }
    size__--;
    return 1;
  }

private:
  size_t size__;
  key_node * key_root;
  key_node * key_head;
  value_node * value_root;
  value_node * value_head;

  void create ();
  void copy( const bidirectional_map<A,B> & other );
  void copy_key( key_node * child, const key_node * neighbor );
  void copy_value( value_node * child, const value_node * neighbor );
  void destruct();
  void destroy_key( key_node * child ) {
    if ( child->left != NULL ) {
        destroy_key( child->left );
      }
    if ( child->right != NULL ) {
        destroy_key( child->right );
      }
    delete child->mate;
    delete child;
  }
  //
  key_node * find_key ( key_node * ptr, const A& key ) {
    if ( ptr != NULL ) {
        A foo = ptr->value;
        if ( ptr->value > key ) {
            return find_key ( ptr->left, key );
          }
        else if ( ptr->value < key ) {
            return find_key ( ptr->right, key );
          }
        else {
            return ptr;
          }
      }
    return NULL;
  }
  //
  value_node * find_value ( value_node * ptr, const B& value ) {
    if ( ptr != NULL ) {
        if ( ptr->value > value ) {
            return find_value ( ptr->left, value );
          }
        else if ( ptr->value < value ) {
            return find_value ( ptr->right, value );
          }
        else {
            return ptr;
          }
      }
    return NULL;
  }
  //
  void print_key ( key_node * k, int i, std::ostream& fout ) {
    if ( k == NULL ) return;
    print_key ( k->left, i + 1, fout );
    fout << std::setw( i*2 ) << "" << k->value << ": " << k->mate->value << std::endl;
    print_key ( k->right, i + 1, fout );
  }
  //
  void erase_root_key_value () {
    // deal with the keys first
    key_node * ptr = key_root, *left = key_root->left, *right = key_root->right;
    value_node * v_ptr = value_root;
    assert ( key_root->mate = value_root );
    if ( left == NULL ) {
        key_root = right;
        if ( right != NULL ) right->parent = NULL;
      }
    else if ( right == NULL ) {
        key_root = left;
        left->parent = NULL;
      }
    else {
        key_iterator temp( ptr );
        --temp;
        key_node* lptr = temp.node_ptr_;
        if ( lptr->parent != ptr ) {
            if ( lptr->left != NULL ) {
                lptr->parent->right = lptr->left;
                lptr->right->parent = lptr->parent;
              }
            lptr->left = left;
            left->parent = lptr;
            if ( lptr->parent->left == lptr ) lptr->parent->left = NULL;
            else if ( lptr->parent->right == lptr ) lptr->parent->right = NULL;
          }
        lptr->right = right;
        right->parent = lptr;
        lptr->parent = NULL;
        key_root = lptr;
      }

    // now do the values
    value_node * v_left = v_ptr->left, * v_right = v_ptr->right;
    if ( v_left == NULL ) {
        value_root = v_right;
        if ( v_right != NULL ) v_right->parent = NULL;
      }
    else if ( v_right == NULL ) {
        value_root = v_left;
        v_left->parent = NULL;
      }
    else {
        value_iterator temp( v_ptr );
        --temp;
        value_node* lptr = temp.node_ptr_;
        if ( lptr->parent != v_ptr ) {
            if ( lptr->left != NULL ) {
                lptr->parent->right = lptr->left;
                lptr->right->parent = lptr->parent;
              }
            lptr->left = v_left;
            v_left->parent = lptr;
            if ( lptr->parent->left == lptr ) lptr->parent->left = NULL;
            else if ( lptr->parent->right == lptr ) lptr->parent->right = NULL;
          }
        lptr->right = v_right;
        v_right->parent = lptr;
        lptr->parent = NULL;
        value_root = lptr;
      }
    // don't need these any more
    delete ptr;
    delete v_ptr;
  }
  //
  void erase_root_key () {
    // deal with the keys first
    key_node * ptr = key_root, *left = key_root->left, *right = key_root->right;
    value_node * v_ptr = key_root->mate;
    if ( left == NULL ) {
        key_root = right;
        if ( right != NULL ) right->parent = NULL;
      }
    else if ( right == NULL ) {
        key_root = left;
        left->parent = NULL;
      }
    else {
        key_iterator temp( ptr );
        --temp;
        key_node* lptr = temp.node_ptr_;
        if ( lptr->parent != ptr ) {
            if ( lptr->left != NULL ) {
                lptr->parent->right = lptr->left;
                lptr->right->parent = lptr->parent;
              }
            lptr->left = left;
            left->parent = lptr;
            if ( lptr->parent->left == lptr ) lptr->parent->left = NULL;
            else if ( lptr->parent->right == lptr ) lptr->parent->right = NULL;
          }
        lptr->right = right;
        right->parent = lptr;
        lptr->parent = NULL;
        key_root = lptr;
      }

    // now do the values
    value_node * v_parent = v_ptr->parent, *v_left = v_ptr->left, *v_right = v_ptr->right;
    if ( v_left == NULL ) {
        if ( v_parent->left == v_ptr ) v_parent->left = v_right;
        else v_parent->right = v_right;
        if ( v_right != NULL ) v_right->parent = v_parent;
      }
    else if ( v_right == NULL ) {
        if ( v_parent->left == v_ptr ) v_parent->left = v_left;
        else v_parent->right = v_left;
        v_left->parent = v_parent;
      }
    else {
        value_iterator v_temp( v_ptr );
        --v_temp;
        value_node* vlptr = v_temp.node_ptr_;
        if ( vlptr->parent != v_ptr ) {
            if ( vlptr->left != NULL ) {
                vlptr->parent->right = vlptr->left;
                vlptr->right->parent = vlptr->parent;
              }
            vlptr->left = v_left;
            v_left->parent = vlptr;
            if ( vlptr->parent->left == vlptr ) vlptr->parent->left = NULL;
            else if ( vlptr->parent->right == vlptr ) vlptr->parent->right = NULL;
          }
        vlptr->right = v_right;
        v_right->parent = vlptr;
        vlptr->parent = v_parent;
        if ( v_parent->right == v_ptr ) v_parent->right = vlptr;
        else v_parent->left = vlptr;
      }
    // don't need these any more
    delete ptr;
    delete v_ptr;
  }
  //
  void erase_root_value () {
    // deal with the keys first
    key_node * ptr = value_root->mate, * parent = value_root->mate->parent,
               * left = value_root->mate->left, *right = value_root->mate->right;
    value_node * v_ptr = value_root;
    if ( left == NULL ) {
        if ( parent->left == ptr ) parent->left = right;
        else parent->right = right;
        if ( right != NULL ) right->parent = parent;
      }
    else if ( right == NULL ) {
        if ( parent->left == ptr ) parent->left = left;
        else parent->right = left;
        left->parent = parent;
      }
    else {
        key_iterator temp( ptr );
        --temp;
        key_node* lptr = temp.node_ptr_;
        if ( lptr->parent != ptr ) {
            if ( lptr->left != NULL ) {
                lptr->parent->right = lptr->left;
                lptr->right->parent = lptr->parent;
              }
            lptr->left = left;
            left->parent = lptr;
            if ( lptr->parent->left == lptr ) lptr->parent->left = NULL;
            else if ( lptr->parent->right == lptr ) lptr->parent->right = NULL;
          }
        lptr->right = right;
        right->parent = lptr;
        lptr->parent = parent;
        if ( parent->right == ptr ) parent->right = lptr;
        else parent->left = lptr;

      }

    // now do the values
    value_node * v_left = v_ptr->left, * v_right = v_ptr->right;
    if ( v_left == NULL ) {
        value_root = v_right;
        if ( v_right != NULL ) v_right->parent = NULL;
      }
    else if ( v_right == NULL ) {
        value_root = v_left;
        v_left->parent = NULL;
      }
    else {
        value_iterator temp( v_ptr );
        --temp;
        value_node* lptr = temp.node_ptr_;
        if ( lptr->parent != v_ptr ) {
            if ( lptr->left != NULL ) {
                lptr->parent->right = lptr->left;
                lptr->right->parent = lptr->parent;
              }
            lptr->left = v_left;
            v_left->parent = lptr;
            if ( lptr->parent->left == lptr ) lptr->parent->left = NULL;
            else if ( lptr->parent->right == lptr ) lptr->parent->right = NULL;
          }
        lptr->right = v_right;
        v_right->parent = lptr;
        lptr->parent = NULL;
        value_root = lptr;
      }
    // don't need these any more
    delete ptr;
    delete v_ptr;
  }
};



template <class A, class B>
void bidirectional_map<A,B>::create() {
  size__ = 0;
  key_root = NULL;
}

template <class A, class B>
void bidirectional_map<A,B>::copy( const bidirectional_map<A,B> & other ) {
  if ( other == *this ) return;
  this->destruct();
  if ( other.key_root != NULL ) {
      this->key_root = new bimap_node<A,B> ( other.key_root->value );
      this->value_root = new bimap_node<B,A> ( other.value_root->value );
      if ( other.key_root->left != NULL ) {
          this->key_root->left = new bimap_node<A,B> ( other.key_root->left->value );
          this->key_root->left->parent = this->key_root;
          copy_key( this->key_root->left, other.key_root->left );
        }
      if ( other.key_root->right != NULL ) {
          this->key_root->right = new bimap_node<A,B> ( other.key_root->right->value );
          this->key_root->right->parent = this->key_root;
          copy_key( this->key_root->right, other.key_root->right );
        }

      if ( other.value_root->left != NULL ) {
          this->value_root->left = new bimap_node<B,A> ( other.value_root->left->value );
          this->value_root->left->parent = this->value_root;
          this->value_root->left->mate = find_key( key_root, other.value_root->left->mate->value );
          this->value_root->left->mate->mate = this->value_root->left;
          copy_value( this->value_root->left, other.value_root->left );
        }
      if ( other.value_root->right != NULL ) {
          this->value_root->right = new bimap_node<B,A> ( other.value_root->right->value );
          this->value_root->right->parent = this->value_root;
          this->value_root->right->mate = find_key( key_root, other.value_root->right->mate->value );
          this->value_root->right->mate->mate = this->value_root->right;
          copy_value( this->value_root->right, other.value_root->right );
        }

      this->key_head = find_key ( this->key_root, other.key_head->value );
      assert ( this->key_head != NULL );

    }

}

template <class A, class B>
void bidirectional_map<A,B>::copy_key( bimap_node <A,B> * child, const bimap_node <A,B> * neighbor ) {
  if ( neighbor->left != NULL ) {
      child->left = new bimap_node <A,B> ( neighbor->left->value );
      child->left->parent = child;
      copy_key( child->left, neighbor->left );
    }
  if ( neighbor->right != NULL ) {
      child->right = new bimap_node <A,B> ( neighbor->right->value );
      child->right->parent = child;
      copy_key( child->right, neighbor->right );
    }
}

template <class A, class B>
void bidirectional_map<A,B>::copy_value( bimap_node <B,A> * child, const bimap_node <B,A> * neighbor ) {
  if ( neighbor->left != NULL ) {
      child->left = new bimap_node <B,A> ( neighbor->left->value );
      child->left->parent = child;
      child->left->mate = find_key( key_root, neighbor->left->mate->value );
      child->left->mate->mate = child->left;
      copy_value( child->left, neighbor->left );
    }
  if ( neighbor->right != NULL ) {
      child->right = new bimap_node <B,A> ( neighbor->right->value );
      child->right->parent = child;
      child->right->mate = find_key( key_root, neighbor->right->mate->value );
      child->right->mate->mate = child->right;
      copy_value( child->right, neighbor->right );
    }
}


template<class A, class B>
void bidirectional_map<A,B>::destruct () {
  if ( key_root != NULL ) {
      if ( key_root->left != NULL ) destroy_key ( key_root->left );
      if ( key_root->right != NULL ) destroy_key ( key_root->right );
      delete key_root->mate;
      delete key_root;
      key_root = NULL;
      key_head = NULL;
      value_root = NULL;
      value_head = NULL;
      size__ = 0;
    }
}


#endif // BIDIRECTIONAL_MAP_H_INCLUDED
