#pragma once

/*
MIT License

Copyright(c)[2018][Igor Mitrovic]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <vector>
#include <deque>
#include <utility>
#include <memory>
#include <algorithm>


namespace rea {

// Utility type functions.
template<typename T>
using SizeType = typename T::size_type;

template<typename T>
using ValueType = typename T::value_type;

template<typename T>
using ContainerType = typename T::container_type;

template<typename T>
using AllocatorType = typename T::allocator_type;

template<typename T>
using VersionType = typename T::version_type;

template<typename T>
using DifferenceType = typename T::difference_type;

template<typename T>
using IteratorDifferenceType = DifferenceType<std::iterator_traits<T>>;

template<typename T>
using IteratorReferenceType = typename std::iterator_traits<T>::reference;

template<typename T>
using IteratorValueType = ValueType<std::iterator_traits<T>>;

template<typename A, typename O>
using AllocatorRebindType = typename std::allocator_traits<A>::template rebind_alloc<O>;

template<typename I>
using SlotSizeType = SizeType<IteratorValueType<I>>;

template<typename I>
using SlotValueType = ValueType<IteratorValueType<I>>;



template<typename I>
// I models integral
constexpr I min_type_value() {
return (std::numeric_limits<I>::min)();
}

template<typename I>
// I models integral
constexpr I max_type_value() {
return (std::numeric_limits<I>::max)();
}

template<typename S>
// S models Integral
struct trivial_slot {
	using value_type = S;
	using size_type = value_type;

	value_type value;

	trivial_slot() = default;
	explicit trivial_slot(value_type v) : value(v) {}
};


template<typename S, typename V>
// S models Integral
// V models Integral
struct versioned_trivial_slot {
	using value_type = S;
	using size_type = value_type;
	using version_type = V;

	value_type value;
	version_type version;

	versioned_trivial_slot() = default;
	explicit versioned_trivial_slot(value_type v) : version(min_type_value<version_type>()), value(v) {}
};



template<typename T, typename S>
// T models SemiRegular
// S models Integral
struct bidirectional_slot {
	using value_type = T;
	using size_type = S;

	size_type prev;
	size_type next;
	value_type value;

	template<typename... Args>
	explicit bidirectional_slot(Args&&... args) : prev(size_type(0)), next(size_type(0)), value(std::forward<Args>(args)...) {}
};


template<typename T, typename S, typename V>
// T models SemiRegular
// S models Integral
// V models Integral
struct versioned_bidirectional_slot {
	using value_type = T;
	using size_type = S;
	using version_type = V;

	size_type prev;
	size_type next;
	version_type version;
	value_type value;

	template<typename... Args>
	explicit versioned_bidirectional_slot(Args&&... args) : prev(size_type(0)), next(size_type(0)), version(min_type_value<version_type>()), value(std::forward<Args>(args)...) {}
};


using default_size_type = std::size_t;

using default_version_type = std::size_t;

template<typename T>
using default_allocator_type = std::allocator<T>;


//**************************************************************************************************
// In case the user wishes to use his own allocators, 
// these type functions tell him what should be their "value_type".
//**************************************************************************************************

// "value_type" of "slot_list::container_type"
template<typename T, typename S>
// T models SemiRegular
// S models Integral
using sl_slot_type = bidirectional_slot<T, S>;

// "value_type" of "versioned_slot_list::container_type"
template<typename T, typename S, typename V>
// T models SemiRegular
// S models Integral
// V models Integral
using sl_versioned_slot_type = versioned_bidirectional_slot<T, S, V>;

// "value_type" of "controlled_slot_list::container_type"
template<typename T, typename S>
// T models SemiRegular
// S models Integral
using sl_controlled_slot_type = sl_slot_type<T, S>;

// "value_type" of "regulated_slot_list::container_type"
template<typename T, typename S, typename V>
// T models SemiRegular
// S models Integral
// V models Integral
using sl_regulated_slot_type = sl_versioned_slot_type<T, S, V>;

// "value_type" of "slot_map::id_slot_container_type"
template<typename S>
// S models Integral
using sm_slot_type = trivial_slot<S>;

// "value_type" of "versioned_slot_map::id_slot_container_type"
template<typename S, typename V>
// S models Integral
// V models Integral
using sm_versioned_slot_type = versioned_trivial_slot<S, V>;



//**************************************************************************************************
// Concept : SlotContainer 
// - Models RandomAccesSequenceContainer
// - Has void resize(size_type) method defined
// - Has iterator begin(), iterator end(), const_iterator cbegin(), const_iterator cend() methods defined   
//**************************************************************************************************

//**************************************************************************************************
// Concept : DenseContainer 
// - Models RandomAccesSequenceContainer
// - Has void resize(size_type) method defined
// - Has void reserve(size_type) method defined
// - Has iterator begin(), iterator end(), const_iterator cbegin(), const_iterator cend(),
//   reverse_iterator rbegin(), reverse_iterator rend(), const_reverse_iterator crbegin(),
//   const_reverse_iterator crend() methods defined
//**************************************************************************************************

//**************************************************************************************************
// These are the global types of the underlying containers which are used in the library.
// They may be changed to some other containers only if they model given concepts.
//**************************************************************************************************

// Container which will be used to store slots of "slot_list"
template<typename N, typename A>
// N models Slot
// A models Allocator
// sl_container_type models SlotContainer
using sl_container_type = std::deque<N, AllocatorRebindType<A, N>>;

// Container which will be used to store values of "slot_map" 
template<typename T, typename A>
// T models SemiRegular
// A models Allocator
// sm_value_container_type models DenseContainer(E.g std::deque can't be use because it doesn't have reserve method)
using sm_value_container_type = std::vector<T, A>;

// Container which will be used to store id slot indices of "slot_map", 
template<typename S, typename A>
// S models Integral
// A models Allocator
// sm_id_pos_container_type models DenseContainer(E.g std::deque can't be use because it doesn't have reserve method)
using sm_id_pos_container_type = std::vector<S, AllocatorRebindType<A, S>>;

// Container which will be used to store id slots of "slot_map", 
template<typename N, typename A>
// N models Slot
// A models Allocator
// sm_id_slot_container_type models SlotContainer
using sm_id_slot_container_type = std::vector<N, AllocatorRebindType<A, N>>;



// Global succesor and predecessor functions and functions objects 
// designed to work with slots.
template<typename N>
// N models ForwardSlot
inline
SizeType<N> get_successor(N& slot) { return slot.next; }

template<typename N>
inline
// N models ForwardSlot
void set_successor(N& slot, SizeType<N> next) { slot.next = next; }

template<typename N>
inline
// N models BidirectionalSlot
SizeType<N> get_predecessor(N& slot) { return slot.prev; }

template<typename N>
inline
// N models BidirectionalSlot
void set_predecessor(N& slot, SizeType<N> prev) { slot.prev = prev; }

template<typename N>
// N models TrivialSlot
inline
SizeType<N> trivial_get_successor(N& slot) { return slot.value; }

template<typename N>
inline
// N models TrivialSlot
void trivial_set_successor(N& slot, SizeType<N> next) { slot.value = next; }



template<typename N>
// N models TrivialSlot 
struct trivial_get_successor_obj {
	SizeType<N> operator()(N& slot) const {
		return slot.value;
	}
};

template<typename N>
// N models TrivialSlot 
struct trivial_set_successor_obj {
	void operator()(N& slot, SizeType<N> next) const {
		slot.value = next;
	}
};

template<typename N>
// N models TrivialSlot 
struct versioned_trivial_set_successor_obj {
	void operator()(N& slot, SizeType<N> next) const {
		++slot.version;
		slot.value = next;
	}
};

template<typename N>
// N models TrivialSlot 
struct versioned_trivial_set_successor_resetter_obj {
	void operator()(N& slot, SizeType<N> next) const {
		slot.version = min_type_value<VersionType<N>>();
		slot.value = next;
	}
};

template<typename N>
// N models ForwardSlot 
struct get_successor_obj {
	SizeType<N> operator()(N& slot) const {
		return slot.next;
	}
};

template<typename N>
// N models ForwardSlot 
struct set_successor_obj {
	void operator()(N& slot, SizeType<N> next) const {
		slot.next = next;
	}
};


template<typename N>
// N models BidirectionalSlot 
struct get_predecessor_obj {
	SizeType<N> operator()(N& slot) const {
		return slot.prev;
	}
};

template<typename N>
// N models BidirectionalSlot
struct set_predecessor_obj {
	void operator()(N& slot, SizeType<N> prev) const {
		slot.prev = prev;
	}
};


template<typename N>
// N models VersionedForwardSlot
struct versioned_set_successor_obj {
	void operator()(N& slot, SizeType<N> next) {
		++slot.version;
		slot.next = next;
	}
};

template<typename N>
// N models VersionedForwardSlot
struct versioned_set_successor_resetter_obj {
	void operator()(N& slot, SizeType<N> next) {
		slot.version = min_type_value<VersionType<N>>();
		slot.next = next;
	}
};

template<typename N>
// N models VersionedSlot
struct resetter_obj {
	void operator()(N& slot) {
		slot.version = min_type_value<VersionType<N>>();
	}
};

template<typename N>
// N models ControlledForwardSlot
struct controlled_set_successor_obj {
	const ValueType<N> *value;

	controlled_set_successor_obj(const ValueType<N> &value) : value(&value) {}

	void operator()(N& slot, SizeType<N> next) {
		slot.next = next;
		slot.value = *value;
	}
};

template<typename N>
// N models VersionedControlledForwardSlot
struct regulated_set_successor_obj {
	const ValueType<N> *value;

	regulated_set_successor_obj(const ValueType<N> &value) : value(&value) {}

	void operator()(N& slot, SizeType<N> next) {
		slot.next = next;
		++slot.version;
		slot.value = *value;
	}
};

template<typename N>
// N models VersionedControlledForwardSlot
struct regulated_set_successor_resetter_obj {
	const ValueType<N> *value;

	regulated_set_successor_resetter_obj(const ValueType<N> &value) : value(&value) {}

	void operator()(N& slot, SizeType<N> next) {
		slot.next = next;
		slot.version = min_type_value<VersionType<N>>();
		slot.value = *value;
	}
};







template<typename I, typename D>
// I models RandomAccessIterator
// D models Integral
inline
I next_iterator(I it, D diff) {
	return it + static_cast<IteratorDifferenceType<I>>(diff);
}

template<typename I, typename D = IteratorDifferenceType<I>>
// I models RandomAccessIterator
// D models Integral
inline
D iterator_distance(I it1, I it2) {
	return static_cast<D>(it2 - it1);
}


template<typename I, typename D>
// I models RandomAccessIterator
// D models Integral
inline
IteratorReferenceType<I> iterator_slot(I it, D diff) {
	return *next_iterator(it, diff);
}

template<typename S>
// N models BidirectionalSlot
struct bidirectional_slot_meta_positions {
	std::pair<S, S> empty;
	std::pair<S, S> filled;
};



// Default object for all controlled versions of "slot_list".
// Returns default constructed object.
template<typename T>
struct get_empty {
	T operator()() { return T{}; }
};

template<typename I>
// I models integral
constexpr bool is_over_breakoff(I capacity, I size) {
	return size > static_cast<I>(capacity * (5.f / 9.f));
}

template<typename I>
// I models integral
constexpr I grow_size(I size) {
	return static_cast<I>(size * 1.5f) + I{ 2 };
}





// Links the slot with "pos.empty" index to doubly linked list of filled slots, and removes it from the singly linked list of empty slots.
// That slot becomes the tail of the filled list. This has to be a different function from bidirectional_link_to_filled because it also needs
// to set "pos.empty.second", in case the "pos.empty.first == pos.empty.second".
template<typename I>
// I models BidirectionalSlot_Iterator
inline
bidirectional_slot_meta_positions<SlotSizeType<I>> bidirectional_link_to_filled(I first, const bidirectional_slot_meta_positions<SlotSizeType<I>> &pos, 
	SlotSizeType<I> npos) {

	auto new_pos = pos;
	const auto slot_pos = pos.empty.first;
	auto &slot = iterator_slot(first, slot_pos);

	if (pos.filled.first == npos) new_pos.filled.first = slot_pos;
	if (pos.filled.second != npos) set_successor(iterator_slot(first, pos.filled.second), slot_pos);

	if (new_pos.empty.first == new_pos.empty.second) {
		new_pos.empty.first = npos;
		new_pos.empty.second = npos;
	}
	else {
		new_pos.empty.first = get_successor(slot);
	}

	set_successor(slot, npos);
	set_predecessor(slot, pos.filled.second);

	new_pos.filled.second = slot_pos;

	return new_pos;
}



// Links the slot with "filled_pos" index to singly linked list of empty slots. and removes it from the doubly linked list of filled slots.
// That slot becomes the head of the empty list. If no other erasures follow this one, it will be used for next insertion.
template<typename I>
// I models BidirectionalSlot_Iterator
inline
bidirectional_slot_meta_positions<SlotSizeType<I>> bidirectional_link_to_empty(I first, const bidirectional_slot_meta_positions<SlotSizeType<I>> &pos,
	SlotSizeType<I> npos, SlotSizeType<I> filled_pos) {

	auto new_pos = pos;
	auto &slot = iterator_slot(first, filled_pos);

	if (filled_pos == pos.filled.first) {
		new_pos.filled.first = get_successor(slot);
		if (new_pos.filled.first != npos)
			set_predecessor(iterator_slot(first, new_pos.filled.first), npos);
		else
			new_pos.filled.second = npos;
	}
	else if (filled_pos == pos.filled.second) {
		new_pos.filled.second = get_predecessor(slot);
		set_successor(iterator_slot(first, new_pos.filled.first), npos);
	}
	else {
		set_predecessor(iterator_slot(first, get_successor(slot)), get_predecessor(slot));
		set_successor(iterator_slot(first, get_predecessor(slot)), get_successor(slot));
	}

	set_successor(slot, pos.empty.first);
	new_pos.empty.first = filled_pos;

	return new_pos;
}





// Links the slot with "filled_pos" index to singly linked list of empty slots. and removes it from the doubly linked list of filled slots.
// That slot, instead of becoming the head of the empty list like in the "bidirectional_link_to_empty", becomes its tail. If no other erasures follow this one,
// and no reallocation happens afterwards, it will be the last slot used for insertion. The reason that this new empty slot is put at tail is beacuse we want 
// to be seldmomly reusing the same slots in order to not increase their version counts often. And as such, guard against version count overflow as much as possible.
template<typename I>
// I models BidirectionalSlot_Iterator
inline
bidirectional_slot_meta_positions<SlotSizeType<I>> versioned_bidirectional_link_to_empty(I first, const bidirectional_slot_meta_positions<SlotSizeType<I>> &pos,
	SlotSizeType<I> npos, SlotSizeType<I> filled_pos) {

	auto new_pos = pos;
	auto &slot = iterator_slot(first, filled_pos);

	if (filled_pos == pos.filled.first) {
		new_pos.filled.first = get_successor(slot);
		if (new_pos.filled.first != npos)
			set_predecessor(iterator_slot(first, new_pos.filled.first), npos);
		else
			new_pos.filled.second = npos;
	}
	else if (filled_pos == pos.filled.second) {
		new_pos.filled.second = get_predecessor(slot);
		set_successor(iterator_slot(first, new_pos.filled.first), npos);
	}
	else {
		set_predecessor(iterator_slot(first, get_successor(slot)), get_predecessor(slot));
		set_successor(iterator_slot(first, get_predecessor(slot)), get_successor(slot));
	}

	set_successor(slot, npos);
	if (new_pos.empty.second != npos) {
		set_successor(iterator_slot(first, new_pos.empty.second), filled_pos);
	}
	else {
		new_pos.empty.first = filled_pos;
	}
	new_pos.empty.second = filled_pos;

	return new_pos;
}



// Empties all slots in memory successive order.
// Does array traversal.
// Word empty is intentonally used instead of link, because in the "set_successor" function object, next to seetting the successor,
// one may do all sorts of things to the slot in order to make it empty.
// E.g. 
// - For version slots, version is inrecemented.
// - For controlled slots, empty value is set.
// - For regulated both version is incremented and empty value is set.
template<typename I, typename S, typename P>
// I models RandomAccessIterator
// S models Integral
// P models BinaryPredicate : void operator()(ValueType<I> &, S);
inline
void forward_empty_all_slots_basis(I first, I last, S start_index, S npos, P set_successor) {
	if (first == last) return;
	--last;
	while (first != last) {
		set_successor(*first, ++start_index);
		++first;
	}
	set_successor(*last, npos);
}

template<typename I>
// I models ForwardSlot_Iterator
inline
void forward_empty_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos) {
	forward_empty_all_slots_basis(first, last, start_index, npos, set_successor_obj<ValueType<I>>{});
}

template<typename I>
// I models ForwardSlot_Iterator
inline
void trivial_forward_empty_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos) {
	forward_empty_all_slots_basis(first, last, start_index, npos, trivial_set_successor_obj<ValueType<I>>{});
}

template<typename I>
// I models ForwardSlot_Iterator
inline
void versioned_trivial_forward_empty_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos) {
	forward_empty_all_slots_basis(first, last, start_index, npos, versioned_trivial_set_successor_obj<ValueType<I>>{});
}

template<typename I>
// I models ForwardSlot_Iterator
inline
void versioned_trivial_forward_empty_and_reset_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos) {
	forward_empty_all_slots_basis(first, last, start_index, npos, versioned_trivial_set_successor_resetter_obj<ValueType<I>>{});
}


template<typename I>
// I models ForwardSlot_Iterator
inline
void controlled_forward_empty_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos, const SlotValueType<I> &value) {
	forward_empty_all_slots_basis(first, last, start_index, npos, controlled_set_successor_obj<ValueType<I>>{ value });
}

template<typename I>
// I models VersionedForwardSlot_Iterator
inline
void versioned_forward_empty_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos) {
	forward_empty_all_slots_basis(first, last, start_index, npos, versioned_set_successor_obj<ValueType<I>>{});
}

template<typename I>
// I models VersionedForwardSlot_Iterator
inline
void versioned_forward_empty_and_reset_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos) {
	forward_empty_all_slots_basis(first, last, start_index, npos, versioned_set_successor_resetter_obj<ValueType<I>>{});
}

template<typename I>
// I models VersionedForwardSlot_Iterator
inline
void regulated_forward_empty_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos, const SlotValueType<I> &value) {
	forward_empty_all_slots_basis(first, last, start_index, npos, regulated_set_successor_obj<ValueType<I>>{ value });
}

template<typename I>
// I models VersionedForwardSlot_Iterator
inline
void regulated_forward_empty_and_reset_all_slots(I first, I last, SlotSizeType<I> start_index, SlotSizeType<I> npos, const SlotValueType<I> &value) {
	forward_empty_all_slots_basis(first, last, start_index, npos, regulated_set_successor_resetter_obj<ValueType<I>>{ value });
}


template<typename I>
// I models VersionedSlot_Iterator
inline
void versioned_reset(I first, I last) {
	std::for_each(first, last, resetter_obj<ValueType<I>>{});
}


template<typename I, typename P>
// I models ForwardSlot_Iterator
// P models BinaryPredicate : void operator()(ValueType<I> &, S);
inline
std::pair<SlotSizeType<I>, SlotSizeType<I>> forward_empty_all_slots_with_meta_basis(I first, SlotSizeType<I> new_empty, I last,
	const std::pair<SlotSizeType<I>, SlotSizeType<I>> &empty, SlotSizeType<I> npos, P set_successor) {
	if (empty.second != npos)
		set_successor(iterator_slot(first, empty.second), new_empty);

	forward_empty_all_slots_basis(first + static_cast<DifferenceType<I>>(new_empty), last, new_empty, npos, set_successor);

	const auto last_empty = static_cast<SlotSizeType<I>>(last - first - 1);
	return empty.first != npos ? std::pair<SlotSizeType<I>, SlotSizeType<I>>{ empty.first, last_empty } :
		std::pair<SlotSizeType<I>, SlotSizeType<I>>{ new_empty, last_empty };
}

template<typename I>
// I models ForwardSlot_Iterator
inline
std::pair<SlotSizeType<I>, SlotSizeType<I>> forward_empty_all_slots_with_meta(I first, SlotSizeType<I> new_empty, I last,
	const std::pair<SlotSizeType<I>, SlotSizeType<I>> &empty, SlotSizeType<I> npos) {
	return forward_empty_all_slots_with_meta_basis(first, new_empty, last, empty, npos, set_successor_obj<ValueType<I>>{});
}

template<typename I>
// I models ForwardSlot_Iterator
inline
std::pair<SlotSizeType<I>, SlotSizeType<I>> trivial_forward_empty_all_slots_with_meta(I first, SlotSizeType<I> new_empty, I last,
	const std::pair<SlotSizeType<I>, SlotSizeType<I>> &empty, SlotSizeType<I> npos) {
	return forward_empty_all_slots_with_meta_basis(first, new_empty, last, empty, npos, trivial_set_successor_obj<ValueType<I>>{});
}



// Empties all slots in slot successive order.
// Does list traversal(jumps all over memory).
// Word empty is intentonally used instead of link, because in the "set_successor" function object, next to seetting the sucecssor,
// one may do all sorts of things to the slot in order to make it empty.
// E.g. 
// - For version slots, version is inrcemented.
// - For controlled slots, empty value is set.
// - For regulated both version is incremented and empty value is set.
template<typename I, typename S, typename P1, typename P2>
// I models RandomAccessIterator
// S models Integral
// P1 models BinaryPredicate : void operator()(ValueType<I> &, S);
// P2 models UnaryPredicate : S operator()(ValueType<I> &);
inline
S forward_empty_filled_slots_basis(I first, S empty, S filled, S npos, P1 set_successor, P2 get_successor) {
	if (filled == npos) return empty;
	do {
		auto &slot = iterator_slot(first, filled);
		const auto next = get_successor(slot);
		set_successor(slot, empty);
		empty = filled;
		filled = next;
	} while (filled != npos);

	return empty;
}



template<typename I>
// I models ForwardSlot_Iterator
inline
SlotSizeType<I> forward_empty_filled_slots(I first, SlotSizeType<I> empty, SlotSizeType<I> filled, SlotSizeType<I> npos) {
	return forward_empty_filled_slots_basis(first, empty, filled, npos, set_successor_obj<ValueType<I>>{}, get_successor_obj<ValueType<I>>{});
}

template<typename I>
// I models ForwardSlot_Iterator
inline
SlotSizeType<I> controlled_forward_empty_filled_slots(I first, SlotSizeType<I> empty, SlotSizeType<I> filled, SlotSizeType<I> npos, const SlotValueType<I> &value) {
	return forward_empty_filled_slots_basis(first, empty, filled, npos, controlled_set_successor_obj<ValueType<I>>{value}, get_successor_obj<ValueType<I>>{});
}

template<typename I>
// I models VersionedForwardSlot_Iterator
inline
SlotSizeType<I> versioned_forward_empty_filled_slots(I first, SlotSizeType<I> empty, SlotSizeType<I> filled, SlotSizeType<I> npos) {
	return forward_empty_filled_slots_basis(first, empty, filled, npos, versioned_set_successor_obj<ValueType<I>>{}, get_successor_obj<ValueType<I>>{});
}

template<typename I>
// I models VersionedForwardSlot_Iterator
inline
SlotSizeType<I> regulated_forward_empty_filled_slots(I first, SlotSizeType<I> empty, SlotSizeType<I> filled, SlotSizeType<I> npos, const SlotValueType<I> &value) {
	return forward_empty_filled_slots_basis(first, empty, filled, npos, regulated_set_successor_obj<ValueType<I>>{value}, get_successor_obj<ValueType<I>>{});
}


// Empties slots in order based on positions in range "first_position" to "last_position".
// Does array traversal for positions, but jumps all over memory for slots.
// Unlike the previous emptying functions, this one should always only do relinking.
// Word empty is used only to reflect those functions.
template<typename I1, typename I2, typename P>
// I1 models RandomAccessIterator
// I2 models RandomAccessIterator
// P1 models BinaryPredicate : void operator()(ValueType<I> &, S);
inline
SlotSizeType<I1> forward_empty_filled_dense_slots_basis(I1 &first_position, I1 last_position, I2 &first_slot, SlotSizeType<I1> empty, P p) {
	while (first_position != last_position) {
		const auto filled = *first_position;
		auto &slot = iterator_slot(first_slot, filled);
		p(slot, empty);
		empty = filled;
	}
	return empty;
}


template<typename I1, typename I2>
// I1 models ForwardSlotPos_Iterator
// I2 models TrivialSlot_Iterator
inline
SlotSizeType<I1> forward_empty_filled_dense_slots(I1 &first_position, I1 last_position, I2 &first_slot, SlotSizeType<I1> empty) {
	return forward_empty_filled_dense_slots_basis(first_position, last_position, first_slot, empty, trivial_set_successor_obj<ValueType<I2>>{});
}

template<typename I1, typename I2>
// I1 models ForwardSlotPos_Iterator
// I2 models TrivialSlot_Iterator
inline
SlotSizeType<I1> versioned_forward_empty_filled_dense_slots(I1 &first_position, I1 last_position, I2 &first_slot, SlotSizeType<I1> empty) {
	return forward_empty_filled_dense_slots_basis(first_position, last_position, first_slot, empty, versioned_trivial_set_successor_obj<ValueType<I2>>{});
}



template<typename T,
	typename S = default_size_type,
	typename A = default_allocator_type<T>>
class slot_list {
public:
	using value_type = T;
	using size_type = S;
	using id_type = size_type;
	using slot_type = sl_slot_type<value_type, size_type>;
	using container_type = sl_container_type<slot_type, A>;

private:
	static constexpr auto npos = max_type_value<size_type>();
	using meta_positions_type = bidirectional_slot_meta_positions<S>;

	container_type slots;
	meta_positions_type pos;
	size_type filled_size;

	void _reserve(size_type s) {
		size_type prev_size = slots.size();
		slots.resize(s);
		pos.empty = forward_empty_all_slots_with_meta(slots.begin(), prev_size, slots.end(), pos.empty, npos);
	}

	template<typename U>
	id_type _insert(U&& v) {
		pos = bidirectional_link_to_filled(slots.begin(), pos, npos);
		iterator_slot(slots.begin(), pos.filled.second).value = std::forward<U>(v);
		++filled_size;
		return pos.filled.second;
	}

	id_type _erase(size_type index) {
		id_type next = id_next(index);
		pos = bidirectional_link_to_empty(slots.begin(), pos, npos, index);
		--filled_size;
		return next;
	}

	void _clear() {
		if (is_over_breakoff(capacity(), size())) {
			forward_empty_all_slots(slots.begin(), slots.end(), size_type(0), npos);
			pos = meta_positions_type{ { 0, slots.size() - 1 },{ npos, npos } };
		}
		else {
			const auto empty_pos = forward_empty_filled_slots(slots.begin(), pos.empty.first, pos.filled.first, npos);
			pos = meta_positions_type{ { empty_pos, pos.empty.second },{ npos, npos } };
		}
		filled_size = size_type(0);
	}


public:
	slot_list(AllocatorType<container_type> &&alloc = AllocatorType<container_type>{}) :
		slots(std::move(alloc)),
		filled_size(size_type(0)),
		pos({ { npos, npos },{ npos, npos } })
	{}

	slot_list(const AllocatorType<container_type> &alloc) :
		slots(alloc),
		filled_size(size_type(0)),
		pos({ { npos, npos },{ npos, npos } })
	{}

	value_type& id_value(id_type id) {
		return iterator_slot(slots.begin(), id).value;
	}

	const value_type& id_value(id_type id) const {
		return iterator_slot(slots.cbegin(), id).value;
	}

	id_type id_next(id_type id) const {
		return iterator_slot(slots.cbegin(), id).next;
	}

	bool id_is_end(id_type id) const {
		return id == npos;
	}

	constexpr static void reset_version_counts() {

	}

	void clear_and_reset_version_counts() {
		clear();
	}

	constexpr static bool id_is_valid(id_type id){
		return true;
	}

	id_type id_begin() const {
		return pos.filled.first;
	}

	size_type size() const {
		return filled_size;
	}

	size_type capacity() const {
		return slots.size();
	}

	void reserve(size_type s) {
		if (s > capacity()) _reserve(s);
	}

	id_type insert(const value_type &v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(v);
	}

	id_type insert(value_type &&v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(std::move(v));
	}

	id_type erase(id_type id) {
		return _erase(static_cast<size_type>(id));
	}

	void clear() {
		_clear();
	}

	bool empty() const {
		return slots.empty();
	}
};




template<typename T,
	typename E = get_empty<T>,
	typename S = default_size_type,
	typename A = default_allocator_type<T>>
class controlled_slot_list {
public:
	using value_type = T;
	using get_empty_type = E;
	using size_type = S;
	using id_type = size_type;
	using slot_type = sl_controlled_slot_type<value_type, size_type>;
	using container_type = sl_container_type<slot_type, A>;


private:
	static constexpr auto npos = max_type_value<size_type>();
	using meta_positions_type = bidirectional_slot_meta_positions<S>;

	container_type slots;
	meta_positions_type pos;
	size_type filled_size;
	get_empty_type get_empty_obj;

	void _reserve(size_type s) {
		slot_type empty_slot{ get_empty_obj() };
		size_type prev_size = slots.size();
		slots.resize(s, empty_slot);
		pos.empty = forward_empty_all_slots_with_meta(slots.begin(), prev_size, slots.end(), pos.empty, npos);
	}

	template<typename U>
	id_type _insert(U&& v) {
		pos = bidirectional_link_to_filled(slots.begin(), pos, npos);
		iterator_slot(slots.begin(), pos.filled.second).value = std::forward<U>(v);
		++filled_size;
		return pos.filled.second;
	}

	id_type _erase(size_type index) {
		id_type next = id_next(index);
		pos = bidirectional_link_to_empty(slots.begin(), pos, npos, index);
		iterator_slot(slots.begin(), index).value = get_empty_obj();
		--filled_size;
		return next;
	}

	void _clear() {
		if (is_over_breakoff(capacity(), size())) {
			controlled_forward_empty_all_slots(slots.begin(), slots.end(), size_type(0), npos, get_empty_obj());
			pos = meta_positions_type{ { 0, slots.size() - 1 },{ npos, npos } };
		}
		else {
			const auto empty_pos = controlled_forward_empty_filled_slots(slots.begin(), pos.empty.first, pos.filled.first, npos, get_empty_obj());
			pos = meta_positions_type{ { empty_pos, pos.empty.second },{ npos, npos } };
		}
		filled_size = size_type(0);
	}


public:
	controlled_slot_list(get_empty_type &&get_empty_obj = get_empty<value_type>{}, AllocatorType<container_type> &&alloc = AllocatorType<container_type>{}) :
		slots(std::move(alloc)),
		filled_size(size_type(0)),
		get_empty_obj(std::move(get_empty_obj)),
		pos({ { npos, npos },{ npos, npos } })
	{}

	controlled_slot_list(const get_empty_type &get_empty_obj, AllocatorType<container_type> &&alloc = AllocatorType<container_type>{}) :
		slots(std::move(alloc)),
		filled_size(size_type(0)),
		get_empty_obj(get_empty_obj),
		pos({ { npos, npos },{ npos, npos } })
	{}

	controlled_slot_list(const get_empty_type &get_empty_obj, const AllocatorType<container_type> &alloc) :
		slots(alloc),
		filled_size(size_type(0)),
		get_empty_obj(get_empty_obj),
		pos({ { npos, npos },{ npos, npos } })
	{}


	value_type& id_value(id_type id) {
		return iterator_slot(slots.begin(), id).value;
	}

	const value_type& id_value(id_type id) const {
		return iterator_slot(slots.cbegin(), id).value;
	}

	id_type id_next(id_type id) const {
		return iterator_slot(slots.cbegin(), id).next;
	}

	bool id_is_end(id_type id) const {
		return id == npos;
	}

	constexpr static void reset_version_counts() {

	}

	void clear_and_reset_version_counts() {
		clear();
	}

	constexpr static bool id_is_valid(id_type id) {
		return true;
	}

	id_type id_begin() const {
		return pos.filled.first;
	}

	size_type size() const {
		return filled_size;
	}

	size_type capacity() const {
		return slots.size();
	}

	void reserve(size_type s) {
		if (s > capacity()) _reserve(s);
	}

	id_type insert(const value_type &v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(v);
	}

	id_type insert(value_type &&v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(std::move(v));
	}

	id_type erase(id_type id) {
		return _erase(static_cast<size_type>(id));
	}

	void clear() {
		_clear();
	}

	bool empty() const {
		return slots.empty();
	}
};




template<typename T,
	typename V = default_version_type,
	typename S = default_size_type,
	typename A = default_allocator_type<T>>
class versioned_slot_list {
public:
	using value_type = T;
	using version_type = V;
	using size_type = S;
	using id_type = std::pair<size_type, version_type>;
	using slot_type = sl_versioned_slot_type<value_type, size_type, version_type>;
	using container_type = sl_container_type<slot_type, A>;

private:
	static constexpr auto npos = max_type_value<size_type>();
	using meta_positions_type = bidirectional_slot_meta_positions<S>;

	container_type slots;
	meta_positions_type pos;
	size_type filled_size;

	void _reserve(size_type s) {
		size_type prev_size = slots.size();
		slots.resize(s);
		pos.empty = forward_empty_all_slots_with_meta(slots.begin(), prev_size, slots.end(), pos.empty, npos);
	}

	template<typename U>
	id_type _insert(U&& v) {
		pos = bidirectional_link_to_filled(slots.begin(), pos, npos);
		iterator_slot(slots.begin(), pos.filled.second).value = std::forward<U>(v);
		++filled_size;
		return _get_id_unguarded(pos.filled.second);
	}

	id_type _erase(size_type index) {
		id_type next = _id_next(index);
		pos = versioned_bidirectional_link_to_empty(slots.begin(), pos, npos, index);
		++iterator_slot(slots.begin(), index).version;
		--filled_size;
		return next;
	}

	void _clear() {
		if (is_over_breakoff(capacity(), size())) {
			versioned_forward_empty_all_slots(slots.begin(), slots.end(), size_type(0), npos);
			pos = meta_positions_type{ { 0, slots.size() - 1 },{ npos, npos } };
		}
		else {
			const auto empty_pos = versioned_forward_empty_filled_slots(slots.begin(), pos.empty.first, pos.filled.first, npos);
			pos = meta_positions_type{ { empty_pos, pos.empty.second },{ npos, npos } };
		}
		filled_size = size_type(0);
	}

	id_type _get_id_unguarded(size_type index) const {
		return{ index, iterator_slot(slots.begin(), index).version };
	}

	id_type _get_id(size_type index) const {
		return{ index, index == npos ? npos : iterator_slot(slots.begin(), index).version };
	}

	id_type _id_next(size_type index) const {
		return _get_id(slots[index].next);
	}

public:
	versioned_slot_list(AllocatorType<container_type> &&alloc = AllocatorType<container_type>{}) :
		slots(std::move(alloc)),
		filled_size(size_type(0)),
		pos({ { npos, npos },{ npos, npos } })
	{}

	versioned_slot_list(const AllocatorType<container_type> &alloc) :
		slots(alloc),
		filled_size(size_type(0)),
		pos({ { npos, npos },{ npos, npos } })
	{}

	value_type& id_value(id_type id) {
		return iterator_slot(slots.begin(), id.first).value;
	}

	const value_type& id_value(id_type id) const {
		return iterator_slot(slots.cbegin(), id.first).value;
	}

	id_type id_next(id_type id) const {
		return _id_next(id.first);
	}
	
	bool id_is_end(id_type id) const {
		return id.first == npos;
	}

	void reset_version_counts() {
		versioned_reset(slots.begin(), slots.end());
	}

	void clear_and_reset_version_counts() {
		versioned_forward_empty_and_reset_all_slots(slots.begin(), slots.end(), size_type(0), npos);
		pos = meta_positions_type{ { 0, slots.size() - 1 },{ npos, npos } };
	}

	bool id_is_valid(id_type id) {
		return id.second == iterator_slot(slots.begin(), id.first).version;
	}

	id_type id_begin() const {
		return _get_id(pos.filled.first);
	}

	size_type size() const {
		return filled_size;
	}

	size_type capacity() const {
		return slots.size();
	}

	void reserve(size_type s) {
		if (s > capacity()) _reserve(s);
	}

	id_type insert(const value_type &v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(v);
	}

	id_type insert(value_type &&v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(std::move(v));
	}

	id_type erase(id_type id) {
		return _erase(id.first);
	}

	void clear() {
		_clear();
	}

	bool empty() const {
		return slots.empty();
	}
};









template<typename T,
	typename E = get_empty<T>,
	typename V = default_version_type,
	typename S = default_size_type,
	typename A = default_allocator_type<T>>
class regulated_slot_list {
public:
	using value_type = T;
	using size_type = S;
	using version_type = V;
	using get_empty_type = E;
	using id_type = std::pair<size_type, version_type>;
	using slot_type = sl_regulated_slot_type<value_type, size_type, version_type>;
	using container_type = sl_container_type<slot_type, A>;

private:
	static constexpr auto npos = max_type_value<size_type>();
	using meta_positions_type = bidirectional_slot_meta_positions<S>;

	container_type slots;
	meta_positions_type pos;
	size_type filled_size;
	get_empty_type get_empty_obj;

	void _reserve(size_type s) {
		slot_type empty_slot{ get_empty_obj() };
		size_type prev_size = slots.size();
		slots.resize(s, empty_slot);
		pos.empty = forward_empty_all_slots_with_meta(slots.begin(), prev_size, slots.end(), pos.empty, npos);
	}

	template<typename U>
	id_type _insert(U&& v) {
		pos = bidirectional_link_to_filled(slots.begin(), pos, npos);
		iterator_slot(slots.begin(), pos.filled.second).value = std::forward<U>(v);
		++filled_size;
		return _get_id_unguarded(pos.filled.second);
	}

	id_type _erase(size_type index) {
		id_type id_next = _id_next(index);
		pos = versioned_bidirectional_link_to_empty(slots.begin(), pos, npos, index);
		auto &slot = iterator_slot(slots.begin(), index);
		slot.value = get_empty_obj();
		++slot.version;
		--filled_size;
		return id_next;
	}

	void _clear() {
		if (is_over_breakoff(capacity(), size())) {
			regulated_forward_empty_all_slots(slots.begin(), slots.end(), size_type(0), npos, get_empty_obj());
			pos = meta_positions_type{ { 0, slots.size() - 1 },{ npos, npos } };
		}
		else {
			const auto empty_pos = regulated_forward_empty_filled_slots(slots.begin(), pos.empty.first, pos.filled.first, npos, get_empty_obj());
			pos = meta_positions_type{ { empty_pos, pos.empty.second },{ npos, npos } };
		}
		filled_size = size_type(0);
	}

	id_type _get_id_unguarded(size_type index) const {
		return{ index, iterator_slot(slots.begin(), index).version };
	}

	id_type _get_id(size_type index) const {
		return{ index, index == npos ? npos : iterator_slot(slots.begin(), index).version };
	}

	id_type _id_next(size_type index) const {
		return _get_id(slots[index].next);
	}

public:
	regulated_slot_list(get_empty_type &&get_empty_obj = get_empty<value_type>{}, AllocatorType<container_type> &&alloc = AllocatorType<container_type>{}) :
		slots(std::move(alloc)),
		filled_size(size_type{ 0 }),
		get_empty_obj(std::move(get_empty_obj)),
		pos({ { npos, npos },{ npos, npos } })
	{}

	regulated_slot_list(const get_empty_type &get_empty_obj, AllocatorType<container_type> &&alloc = AllocatorType<container_type>{}) :
		slots(std::move(alloc)),
		filled_size(size_type{ 0 }),
		get_empty_obj(get_empty_obj),
		pos({ { npos, npos },{ npos, npos } })
	{}

	regulated_slot_list(const get_empty_type &get_empty_obj, const AllocatorType<container_type> &alloc) :
		slots(alloc),
		filled_size(size_type{ 0 }),
		get_empty_obj(get_empty_obj),
		pos({ { npos, npos },{ npos, npos } })
	{}

	value_type& id_value(id_type id) {
		return iterator_slot(slots.begin(), id.first).value;
	}

	const value_type& id_value(id_type id) const {
		return iterator_slot(slots.cbegin(), id.first).value;
	}

	id_type id_next(id_type id) const {
		return _id_next(id.first);
	}

	bool id_is_end(id_type id) const {
		return id.first == npos;
	}

	void reset_version_counts() {
		versioned_reset(slots.begin(), slots.end());
	}

	void clear_and_reset_version_counts() {
		regulated_forward_empty_and_reset_all_slots(slots.begin(), slots.end(), size_type(0), npos, get_empty_obj());
		pos = meta_positions_type{ { 0, slots.size() - 1 },{ npos, npos } };
	}

	bool id_is_valid(id_type id) {
		return id.second == iterator_slot(slots.begin(), id.first).version;
	}

	id_type id_begin() const {
		return _get_id(pos.filled.first);
	}

	size_type size() const {
		return filled_size;
	}

	size_type capacity() const {
		return slots.size();
	}

	void reserve(size_type s) {
		if (s > capacity()) _reserve(s);
	}

	id_type insert(const value_type &v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(v);
	}

	id_type insert(value_type &&v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(std::move(v));
	}

	id_type erase(id_type id) {
		return _erase(id.first);
	}

	void clear() {
		_clear();
	}

	bool empty() const {
		return slots.empty();
	}
};











template<typename T,
	typename S = default_size_type,
	typename A = default_allocator_type<T>>
class slot_map {
public:
	using value_type = T;
	using size_type = S;
	using id_type = size_type;

	using slot_type = sm_slot_type<size_type>;
	using id_slot_container_type = sm_id_slot_container_type<slot_type, A>;
	using value_container_type = sm_value_container_type<value_type, A>;
	using id_pos_container_type = sm_id_pos_container_type<size_type, A>;

	using difference_type = typename value_container_type::difference_type;
	using iterator = typename value_container_type::iterator;
	using const_iterator = typename value_container_type::const_iterator;
	using reverse_iterator = typename value_container_type::reverse_iterator;
	using const_reverse_iterator = typename value_container_type::const_reverse_iterator;

private:
	static constexpr auto npos = max_type_value<size_type>();

	value_container_type values;
	id_slot_container_type id_slots;
	id_pos_container_type id_positions;
	std::pair<size_type, size_type> empty_pos;

	void _reserve(size_type s) {
		const auto prev_size = static_cast<size_type>(id_slots.size());
		id_slots.resize(s);
		empty_pos = trivial_forward_empty_all_slots_with_meta(id_slots.begin(), prev_size, id_slots.end(), empty_pos, npos);

		values.reserve(s);
		id_positions.reserve(s);
	}

	template<typename U>
	std::pair<iterator, id_type> _insert(U&& v) {
		const auto value_pos = values.size();
		values.push_back(std::forward<U>(v));
		id_positions.push_back(empty_pos.first);

		const auto id = empty_pos.first;
		auto &slot = iterator_slot(id_slots.begin(), empty_pos.first);

		if (empty_pos.first == empty_pos.second) {
			empty_pos.second = npos;
			empty_pos.first = npos;
		}
		else {
			empty_pos.first = slot.value;
		}
		slot.value = value_pos;

		return{ values.end() - 1, id };
	}

	void _erase(size_type id_index, size_type value_pos) {
		const auto last_pos = static_cast<size_type>(size() - 1);
		if (value_pos != last_pos) {
			const auto pos = iterator_slot(id_positions.begin(), last_pos);
			iterator_slot(id_positions.begin(), value_pos) = pos;
			iterator_slot(id_slots.begin(), pos).value = value_pos;
			iterator_slot(values.begin(), value_pos) = std::move(iterator_slot(values.begin(), last_pos));
		}
		id_positions.pop_back();
		values.pop_back();
		iterator_slot(id_slots.begin(), id_index).value = empty_pos.first;
		if (empty_pos.second == npos)
			empty_pos.second = id_index;
		empty_pos.first = id_index;
	}


	void _clear() {
		if (is_over_breakoff(capacity(), size())) {
			trivial_forward_empty_all_slots(id_slots.begin(), id_slots.end(), size_type(0), npos);
			empty_pos.first = 0;
			empty_pos.second = id_slots.size() - 1;
		}
		else {
			empty_pos.first = forward_empty_filled_dense_slots(id_positions.begin(), id_positions.end(), id_slots.begin(), empty_pos.first);
		}
		values.clear();
		id_positions.clear();
	}

	template<typename I>
	const value_type &_get_value(I index) const {
		return iterator_slot(values.cbegin(), index);
	}

	template<typename I>
	value_type &_get_value(I index) {
		return iterator_slot(values.begin(), index);
	}

public:
	slot_map(AllocatorType<id_slot_container_type> &&id_slots_allocator = AllocatorType<id_slot_container_type>{},
		AllocatorType<value_container_type> &&value_allocator = AllocatorType<value_container_type>{},
		AllocatorType<id_pos_container_type> &&is_positions_allocator = AllocatorType<id_pos_container_type>{}) :
		id_slots(std::move(id_slots_allocator)),
		values(std::move(value_allocator)),
		id_positions(std::move(is_positions_allocator))
	{

	}

	slot_map(const AllocatorType<id_slot_container_type> &id_slots_allocator,
		const AllocatorType<value_container_type> &value_allocator,
		const AllocatorType<id_pos_container_type> &is_positions_allocator) :
		id_slots(id_slots_allocator),
		values(value_allocator),
		id_positions(is_positions_allocator)
	{

	}


	value_type& id_value(id_type id) {
		return _get_value(iterator_slot(id_slots.begin(), id).value);
	}

	const value_type& id_value(id_type id) const {
		return _get_value(iterator_slot(id_slots.cbegin(), id).value);
	}

	constexpr static void reset_version_counts() {

	}

	void clear_and_reset_version_counts() {
		clear();
	}

	constexpr static bool id_is_valid(id_type id) {
		return true;
	}

	size_type size() const {
		return values.size();
	}

	size_type capacity() const {
		return values.capacity();
	}

	void reserve(size_type s) {
		if (s > capacity()) _reserve(s);
	}

	std::pair<iterator, id_type> insert(const value_type &v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(v);
	}

	std::pair<iterator, id_type> insert(value_type &&v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(std::move(v));
	}

	void erase(id_type id) {
		return _erase(id, iterator_slot(id_slots.begin(), id).value);
	}

	iterator erase(iterator it) {
		const auto value_pos = iterator_distance<size_type>(begin(), it);
		if (it == end()) {
			_erase(iterator_slot(id_positions.begin(), value_pos), value_pos);
			return end();
		}
		else {
			_erase(iterator_slot(id_positions.begin(), value_pos), value_pos);
			return it;
		}
	}

	const_iterator erase(const_iterator it) {
		const auto value_pos = iterator_distance<size_type>(cbegin(), it);
		if (it == cend()) {
			_erase(iterator_slot(id_positions.begin(), value_pos), value_pos);
			return cend();
		}
		else {
			_erase(iterator_slot(id_positions.begin(), value_pos), value_pos);
			return it;
		}
	}

	void clear() {
		_clear();
	}

	bool empty() const {
		return values.empty();
	}

	iterator begin() {
		return values.begin();
	}

	const_iterator cbegin() const {
		return values.cbegin();
	}

	const_iterator begin() const {
		return cbegin();
	}

	reverse_iterator rbegin() {
		return values.rbegin();
	}

	const_reverse_iterator crbegin() const {
		return values.crbegin();
	}

	const_reverse_iterator rbegin() const {
		return crbegin();
	}

	iterator end() {
		return values.end();
	}

	const_iterator cend() const {
		return values.cend();
	}

	const_iterator end() const {
		return cend();
	}

	iterator rend() {
		return values.rend();
	}

	const_reverse_iterator crend() const {
		return values.crend();
	}

	const_reverse_iterator rend() const {
		return crend();
	}

	iterator iterator_of_id(id_type id) {
		return next_iterator(values.begin(), iterator_slot(id_slots.begin(), id).value);
	}

	const_iterator const_iterator_of_id(id_type id) const {
		return next_iterator(values.cbegin(), iterator_slot(id_slots.cbegin(), id).value);
	}

	const_iterator iterator_of_id(id_type id) const {
		return const_iterator_of_id(id);
	}

	id_type id_of_iterator(const_iterator it) const {
		return iterator_slot(id_positions.cbegin(), iterator_distance(values.cbegin(), it));
	}

	id_type id_of_iterator(iterator it) const {
		return iterator_slot(id_positions.cbegin(), iterator_distance(values.cbegin(), it));
	}

};



template<typename T,
	typename S = default_size_type,
	typename V = default_version_type,
	typename A = default_allocator_type<T>>
class versioned_slot_map {
public:
	using value_type = T;
	using size_type = S;
	using version_type = V;
	using id_type = std::pair<size_type, version_type>;

	using slot_type = sm_versioned_slot_type<size_type, version_type>;
	using id_slot_container_type = sm_id_slot_container_type<slot_type, A>;
	using value_container_type = sm_value_container_type<value_type, A>;
	using id_pos_container_type = sm_id_pos_container_type<size_type, A>;

	using difference_type = typename value_container_type::difference_type;
	using iterator = typename value_container_type::iterator;
	using const_iterator = typename value_container_type::const_iterator;
	using reverse_iterator = typename value_container_type::reverse_iterator;
	using const_reverse_iterator = typename value_container_type::const_reverse_iterator;

private:
	static constexpr auto npos = max_type_value<size_type>();

	value_container_type values;
	id_slot_container_type id_slots;
	id_pos_container_type id_positions;
	std::pair<size_type, size_type> empty_pos;

	void _reserve(size_type s) {
		const auto prev_size = static_cast<size_type>(id_slots.size());
		id_slots.resize(s);
		empty_pos = trivial_forward_empty_all_slots_with_meta(id_slots.begin(), prev_size, id_slots.end(), empty_pos, npos);

		values.reserve(s);
		id_positions.reserve(s);
	}

	template<typename U>
	std::pair<iterator, id_type> _insert(U&& v) {
		const auto value_pos = values.size();
		values.push_back(std::forward<U>(v));
		id_positions.push_back(empty_pos.first);

		const auto id = empty_pos.first;
		auto &slot = iterator_slot(id_slots.begin(), empty_pos.first);

		if (empty_pos.first == empty_pos.second) {
			empty_pos.second = npos;
			empty_pos.first = npos;
		}
		else {
			empty_pos.first = slot.value;
		}
		slot.value = value_pos;

		return{ values.end() - 1, id_type{ id, slot.version } };
	}

	void _erase(size_type id_index, size_type value_pos) {
		const auto last_pos = static_cast<size_type>(size() - 1);
		if (value_pos != last_pos) {
			const auto pos = iterator_slot(id_positions.begin(), last_pos);
			iterator_slot(id_positions.begin(), value_pos) = pos;
			iterator_slot(id_slots.begin(), pos).value = value_pos;
			iterator_slot(values.begin(), value_pos) = std::move(iterator_slot(values.begin(), last_pos));
		}
		id_positions.pop_back();
		values.pop_back();
		auto &slot = iterator_slot(id_slots.begin(), id_index);
		slot.value = npos;
		++slot.version;

		if (empty_pos.second == npos) {
			empty_pos.first = id_index;
			empty_pos.second = id_index;
		}
		else {
			trivial_set_successor(iterator_slot(id_slots.begin(), empty_pos.second), id_index);
			empty_pos.second = id_index;
		}
	}


	void _clear() {
		if (is_over_breakoff(capacity(), size())) {
			versioned_trivial_forward_empty_all_slots(id_slots.begin(), id_slots.end(), size_type(0), npos);
			empty_pos.first = 0;
			empty_pos.second = id_slots.size() - 1;
		}
		else {
			empty_pos.first = versioned_forward_empty_filled_dense_slots(id_positions.begin(), id_positions.end(), id_slots.begin(), empty_pos.first);
		}
		values.clear();
		id_positions.clear();
	}

	template<typename I>
	value_type &_get_value(I index) {
		return iterator_slot(values.begin(), index);
	}

	template<typename I>
	const value_type &_get_value(I index) const {
		return iterator_slot(values.cbegin(), index);
	}

public:
	versioned_slot_map(AllocatorType<id_slot_container_type> &&id_slots_allocator = AllocatorType<id_slot_container_type>{},
		AllocatorType<value_container_type> &&value_allocator = AllocatorType<value_container_type>{},
		AllocatorType<id_pos_container_type> &&id_positions_allocator = AllocatorType<id_pos_container_type>{}) :
		id_slots(id_slots_allocator),
		values(value_allocator),
		id_positions(id_positions_allocator)
	{

	}

	versioned_slot_map(const AllocatorType<id_slot_container_type> &id_slots_allocator,
		const AllocatorType<value_container_type> &value_allocator,
		const AllocatorType<id_pos_container_type> &id_positions_allocator) :
		id_slots(id_slots_allocator),
		values(value_allocator),
		id_positions(id_positions_allocator)
	{

	}

	value_type& id_value(id_type id) {
		return _get_value(iterator_slot(id_slots.begin(), id.first).value);
	}

	const value_type& id_value(id_type id) const {
		return _get_value(iterator_slot(id_slots.cbegin(), id.first).value);
	}

	void reset_version_counts() {
		versioned_reset(id_slots.begin(), id_slots.end());
	}

	void clear_and_reset_version_counts() {
		versioned_trivial_forward_empty_and_reset_all_slots(id_slots.begin(), id_slots.end(), size_type(0), npos);
		empty_pos.first = 0;
		empty_pos.second = id_slots.size() - 1;
		values.clear();
		id_positions.clear();
	}

	bool id_is_valid(id_type id) {
		return id.second == iterator_slot(id_slots.cbegin(), id.first).version;
		return true;
	}

	size_type size() const {
		return values.size();
	}

	size_type capacity() const {
		return values.capacity();
	}

	void reserve(size_type s) {
		if (s > capacity()) _reserve(s);
	}

	std::pair<iterator, id_type> insert(const value_type &v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(v);
	}

	std::pair<iterator, id_type> insert(value_type &&v) {
		if (capacity() == size())
			_reserve(grow_size(size()));
		return _insert(std::move(v));
	}

	void erase(id_type id) {
		return _erase(id.first, iterator_slot(id_slots.begin(), id.first).value);
	}

	iterator erase(iterator it) {
		const auto value_pos = iterator_distance<size_type>(begin(), it);
		if (it == end()) {
			_erase(iterator_slot(id_positions.begin(), value_pos), value_pos);
			return end();
		}
		else {
			_erase(iterator_slot(id_positions.begin(), value_pos), value_pos);
			return it;
		}
	}

	const_iterator erase(const_iterator it) {
		const auto value_pos = iterator_distance<size_type>(cbegin(), it);
		if (it == cend()) {
			_erase(iterator_slot(id_positions.begin(), value_pos), value_pos);
			return cend();
		}
		else {
			_erase(iterator_slot(id_positions.begin(), value_pos), value_pos);
			return it;
		}
	}

	void clear() {
		_clear();
	}

	bool empty() const {
		return values.empty();
	}

	iterator begin() {
		return values.begin();
	}

	const_iterator cbegin() const {
		return values.cbegin();
	}

	const_iterator begin() const {
		return cbegin();
	}

	reverse_iterator rbegin() {
		return values.rbegin();
	}

	const_reverse_iterator crbegin() const {
		return values.crbegin();
	}

	const_reverse_iterator rbegin() const {
		return crbegin();
	}

	iterator end() {
		return values.end();
	}

	const_iterator cend() const {
		return values.cend();
	}

	const_iterator end() const {
		return cend();
	}

	iterator rend() {
		return values.rend();
	}

	const_reverse_iterator crend() const {
		return values.crend();
	}

	const_reverse_iterator rend() const {
		return crend();
	}

	iterator iterator_of_id(id_type id) {
		return next_iterator(begin(), iterator_slot(id_slots.begin(), id).value);
	}

	const_iterator const_iterator_of_id(id_type id) const {
		return next_iterator(cbegin(), iterator_slot(id_slots.cbegin(), id).value);
	}

	const_iterator iterator_of_id(id_type id) const {
		return const_iterator_of_id(id);
	}

	id_type id_of_iterator(const_iterator it) const {
		const auto slot_index = iterator_slot(id_positions.cbegin(), iterator_distance(values.cbegin(), it));
		return id_type{ slot_index, iterator_slot(id_slots.cbegin(), slot_index).version };
	}

	id_type id_of_iterator(iterator it) const {
		return id_of_iterator(const_iterator{ it });
	}

};

} // namespace rea
