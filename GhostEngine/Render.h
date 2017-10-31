#pragma once
#include "GhostEngine.h"
#include "VDeleter.hpp"
#include <regex>


class Render
{
public:
	Render();
	~Render();

	/**
	* Create a graphics pipeline to using as your windows pfd;
	*/
	void createGraphicsPipelines();

	/**
	* Function Helper to read a FileShader.
	* @param const string& filename = A given filename to open.
	* @return vector<char> Return a buffer with file content.
	*/
	static inline std::vector<char> readFile(const std::string& filename)
	{

		std::regex fileextension("spv");
		std::smatch sm;
		
		if (std::regex_search(filename, sm, fileextension, std::regex_constants::match_default) == false)
		{
			ERRORMESSAGE(L"Error in loading correct shader files.\nPlease see if they are in correct extension.", L"Erro Shader Module");
			std::runtime_error("Failed to get a correct shader file.");
		}
			

		

		
		#ifdef NDEBUG
		
		#else

		std::cout << "Filename: " << filename << std::endl;
		std::cout << "Good files found: " << sm.size() << std::endl;
	
	

		//Forloop auto iterator with initialization list
		for (auto& x : sm)
		{
			std::cout << "DEBUG: Extension Match Regex: " << "[" << x << "] " << std::endl;
		}
		#endif

		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			SHOWMESSAGE(L"We cant open current file\n Please check if file is present or acessible.", L"Error in Opening File", MB_ICONERROR | MB_OK);

			std::runtime_error("Failed to open file.");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;

	}



};

