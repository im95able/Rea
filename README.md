# Rea
Rea is a lightweight library of data structures implemented in C++11, designed for constant time insertion, erasure, lookup, and fastest possible iteration. Great for using in games or any other software which needs to manage thousands upon thousands of objects.

There are 6 data structures included in this library : `slot_list`, `controlled_slot_list`, `versioned_slot_list`, `regulated_slot_list`, `slot_map` and `versioned_slot_map`. The two key data structures are `slot_list` and `list_map`: the other four are only simple variations of those 2. 

# SlotList 
Use SlotList when you have to insert, erase, or look up data in constant time, without the need for constantly repeated iteration. If you require all of those things plus fast iteration, use SlotMap.

## Implementation
SlotList internally stores its objects in some RandomAccessContainer (by default `std::deque`). Once you erase an object from SlotList, the slot where that object used to reside becomes available for reuse. The next object you insert will be put in the last emptied slot.
The internal container will never grow unless all slots are filled. The "Discussion" section shows how to change the 
internal container from `std::deque` to some other container.

Whenever you insert a value into the SlotList you get its id, which you can later use to access that object. "id" is an index with or without a version count in case of the version variatons of SlotLists and DenseMaps. More on that later on. That index is what allows us to access the objects in constant time.

While you can use the ids to iterate over all stored objects, it's not advisable to do it repeatedly: you might be jumping all over memory and hence destroying cache locality. As stated above, for iteration use SlotMap.

## Usage
All SlotLists have the same first, and the last 2 template arguments.
```cpp
some_slot_list<T,                      // value_type 
               .
               .
               .
               S = std::size_t,        // size_type 
               A = std::allocator<T>>; // allocator_type 
```
Considering that each slot will store two objects of "size_type" type as well as the single object of "value_type", if you know in advance the maximum size of the container, you might want to restrict how much space slots take up by choosing a smaller "size_type" which can still be used to reference all objects in the container.

Template arguments for all SlotList variations are explained below.

### variation 1 : slot_list
`rea::slot_list` acts as a basic SlotList. It has no additional template arguments, only the ones described above.
```cpp
rea::slot_list<T,                      // value_type
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
   using sl_type = rea::slot_list<std::pair<int, int>>;
   sl_type sl;
   std::vector<sm_type::id_type> ids;
   ids.reserve(1000);
   sl.reserve(1000);
   
   // insertion
   for(size_t i = 0; i < 1000; ++i) {
      ids.push_back(sl.insert({i, i*2}));
   }
   
   // erasure
   for(size_t i = 0; i < 100; ++i) {
      auto id = get_and_remove_rand_value(ids);
      sl.erase(id);
   }
   
   // lookup
   for(auto id : ids) 
      std::cout << sl.id_value(id).first << std::endl;
   
   
   // iteration
   for(auto id = sl.id_begin(); !sl.id_is_end(id); id = sl.id_next(id)) 
      std::cout << sl.id_value(id).first << std::endl;
}
```


### variation 2 : controlled_slot_list
Objects which you erase are not destroyed, only "marked" as empty, so they can be reassigned to in the future. If you are storing objects which themselves are holding some resources (e.g. a pointer to some allocated memory) this could be problematic, beacuse you won't be able to release that memory until the entire SlotMap is destroyed or another value is assigned to that object. 

To solve that issue, `rea::controlled_slot_list` is introduced. Its second template argument is a functor, which returns a value to be assigned to all empty slots (it's defaulted to a `rea::get_empty` functor which returns a default constructed object).

```cpp
rea::controlled_slot_list<T,                      // value_type
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

rea::controlled_slot_list<double, set_empty_double, std::unit16_t> c_sm(set_empty_double{3.14159});
```



### variation 3 : versioned_slot_list
If, for instance, you are erasing objects inside the SlotMap from 2 different parts of your program, ids might no longer
point to correct objects, but to either erased or objects filled with different values than what the id originally pointed to.

To solve that issue, `rea::versioned_slot_list` is introduced. It takes as a second template argument an IntegralType, which will represent the current version of the slot (default is `std::size_t`). Each time an object is erased, the slot which contains that object increases its version count by 1.
```cpp
rea::versioned_slot_list<T,                      // value_type
                         V = std::size_t,        // version_type
                         S = std::size_t,        // size_type
                         A = std::allocator<T>>  // allocator_type
```
Here slots also store a "version_type". Choosing different "version_type" may effect size of the slot, and hence the amount of memory needed.

```cpp
#include <rea.h>
#include <vector>

int main() {
   using sl_type = rea::versioned_slot_list<int>;
   sl_type sl;
   std::vector<sl_type::id_type> ids;
   ids.reserve(1000);
   sl.reserve(1000);
   
    for(int i = 0; i < 1000; ++i) {
      ids.push_back(sl.insert(i));
   }
   
   auto some_random_valid_id = ids[36];
   
   bool is_valid = sl.id_is_valid(some_random_valid_id);
   // is_valid == true;
   sl.erase(some_random_valid_id);
   bool is_valid_now = sl.id_is_valid(some_random_valid_id);
   // is_valid_now == false;
   
}
```
 `rea::slot_list` and `rea::controlled_slot_list` also have `id_is_valid(id_type id)` method defined.
 It's a constexpr function which always returns true.



### variation 4 : regulated_slot_list 
If you need both the ability to set an empty value and to keep a version count, use `rea::regulated_slot_list`. Its second template argument is functor which returns an empty value, and its third is an IntegralType to be used for versioning. They are defaulted same as `rea::controlled_slot_list` and `rea::versiond_slot_list`.

```cpp
rea::versioned_slot_map<T,                      // value_type
                        E, = rea::get_empty<T>  // get_empty_type
                        V = std::size_t,        // version_type
                        S = std::size_t,        // size_type
                        A = std::allocator<T>>  // allocator_type
```
Here, slots store same things as in the `rea::version_slot_list`.

```cpp
struct get_empty_string {
   std::string operator()() const {
      return std::string{};
   }
}

rea::regulated_slot_list<std::string, get_empty_string> sm_strings;
```


# SlotMap
If like in the SlotList you need constant time insertion, removal, and lookup, as well as cache friendly iteration through a contiguous array, use SlotMap.

Implementation details are given below, although there is a [video](https://www.youtube.com/watch?v=SHaAR7XPtNU) which explains exactly what this data structure is. If you've seen it, "Implementation" section won't give you any more details and could be skipped.

## Implementation
SlotMap is internally implemented as 2 std::vectors and a slot_list like data structure.
- vector 1 = *ValueContainer*;
- vector 2 = *IDPosContainer*;
- slot_list = *IDSlotContainer*;

*ValueContainer* stores objects of type "value_type" of the SlotMap, and just like any other vector they are stored contiguously. The *IDSlotContainer* stores indices which point to an object inside the *ValueContainer*. Once an object is erased, last object inside the *ValueContainer* is moved into its place, hence all objects remain densely packed at the cost of not preserving order. Slot of the *IDSlotContainer* which points to the erased object becomes available for reuse.

Now we have a problem though. The slot which pointed to the last object inside *ValueContainer* now points to past the end object. In order to find that slot and update it to point to a new location, we introduce the *IDPosContainer*.

*IDPosContainer* stores indices of *IDSlotContainer* slots, which correspond to objects stored *ValueContainer*. E.g., third object of *IDPosContainer* is an index of an *IDSlotContainer* slot, and that slot points to the third object of *ValueContainer*. Once past the end object is moved to the erased location, its index inside *IDSlotContainer* is also moved to the corresponding location of *IDSlotContainer*. In that way all lookup operations are done in constant time.

## Usage
As stated earlier the main difference between the SlotList and the SlotMap is in iteration. It's not possible to iterate through the objects stored in SlotMap using their ids. IDs can only be used for lookup. For iteration regular RandomAccess iterators are used (by default `std::vector::iterator`, as with SlotList you can change the internal containers, "Discussion" section shows how to do that). 

Considering all of the users objects are kept in a contiguous array, and all erased objects are destructed, there is no need for controlled or regulated version of SlotMap.

All SlotMaps, just like all SlotLists, have the same first, and the last 2 template arguments.
```cpp
some_slot_map<T,                      // value_type 
              .
              .
              .
              S = std::size_t,        // size_type 
              A = std::allocator<T>>; // allocator_type 
```

### variation 1 : slot_map 
`rea::slot_map` acts as a basic SlotMap. It has no additional template arguments, only the ones described above.
```cpp
rea::slot_map<T,                      // value_type
              S = std::size_t,        // size_type
              A = std::allocator<T>>  // allocator_type
```

```cpp
#include <rea.h>
#include <vector>
#include <iostream>

int main() {
   using sm_type = rea::slot_map<int>;
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
      auto id = get_and_remove_rand_value(ids); //this function is defined in slot map code above
      sm.erase(id);
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
### variation 2 : versioned_slot_map
`rea::versioned_slot_map` is to `rea::slot_map`, what `rea::versioned_slot_list` is to `rea::slot_list`. It keeps a version count for you. And just like for `rea::versioned_slot_list`, `rea::versioned_slot_map::id_is_valid(id_type)` method may return false if the version counts dont match. 
```cpp
rea::versioned_slot_map<T,                      // value_type
                        V = std::size_t,        // version_type
                        S = std::size_t,        // size_type
                        A = std::allocator<T>>  // allocator_type
```
# Installation
Include "rea.h" header file in your project, and you're ready to go. It should work with any C++11 compliant compiler.

# Discussion
Discussion section will be added shortly in the future.

