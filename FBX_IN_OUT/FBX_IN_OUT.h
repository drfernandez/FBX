// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the FBXINOUT_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// FBXINOUT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FBXINOUT_EXPORTS
#define FBXINOUT_API __declspec(dllexport)
#else
#define FBXINOUT_API __declspec(dllimport)
#endif

extern "C"
{
	namespace FBXINOUT
	{
		FBXINOUT_API BOOL ExportFbxDataFromFile(
			const char* in_file_name, 
			const char* out_file_name, 
			const char* out_pose_name,
			const char* out_anim_name
		);
	};
};