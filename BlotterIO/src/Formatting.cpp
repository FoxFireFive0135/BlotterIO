#include <BlotterIO/Formatting.hpp>

std::string BlotterIO::format_version(BlotterIO::Version version) {
	std::string str;
	if (version.major != static_cast<uint32_t>(-1)) str += std::to_string(version.major);
	if (version.minor != static_cast<uint32_t>(-1)) str += static_cast<std::string>(".") += std::to_string(version.minor);
	if (version.build != static_cast<uint32_t>(-1)) str += static_cast<std::string>(".") += std::to_string(version.build);
	if (version.revision != static_cast<uint32_t>(-1)) str += static_cast<std::string>(".") += std::to_string(version.revision);
	return str;
}

std::string BlotterIO::format_mod_version(BlotterIO::ModVersion mod) {
	std::string str;

	for (auto c : mod.id) str += static_cast<char>(c);
	str += " - ";
	str += BlotterIO::format_version(mod.version);

	return str;
}

std::string BlotterIO::format_component_id(BlotterIO::ComponentID id) {
	std::string str;

	for (auto c : id.text_id) str += static_cast<char>(c);
	str += " - ";
	str += std::to_string(id.id);

	return str;
}

std::string BlotterIO::format_position(BlotterIO::Position pos) {
	std::string str = "(";

	str += std::to_string(pos.x) += ", ";
	str += std::to_string(pos.y) += ", ";
	str += std::to_string(pos.z) += ")";

	return str;
}

std::string BlotterIO::format_rotation(BlotterIO::Rotation rot) {
	std::string str = "(";

	str += std::to_string(static_cast<float>(rot.x)) += ", ";
	str += std::to_string(static_cast<float>(rot.y)) += ", ";
	str += std::to_string(static_cast<float>(rot.z)) += ", ";
	str += std::to_string(static_cast<float>(rot.w)) += ")";

	return str;
}

void BlotterIO::print_save_info(BlotterIO::SaveInfo& info) {
	printf("Save Format Version: %d\n", info.save_version);
	printf("Game Version: %s\n", BlotterIO::format_version(info.game_version).c_str());
	printf("Save type: %d\n", info.save_type);
	printf("Number of components: %d\n", info.component_count);
	printf("Number of wires: %d\n", info.wire_count);

	printf("Mod versions:\n");
	for (auto& mod : info.mod_versions) printf("\t%s\n", BlotterIO::format_mod_version(mod).c_str());
	printf("Component IDs:\n");
	for (auto& id : info.component_ids) printf("\t%s\n", BlotterIO::format_component_id(id).c_str());

	printf("\n\n");
}

void BlotterIO::print_world_data(BlotterIO::WorldData& data, bool print_input_states, bool print_output_states, bool print_custom_data) {
	printf("World Data:\n");
	int x = 1;
	for (auto& c : data.component_data) {
		printf("\tComponent %d:\n", x);
		printf("\t\tAddress: %d\n", c.address);
		printf("\t\tParent address: %d\n", c.parent_address);
		printf("\t\tComponent ID: %d\n", c.id);
		printf("\t\tPosition: %s\n", BlotterIO::format_position(c.position).c_str());
		printf("\t\tRotation: %s\n", BlotterIO::format_rotation(c.rotation).c_str());
		printf("\t\tInputs: %zu\n", c.inputs.size());

		if (print_input_states) {
			for (auto i : c.inputs) {
				printf("\t\t\t%d\n", i);
			}
		}

		printf("\t\tOutputs: %zu\n", c.outputs.size());

		if (print_output_states) {
			for (auto o : c.outputs) {
				printf("\t\t\t%d\n", o);
			}
		}

		if (c.custom_data.size() > 0) {
			printf("\t\tCustom Data: %zu\n", c.custom_data.size());

			if (print_custom_data) {
				printf("\t\t\t");
				for (auto d : c.custom_data) {
					printf("%02X ", d);
				}
			}
		}
		else {
			printf("\t\tNo custom data\n");
		}

		printf("\n");
		x++;
	}

	printf("\n\n");
}