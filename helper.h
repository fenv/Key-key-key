#include <filesystem>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>

template<typename T>
T getvfunc(void* table, unsigned int offset) {
	return *reinterpret_cast<T*>(*(unsigned int*)table + offset);
}

std::string string_format(const std::string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}

bool safe_file_write(std::experimental::filesystem::path file, const char* body, std::string comment) {
	std::experimental::filesystem::path parent = file.parent_path();
	bool created = std::experimental::filesystem::create_directories(parent);

	if(std::experimental::filesystem::exists(parent) || created) {
		FILE* handle = fopen(file.string().c_str(), "w");

		if(comment.empty() == false)
			fwrite(comment.c_str(), comment.size(), 1, handle);

		fwrite(body, strlen(body), 1, handle);
		fwrite("\n\n", 2, 1, handle);
		fclose(handle);
		return true;
	}

	return false;
}

bool safe_file_append(std::experimental::filesystem::path file, const char* body, std::string comment) {
	std::experimental::filesystem::path parent = file.parent_path();
	bool created = std::experimental::filesystem::create_directories(parent);

	if(std::experimental::filesystem::exists(parent) || created) {
		FILE* handle = fopen(file.string().c_str(), "a");

		if(comment.empty() == false)
			fwrite(comment.c_str(), comment.size(), 1, handle);

		fwrite(body, strlen(body), 1, handle);
		fwrite("\n\n", 2, 1, handle);
		fclose(handle);
		return true;
	}

	return false;
}