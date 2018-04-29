#include "rea_test_base.h"
#include "rea.h"
#include <iostream>

#if defined(TEST_SLOT)


template<typename SM>
void slot_map_print_instrumented_values(const SM &sm) {
	std::cout << std::endl;
	std::cout << "================================================" << std::endl;
	for (auto id = sm.id_begin(); !sm.id_is_end(id); id = sm.id_next(id)) {
		std::cout << "index = " << get_id_index(id)
			<< ", version = " << get_id_version(id)
			<< ", value = " << sm.id_value(id).value
			<< std::endl;
	}
	std::cout << "================================================" << std::endl;
}

int main() {

#if defined(TEST_SLOT_MAP)
	using sm_container = rea::slot_map<instrumented>;
#elif defined(TEST_CONTROLLED_SLOT_MAP)
	using sm_container = rea::controlled_slot_map<instrumented>;
#elif defined(TEST_VERSIONED_SLOT_MAP)
	using sm_container = rea::versioned_slot_map<instrumented>;
#else
	using sm_container = rea::regulated_slot_map<instrumented>;
#endif

	sm_container sm;
	sm.reserve(10);

	constexpr auto nm_ids = 100;
	std::vector<sm_container::id_type> ids;
	ids.reserve(nm_ids);

	for (size_t i = 0; i < nm_ids; ++i)
		ids.push_back(sm.insert(instrumented{ static_cast<int>(i) }));

	slot_map_print_instrumented_values(sm);

	sm.erase(ids[1]);
	sm.erase(ids[5]);
	sm.erase(ids[8]);

	slot_map_print_instrumented_values(sm);


	sm.insert(instrumented{ 1000 });
	sm.insert(instrumented{ 2000 });
	sm.insert(instrumented{ 3000 });

	slot_map_print_instrumented_values(sm);

	bool valid = sm.id_is_valid(ids[1]);
	valid = sm.id_is_valid(ids[5]);
	valid = sm.id_is_valid(ids[8]);
	valid = sm.id_is_valid(ids[3]);

	sm.insert(instrumented{ 4000 });

	slot_map_print_instrumented_values(sm);


	sm.clear();

	slot_map_print_instrumented_values(sm);
}



#endif
