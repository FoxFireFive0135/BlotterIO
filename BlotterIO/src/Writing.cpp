#include <string>
#include <cstdio>
#include <fstream>
#include <cstdarg>
#include <filesystem>

#include <BlotterIO/Writing.hpp>

static std::vector<uint8_t> src;
static bool log_status = false;

static void print(const char* format, ...) {
	va_list args;
	va_start(args, format);

	if (log_status) vprintf(format, args);

	va_end(args);
}

static void write_byte(int byte) {
	src.push_back(static_cast<uint8_t>(byte));
}

static void write_word(uint16_t word) {
	write_byte(word & 0x00FF);
	write_byte(word >> 8);
}

static void write_int(uint32_t val) {
	write_byte(val & 0x000000FF);
	write_byte(val >> 8);
	write_byte(val >> 16);
	write_byte(val >> 24);
}

static void write_string(std::vector<uint8_t>& str) {
	write_int(static_cast<uint32_t>(str.size()));

	for (auto c : str) {
		write_byte(c);
	}
}

static void write_version(BlotterIO::Version version) {
	write_int(version.major);
	write_int(version.minor);
	write_int(version.build);
	write_int(version.revision);
}

static void write_position(BlotterIO::Position pos) {
	write_int(pos.x);
	write_int(pos.y);
	write_int(pos.z);
}

static void write_rotation(BlotterIO::Rotation rot) {
	write_int(rot.x);
	write_int(rot.y);
	write_int(rot.z);
	write_int(rot.w);
}

static void write_peg_address(BlotterIO::PegAddress pa) {
	write_byte(pa.type);
	write_int(pa.address);
	write_int(pa.index);
}

static void write_header(void) {
	std::string head = "Logic World save";
	for (auto c : head) write_byte(c);
}

static void write_footer(void) {
	std::string foot = "redstone sux lol";
	for (auto c : foot) write_byte(c);
}

int BlotterIO::write_world(BlotterIO::WorldFile& file, std::string path, bool log) {
	src.clear();
	log_status = log;

	write_header();

	// Write save info
	write_byte(file.info.save_version);
	write_version(file.info.game_version);
	write_byte(file.info.save_type);
	write_int(file.info.component_count);
	write_int(file.info.wire_count);
	
	
	// Write mod versions for save info
	write_int(static_cast<uint32_t>(file.info.mod_versions.size()));

	for (auto& mv : file.info.mod_versions) {
		write_string(mv.id);
		write_version(mv.version);
	}

	// Write component ID map
	write_int(static_cast<uint32_t>(file.info.component_ids.size()));

	for (auto& id : file.info.component_ids) {
		write_word(id.id);
		write_string(id.text_id);
	}

	// Write component data
	for (auto& c : file.data.component_data) {
		write_int(c.address);
		write_int(c.parent_address);
		write_word(c.id);
		write_position(c.position);
		write_rotation(c.rotation);

		write_int(static_cast<uint32_t>(c.inputs.size()));
		for (auto i : c.inputs) write_int(i);

		write_int(static_cast<uint32_t>(c.outputs.size()));
		for (auto o : c.outputs) write_int(o);

		if (c.custom_data.size() == 0) write_int(static_cast<uint32_t>(-1));
		else write_int(static_cast<uint32_t>(c.custom_data.size()));
		for (auto cd : c.custom_data) write_byte(cd);
	}

	// Write wire data
	for (auto& wd : file.data.wire_data) {
		write_peg_address(wd.start);
		write_peg_address(wd.end);
		write_int(wd.id);
		write_int(wd.angle);
	}

	// Write circuit states
	write_int(static_cast<uint32_t>(file.data.states.size()));
	for (auto s : file.data.states) write_byte(s);

	write_footer();

	std::filesystem::path filePath(path);
	std::filesystem::path dir = filePath.parent_path();

	if (!dir.empty() && !std::filesystem::exists(dir)) {
		std::error_code ec;
		std::filesystem::create_directories(dir, ec);
		if (ec) return 2;
	}

	std::ofstream fdst(filePath, std::ios::binary);
	if (!fdst) return 1;

	fdst.write(reinterpret_cast<const char*>(src.data()),
		static_cast<std::streamsize>(src.size()));
	fdst.close();


	log_status = false;
	src.clear();
	return 0;
}

int BlotterIO::write_partial_world(BlotterIO::PartialWorldFile& file, std::string path, bool log) {
	src.clear();
	log_status = log;

	write_header();

	// Write save info
	write_byte(file.info.save_version);
	write_version(file.info.game_version);
	write_byte(file.info.save_type);
	write_int(file.info.component_count);
	write_int(file.info.wire_count);


	// Write mod versions for save info
	write_int(static_cast<uint32_t>(file.info.mod_versions.size()));

	for (auto& mv : file.info.mod_versions) {
		write_string(mv.id);
		write_version(mv.version);
	}

	// Write component ID map
	write_int(static_cast<uint32_t>(file.info.component_ids.size()));

	for (auto& id : file.info.component_ids) {
		write_word(id.id);
		write_string(id.text_id);
	}

	// Write component data
	for (auto& c : file.data.component_data) {
		write_int(c.address);
		write_int(c.parent_address);
		write_word(c.id);
		write_position(c.position);
		write_rotation(c.rotation);

		write_int(static_cast<uint32_t>(c.inputs.size()));
		for (auto i : c.inputs) write_int(i);

		write_int(static_cast<uint32_t>(c.outputs.size()));
		for (auto o : c.outputs) write_int(o);

		if (c.custom_data.size() == 0) write_int(static_cast<uint32_t>(-1));
		else write_int(static_cast<uint32_t>(c.custom_data.size()));
		for (auto cd : c.custom_data) write_byte(cd);
	}

	// Write wire data
	for (auto& wd : file.data.wire_data) {
		write_peg_address(wd.start);
		write_peg_address(wd.end);
		write_int(wd.id);
		write_int(wd.angle);
	}

	// Write circuit states
	write_int(static_cast<uint32_t>(file.data.states.size()));
	for (auto s : file.data.states) write_int(s);

	write_footer();

	std::filesystem::path filePath(path);
	std::filesystem::path dir = filePath.parent_path();

	if (!dir.empty() && !std::filesystem::exists(dir)) {
		std::error_code ec;
		std::filesystem::create_directories(dir, ec);
		if (ec) return 2;
	}

	std::ofstream fdst(filePath, std::ios::binary);
	if (!fdst) return 1;

	fdst.write(reinterpret_cast<const char*>(src.data()), static_cast<std::streamsize>(src.size()));
	fdst.close();

	log_status = false;
	src.clear();
	return 0;
}