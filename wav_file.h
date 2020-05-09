#pragma once

#include <vector>
#include <string>
#include "pugixml.hpp"

class WavFile
{
public:
	WavFile();
	bool load(const std::string& file_path);
	bool save(const std::string& file_path);
	bool set_metadata(const std::string& name, const std::string& value);

private:
	std::string read_string(std::ifstream& file_in, size_t num_bytes);
	size_t read_int_four_bytes(std::ifstream& file_in);
	std::string get_ixml_content() const;
	bool is_ixml_empty() const;
	pugi::xml_node get_xml_node_path(const std::vector<std::string>& names);
	void set_metadata_steinberg(const std::string& name, const std::string& value);
	void set_metadata_bwfxml(const std::string& name, const std::string& value, const std::vector<std::string> nodepath);
	void set_metadata_bwf_bext(const std::string& name, const std::string& value);
	void set_metadata_bwf_root(const std::string& name, const std::string& value);
	
	static bool copy_file(const std::string& src_file_path, const std::string& dest_file_path);

	std::string m_file_path;
	size_t m_file_size = 0;
	std::streampos m_ixml_chunk_pos = -1;
	size_t m_ixml_chunk_size = 0;
	pugi::xml_document m_ixml_chunk_content_root;
};

