#include "wav_file.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <unordered_set>

WavFile::WavFile()
{
}

bool WavFile::load(const std::string& file_path)
{
	std::ifstream file(file_path, std::ios::binary);

	if (!file.good())
	{
		LOGGER::log_error("File doesn't exist or otherwise can't load file '" + file_path + "'");
		return false;
	}

	m_file_path = file_path;

	file.unsetf(std::ios::skipws);

	file.seekg(0, file.end);
	std::streampos length = file.tellg();
	file.seekg(0, file.beg);

	LOGGER::log_debug("Length: " + std::to_string(length));

	LOGGER::log_debug("RIFF: " + read_string(file, 4));
	m_file_size = read_int_four_bytes(file);
	LOGGER::log_debug("File Size: " + std::to_string(m_file_size));
	LOGGER::log_debug("Format: " + read_string(file, 4));

	while (file.tellg() >= 0 && file.tellg() < length)
	{
		std::string chunk_id = read_string(file, 4);
		if (file.tellg() < 0 || file.tellg() >= length)
		{
			break;
		}
		size_t chunk_size = read_int_four_bytes(file);
		if (file.tellg() < 0 || file.tellg() >= length)
		{
			break;
		}
		if (chunk_id == "iXML")
		{
			m_ixml_chunk_pos = file.tellg();
			m_ixml_chunk_pos -= 8;
			m_ixml_chunk_size = chunk_size;
			std::string ixml_chunk_content = read_string(file, chunk_size);
			LOGGER::log_debug("Loading iXML content:\n" + ixml_chunk_content);
			if (!m_ixml_chunk_content_root.load_string(ixml_chunk_content.c_str()))
			{
				LOGGER::log_error("The iXML chunk holds invalid XML.");
			}
		}
		file.seekg(chunk_size, std::ios_base::cur);
	}
	return true;
}

bool WavFile::save(const std::string& new_file_path)
{
	if (m_file_path != new_file_path)
	{
		bool copy_file_success = copy_file(m_file_path, new_file_path);
		if (!copy_file_success)
		{
			LOGGER::log_error("Couldn't copy file '" + m_file_path + "' to '" + new_file_path + "'.");
			return false;
		}
	}

	std::ofstream output_file(new_file_path, std::ios::binary | std::ios::out | std::ios::in);

	if (!is_ixml_empty())
	{
		std::string new_ixml_chunk_content = get_ixml_content();
		size_t new_ixml_chunk_size = new_ixml_chunk_content.size();
		if (m_ixml_chunk_pos != -1)
		{
			output_file.seekp(m_ixml_chunk_pos, std::ios_base::beg);
		}
		else
		{
			output_file.seekp(0, std::ios_base::end);
		}
		output_file.write("iXML", 4);
		output_file.write(reinterpret_cast<char*>(&new_ixml_chunk_size), 4);
		output_file.write(new_ixml_chunk_content.c_str(), new_ixml_chunk_size);
		m_file_size -= m_ixml_chunk_size;
		m_file_size += new_ixml_chunk_size;
		if (m_ixml_chunk_pos == -1)
		{
			m_file_size += 8;
		}
		m_ixml_chunk_size = new_ixml_chunk_size;
		output_file.seekp(0, std::ios_base::beg);
		output_file.write("RIFF", 4);
		output_file.write(reinterpret_cast<char*>(&m_file_size), 4);
	}

	output_file.close();

	return true;
}

bool WavFile::set_metadata(const std::string& name, const std::string& value)
{
	static std::unordered_set<std::string> valid_bext_metadata = {
		"BWF_CODING_HISTORY",
		"BWF_DESCRIPTION",
		"BWF_ORIGINATION_DATE",
		"BWF_ORIGINATION_TIME",
		"BWF_ORIGINATOR_REFERENCE",
		"BWF_ORIGINATOR"
	};
	static std::unordered_set<std::string> valid_bwfxml_metadata = {
		"NOTE",
		"PROJECT",
		"SCENE",
		"TAKE",
		"TAPE"
	};
	static std::unordered_set<std::string> valid_steinberg_metadata = {
		"MusicalInstrument",
		"MediaRecordingMethod",
		"MediaArtist",
		"MediaCategoryPost",
		"MediaLibraryName",
		"MusicalTempo",
		"MediaAlbum",
		"AudioCDName",
		"MediaComment",
		"AudioActor",
		"SmfCopyRight",
		"MediaTrackNumber",
		"AudioSoundEditor",
		"AudioEpisode",
		"MediaGenre",
		"MediaRecordingLocation",
		"MediaLibraryManufacturerName",
		"MusicalCharacter",
		"MediaCompany",
		"AudioSoundMixer",
		"MediaTemplateCategory",
		"MusicalStyle",
		"MusicalCategory",
		"MediaTrackTitle"
	};

	auto bext_it = valid_bext_metadata.find(name);
	auto stein_it = valid_steinberg_metadata.find(name);
	auto bwfxml_it = valid_bwfxml_metadata.find(name);

	if (bext_it == valid_bext_metadata.end() && stein_it == valid_steinberg_metadata.end() && bwfxml_it == valid_bwfxml_metadata.end())
	{
		LOGGER::log_error("Unsupported metadata tag '" + name + "' while updating metadata for file '" + m_file_path + "'.");
		return false;
	}

	if (bext_it != valid_bext_metadata.end())
	{
		set_metadata_bwf_bext(name, value);
	}
	else if (stein_it != valid_steinberg_metadata.end())
	{
		set_metadata_steinberg(name, value);
	}
	else
	{
		set_metadata_bwf_root(name, value);
	}
	return true;
}

bool WavFile::is_ixml_empty() const
{
	return !m_ixml_chunk_content_root.child("BWFXML");
}

void WavFile::set_metadata_steinberg(const std::string& name, const std::string& value)
{
	const std::vector<std::string> names = { "BWFXML", "STEINBERG", "ATTR_LIST" };
	pugi::xml_node attr_list_node = get_xml_node_path(names);

	auto attr_node_range = attr_list_node.children("ATTR");
	for (auto it = attr_node_range.begin(); it != attr_node_range.end(); it++)
	{
		if (strcmp(it->child("NAME").text().as_string(), name.c_str()) == 0)
		{
			it->child("TYPE").text().set("string");
			it->child("VALUE").text().set(value.c_str());
			return;
		}
	}

	pugi::xml_node attr_node = attr_list_node.append_child("ATTR");
	attr_node.append_child("NAME").text().set(name.c_str());
	attr_node.append_child("TYPE").text().set("string");
	attr_node.append_child("VALUE").text().set(value.c_str());
}

void WavFile::set_metadata_bwfxml(const std::string& name, const std::string& value, const std::vector<std::string> nodepath)
{
	pugi::xml_node bext_node = get_xml_node_path(nodepath);

	for (auto it = bext_node.begin(); it != bext_node.end(); it++)
	{
		if (strcmp(it->name(), name.c_str()) == 0)
		{
			it->text().set(value.c_str());
			return;
		}
	}
	bext_node.append_child(name.c_str()).text().set(value.c_str());
}

void WavFile::set_metadata_bwf_bext(const std::string& name, const std::string& value)
{
	set_metadata_bwfxml(name, value, std::vector<std::string>{ "BWFXML", "BEXT" });
}

void WavFile::set_metadata_bwf_root(const std::string& name, const std::string& value)
{
	set_metadata_bwfxml(name, value, std::vector<std::string>{ "BWFXML" });
}

pugi::xml_node WavFile::get_xml_node_path(const std::vector<std::string>& names)
{
	pugi::xml_node current_node = m_ixml_chunk_content_root;
	for (std::string name : names)
	{
		pugi::xml_node next_node = current_node.child(name.c_str());
		if (!next_node)
		{
			next_node = current_node.append_child(name.c_str());
		}
		current_node = next_node;
	}
	return current_node;
}

std::string WavFile::read_string(std::ifstream& file_in, size_t num_bytes)
{
	char* buff = new char[num_bytes];
	file_in.read(buff, num_bytes);
	std::string str(buff, num_bytes);
	delete[] buff;
	return str;
}

size_t WavFile::read_int_four_bytes(std::ifstream& file_in)
{
	size_t sz;
	file_in.read(reinterpret_cast<char*>(&sz), 4);
	return sz;
}

std::string WavFile::get_ixml_content() const
{
	std::ostringstream ixml_ostream;
	m_ixml_chunk_content_root.save(ixml_ostream);
	return ixml_ostream.str();
}

bool WavFile::copy_file(const std::string& src_file_path, const std::string& dest_file_path)
{
	std::ifstream src(src_file_path.c_str(), std::ios::binary);
	std::ofstream dest(dest_file_path.c_str(), std::ios::binary);
	dest << src.rdbuf();
	return src && dest;
}