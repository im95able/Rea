#include "rea_test_base.h"
#include "rea.h"
#include <iostream>

#if defined(_TEST_SLOT_LIST)


template<typename SL>
void slot_list_print_instrumented_values(const SL &sl) {
	std::cout << std::endl;
	std::cout << "================================================" << std::endl;
	for (auto id = sl.id_begin(); !sl.id_is_end(id); id = sl.id_next(id)) {
		std::cout << "index = " << get_id_index(id)
			      << ", version = " << get_id_version(id)
			      << ", value = " << sl.id_value(id).value
			      << std::endl;
	}
	std::cout << "================================================" << std::endl;
}

int main() {

#if defined(TEST_SLOT_LIST)
	using sl_container = rea::slot_list<instrumented>;
#elif defined(TEST_CONTROLLED_SLOT_LIST)
	using sl_container = rea::controlled_slot_list<instrumented>;
#elif defined(TEST_VERSIONED_SLOT_LIST)
	using sl_container = rea::versioned_slot_list<instrumented>;
#else
	using sl_container = rea::regulated_slot_list<instrumented>;
#endif

	sl_container sl;
	//sl.reserve(10);

	constexpr auto nm_ids = 10;
	std::vector<sl_container::id_type> ids;
	ids.reserve(nm_ids);

	for (size_t i = 0; i < nm_ids; ++i)
		ids.push_back(sl.insert(instrumented{ static_cast<int>(i) }));

	slot_list_print_instrumented_values(sl);

	sl.erase(ids[1]);
	sl.erase(ids[5]);
	sl.erase(ids[8]);

	slot_list_print_instrumented_values(sl);


	sl.insert(instrumented{ 1000 });
	sl.insert(instrumented{ 2000 });
	sl.insert(instrumented{ 3000 });

	slot_list_print_instrumented_values(sl);

	bool valid = sl.id_is_valid(ids[1]);
	valid = sl.id_is_valid(ids[5]);
	valid = sl.id_is_valid(ids[8]);
	valid = sl.id_is_valid(ids[3]);

	sl.insert(instrumented{ 4000 });
	sl.insert(instrumented{ 5000 });
	sl.insert(instrumented{ 6000 });
	sl.insert(instrumented{ 7000 });
	sl.insert(instrumented{ 8000 });
	sl.insert(instrumented{ 9000 });

	slot_list_print_instrumented_values(sl);


	sl.clear();

	slot_list_print_instrumented_values(sl);
}



#endif
