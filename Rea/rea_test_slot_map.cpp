#include "rea_test_base.h"
#include "rea.h"
#include <iostream>

#if defined(_TEST_SLOT_MAP) 

template<typename SM>
void slot_map_print_instrumented_values(const SM &sm) {
	std::cout << std::endl;
	std::cout << "================================================" << std::endl;
	for (auto it = sm.begin(); it != sm.end(); ++it) {
		const auto id = sm.id_of_iterator(it);
		const auto value = sm.id_value(id).value;
		std::cout << "index = " << get_id_index(id)
			<< ", version = " << get_id_version(id)
			<< ", value = " << value
			<< std::endl;
	}
	std::cout << "================================================" << std::endl;
}


int main() {

#if defined(TEST_SLOT_MAP)
	using sm_container = rea::slot_map<instrumented>;
#else
	using sm_container = rea::versioned_slot_map<instrumented>;
#endif
	sm_container sm;
	sm.reserve(10);

	constexpr auto nm_ids = 10;
	std::vector<sm_container::id_type> ids;
	ids.reserve(nm_ids);

	for (size_t i = 0; i < nm_ids; ++i)
		ids.push_back(sm.insert(instrumented{ static_cast<int>(i) }).second);

	slot_map_print_instrumented_values(sm);

	sm.erase(ids[1]);
	sm.erase(ids[5]);
	sm.erase(ids[8]);
	sm.erase(ids[4]);

	sm.reserve(40);

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

	sm.clear_and_reset_version_counts();
	//sm.reset_version_counts();

	slot_map_print_instrumented_values(sm);

}

#endif