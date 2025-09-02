#include <bits/stdc++.h>
#include <filesystem>

#ifndef CPP_PROJECT_SOURCE_DIR
#define CPP_PROJECT_SOURCE_DIR "unknown"
#endif

void update_name_and_exec(std::string &file_content, const std::string &project_name, const std::string &project_exec)
{
    // Update the project name and executable in the file content
    std::string old_name = "{{project_name}}";
    std::string old_exec = "{{project_exec}}";
    std::string new_name = project_name;
    std::string new_exec = project_exec;

    // Replace project name
    size_t pos = file_content.find(old_name);
    while (pos != std::string::npos)
    {
        file_content.replace(pos, old_name.length(), new_name);
        pos = file_content.find(old_name, pos + new_name.length());
    }

    // Replace project executable
    pos = file_content.find(old_exec);
    while (pos != std::string::npos)
    {
        file_content.replace(pos, old_exec.length(), new_exec);
        pos = file_content.find(old_exec, pos + new_exec.length());
    }
}

std::string read_file(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file)
    {
        std::cerr << "Error reading file: " << file_path << std::endl;
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void write_file(const std::string &file_path, const std::string &content)
{
    std::ofstream file(file_path);
    if (!file)
    {
        std::cerr << "Error writing file: " << file_path << std::endl;
        return;
    }
    file << content;
}

bool should_process_template(const std::string &file_path)
{
    return true;
}

void copy_template_recursively(const std::string &source_dir, const std::string &dest_dir,
                               const std::string &project_name, const std::string &project_exec)
{
    namespace fs = std::filesystem;

    try
    {
        // Create destination directory if it doesn't exist
        fs::create_directories(dest_dir);

        // Iterate through all entries in source directory
        for (const auto &entry : fs::recursive_directory_iterator(source_dir))
        {
            const auto &source_path = entry.path();

            // Calculate relative path from source_dir
            auto relative_path = fs::relative(source_path, source_dir);
            auto dest_path = fs::path(dest_dir) / relative_path;

            if (entry.is_directory())
            {
                // Create directory
                fs::create_directories(dest_path);
                std::cout << "Created directory: " << dest_path << std::endl;
            }
            else if (entry.is_regular_file())
            {
                // Read file content
                std::string file_content = read_file(source_path.string());

                if (file_content.empty() && fs::file_size(source_path) > 0)
                {
                    std::cerr << "Warning: Could not read file " << source_path << std::endl;
                    continue;
                }

                // Process template if it's a text file
                if (should_process_template(source_path.string()))
                {
                    update_name_and_exec(file_content, project_name, project_exec);
                }

                // Write to destination
                write_file(dest_path.string(), file_content);
                std::cout << "Copied file: " << dest_path << std::endl;

                // Make shell scripts executable
                if (source_path.extension() == ".sh")
                {
                    fs::permissions(dest_path, fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                                    fs::perm_options::add);
                }
            }
        }
    }
    catch (const fs::filesystem_error &ex)
    {
        std::cerr << "Filesystem error: " << ex.what() << std::endl;
    }
}

void build_cmake(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " cmake <project_name> <project_exec>" << std::endl;
        return;
    }

    std::string project_name = argv[2];
    std::string project_exec = argv[3];

    std::string source_template_dir = CPP_PROJECT_SOURCE_DIR + std::string("templates/cmake");

    std::cout << "Creating CMake project '" << project_name << "' with executable '" << project_exec << "'..." << std::endl;
    std::cout << "Source template directory: " << source_template_dir << std::endl;

    // Check if template directory exists
    if (!std::filesystem::exists(source_template_dir))
    {
        std::cerr << "Error: Template directory does not exist: " << source_template_dir << std::endl;
        return;
    }

    // Copy all template files recursively
    copy_template_recursively(source_template_dir, project_name, project_name, project_exec);

    std::cout << "CMake project '" << project_name << "' with executable '" << project_exec << "' created successfully." << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <what_project>" << std::endl;
        return 1;
    }

    if (argc >= 2)
    {
        std::string what_project = argv[1];

        if (what_project == "cmake")
        {
            build_cmake(argc, argv);
        }
        else
        {
            std::cerr << "Unknown project type: " << what_project << std::endl;
            std::cerr << "Available project: " << std::endl;
            std::cerr << "  - cmake" << std::endl;
            return 1;
        }
    }
    return 0;
}