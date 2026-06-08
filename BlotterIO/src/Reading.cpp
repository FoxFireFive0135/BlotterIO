#include <string>
#include <cstdio>
#include <cstdarg>
#include <fstream>

#include <BlotterIO/Reading.hpp>

static std::vector<char> src;
static std::size_t index = 0;
static bool log_status = false;

static void print(const char* format, ...) {
	va_list args;
	va_start(args, format);

	if (log_status) vprintf(format, args);

	va_end(args);
}

// Returns a byte from src
static uint8_t get_byte(void) {
	return static_cast<uint8_t>(src[index++]);
}

// Returns a 16-bit little endian int from src
static uint16_t get_word(void) {
	uint16_t value = static_cast<uint16_t>(get_byte());
	value |= static_cast<uint16_t>(get_byte()) << 8;
	return value;
}

// Returns a 32-bit little endian int from src
static uint32_t get_int(void) {
	uint32_t value = static_cast<uint32_t>(get_byte());
	value |= static_cast<uint32_t>(get_byte()) << 8;
	value |= static_cast<uint32_t>(get_byte()) << 16;
	value |= static_cast<uint32_t>(get_byte()) << 24;
	return value;
}

// Returns a string from src
static std::vector<uint8_t> get_string(void) {
	uint32_t len = get_int();
	std::vector<uint8_t> str = {};
	str.resize(len);
	for (uint32_t x = 0; x < len; x++) str[x] = get_byte();
	return str;
}

// Returns a version from src
static BlotterIO::Version get_version(void) {
	return { get_int(), get_int(), get_int(), get_int() };
}

// If the header is correct, this will return true
static bool check_head(void) {
	std::string head;
	head.resize(16);
	for (int x = 0; x < 16; x++) head[x] = get_byte();
	return head == "Logic World save";
}

// If the footer is correct, this will return true
static bool check_foot(void) {
	std::string foot;
	foot.resize(16);
	for (int x = 0; x < 16; x++) foot[x] = get_byte();
	return foot == "redstone sux lol";
}

// Returns a vector of mod versions from src
static std::vector<BlotterIO::ModVersion> get_mod_versions(uint32_t mod_ver_count) {
	std::vector<BlotterIO::ModVersion> mods;
	mods.resize(static_cast<size_t>(mod_ver_count));

	print("Reading mod versions... ");

	for (uint32_t x = 0; x < mod_ver_count; x++) mods[x] = { get_string(), get_version() };

	print("Done\n");

	return mods;
}

// Returns a vector of component IDs from src
static std::vector<BlotterIO::ComponentID> get_component_ids(uint32_t comp_id_count) {
	std::vector<BlotterIO::ComponentID> ids;
	ids.resize(static_cast<size_t>(comp_id_count));

	print("Reading component IDs... ");

	for (uint32_t x = 0; x < comp_id_count; x++) ids[x] = { get_word(), get_string() };

	print("Done\n");

	return ids;
}

// Returns a vector of component data from src
static std::vector<BlotterIO::ComponentData> get_component_data(uint32_t comp_count) {
	std::vector<BlotterIO::ComponentData> data;
	print("Reading component data... ");

	if (comp_count > 0) {
		data.resize(static_cast<size_t>(comp_count));

		for (uint32_t x = 0; x < comp_count; x++) {
			BlotterIO::ComponentData comp = {
				get_int(), get_int(), get_word(),
				{get_int(), get_int(), get_int()},
				{get_int(), get_int(), get_int(), get_int()}
			};

			uint32_t input_count = get_int();
			if (input_count > 0) {
				comp.inputs.resize(static_cast<size_t>(input_count));
				for (uint32_t y = 0; y < input_count; y++) comp.inputs[y] = get_int();
			}

			uint32_t output_count = get_int();
			if (output_count > 0) {
				comp.outputs.resize(static_cast<size_t>(output_count));
				for (uint32_t y = 0; y < output_count; y++) comp.outputs[y] = get_int();
			}

			uint32_t custom_data_length = get_int();
			if (custom_data_length != static_cast<uint32_t>(-1)) {
				comp.custom_data.resize(static_cast<size_t>(custom_data_length));
				for (uint32_t y = 0; y < custom_data_length; y++) comp.custom_data[y] = get_byte();
			}

			data[x] = comp;
		}
	}

	print("Done\n");

	return data;
}

// Returns a vector of wire data from src
static std::vector<BlotterIO::WireData> get_wire_data(uint32_t wire_count) {
	std::vector<BlotterIO::WireData> data;

	print("Reading wire data... ");

	if (wire_count > 0) {
		data.resize(static_cast<size_t>(wire_count));

		for (uint32_t x = 0; x < wire_count; x++) {
			data[x] = {
				{get_byte(), get_int(), get_int()},
				{get_byte(), get_int(), get_int()},
				get_int(), get_int()
			};
		}
	}

	print("Done\n");

	return data;
}

// Returns all the states from a world file
static std::vector<uint8_t> get_world_states(void) {
	std::vector<uint8_t> states = {};

	uint32_t state_count = get_int();

	print("Reading world states, %d bytes... ", state_count);

	if (state_count > 0) {
		states.resize(static_cast<size_t>(state_count));
		for (uint32_t x = 0; x < state_count; x++) states[x] = get_byte();
	}

	print("Done\n");

	return states;
}

static void reset(void) {
	src.clear();
	index = 0;
}

// Returns all the states from a partial world file
static std::vector<uint32_t> get_partial_world_states(void) {
	std::vector<uint32_t> states = {};

	uint32_t state_count = get_int();
	
	print("Reading partial world states, %d bytes... ", state_count);

	if (state_count > 0) {
		states.resize(static_cast<size_t>(state_count));
		for (uint32_t x = 0; x < state_count; x++) states[x] = get_int();
	}

	print("Done\n");

	return states;
}

int BlotterIO::read_world(BlotterIO::WorldFile* file, std::string path, bool log) {
	reset();
	log_status = log;

	std::ifstream fsrc(path, std::ios::binary);
	fsrc.seekg(0, std::ios::end);
	std::size_t fsize = fsrc.tellg();
	fsrc.seekg(0, std::ios::beg);

	src.resize(fsize);
	fsrc.read(src.data(), fsize);
	fsrc.close();
	
	if (!check_head()) return 1;

	BlotterIO::SaveInfo info = {};

	info.save_version = get_byte();
	info.game_version = {get_int(), get_int(), get_int(), get_int()};
	info.save_type = get_byte();
	info.component_count = get_int();
	info.wire_count = get_int();

	uint32_t mod_ver_count = get_int();
	info.mod_versions = get_mod_versions(mod_ver_count);
	uint32_t comp_id_count = get_int();
	info.component_ids = get_component_ids(comp_id_count);

	print("Save info successfully read.\n");

	BlotterIO::WorldData data = {get_component_data(info.component_count), get_wire_data(info.wire_count), get_world_states()};

	print("World data successfully read.\n");

	if (!check_foot()) return 2;

	std::vector<char>().swap(src);

	print("World file successfully read.\n\n");

	BlotterIO::WorldFile result = { info, data };
	*file = result;

	log_status = false;
	reset();

	return 0;
}

int BlotterIO::read_partial_world(BlotterIO::PartialWorldFile* file, std::string path, bool log) {
	reset();
	log_status = log;

	std::ifstream fsrc(path, std::ios::binary);
	fsrc.seekg(0, std::ios::end);
	std::size_t fsize = fsrc.tellg();
	fsrc.seekg(0, std::ios::beg);

	src.resize(fsize);
	fsrc.read(src.data(), fsize);
	fsrc.close();

	if (!check_head()) {
		printf("Header error in: %s\n", path.c_str());
		return 1;
	}

	BlotterIO::SaveInfo info = {};

	info.save_version = get_byte();
	info.game_version = { get_int(), get_int(), get_int(), get_int() };
	info.save_type = get_byte();
	info.component_count = get_int();
	info.wire_count = get_int();

	uint32_t mod_ver_count = get_int();
	info.mod_versions = get_mod_versions(mod_ver_count);
	uint32_t comp_id_count = get_int();
	info.component_ids = get_component_ids(comp_id_count);

	print("Save info successfully read.\n");

	BlotterIO::PartialWorldData data = { get_component_data(info.component_count), get_wire_data(info.wire_count), get_partial_world_states() };

	print("Partial world data successfully read.\n");

	if (!check_foot()) {
		printf("Footer error in: %s\n", path.c_str());
		return 2;
	}

	std::vector<char>().swap(src);

	print("Partial world file successfully read.\n\n");

	BlotterIO::PartialWorldFile result = { info, data };
	*file = result;

	log_status = false;
	reset();

	return 0;
}