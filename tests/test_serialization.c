#include <cspecs/cspec.h>
#include <connect/serialization.h>

context (test_serialization) {

	describe("Serialization Tests") {
		
		it("create a package with header 5") {
			t_package* package = package_create(5);

			should_ptr(package) not be null;
			should_ptr(package -> buffer) not be null;
			should_ptr(package -> buffer -> stream) be null;
			should_int(package -> header) be equal to (5);

			int number = 30;
			package_add(package, &number, sizeof(uint32_t));

			should_ptr(package -> buffer) not be null;

			package_destroy(package);
		} end

		it("create a package and add many integers from different sizes to it") {
			t_package* package = package_create(0);

			uint32_t number_1 = 304;
			uint16_t number_2 = 12;
			uint8_t number_3 = 4;
			package_add(package, &number_1, sizeof(uint32_t));
			package_add(package, &number_2, sizeof(uint16_t));
			package_add(package, &number_3, sizeof(uint8_t));

			should_int(package -> buffer -> offset) be equal to (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t));

			// Set offset to 0 for start reading
			package -> buffer -> offset = 0;
			should_int(*(int*) package_get(package, sizeof(uint32_t))) be equal to (304);
			should_int(*(int*) package_get(package, sizeof(uint16_t))) be equal to (12);
			should_int(*(int*) package_get(package, sizeof(uint8_t))) be equal to (4);

			package_destroy(package);
		} end

		it("create a package and add many strings to it") {
			t_package* package = package_create(0);

			char* first_name = "Michael";
			char* last_name = "Scott";
			char* position = "Regional Manager";
			char* assistant = "Dwight Schrute";
			char* company = "Dunder Mifflin";
			
			package_add_str(package, first_name);
			package_add_str(package, last_name);
			package_add_str(package, position);
			package_add_str(package, assistant);
			package_add_str(package, company);
			
			// Test the size of the buffer and the offset (must be equals)
			int expected_size = strlen(first_name) + strlen(last_name) + strlen(position) + strlen(assistant) + strlen(company) + 5 * sizeof(uint32_t) + 5;
			should_int(package -> buffer -> size) be equal to (expected_size);
			should_int(package -> buffer -> offset) be equal to (expected_size);


			// Set offset to 0 for start reading
			package -> buffer -> offset = 0;

			should_string((char*) package_get_str(package)) be equal to (first_name);
			should_string((char*) package_get_str(package)) be equal to (last_name);
			should_string((char*) package_get_str(package)) be equal to (position);
			should_string((char*) package_get_str(package)) be equal to (assistant);
			should_string((char*) package_get_str(package)) be equal to (company);

			package_destroy(package);
		} end

		it("create a package mixing numbers and strings") {
			t_package* package = package_create(0);

			char* first_name = "Michael";
			char* last_name = "Scott";
			uint32_t age = 40;
			uint32_t salary = 100000;
			char* favorite_quote = "I'm not superstitious, but I am a little stitious.";
			uint8_t vasectomy_amount = 2;

			package_add_str(package, first_name);
			package_add_str(package, last_name);
			package_add(package, &age, sizeof(uint32_t));
			package_add(package, &salary, sizeof(uint32_t));
			package_add_str(package, favorite_quote);
			package_add(package, &vasectomy_amount, sizeof(uint8_t));

			// Set offset to 0 for start reading
			package -> buffer -> offset = 0;

			should_string((char*) package_get_str(package)) be equal to (first_name);
			should_string((char*) package_get_str(package)) be equal to (last_name);
			should_int(*(int*) package_get(package, sizeof(uint32_t))) be equal to (40);
			should_int(*(int*) package_get(package, sizeof(uint32_t))) be equal to (100000);
			should_string((char*) package_get_str(package)) be equal to (favorite_quote);
			should_int(*(int*) package_get(package, sizeof(uint8_t))) be equal to (2);

			// The entire buffer must have been read
			should_int(package -> buffer -> offset) be equal to (package -> buffer -> size);
		} end
	} end
}