#include "command_line_parser.h"
#include "logger.h"
#include "wav_file.h"
#include <map>

void print_usage();

void print_finish_message(bool success);

int main(int argc, char* argv[])
{
    CommandLineParser command_line_parser(argc, argv);

    if (!command_line_parser.is_valid())
    {
        LOGGER::log_error("Invalid command line argument(s) passed.");
        print_usage();
        print_finish_message(false);
        return 1;
    }

    std::string path = command_line_parser.get_path();
    std::map<std::string, std::string> metadata = command_line_parser.get_metadata();

    LOGGER::log_info("Saving metadata to '" + path + "'...");

    WavFile wav_file;

    if (!wav_file.load(path))
    {
        LOGGER::log_error("Couldn't load file '" + path + "'.");
        print_finish_message(false);
        return 1;
    }

    for (auto it = metadata.begin(); it != metadata.end(); it++)
    {
        if (!wav_file.set_metadata(it->first, it->second))
        {
            LOGGER::log_error("Couldn't set metadata '" + it->first + "' = '" + it->second + "'.");
            print_finish_message(false);
            return 1;
        }
    }

    if (!wav_file.save(path))
    {
        LOGGER::log_error("Couldn't save file '" + path + "'.");
        print_finish_message(false);
        return 1;
    }

    print_finish_message(true);
    return 0;
}

void print_usage()
{
    LOGGER::log_info("Usage: wav-metadata-tagger.exe --path <FILE_PATH> --metadata <KEY_1>=<VALUE 1> --metadata <KEY_2>=<VALUE_2> ... --metadata <KEY_N>=<VALUE_N>");
}

void print_finish_message(bool success)
{
    std::string msg = success ? "Metadata successfully saved to file." : "Errors occurred while saving metadata to file.";
    success ? LOGGER::log_info(msg) : LOGGER::log_error(msg);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
