#include "FBX Utility.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif


void LoadChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve), void(*DisplayListCurve) (FbxAnimCurve* pCurve, FbxProperty* pProperty), bool isSwitcher);
void LoadCurveKeys(FbxAnimCurve* pCurve);
void LoadListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty);


static int InterpolationFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
	if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
	if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
	return 0;
}

static int ConstantmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
	if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
	return 0;
}

static int TangentmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
	if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
	if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
	if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
	if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
	if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
	return 0;
}

static int TangentweightFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
	if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
	if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
	return 0;
}

static int TangentVelocityFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
	if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
	if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
	return 0;
}

void FBXINOUT::FBX::InitializeSdkObjects(FbxManager *& pManager, FbxScene *& pScene)
{
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}

bool FBXINOUT::FBX::LoadScene(FbxManager * pManager, FbxDocument * pScene, const char * pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	double rad = (3.1415 / 180.0) * 180.0;
	m_Rotate180 = FbxMatrix(
		std::cos(rad), 0.0, -std::sin(rad), 0.0,
		0.0, 1.0, 0.0, 0.0,
		std::sin(rad), 0.0, std::cos(rad), 0.0,
		0.0, 0.0, 0.0, 1.0
	);


	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for (i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

void FBXINOUT::FBX::DestroySdkObjects(FbxManager * pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

void FBXINOUT::FBX::FlattenBindPoseHierarchy(void)
{
	for (size_t i = 1; i < m_MeshBindPose.size(); i++)
	{
		for (size_t j = 0; j < m_MeshBindPose.size() - 1; j++)
		{
			if (m_MeshBindPose[i].parent_node == m_MeshBindPose[j].node)
			{
				m_MeshBindPose[i].parent_index = (int)j;
			}
		}
	}
}

void FBXINOUT::FBX::RemoveWeakestWeights(Influence & influence)
{
	if (influence.joint.size() > 4)
	{
		std::vector<std::pair<unsigned int, float>> influence_vector;
		// populate the influence vector
		for (size_t i = 0; i < influence.joint.size(); i++)
		{
			std::pair<unsigned int, float> inf;
			inf.first = influence.joint[(int)i];
			inf.second = influence.weight[(int)i];
			influence_vector.push_back(inf);
		}
		// sort from highest to lowest
		std::sort(influence_vector.begin(), influence_vector.end(), 
			[](std::pair<unsigned int, float> a, std::pair<unsigned int, float> b)
			{
				return a.second > b.second;
			}
		);

		// remove any past 4
		do
		{
			influence_vector.erase(influence_vector.end() - 1);
		} while (influence_vector.size() > 4);

		// normalize the values
		float sum = 0;
		for (size_t i = 0; i < influence_vector.size(); i++)
		{
			sum += influence_vector[(int)i].second;
		}

		for (size_t i = 0; i < influence_vector.size(); i++)
		{
			influence_vector[(int)i].second /= sum;
		}
		// clear the reference parameter and reinsert data
		influence.joint.clear();
		influence.weight.clear();

		for (size_t i = 0; i < influence_vector.size(); i++)
		{
			influence.joint.push_back(influence_vector[(int)i].first);
			influence.weight.push_back(influence_vector[(int)i].second);
		}
		// debugging variable
		int d = 0;
	}
	else
	{
		// sort the vector by strongest weight to weakest weight
		std::vector<std::pair<unsigned int, float>> influence_vector;
		// populate the influence vector
		for (size_t i = 0; i < influence.joint.size(); i++)
		{
			std::pair<unsigned int, float> inf;
			inf.first = influence.joint[(int)i];
			inf.second = influence.weight[(int)i];
			influence_vector.push_back(inf);
		}
		// sort from highest to lowest
		std::sort(influence_vector.begin(), influence_vector.end(), [](std::pair<unsigned int, float> a, std::pair<unsigned int, float> b)
		{
			return a.second > b.second;
		});
		// clear the reference parameter and reinsert data
		influence.joint.clear();
		influence.weight.clear();

		for (size_t i = 0; i < influence_vector.size(); i++)
		{
			influence.joint.push_back(influence_vector[(int)i].first);
			influence.weight.push_back(influence_vector[(int)i].second);
		}

		// debugging variable
		int d = 0;
	}
}

void FBXINOUT::FBX::LoadContent(FbxScene * pScene)
{
	int i;
	FbxNode* lNode = pScene->GetRootNode();

	if (lNode)
	{
		// TODO: make loop for skeleton then mesh data
		for (i = 0; i < lNode->GetChildCount(); i++)
		{
			LoadSkeletonContent(lNode->GetChild(i));
		}

		for (i = 0; i < lNode->GetChildCount(); i++)
		{
			LoadContent(lNode->GetChild(i));
		}

		FlattenBindPoseHierarchy();
	}
}

void FBXINOUT::FBX::LoadBindPose(FbxScene * pScene)
{
	int lPoseCount;
	FbxString  lName;
	
	lPoseCount = pScene->GetPoseCount();

	for (int i = 0; i < lPoseCount; i++)
	{
		FbxPose* lPose = pScene->GetPose(i);

		if (lPose->IsBindPose())
		{
			lName = lPose->GetName();
			int pose_size = lPose->GetCount();
			for (int j = 0; j < pose_size; j++)
			{
				lName = lPose->GetNodeName(j).GetCurrentName();

				FbxString lMatrixValue;
				FbxMatrix lMatrix = lPose->GetMatrix(j);

				Joint joint;
				FbxNode* pNode = lPose->GetNode(j);
				joint.matrix = pNode->EvaluateGlobalTransform();
				joint.name = pNode->GetName();
				joint.node = pNode;
				joint.parent_index = -1;
				joint.parent_node = pNode->GetParent();
				//m_MeshBindPose.push_back(joint);
			}
		}
		break;
	}

	lPoseCount = pScene->GetCharacterPoseCount();

	for (int i = 0; i < lPoseCount; i++)
	{
		FbxCharacterPose* lPose = pScene->GetCharacterPose(i);
		FbxCharacter*     lCharacter = lPose->GetCharacter();

		if (!lCharacter) break;


		FbxCharacterLink lCharacterLink;
		FbxCharacter::ENodeId  lNodeId = FbxCharacter::eHips;

		while (lCharacter->GetCharacterLink(lNodeId, &lCharacterLink))
		{
			FbxAMatrix& lGlobalPosition = lCharacterLink.mNode->EvaluateGlobalTransform(FBXSDK_TIME_ZERO);

			FbxString lMatrixValue;
			lNodeId = FbxCharacter::ENodeId(int(lNodeId) + 1);
		}
	}

	//FlattenBindPoseHierarchy();
}

void FBXINOUT::FBX::LoadAnimation(FbxScene * pScene)
{
	int i;
	for (i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);

		FbxString lOutputString = "Animation Stack Name: ";
		lOutputString += lAnimStack->GetName();
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);

		LoadAnimation(lAnimStack, pScene->GetRootNode());
	}
}

void FBXINOUT::FBX::LoadAnimationSimple(FbxScene * pScene)
{
	int numStacks = pScene->GetSrcObjectCount<FbxAnimStack>();

	for (int i = 0; i < numStacks; i++)
	{
		Animation anim;

		FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
		FbxString lAnimStackName = lAnimStack->GetName();
		FbxTakeInfo* lTakeInfo = pScene->GetTakeInfo(lAnimStackName);

		if (lAnimStackName != pScene->ActiveAnimStackName.Get())
		{
			continue;
		}

		anim.name = lAnimStackName.Buffer();
		anim.duration = (float)lTakeInfo->mLocalTimeSpan.GetDuration().GetSecondDouble();

		int num_frames = (int)lTakeInfo->mLocalTimeSpan.GetDuration().GetFrameCount(FbxTime::eFrames24)+1;


		for (size_t j = 0; j < num_frames; j++)
		{
			KeyFrame keyFrame;

			FbxTime keyTime;
			keyTime.SetFrame(j, FbxTime::eFrames24);
			keyFrame.time = (float)keyTime.GetSecondDouble();

			size_t num_joints = m_MeshBindPose.size();
			for (size_t k = 0; k < num_joints; k++)
			{
				Joint joint = m_MeshBindPose[k];
				FbxAMatrix curr_matrix = m_MeshBindPose[k].node->EvaluateGlobalTransform(keyTime);
				joint.matrix = (m_SystemConverted) ? ConvertMatrixToSystemCoord(curr_matrix) : curr_matrix;
				keyFrame.joints.push_back(joint);
			}
			anim.keyframes.push_back(keyFrame);
		}
		m_Animations.push_back(anim);
	}

	//int d = 0;
}

void FBXINOUT::FBX::ResetMesh(Mesh & current_mesh)
{
	current_mesh.m_IndexOffset = -1;
	current_mesh.m_IndexTotal = -1;
	current_mesh.m_Indicies.clear();
	current_mesh.m_Influences.clear();
	current_mesh.m_MeshName = "";
	current_mesh.m_TextureDiffuseID = -1;
	current_mesh.m_TextureDiffuseName = "";
	current_mesh.m_TextureNormalID = -1;
	current_mesh.m_TextureNormalName = "";
	current_mesh.m_TextureSpecularID = -1;
	current_mesh.m_TextureSpecularName = "";
	current_mesh.m_Vertices.clear();
	current_mesh.m_VertOffset = -1;
	current_mesh.m_VertTotal = -1;
	current_mesh.m_WorldMatrix = FbxAMatrix();
}

void FBXINOUT::FBX::LoadSkeletonContent(FbxNode * pNode)
{
	FbxNodeAttribute::EType lAttributeType;
	int i;

	if (pNode->GetNodeAttribute() == NULL)
	{
		FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
		default:
			break;
		case FbxNodeAttribute::eSkeleton:
			LoadSkeleton(pNode);
			break;
		}
	}

	// if this node has a parent
	// load content on it's children
	for (i = 0; i < pNode->GetChildCount(); i++)
	{
		LoadSkeletonContent(pNode->GetChild(i));
	}
}

void FBXINOUT::FBX::LoadContent(FbxNode * pNode)
{
	FbxNodeAttribute::EType lAttributeType;
	int i;

	if (pNode->GetNodeAttribute() == NULL)
	{
		FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
		default:
			break;
		case FbxNodeAttribute::eMarker:
			LoadMarker(pNode);
			break;

		//case FbxNodeAttribute::eSkeleton:
		//	LoadSkeleton(pNode);
		//	break;

		case FbxNodeAttribute::eMesh:
			LoadMesh(pNode);
			break;

		case FbxNodeAttribute::eNurbs:
			LoadNurb(pNode);
			break;

		case FbxNodeAttribute::ePatch:
			LoadPatch(pNode);
			break;

		case FbxNodeAttribute::eCamera:
			LoadCamera(pNode);
			break;

		case FbxNodeAttribute::eLight:
			LoadLight(pNode);
			break;

		case FbxNodeAttribute::eLODGroup:
			LoadLodGroup(pNode);
			break;
		}
	}

	//LoadUserProperties(pNode);
	//LoadTarget(pNode);
	//LoadPivotsAndLimits(pNode);
	//LoadTransformPropagation(pNode);
	//LoadGeometricTransform(pNode);

	// if this node has a parent
	// load content on it's children

	for (i = 0; i < pNode->GetChildCount(); i++)
	{
		LoadContent(pNode->GetChild(i));
	}
}

void FBXINOUT::FBX::LoadMetaData(FbxScene * pScene)
{
	FbxDocumentInfo* sceneInfo = pScene->GetSceneInfo();
	if (sceneInfo)
	{
		FBXSDK_printf("\n\n--------------------\nMeta-Data\n--------------------\n\n");
		FBXSDK_printf("    Title: %s\n", sceneInfo->mTitle.Buffer());
		FBXSDK_printf("    Subject: %s\n", sceneInfo->mSubject.Buffer());
		FBXSDK_printf("    Author: %s\n", sceneInfo->mAuthor.Buffer());
		FBXSDK_printf("    Keywords: %s\n", sceneInfo->mKeywords.Buffer());
		FBXSDK_printf("    Revision: %s\n", sceneInfo->mRevision.Buffer());
		FBXSDK_printf("    Comment: %s\n", sceneInfo->mComment.Buffer());

		FbxThumbnail* thumbnail = sceneInfo->GetSceneThumbnail();
		if (thumbnail)
		{
			FBXSDK_printf("    Thumbnail:\n");

			switch (thumbnail->GetDataFormat())
			{
			case FbxThumbnail::eRGB_24:
				FBXSDK_printf("        Format: RGB\n");
				break;
			case FbxThumbnail::eRGBA_32:
				FBXSDK_printf("        Format: RGBA\n");
				break;
			}

			switch (thumbnail->GetSize())
			{
			default:
				break;
			case FbxThumbnail::eNotSet:
				FBXSDK_printf("        Size: no dimensions specified (%ld bytes)\n", thumbnail->GetSizeInBytes());
				break;
			case FbxThumbnail::e64x64:
				FBXSDK_printf("        Size: 64 x 64 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
				break;
			case FbxThumbnail::e128x128:
				FBXSDK_printf("        Size: 128 x 128 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
			}
		}
	}
}

void FBXINOUT::FBX::LoadMarker(FbxNode * pNode)
{
}

void FBXINOUT::FBX::LoadSkeleton(FbxNode * pNode)
{
	Joint joint;
	FbxAMatrix curr_matrix = pNode->EvaluateGlobalTransform();
	joint.matrix = (m_SystemConverted) ? ConvertMatrixToSystemCoord(curr_matrix) : curr_matrix;
	//joint.matrix = pNode->EvaluateGlobalTransform();
	joint.name = pNode->GetName();
	joint.node = pNode;
	joint.parent_index = -1;
	joint.parent_node = pNode->GetParent();
	m_MeshBindPose.push_back(joint);
}

void FBXINOUT::FBX::LoadMesh(FbxNode * pNode)
{
	FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();

	DisplayString("Mesh Name: ", (char *)pNode->GetName(), "");
	//DisplayMetaDataConnections(lMesh);
	LoadLink(lMesh);
	LoadPolygons(lMesh);
	//LoadMaterialMapping(lMesh);
	LoadMaterial(lMesh);
	LoadTexture(lMesh);
	LoadMaterialConnections(lMesh);
	//DisplayShape(lMesh);
	//DisplayCache(lMesh);

	m_MeshIndex++;
}

void FBXINOUT::FBX::LoadNurb(FbxNode * pNode)
{
}

void FBXINOUT::FBX::LoadPatch(FbxNode * pNode)
{
}

void FBXINOUT::FBX::LoadCamera(FbxNode * pNode)
{
}

void FBXINOUT::FBX::LoadLight(FbxNode * pNode)
{
}

void FBXINOUT::FBX::LoadLodGroup(FbxNode * pNode)
{
}

void FBXINOUT::FBX::LoadAnimation(FbxAnimStack * pAnimStack, FbxNode * pNode, bool isSwitcher)
{
	int l;
	int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
	int nbAudioLayers = pAnimStack->GetMemberCount<FbxAudioLayer>();

	for (l = 0; l < nbAnimLayers; l++)
	{
		FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);
		LoadAnimation(lAnimLayer, pNode, isSwitcher);
	}

	for (l = 0; l < nbAudioLayers; l++)
	{
		FbxAudioLayer* lAudioLayer = pAnimStack->GetMember<FbxAudioLayer>(l);
		LoadAnimation(lAudioLayer, isSwitcher);
	}
}

void FBXINOUT::FBX::LoadAnimation(FbxAnimLayer * pAnimLayer, FbxNode * pNode, bool isSwitcher)
{
	int lModelCount;
	FbxString lOutputString;

	lOutputString = "     Node Name: ";
	lOutputString += pNode->GetName();
	lOutputString += "\n\n";
	FBXSDK_printf(lOutputString);

	LoadChannels(pNode, pAnimLayer, LoadCurveKeys, LoadListCurveKeys, isSwitcher);
	FBXSDK_printf("\n");

	for (lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
	{
		LoadAnimation(pAnimLayer, pNode->GetChild(lModelCount), isSwitcher);
	}
}

void FBXINOUT::FBX::LoadAnimation(FbxAudioLayer * pAudioLayer, bool isSwitcher)
{
	int lClipCount;
	FbxString lOutputString;

	lClipCount = pAudioLayer->GetMemberCount<FbxAudio>();

	lOutputString = "     Name: ";
	lOutputString += pAudioLayer->GetName();
	lOutputString += "\n\n";
	lOutputString += "     Nb Audio Clips: ";
	lOutputString += lClipCount;
	lOutputString += "\n";
	FBXSDK_printf(lOutputString);

	for (int i = 0; i < lClipCount; i++)
	{
		FbxAudio* lClip = pAudioLayer->GetMember<FbxAudio>(i);
		lOutputString = "        Clip[";
		lOutputString += i;
		lOutputString += "]:\t";
		lOutputString += lClip->GetName();
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}

void LoadChannels(FbxNode * pNode, FbxAnimLayer * pAnimLayer, void(*DisplayCurve)(FbxAnimCurve *pCurve), void(*DisplayListCurve)(FbxAnimCurve *pCurve, FbxProperty *pProperty), bool isSwitcher)
{
	FbxAnimCurve* lAnimCurve = NULL;

	// Display general curves.
	if (!isSwitcher)
	{
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			FBXSDK_printf("        TX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        TY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        TZ\n");
			DisplayCurve(lAnimCurve);
		}

		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RZ\n");
			DisplayCurve(lAnimCurve);
		}

		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SZ\n");
			DisplayCurve(lAnimCurve);
		}
	}

	// Display curves specific to a light or marker.
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_RED);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Red\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_GREEN);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Green\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_BLUE);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Blue\n");
			DisplayCurve(lAnimCurve);
		}

		// Display curves specific to a light.
		FbxLight* light = pNode->GetLight();
		if (light)
		{
			lAnimCurve = light->Intensity.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Intensity\n");
				DisplayCurve(lAnimCurve);
			}

			lAnimCurve = light->OuterAngle.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Outer Angle\n");
				DisplayCurve(lAnimCurve);
			}

			lAnimCurve = light->Fog.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Fog\n");
				DisplayCurve(lAnimCurve);
			}
		}

		// Display curves specific to a camera.
		FbxCamera* camera = pNode->GetCamera();
		if (camera)
		{
			lAnimCurve = camera->FieldOfView.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View\n");
				DisplayCurve(lAnimCurve);
			}

			lAnimCurve = camera->FieldOfViewX.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View X\n");
				DisplayCurve(lAnimCurve);
			}

			lAnimCurve = camera->FieldOfViewY.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View Y\n");
				DisplayCurve(lAnimCurve);
			}

			lAnimCurve = camera->OpticalCenterX.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Optical Center X\n");
				DisplayCurve(lAnimCurve);
			}

			lAnimCurve = camera->OpticalCenterY.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Optical Center Y\n");
				DisplayCurve(lAnimCurve);
			}

			lAnimCurve = camera->Roll.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Roll\n");
				DisplayCurve(lAnimCurve);
			}
		}

		// Display curves specific to a geometry.
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			FbxGeometry* lGeometry = (FbxGeometry*)lNodeAttribute;

			int lBlendShapeDeformerCount = lGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
			for (int lBlendShapeIndex = 0; lBlendShapeIndex < lBlendShapeDeformerCount; ++lBlendShapeIndex)
			{
				FbxBlendShape* lBlendShape = (FbxBlendShape*)lGeometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);

				int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
				for (int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; ++lChannelIndex)
				{
					FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
					const char* lChannelName = lChannel->GetName();

					lAnimCurve = lGeometry->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer, true);
					if (lAnimCurve)
					{
						FBXSDK_printf("        Shape %s\n", lChannelName);
						DisplayCurve(lAnimCurve);
					}
				}
			}
		}
	}

	// Display curves specific to properties
	FbxProperty lProperty = pNode->GetFirstProperty();
	while (lProperty.IsValid())
	{
		if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined))
		{
			FbxString lFbxFCurveNodeName = lProperty.GetName();
			FbxAnimCurveNode* lCurveNode = lProperty.GetCurveNode(pAnimLayer);

			if (!lCurveNode) {
				lProperty = pNode->GetNextProperty(lProperty);
				continue;
			}

			FbxDataType lDataType = lProperty.GetPropertyDataType();
			if (lDataType.GetType() == eFbxBool || lDataType.GetType() == eFbxDouble || lDataType.GetType() == eFbxFloat || lDataType.GetType() == eFbxInt)
			{
				FbxString lMessage;

				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				};

				//DisplayString(lMessage.Buffer());

				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
						DisplayCurve(lAnimCurve);
				}
			}
			else if (lDataType.GetType() == eFbxDouble3 || lDataType.GetType() == eFbxDouble4 || lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT))
			{
				char* lComponentName1 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_RED : (char*)"X";
				char* lComponentName2 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_GREEN : (char*)"Y";
				char* lComponentName3 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_BLUE : (char*)"Z";
				FbxString      lMessage;

				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				}
				//DisplayString(lMessage.Buffer());

				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
					{
						//DisplayString("        Component ", lComponentName1);
						DisplayCurve(lAnimCurve);
					}
				}

				for (int c = 0; c < lCurveNode->GetCurveCount(1U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(1U, c);
					if (lAnimCurve)
					{
						//DisplayString("        Component ", lComponentName2);
						DisplayCurve(lAnimCurve);
					}
				}

				for (int c = 0; c < lCurveNode->GetCurveCount(2U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(2U, c);
					if (lAnimCurve)
					{
						//DisplayString("        Component ", lComponentName3);
						DisplayCurve(lAnimCurve);
					}
				}
			}
			else if (lDataType.GetType() == eFbxEnum)
			{
				FbxString lMessage;

				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				};
				//DisplayString(lMessage.Buffer());

				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
						DisplayListCurve(lAnimCurve, &lProperty);
				}
			}
		}

		lProperty = pNode->GetNextProperty(lProperty);
	} // while
}

void LoadCurveKeys(FbxAnimCurve * pCurve)
{
	static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
	static const char* constantMode[] = { "?", "Standard", "Next" };
	static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
	static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

	FbxTime   lKeyTime;
	float   lKeyValue;
	char    lTimeString[256];
	FbxString lOutputString;
	int     lCount;

	int lKeyCount = pCurve->KeyGetCount();

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);

		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " [ ";
		lOutputString += interpolation[InterpolationFlagToIndex(pCurve->KeyGetInterpolation(lCount))];
		if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			lOutputString += " | ";
			lOutputString += constantMode[ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount))];
		}
		else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			lOutputString += " | ";
			lOutputString += cubicMode[TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount))];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode())];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode())];
		}
		lOutputString += " ]";
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}

void LoadListCurveKeys(FbxAnimCurve * pCurve, FbxProperty * pProperty)
{
	FbxTime   lKeyTime;
	int     lKeyValue;
	char    lTimeString[256];
	FbxString lListValue;
	FbxString lOutputString;
	int     lCount;

	int lKeyCount = pCurve->KeyGetCount();

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<int>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);

		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " (";
		lOutputString += pProperty->GetEnumValue(lKeyValue);
		lOutputString += ")";

		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}

void FBXINOUT::FBX::DisplayString(const char* pHeader, const char* pValue, const char* pSuffix)
{
	FbxString lString;

	lString = pHeader;
	lString += pValue;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}

void FBXINOUT::FBX::LoadPolygons(FbxMesh* pMesh)
{
	FbxStringList uvSetName;
	pMesh->GetUVSetNames(uvSetName);
	int uvSetNamesCount = uvSetName.GetCount();
	if (uvSetNamesCount > 0)
	{
		bool t = pMesh->GenerateTangentsData(uvSetName[0]);
	}

	int lPolygonCount = pMesh->GetPolygonCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();

	int control_point_count = pMesh->GetControlPointsCount();

	std::vector<Vertex>			lVertices;
	std::vector<unsigned int>	lIndicies;
	std::unordered_map<Vertex, unsigned int, VertexHash, VertexEqual> search_map;
	std::unordered_map<Vertex, unsigned int, VertexHash, VertexEqual>::iterator search_map_iter;


	int vertexId = 0;
	for (int i = 0; i < lPolygonCount; i++)
	{
		int lPolygonSize = pMesh->GetPolygonSize(i);

		for (int j = 0; j < lPolygonSize; j++)
		{
			Vertex vert;

			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);
			if (lControlPointIndex < 0) continue;

			vert.position = lControlPoints[lControlPointIndex];

			vert.control_point_index = lControlPointIndex;

			for (int l = 0; l < pMesh->GetElementVertexColorCount(); l++)
			{
				FbxGeometryElementVertexColor* lVertexColor = pMesh->GetElementVertexColor(l);
				vert.color = FindFbxColor(lVertexColor, lControlPointIndex, vertexId);
			}

			for (int l = 0; l < pMesh->GetElementUVCount(); l++)
			{
				FbxGeometryElementUV* lVertexUV = pMesh->GetElementUV(l);
				vert.texcoord = FindVertexUV(lVertexUV, lControlPointIndex, vertexId);
			}

			for (int l = 0; l < pMesh->GetElementNormalCount(); l++)
			{
				FbxGeometryElementNormal* lVertexNormal = pMesh->GetElementNormal(l);
				vert.normal = FindVertexNormal(lVertexNormal, lControlPointIndex, vertexId);
			}

			for (int l = 0; l < pMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* lVertexTangent = pMesh->GetElementTangent(l);
				vert.tangent = FindVertexTangent(lVertexTangent, lControlPointIndex, vertexId);
			}

			for (int l = 0; l < pMesh->GetElementBinormalCount(); ++l)
			{
				FbxGeometryElementBinormal* lVertexBinormal = pMesh->GetElementBinormal(l);
				vert.binormal = FindVertexBinormal(lVertexBinormal, lControlPointIndex, vertexId);
			}

			for (int l = 0; l < m_CurrentMesh.m_Influences[lControlPointIndex].joint.size(); l++)
			{
				vert.joint[l] = m_CurrentMesh.m_Influences[lControlPointIndex].joint[l];
				vert.weight[l] = m_CurrentMesh.m_Influences[lControlPointIndex].weight[l];
			}

			if (m_SystemConverted)
			{
				vert.position.mData[0] = -vert.position.mData[0];
				vert.normal.mData[0] = -vert.normal.mData[0];
				vert.tangent.mData[0] = -vert.tangent.mData[0];
				vert.binormal.mData[0] = -vert.binormal.mData[0];

				vert.position = Vector4MultiplyMatrix(vert.position, m_Rotate180);
				vert.normal = Vector4MultiplyMatrix(vert.normal, m_Rotate180);
				vert.tangent = Vector4MultiplyMatrix(vert.tangent, m_Rotate180);
				vert.binormal = Vector4MultiplyMatrix(vert.binormal, m_Rotate180);
			}


			// Remove duplicates
			search_map_iter = search_map.find(vert);
			if (search_map_iter == search_map.end())
			{
				// not found
				unsigned int index = (unsigned int)lVertices.size();
				search_map[vert] = index;
				lIndicies.push_back(index);
				lVertices.push_back(vert);
			}
			else
			{
				// found
				lIndicies.push_back(search_map_iter->second);
			}

			vertexId++;
		}	// for polygonSize
	}	// for polygonCount

	if (m_SystemConverted)
	{
		// ccw to cw indices conversion
		for (size_t i = 0; i < lIndicies.size(); i += 3)
		{
			std::swap(lIndicies[i + 1], lIndicies[i + 2]);
		}
	}

	m_CurrentMesh.m_Vertices = lVertices;
	m_CurrentMesh.m_Indicies = lIndicies;
	m_CurrentMesh.m_MeshName = pMesh->GetName();

	m_Mesh.push_back(m_CurrentMesh);

	ResetMesh(m_CurrentMesh);

	FBXSDK_printf("Mesh reading complete...\n");

	// debugging variable
	int d = 0;
}

void FBXINOUT::FBX::LoadMaterial(FbxGeometry * pGeometry)
{
	int lMaterialCount = 0;
	FbxNode* lNode = NULL;
	if (pGeometry) {
		lNode = pGeometry->GetNode();
		if (lNode)
			lMaterialCount = lNode->GetMaterialCount();
	}

	if (lMaterialCount > 0)
	{
		FbxPropertyT<FbxDouble3> lKFbxDouble3;
		FbxPropertyT<FbxDouble> lKFbxDouble1;
		FbxColor theColor;

		for (int lCount = 0; lCount < lMaterialCount; lCount++)
		{
			//DisplayInt("        Material ", lCount);

			FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(lCount);

			//DisplayString("            Name: \"", (char *)lMaterial->GetName(), "\"");

			//Get the implementation to see if it's a hardware shader.
			const FbxImplementation* lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_HLSL);
			FbxString lImplemenationType = "HLSL";
			if (!lImplementation)
			{
				lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_CGFX);
				lImplemenationType = "CGFX";
			}
			if (lImplementation)
			{
				//Now we have a hardware shader, let's read it
				FBXSDK_printf("            Hardware Shader Type: %s\n", lImplemenationType.Buffer());
				const FbxBindingTable* lRootTable = lImplementation->GetRootTable();
				FbxString lFileName = lRootTable->DescAbsoluteURL.Get();
				FbxString lTechniqueName = lRootTable->DescTAG.Get();


				const FbxBindingTable* lTable = lImplementation->GetRootTable();
				size_t lEntryNum = lTable->GetEntryCount();

				for (int i = 0; i < (int)lEntryNum; ++i)
				{
					const FbxBindingTableEntry& lEntry = lTable->GetEntry(i);
					const char* lEntrySrcType = lEntry.GetEntryType(true);
					FbxProperty lFbxProp;


					FbxString lTest = lEntry.GetSource();
					FBXSDK_printf("            Entry: %s\n", lTest.Buffer());


					if (strcmp(FbxPropertyEntryView::sEntryType, lEntrySrcType) == 0)
					{
						lFbxProp = lMaterial->FindPropertyHierarchical(lEntry.GetSource());
						if (!lFbxProp.IsValid())
						{
							lFbxProp = lMaterial->RootProperty.FindHierarchical(lEntry.GetSource());
						}


					}
					else if (strcmp(FbxConstantEntryView::sEntryType, lEntrySrcType) == 0)
					{
						lFbxProp = lImplementation->GetConstants().FindHierarchical(lEntry.GetSource());
					}
					if (lFbxProp.IsValid())
					{
						if (lFbxProp.GetSrcObjectCount<FbxTexture>() > 0)
						{
							//do what you want with the textures
							for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxFileTexture>(); ++j)
							{
								FbxFileTexture *lTex = lFbxProp.GetSrcObject<FbxFileTexture>(j);
								FBXSDK_printf("           File Texture: %s\n", lTex->GetFileName());
							}
							for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxLayeredTexture>(); ++j)
							{
								FbxLayeredTexture *lTex = lFbxProp.GetSrcObject<FbxLayeredTexture>(j);
								FBXSDK_printf("        Layered Texture: %s\n", lTex->GetName());
							}
							for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxProceduralTexture>(); ++j)
							{
								FbxProceduralTexture *lTex = lFbxProp.GetSrcObject<FbxProceduralTexture>(j);
								FBXSDK_printf("     Procedural Texture: %s\n", lTex->GetName());
							}
						}
						else
						{
							FbxDataType lFbxType = lFbxProp.GetPropertyDataType();
							FbxString blah = lFbxType.GetName();
							if (FbxBoolDT == lFbxType)
							{
								//DisplayBool("                Bool: ", lFbxProp.Get<FbxBool>());
							}
							else if (FbxIntDT == lFbxType || FbxEnumDT == lFbxType)
							{
								//DisplayInt("                Int: ", lFbxProp.Get<FbxInt>());
							}
							else if (FbxFloatDT == lFbxType)
							{
								//DisplayDouble("                Float: ", lFbxProp.Get<FbxFloat>());

							}
							else if (FbxDoubleDT == lFbxType)
							{
								//DisplayDouble("                Double: ", lFbxProp.Get<FbxDouble>());
							}
							else if (FbxStringDT == lFbxType
								|| FbxUrlDT == lFbxType
								|| FbxXRefUrlDT == lFbxType)
							{
								//DisplayString("                String: ", lFbxProp.Get<FbxString>().Buffer());
							}
							else if (FbxDouble2DT == lFbxType)
							{
								FbxDouble2 lDouble2 = lFbxProp.Get<FbxDouble2>();
								FbxVector2 lVect;
								lVect[0] = lDouble2[0];
								lVect[1] = lDouble2[1];

								//Display2DVector("                2D vector: ", lVect);
							}
							else if (FbxDouble3DT == lFbxType || FbxColor3DT == lFbxType)
							{
								FbxDouble3 lDouble3 = lFbxProp.Get<FbxDouble3>();


								FbxVector4 lVect;
								lVect[0] = lDouble3[0];
								lVect[1] = lDouble3[1];
								lVect[2] = lDouble3[2];
								//Display3DVector("                3D vector: ", lVect);
							}

							else if (FbxDouble4DT == lFbxType || FbxColor4DT == lFbxType)
							{
								FbxDouble4 lDouble4 = lFbxProp.Get<FbxDouble4>();
								FbxVector4 lVect;
								lVect[0] = lDouble4[0];
								lVect[1] = lDouble4[1];
								lVect[2] = lDouble4[2];
								lVect[3] = lDouble4[3];
								//Display4DVector("                4D vector: ", lVect);
							}
							else if (FbxDouble4x4DT == lFbxType)
							{
								FbxDouble4x4 lDouble44 = lFbxProp.Get<FbxDouble4x4>();
								for (int j = 0; j < 4; ++j)
								{

									FbxVector4 lVect;
									lVect[0] = lDouble44[j][0];
									lVect[1] = lDouble44[j][1];
									lVect[2] = lDouble44[j][2];
									lVect[3] = lDouble44[j][3];
									//Display4DVector("                4x4D vector: ", lVect);
								}

							}
						}

					}
				}
			}
			else if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
			{
				// We found a Phong material.  Display its properties.

				// Display the Ambient Color
				lKFbxDouble3 = ((FbxSurfacePhong *)lMaterial)->Ambient;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
				//DisplayColor("            Ambient: ", theColor);

				// Display the Diffuse Color
				lKFbxDouble3 = ((FbxSurfacePhong *)lMaterial)->Diffuse;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
				//DisplayColor("            Diffuse: ", theColor);

				// Display the Specular Color (unique to Phong materials)
				lKFbxDouble3 = ((FbxSurfacePhong *)lMaterial)->Specular;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
				//DisplayColor("            Specular: ", theColor);

				// Display the Emissive Color
				lKFbxDouble3 = ((FbxSurfacePhong *)lMaterial)->Emissive;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
				//DisplayColor("            Emissive: ", theColor);

				//Opacity is Transparency factor now
				lKFbxDouble1 = ((FbxSurfacePhong *)lMaterial)->TransparencyFactor;
				//DisplayDouble("            Opacity: ", 1.0 - lKFbxDouble1.Get());

				// Display the Shininess
				lKFbxDouble1 = ((FbxSurfacePhong *)lMaterial)->Shininess;
				//DisplayDouble("            Shininess: ", lKFbxDouble1.Get());

				// Display the Reflectivity
				lKFbxDouble1 = ((FbxSurfacePhong *)lMaterial)->ReflectionFactor;
				//DisplayDouble("            Reflectivity: ", lKFbxDouble1.Get());
			}
			else if (lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				// We found a Lambert material. Display its properties.
				// Display the Ambient Color
				lKFbxDouble3 = ((FbxSurfaceLambert *)lMaterial)->Ambient;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
				//DisplayColor("            Ambient: ", theColor);

				// Display the Diffuse Color
				lKFbxDouble3 = ((FbxSurfaceLambert *)lMaterial)->Diffuse;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
				//DisplayColor("            Diffuse: ", theColor);

				// Display the Emissive
				lKFbxDouble3 = ((FbxSurfaceLambert *)lMaterial)->Emissive;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
				//DisplayColor("            Emissive: ", theColor);

				// Display the Opacity
				lKFbxDouble1 = ((FbxSurfaceLambert *)lMaterial)->TransparencyFactor;
				//DisplayDouble("            Opacity: ", 1.0 - lKFbxDouble1.Get());
			}
			else {}
			//DisplayString("Unknown type of Material");

			FbxPropertyT<FbxString> lString;
			lString = lMaterial->ShadingModel;
			//DisplayString("            Shading Model: ", lString.Get().Buffer());
			//DisplayString("");
		}
	}
}

void FBXINOUT::FBX::LoadTexture(FbxGeometry * pGeometry)
{
	int lMaterialIndex;
	FbxProperty lProperty;
	if (pGeometry->GetNode() == NULL)
		return;
	int lNbMat = pGeometry->GetNode()->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++) {
		FbxSurfaceMaterial *lMaterial = pGeometry->GetNode()->GetSrcObject<FbxSurfaceMaterial>(lMaterialIndex);
		bool lDisplayHeader = true;

		//go through all the possible textures
		if (lMaterial) {

			int lTextureIndex;
			FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
			{
				//FBXSDK_printf("            Materials for : %s\n", FbxLayerElement::sTextureChannelNames[lTextureIndex]);
				auto list = FbxLayerElement::sTextureChannelNames;
				lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
				FindAndDisplayTextureInfoByProperty(lProperty, lDisplayHeader, lMaterialIndex);
			}

		}//end if(lMaterial)

	}// end for lMaterialIndex    
}

void FBXINOUT::FBX::LoadLink(FbxGeometry * pGeometry)
{
	FbxMesh* pMesh = pGeometry->GetNode()->GetMesh();
	int cp_count = pMesh->GetControlPointsCount();

	m_CurrentMesh.m_Influences.resize(cp_count);

	int lSkinCount = 0;
	int lClusterCount = 0;
	FbxCluster* lCluster;

	lSkinCount = pGeometry->GetDeformerCount(FbxDeformer::eSkin);

	for (int i = 0; i != lSkinCount; ++i)
	{
		lClusterCount = ((FbxSkin *)pGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
		for (int j = 0; j != lClusterCount; ++j)
		{
			lCluster = ((FbxSkin *)pGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);
			FbxNode* pNode = lCluster->GetLink();

			int joint_index = -1;
			for (size_t k = 0; k < m_MeshBindPose.size(); k++)
			{
				if (pNode == m_MeshBindPose[k].node)
				{
					joint_index = (int)k;
					break;
				}
			}

			int lIndexCount = lCluster->GetControlPointIndicesCount();
			int* lIndices = lCluster->GetControlPointIndices();
			double* lWeights = lCluster->GetControlPointWeights();


			for (int i = 0; i < lIndexCount; i++)
			{
				m_CurrentMesh.m_Influences[lIndices[i]].joint.push_back((unsigned int)joint_index);
				m_CurrentMesh.m_Influences[lIndices[i]].weight.push_back((float)lWeights[i]);
			}
		}
	}

	for (size_t i = 0; i < m_CurrentMesh.m_Influences.size(); i++)
	{
		RemoveWeakestWeights(m_CurrentMesh.m_Influences[i]);
	}
}

void FBXINOUT::FBX::FindAndDisplayTextureInfoByProperty(FbxProperty pProperty, bool & pDisplayHeader, int pMaterialIndex)
{
	const char* szDiffuseName = "DiffuseColor";
	const char* szNormalName = "NormalMap";
	const char* szSpecularName = "SpecularColor";

	if (pProperty.IsValid())
	{
		int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();

		for (int j = 0; j < lTextureCount; ++j)
		{
			//Here we have to check if it's layeredtextures, or just textures:
			FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
			if (lLayeredTexture)
			{
				//DisplayInt("    Layered Texture: ", j);
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k < lNbTextures; ++k)
				{
					FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
					if (lTexture)
					{

						if (pDisplayHeader) {
							//DisplayInt("    Textures connected to Material ", pMaterialIndex);
							pDisplayHeader = false;
						}

						//NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
						//Why is that?  because one texture can be shared on different layered textures and might
						//have different blend modes.

						FbxLayeredTexture::EBlendMode lBlendMode;
						lLayeredTexture->GetTextureBlendMode(k, lBlendMode);
						//DisplayString("    Textures for ", pProperty.GetName());
						//DisplayInt("        Texture ", k);
						//DisplayTextureInfo(lTexture, (int)lBlendMode);
						FBXSDK_printf("            Textures for : %s\n", pProperty.GetName().Buffer());
						FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
						FBXSDK_printf("            Textures Info : %s\n", lFileTexture->GetRelativeFileName());

						// TODO: check which texture type it is and insert into the correct variable in the mesh
						// ie. (diffuse) (normal) (specular)
						if (strcmp(pProperty.GetName().Buffer(), szDiffuseName) == 0)
						{
							m_Mesh[m_MeshIndex].m_TextureDiffuseName = ShortenFilePath(lFileTexture->GetRelativeFileName()).c_str();
						}
						else if (strcmp(pProperty.GetName().Buffer(), szNormalName) == 0)
						{
							m_Mesh[m_MeshIndex].m_TextureNormalName = ShortenFilePath(lFileTexture->GetRelativeFileName()).c_str();
						}
						else if (strcmp(pProperty.GetName().Buffer(), szSpecularName) == 0)
						{
							m_Mesh[m_MeshIndex].m_TextureSpecularName = ShortenFilePath(lFileTexture->GetRelativeFileName()).c_str();
						}
					}

				}
			}
			else
			{
				//no layered texture simply get on the property
				FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);


				if (lTexture)
				{
					//display connected Material header only at the first time
					if (pDisplayHeader) {
						//DisplayInt("    Textures connected to Material ", pMaterialIndex);
						pDisplayHeader = false;
					}

					//DisplayString("    Textures for ", pProperty.GetName());
					//DisplayInt("        Texture ", j);
					//DisplayTextureInfo(lTexture, -1);

					FBXSDK_printf("            Textures for : %s\n", pProperty.GetName().Buffer());
					FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
					FBXSDK_printf("            Textures Info : %s\n", lFileTexture->GetRelativeFileName());

					// TODO: check which texture type it is and insert into the correct variable in the mesh
					// ie. (diffuse) (normal) (specular)
					if (strcmp(pProperty.GetName().Buffer(), szDiffuseName) == 0)
					{
						m_Mesh[m_MeshIndex].m_TextureDiffuseName = ShortenFilePath(lFileTexture->GetRelativeFileName()).c_str();
					}
					else if (strcmp(pProperty.GetName().Buffer(), szNormalName) == 0)
					{
						m_Mesh[m_MeshIndex].m_TextureNormalName = ShortenFilePath(lFileTexture->GetRelativeFileName()).c_str();
					}
					else if (strcmp(pProperty.GetName().Buffer(), szSpecularName) == 0)
					{
						m_Mesh[m_MeshIndex].m_TextureSpecularName = ShortenFilePath(lFileTexture->GetRelativeFileName()).c_str();
					}
				}
			}
		}
	}//end if pProperty
}

void FBXINOUT::FBX::LoadMaterialConnections(FbxMesh * pMesh)
{
}

FbxColor FBXINOUT::FBX::FindFbxColor(FbxGeometryElementVertexColor * pElement, int nControlPointIndex, int nVertexId)
{
	FbxColor return_value = FbxColor(0.0, 0.0, 0.0, 1.0);

	switch (pElement->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nControlPointIndex);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nControlPointIndex);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nVertexId);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nVertexId);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
	case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
	case FbxGeometryElement::eNone:      // doesn't make much sense for UVs
		break;
	};

	return return_value;
}

FbxVector2 FBXINOUT::FBX::FindVertexUV(FbxGeometryElementUV * pElement, int nControlPointIndex, int nVertexId)
{
	FbxVector2 return_value = FbxVector2(0.0, 0.0);

	switch (pElement->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nControlPointIndex);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nControlPointIndex);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nVertexId);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nVertexId);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
	case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
	case FbxGeometryElement::eNone:      // doesn't make much sense for UVs
		break;
	};

	return_value.mData[1] = 1.0f - return_value.mData[1];
	return return_value;
}

FbxVector4 FBXINOUT::FBX::FindVertexNormal(FbxGeometryElementNormal * pElement, int nControlPointIndex, int nVertexId)
{
	FbxVector4 return_value = FbxVector4(0.0, 0.0, 0.0, 0.0);

	switch (pElement->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nControlPointIndex);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nControlPointIndex);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nVertexId);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nVertexId);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
	case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
	case FbxGeometryElement::eNone:      // doesn't make much sense for UVs
		break;
	};

	return return_value;
}

FbxVector4 FBXINOUT::FBX::FindVertexTangent(FbxGeometryElementTangent * pElement, int nControlPointIndex, int nVertexId)
{
	FbxVector4 return_value = FbxVector4(0.0, 0.0, 0.0, 0.0);

	switch (pElement->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nControlPointIndex);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nControlPointIndex);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nVertexId);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nVertexId);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
	case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
	case FbxGeometryElement::eNone:      // doesn't make much sense for UVs
		break;
	};

	return return_value;
}

FbxVector4 FBXINOUT::FBX::FindVertexBinormal(FbxGeometryElementBinormal * pElement, int nControlPointIndex, int nVertexId)
{
	FbxVector4 return_value = FbxVector4(0.0, 0.0, 0.0, 0.0);

	switch (pElement->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nControlPointIndex);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nControlPointIndex);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			return_value = pElement->GetDirectArray().GetAt(nVertexId);
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = pElement->GetIndexArray().GetAt(nVertexId);
			return_value = pElement->GetDirectArray().GetAt(id);
			break;
		};
		break;
	case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
	case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
	case FbxGeometryElement::eNone:      // doesn't make much sense for UVs
		break;
	};

	return return_value;
}

FbxAMatrix FBXINOUT::FBX::ConvertMatrixToSystemCoord(const FbxAMatrix & matrix)
{
	FbxVector4 translation = matrix.GetT();
	FbxVector4 rotation = matrix.GetR();
	translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
	rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);
	FbxAMatrix return_mat = matrix;
	return_mat.SetT(translation);
	return_mat.SetR(rotation);
	return MatrixMultiplyMatrix(return_mat, m_Rotate180);

	//return FbxMatrix(
	//	 matrix.mData[0][0],  matrix.mData[0][1],  matrix.mData[0][2], -matrix.mData[0][3],
	//	 matrix.mData[1][0],  matrix.mData[1][1],  matrix.mData[1][2], -matrix.mData[1][3],
	//	-matrix.mData[2][0], -matrix.mData[2][1],  matrix.mData[2][2],  matrix.mData[2][3],
	//	 matrix.mData[3][0],  matrix.mData[3][1], -matrix.mData[3][2],  matrix.mData[3][3]
	//);
}

FbxDouble FBXINOUT::FBX::DotProductVector4(const FbxVector4 & left, const FbxVector4 & right)
{
	FbxDouble return_value = 0.0;
	for (int i = 0; i < 4; i++)
	{
		return_value += (left.mData[i] * right.mData[i]);
	}
	return return_value;
}

FbxVector4 FBXINOUT::FBX::Vector4MultiplyMatrix(const FbxVector4 & left, const FbxMatrix & right)
{
	FbxVector4 return_value = FbxVector4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i < 4; i++)
	{
		return_value.mData[i] = (left.mData[0] * right.mData[0][i]) + (left.mData[1] * right.mData[1][i]) + (left.mData[2] * right.mData[2][i]) + (left.mData[3] * right.mData[3][i]);
	}
	return return_value;
}

FbxAMatrix FBXINOUT::FBX::MatrixMultiplyMatrix(const FbxMatrix & left, const FbxMatrix & right)
{
	FbxAMatrix return_value;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			return_value.mData[r][c] = DotProductVector4(left.GetRow(r), right.GetColumn(c));
		}
	}
	return return_value;
}

std::string FBXINOUT::FBX::ShortenFilePath(std::string filepath)
{
	return filepath;

	//std::string path = filepath;
	//const char* delim = "\\";
	//char* token = "";
	//char* next_token = "";

	//token = strtok_s(const_cast<char*>(path.c_str()), delim, &next_token);
	//while (token)
	//{
	//	token = strtok_s(nullptr, delim, &next_token);
	//	if (strcmp(next_token,"") == 0)
	//	{
	//		break;
	//	}
	//}
	//return std::string(token);
}
