/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "FBX.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    pManager = FbxManager::Create();
    if (!pManager)
    {
        FBXSDK_printf("Error: Unable to create FBX Manager!\n");
        exit(1);
    }
    //else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

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

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
    //Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
    if (pManager) pManager->Destroy();
    if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(pManager, "");

    if (pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount())
    {
        // Write in fall back format in less no ASCII format found
        pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        //Try to export in ASCII if possible
        int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (lFormatIndex = 0; lFormatIndex < lFormatCount; lFormatIndex++)
        {
            if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
            {
                FbxString lDesc = pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                const char* lASCII = "ascii";
                if (lDesc.Find(lASCII) >= 0)
                {
                    pFileFormat = lFormatIndex;
                    break;
                }
            }
        }
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.
    IOS_REF.SetBoolProp(EXP_FBX_MATERIAL, true);
    IOS_REF.SetBoolProp(EXP_FBX_TEXTURE, true);
    IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED, pEmbedMedia);
    IOS_REF.SetBoolProp(EXP_FBX_SHAPE, true);
    IOS_REF.SetBoolProp(EXP_FBX_GOBO, true);
    IOS_REF.SetBoolProp(EXP_FBX_ANIMATION, true);
    IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if (lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
    {
        FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
        FBXSDK_printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
        return false;
    }

    FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    FBXSDK_printf("FBX file format version %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(pScene);

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor, lSDKMinor, lSDKRevision;
    //int lFileFormat = -1;
    int lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

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

        for (int i = 0; i < lAnimStackCount; i++)
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
    if (lStatus == true)
    {
        // Check the scene integrity!
        FbxStatus status;
        FbxArray< FbxString*> details;
        FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(pScene), &status, &details);
        lStatus = sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData);
        bool lNotify = (!lStatus && details.GetCount() > 0) || (lImporter->GetStatus().GetCode() != FbxStatus::eSuccess);
        if (lNotify)
        {
            FBXSDK_printf("\n");
            FBXSDK_printf("********************************************************************************\n");
            if (details.GetCount())
            {
                FBXSDK_printf("Scene integrity verification failed with the following errors:\n");
                for (int i = 0; i < details.GetCount(); i++)
                    FBXSDK_printf("   %s\n", details[i]->Buffer());

                FbxArrayDelete<FbxString*>(details);
            }

            if (lImporter->GetStatus().GetCode() != FbxStatus::eSuccess)
            {
                FBXSDK_printf("\n");
                FBXSDK_printf("WARNING:\n");
                FBXSDK_printf("   The importer was able to read the file but with errors.\n");
                FBXSDK_printf("   Loaded scene may be incomplete.\n\n");
                FBXSDK_printf("   Last error message:'%s'\n", lImporter->GetStatus().GetErrorString());
            }
            FBXSDK_printf("********************************************************************************\n");
            FBXSDK_printf("\n");
        }
    }

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


// Add the specified node to the node array. Also, add recursively
// all the parent node of the specified node to the array.
void AddNodeRecursively(FbxArray<FbxNode*>& pNodeArray, FbxNode* pNode)
{
    if (pNode)
    {
        AddNodeRecursively(pNodeArray, pNode->GetParent());

        if (pNodeArray.Find(pNode) == -1)
        {
            // Node not in the list, add it
            pNodeArray.Add(pNode);
        }
    }
}

// Store the Bind Pose
void StoreBindPose(FbxScene* pScene, FbxNode* pPatch)
{
    // In the bind pose, we must store all the link's global matrix at the time of the bind.
    // Plus, we must store all the parent(s) global matrix of a link, even if they are not
    // themselves deforming any model.

    // In this example, since there is only one model deformed, we don't need walk through 
    // the scene
    //

    // Now list the all the link involve in the patch deformation
    FbxArray<FbxNode*> lClusteredFbxNodes;
    int                       i, j;

    if (pPatch && pPatch->GetNodeAttribute())
    {
        int lSkinCount = 0;
        int lClusterCount = 0;
        switch (pPatch->GetNodeAttribute()->GetAttributeType())
        {
        default:
            break;
        case FbxNodeAttribute::eMesh:
        case FbxNodeAttribute::eNurbs:
        case FbxNodeAttribute::ePatch:

            lSkinCount = ((FbxGeometry*)pPatch->GetNodeAttribute())->GetDeformerCount(FbxDeformer::eSkin);
            //Go through all the skins and count them
            //then go through each skin and get their cluster count
            for (i = 0; i < lSkinCount; ++i)
            {
                FbxSkin* lSkin = (FbxSkin*)((FbxGeometry*)pPatch->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
                lClusterCount += lSkin->GetClusterCount();
            }
            break;
        }
        //if we found some clusters we must add the node
        if (lClusterCount)
        {
            //Again, go through all the skins get each cluster link and add them
            for (i = 0; i < lSkinCount; ++i)
            {
                FbxSkin* lSkin = (FbxSkin*)((FbxGeometry*)pPatch->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
                lClusterCount = lSkin->GetClusterCount();
                for (j = 0; j < lClusterCount; ++j)
                {
                    FbxNode* lClusterNode = lSkin->GetCluster(j)->GetLink();
                    AddNodeRecursively(lClusteredFbxNodes, lClusterNode);
                }

            }

            // Add the patch to the pose
            lClusteredFbxNodes.Add(pPatch);
        }
    }

    // Now create a bind pose with the link list
    if (lClusteredFbxNodes.GetCount())
    {
        // A pose must be named. Arbitrarily use the name of the patch node.
        FbxPose* lPose = FbxPose::Create(pScene, "Bind Pose");
        

        // default pose type is rest pose, so we need to set the type as bind pose
        lPose->SetIsBindPose(true);

        for (i = 0; i < lClusteredFbxNodes.GetCount(); i++)
        {
            FbxNode* lKFbxNode = lClusteredFbxNodes.GetAt(i);
            FbxMatrix lBindMatrix = lKFbxNode->EvaluateGlobalTransform();

            lPose->Add(lKFbxNode, lBindMatrix);
        }
    }
}