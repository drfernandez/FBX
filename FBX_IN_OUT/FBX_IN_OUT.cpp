// FBX_IN_OUT.cpp : Defines the exported functions for the DLL application.
//

#include "header.h"
#include "FBX_IN_OUT.h"
#include "FBX Utility.h"
#include "BinaryWriter.h"


FBXINOUT_API BOOL FBXINOUT::ExportFbxDataFromFile(
	const char* in_file_name, 
	const char* out_file_name, 
	const char* out_pose_name,
	const char* out_anim_name)
{
	FbxManager* lSdkManager = nullptr;
	FbxScene* lScene = nullptr;
	bool lResult;
	FBX loader;
	
	loader.InitializeSdkObjects(lSdkManager, lScene);

	FbxString lFilePath("");
	lFilePath = in_file_name;

	if (lFilePath.IsEmpty())
	{
		lResult = false;
		FBXSDK_printf("\n\nUsage: ImportScene <FBX file name>\n\n");
	}
	else
	{
		FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
		lResult = loader.LoadScene(lSdkManager, lScene, lFilePath.Buffer());
	}
	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
	}
	else
	{
		FbxGeometryConverter converter(lSdkManager);
		FbxSystemUnit fbxUnit = lScene->GetGlobalSettings().GetSystemUnit();

		FbxAxisSystem curr_system = lScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem::ECoordSystem coord_system = curr_system.GetCoorSystem();
		if (coord_system != FbxAxisSystem::eLeftHanded)
		{
			//FbxAxisSystem::DirectX.ConvertScene(lScene);

			curr_system = lScene->GetGlobalSettings().GetAxisSystem();
			coord_system = curr_system.GetCoorSystem();
		}
		loader.SetSystemConverted(true);

		FBXSDK_printf("Trying to split meshes per material...\n");
		lResult = converter.SplitMeshesPerMaterial(lScene, false);
		if (lResult)
		{
			FBXSDK_printf("\t Splitting of meshes was successful!\n");
		}
		else
		{
			FBXSDK_printf("\t Could not split meshes per material!\n");
		}

		FBXSDK_printf("Trying to triangulate mesh...\n");
		lResult = converter.Triangulate(lScene, false, true);
		if (lResult)
		{
			FBXSDK_printf("\t Triangulation of mesh was successful!\n");
		}
		else
		{
			FBXSDK_printf("\t Could not triangulate mesh!\n");
		}

		FBXSDK_printf("Removing bad polygons from mesh!\n");
		converter.RemoveBadPolygonsFromMeshes(lScene);

		loader.LoadMetaData(lScene);
		FBXSDK_printf("\n\n");

		if (out_file_name)
		{
			loader.LoadContent(lScene);
		}
		if (out_anim_name)
		{
			loader.LoadAnimationSimple(lScene);
		}
	}


	// write to binary
	FBXSDK_printf("Writing data to binary files...!\n");
	BinaryWriter* IO = BinaryWriter::GetInstance();
	IO->Initialize();
	if (out_file_name)
	{
		IO->WriteMeshToFile(out_file_name, loader.GetMesh());
		FBXSDK_printf("\tWriting mesh data to binary file completed!\n");
	}
	if (out_pose_name)
	{
		IO->WriteBindPoseToFile(out_pose_name, loader.GetBindPose());
		FBXSDK_printf("\tWriting pose data to binary file completed!\n");
	}
	if (out_anim_name)
	{
		IO->WriteAnimationsToFile(out_anim_name, loader.GetAnimations());
		FBXSDK_printf("\tWriting animation data to binary file completed!\n");
	}
	IO->Shutdown();


	loader.DestroySdkObjects(lSdkManager, lResult);

	return TRUE;
}
