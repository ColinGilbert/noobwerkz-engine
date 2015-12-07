#pragma once

#include <vector>
#include <unordered_map>
#include <atomic>

namespace noob
{
	class filesystem
	{
		public:

			class file_id
			{
				friend class filesystem;

				public:
					file_id() : inner(0) {}
				protected:
					size_t inner;
			};

			filesystem() : _prefix("") {}
			void init(const std::string& prefix);
			std::string load_file(const std::string& name, std::ios_base::openmode mode = std::ios_base::in);
			std::string load_temp(noob::filesystem::file_id id);
			
			// Defaults to overwrite
			bool write_file(const std::string& name, const std::string& content, std::ios_base::openmode mode = std::ios_base::out | std::ios_base::trunc);
			file_id write_temp(const std::string& content);
			
			size_t file_size(const std::string& name);
			size_t file_size(noob::filesystem::file_id);

			bool exists(const std::string& name);
			bool exists(file_id);
			// bool enter_directory(const std::string& name);
			// bool directory_up();
			// std::vector<std::string> list_directory();
		
			
		protected:
			std::atomic<size_t> temp_counter;
			std::string _prefix;
			std::unordered_map<size_t, std::string> temp_cache;
	};
}
