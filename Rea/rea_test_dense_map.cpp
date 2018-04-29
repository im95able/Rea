#include "rea_test_base.h"
#include "rea.h"
#include <iostream>

#if defined(TEST_DENSE) 

template<typename DM>
void dense_map_print_instrumented_values(const DM &dm) {
	std::cout << std::endl;
	std::cout << "================================================" << std::endl;
	for (auto it = dm.begin(); it != dm.end(); ++it) {
		const auto id = dm.id_of_iterator(it);
		const auto value = dm.id_value(id).value;
		std::cout << "index = " << get_id_index(id)
			<< ", version = " << get_id_version(id)
			<< ", value = " << value
			<< std::endl;
	}
	std::cout << "================================================" << std::endl;
}


int main() {

#if defined(TEST_DENSE_MAP)
	using sm_container = rea::dense_map<instrumented>;
#else
	using sm_container = rea::versioned_dense_map<instrumented>;
#endif
	sm_container dm;
	dm.reserve(10);

	constexpr auto nm_ids = 10;
	std::vector<sm_container::id_type> ids;
	ids.reserve(nm_ids);

	for (size_t i = 0; i < nm_ids; ++i)
		ids.push_back(dm.insert(instrumented{ static_cast<int>(i) }).second);

	dense_map_print_instrumented_values(dm);

	dm.erase(ids[1]);
	dm.erase(ids[5]);
	dm.erase(ids[8]);
	dm.erase(ids[4]);


	dense_map_print_instrumented_values(dm);


	dm.insert(instrumented{ 1000 });
	dm.insert(instrumented{ 2000 });
	dm.insert(instrumented{ 3000 });

	dense_map_print_instrumented_values(dm);

	bool valid = dm.id_is_valid(ids[1]);
	valid = dm.id_is_valid(ids[5]);
	valid = dm.id_is_valid(ids[8]);
	valid = dm.id_is_valid(ids[3]);

	dm.insert(instrumented{ 4000 });

	dense_map_print_instrumented_values(dm);

	int b = 6;

}

#endif