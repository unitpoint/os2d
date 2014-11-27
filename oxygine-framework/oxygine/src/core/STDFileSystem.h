#pragma once
#include "oxygine_include.h"
#include "FileSystem.h"
namespace oxygine
{
	namespace file
	{
		class STDFileSystem: public FileSystem
		{
		public:
			STDFileSystem(bool readonly);

			string getFullPath(const char *path);
			void setPath(const char *folder);

		protected:
			string _path;			
			char* _getFullPath(const char *path, char buff[512]);

			status _open(const char *file_, const char *mode, error_policy ep, fileHandle*& fh);
			status _deleteFile(const char* file);
			status _renameFile(const char* src, const char *dest);
			status _makeDirectory(const char* path);
			status _deleteDirectory(const char* path);

			bool _isExists(const char *file);
		};
	}	
}