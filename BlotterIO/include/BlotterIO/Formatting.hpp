#ifndef FOXFIREFIVE_BLOTTERIO_FORMATTING
#define FOXFIREFIVE_BLOTTERIO_FORMATTING

#include <BlotterIO/Types.hpp>
#include <string>

namespace BlotterIO {
	std::string format_version(BlotterIO::Version version);
	std::string format_mod_version(BlotterIO::ModVersion mod);
	std::string format_component_id(BlotterIO::ComponentID id);
	std::string format_position(BlotterIO::Position pos);
	std::string format_rotation(BlotterIO::Rotation rot);

	void print_save_info(BlotterIO::SaveInfo& info);
	void print_world_data(BlotterIO::WorldData& data, bool print_input_states = false, bool print_output_states = false, bool print_custom_data = false);
}

#endif // FOXFIREFIVE_BLOTTERIO_FORMATTING