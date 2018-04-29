# Rea
Rea is a library of data structures implemented in C++11, designed for constant time inserton, erasure, lookup, and fastest possible iteration. Great for using in games or any other software which needs to manage thousands upon thousands of objects.

There are 6 data structures included in this library : slot_map, controlled_slot_map, versioned_slot_map, regulated_slot_map, dense_map 
and versioned_dense_map. Two main versions are slot_map and dense_map since they are the most different of the bunch, while the others are only simple variations of those 2. 

# SlotMap 
Use SlotMap when you have to insert, erase, or lookup data in constant time, without the need for constantly repeated iteration. If you require all of those things plus fast iteratorion, use DenseMap.

## Implementation
SlotMap internally stores its objects in an std::deque. Once you erase an object from SlotMap, the slot where that object used to reside becomes available for reuse. The next object you insert will be put in the last empitied slot.
The internal Random Access Container will never grow unless all slots are filled. 

Whenever you insert a value into the SlotMap you get its id, which you can later use to access that object. 

While you can use the ids to iterate over all stored object, it's not advised to do it repeatedly. You might be jumping all over memory and hence destroying cache locality. As stated above, for iteration use DenseMap.

## Usage
All SlotMaps have the same first, and the last 2 template arguments.
```cpp
some_slot_map<T,                      // value_type 
              .
              .
              .
              S = std::size_t,        // size_type 
              A = std::allocator<T>>; // allocator_type 
```
Considering that each slot will store 2 objects of "size_type" type besides the 1 object of "value_type", knowing in advance that the size of the container will never outgrow max value of the given "size_type", user might want to restrict how much space slots they take up.
Template arguments of all SlotMap variations will be subsequently explained.

### variation 1 : slot_map
rea::slot_map acts as a basic SlotMap. It has no additional template arguments, only the ones desribed above.
```cpp
rea::slot_map<T,                      // value_type
              S = std::size_t,        // size_type
              A = std::allocator<T>>  // allocator_type
```

```cpp
#include <rea.h>
#include <vector>
#include <iostream>
#include <random>

static std::default_random_engine generator;

inline
int rand_uint(std::size_t min, std::size_t max) {
  std::uniform_int_distribution<std::size_t> distribution(min, max);
  return distribution(generator);
}

template<typename T>
inline 
T get_and_remove_rand_value(std::vector<T> &values) {
    auto index = rand_uint(0u, values.size() - 1);
    auto value = values[index];
    values[index] = values.back();
    values.pop_back();
    return value;
}

int main() {
   using sm_type = rea::slot_map<std::pair<int, int>>;
   sm_type sm;
   std::vector<sm_type::id_type> ids;
   ids.reserve(1000);
   sm.reserve(1000);
   
   // insertion
   for(size_t i = 0; i < 1000; ++i) {
      ids.push_back(sm.insert({i, i*2}));
   }
   
   // erasure
   for(size_t i = 0; i < 100; ++i) {
      auto id = get_and_remove_rand_value(ids);
      sm.erase(id);
   }
   
   // lookup
   for(auto id : ids) {
      if(sm.id_is_valid(id)) {
         std::cout << sm.id_value(id).first << std::endl;
      }
   }
   
   // iteration
   for(auto id = sm.id_begin(); !sm.id_is_end(id); id = sm.id_next(id)) 
      std::cout << sm.id_value(id).first << std::endl;
}
```


### variation 2 : controlled_slot_map
Objects which you erase are not destructed, only "marked" as empty, so they can be reassigned to in the future. If you are storing objects which themselves are holding some resources(e.g. pointer to some allocated memory) this could be problematic, beacuse you won't be able to relase that memory until the entire SlotMap is destructed or another value is assigned to that object. 

To solve that issue controlled_slot_map is introduced. Its second template argument is a functor which returns a value to be assigned to all empty slots(it's defaulted to a "rea::get_empty" functor which returns default constructed object).

```cpp
rea::controlled_slot_map<T,                     // value_type
                         E = rea::get_empty<T>,  // get_empty_type
                         S = std::size_t,        // size_type
                         A = std::allocator<T>>  // allocator_type
```

```cpp
struct set_empty_double() {
   set_empty_double(double value) : value(value) {}
   double operator()() const {
      return value;
   }
   double value;
}

rea::controlled_slot_map<double, set_empty_double, std::unit16_t> c_sm(set_empty_double{3.14159});
```



### variation 3 : versioned_slot_map
If, for instance, you are erasing objects inside the SlotMap from 2 different parts of your program, ids might no longer
point to correct objects, but to either erased or objects filled with different values than what the id originally pointed to.

To solve that issue versioned_slot_map is introduced. It takes as a second template argument an IntegralType, which will represent the current vesrion of the slot(it's defaulted to std::size_t). Each time an object is erased, slot which contains that object increases its version count by 1.
```cpp
rea::versioned_slot_map<T,                      // value_type
                        V = std::size_t,        // version_type
                        S = std::size_t,        // size_type
                        A = std::allocator<T>>  // allocator_type
```
Here slots also store a "version_type". Choosing different "version_type" may effect size of the slot, and hence the amount of memory needed.

```cpp
#include <rea.h>
#include <vector>

int main() {
   using sm_type = rea::versioned_slot_map<int>;
   sm_type sm;
   std::vector<sm_type::id_type> ids;
   ids.reserve(1000);
   sm.reserve(1000);
   
    for(int i = 0; i < 1000; ++i) {
      ids.push_back(sm.insert(i));
   }
   
   auto some_random_valid_id = ids[36];
   
   bool is_valid = sm.id_is_valid(some_random_valid_id);
   // is_valid == true;
   sm.erase(some_random_valid_id);
   bool is_valid_now = sm.id_is_valid(some_random_valid_id);
   // is_valid_now == false;
   
}
```
 slot_map and controlled_slot_map also have id_is_valid(id_type id) method defined.
 It's a constexpr function which always returns true.



### variation 4 : regulated_slot_map 
If you need both the ability to set an empty value and to keep a version count, use regulated_slot_map. It's template second template 
is functor which returns an empty value, and its third is an IntegralType to be used for versioning. They are defaulted same as controlled_slot_map and versiond_slot_map.

```cpp
rea::versioned_slot_map<T,                      // value_type
                        E, = rea::get_empty<T>  // get_empty_type
                        V = std::size_t,        // version_type
                        S = std::size_t,        // size_type
                        A = std::allocator<T>>  // allocator_type
```
Here, slots store same things as in the version_slot_map.

```cpp
struct get_empty_string {
   std::string operator()() const {
      return std::string{};
   }
}

rea::regulated_slot_map<std::string, get_empty_string> sm_strings;
```


# DenseMap
If like in the SlotMap you need constant time insertion, removal, and lookup, as well as cache firendly iteration through a contiguous array, use DenseMap.

Implementation details are given bellow, although there is a video which explains exactly what this data structure is. In [the video](https://www.youtube.com/watch?v=SHaAR7XPtNU) this data structure is called SlotMap not DenseMap. If you've seen it, "Implementation" section won't give you any more details and could be skipped.

## Implementation
DenseMap is internally implemented as 2 std::vectors and a slot_map like data structure.
- vector 1 = ValueContainer;
- vector 2 = IDPosContainer;
- slot_map = IDSlotContainer;

ValueContainer stores objects of type "value_type" of the DenseMap, and just like any other vector they are stored contiguously. The IDSlotContainer stores indices which point to an object inside the ValueContainer. Once an object is erased, last object inside the ValueContainer is moved into its place, hence all objects remain densely packed at the cost of not preserving order. Slot of the IDSlotContainer which points to the erased object becomes available for reuse.

Now we have a problem though. The slot which pointed to the last object inside ValueContainer now points to past the end object. In order to find that slot and update it to point to a new location, we introduce the IDPosContainer.

IDPosContainer stores indices of IDSlotContainer slots, which correspond to objects stored ValueContainer. E.g., third object of IDPosContainer is an index of aa IDSlotContainer slot, which corresponds to the third object od ValueContainer. Once the past the end object is moved to the erased location, its index is also moved to the corresponding location of IDPosContainer. In that way all lookup operations are done in constant time.

## Usage
As stated earlier the main difference between the SlotMan and the DenseMap is in iteration. It's not possible to iterate through the objects stored in DenseMap using their ids. IDs can only be used for lookup. For iteration regular RandomAccess iterators are used(by default std::vector::iterator, "Discussion" section shows how to change internal containers for all library maps). 

Considering all of the users objects are kept in contigious array, and all erased objects are gone for real, there is no need for controlled or regulated version of DenseMap.

All DenseMaps, just like all SlotMaps, have the same first, and the last 2 template arguments.
```cpp
some_dense_map<T,                      // value_type 
               .
               .
               .
               S = std::size_t,        // size_type 
               A = std::allocator<T>>; // allocator_type 
```

### variation 1 : dense_map 
rea::dense_map acts as a basic DenseMap. It has no additional template arguments, only the ones desribed above.
```cpp
rea::dense_map<T,                      // value_type
               S = std::size_t,        // size_type
               A = std::allocator<T>>  // allocator_type
```

```cpp
#include <rea.h>
#include <vector>
#include <iostream>

int main() {
   using dm_type = rea::dense_map<int>;
   using dm;
   std::vector<dm_type::id_type> ids;
   ids.reserve(1000);
   sm.reserve(1000);
   
   // insertion
   for(size_t i = 0; i < 1000; ++i) {
      dm.push_back(sm.insert({i, i*2}));
   }
   
   // erasure
   for(size_t i = 0; i < 100; ++i) {
      auto id = get_and_remove_rand_value(ids); //this function is defined in slot map code above
      dm.erase(id);
   }
   
   // lookup
   for(auto id : ids) {
      std::cout << sm.id_value(id).first << std::endl; 
      // Or
      // std::cout << *sm.iterator_of_id(id) << std::endl; 
   }      
   
   ids.clear();
   // iteration is exaclty the same as any std::container
   for(auto it = sm.begin(); it != sm.end(); ++it) { 
      std::cout << *it << std::endl;
      ids.push_back(sm.id_of_iterator(it));
   }
   // "ids" are know in value iteration order
}
```
### variation 2 : versioned_dense_map
rea::versioned_dense_map is to rea::dense_map, what rea::versioned_slot_map is to rea::slot_map. It keeps a version count for you.
And just like for rea::versioned_slot_map, its "id_is_valid(id_type)" method may return false if the version counts dont match. 
```cpp
rea::versioned_dense_map<T,                      // value_type
                         V = std::size_t,        // version_type
                         S = std::size_t,        // size_type
                         A = std::allocator<T>>  // allocator_type
```
# Installation
Include "rea.h" header file in your project, and you're ready to go. It should work with any C++11 compliant compiler.

# Discussion
Discussion section will be added shortly shortly in the future.


