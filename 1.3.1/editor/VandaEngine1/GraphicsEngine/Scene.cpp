/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the SCEA Shared Source License, Version 1.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at:
 * http://research.scea.com/scea_shared_source_license.html
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License 
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or 
 * implied. See the License for the specific language governing permissions and limitations under the 
 * License. 
 */

#include "stdafx.h"
#include "scene.h"
#include "Animation.h"
#include "..\VandaEngine1Dlg.h"

CScene::CScene()
{
	m_collada = NULL;
	m_loadAnimation = CTrue; //Load animations by default. We can read from a file to specify its value
	m_currentClipIndex = 0;
	//m_loadDefaultCamera = CFalse; //Just need to load the defult camera once	
	//used in animation
	m_lastKeyTime = 0; 
	m_firstKeyTime = 0; 
	m_sceneRoot = NULL;
	m_numAnimatedNodes = 0;

	m_hasAnimation = CFalse;
	m_animationStatus = eANIM_PLAY;
	m_updateAnimationLists = CFalse;
	m_update = CTrue; //used to update the scene( computing the local to world matrices, etc. )
	m_updateAnimation = CTrue;
	m_numClips = 0;
	m_isTrigger = CFalse;
	m_isGrass = CFalse;
	m_compress = CTrue;
	m_playAnimation = m_loopAnimationAtStartup = CTrue;
	m_clipIndexForStartup = 0;
}

CScene::~CScene()
{
	Destroy();
}

int CScene::GetFileCrc(const char* filenameinzip,void*buf,unsigned long size_buf,unsigned long* result_crc)
{
   unsigned long calculate_crc=0;
   int err=ZIP_OK;
   FILE * fin = fopen64(filenameinzip,"rb");
   unsigned long size_read = 0;
   unsigned long total_read = 0;
   if (fin==NULL)
   {
       err = ZIP_ERRNO;
   }

    if (err == ZIP_OK)
        do
        {
            err = ZIP_OK;
            size_read = (int)fread(buf,1,size_buf,fin);
            if (size_read < size_buf)
                if (feof(fin)==0)
            {
				CChar temp[MAX_NAME_SIZE];
                sprintf( temp, "\n%s %s", "error in reading ",filenameinzip);
				PrintInfo( temp, COLOR_RED );
                err = ZIP_ERRNO;
            }

            if (size_read>0)
                calculate_crc = crc32(calculate_crc,(Bytef*)buf,size_read);
            total_read += size_read;

        } while ((err == ZIP_OK) && (size_read>0));

    if (fin)
        fclose(fin);

    *result_crc=calculate_crc;
    return err;
}

CInt CScene::WriteZipFile(CChar* zipFileName, CChar* fileInZipName, CChar* fileInZipPath, const CChar* password )
{
    FILE * fin;
	fin = fopen(fileInZipPath,"rb");
    if (fin==NULL)
    {
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s %s %s", "Error in opening",fileInZipPath, "for reading");
		PrintInfo( temp, COLOR_RED );
		return -1;
   }

    int size_buf=WRITEBUFFERSIZE;
    unsigned long crcFile=0;
    unsigned long size_read = 0;
    zip_fileinfo zi;
    int err=0;
    void* buf=NULL;
    buf = (void*)malloc(size_buf);
    if (buf==NULL)
    {
		PrintInfo("\nWriteZipFile: Error allocating memory", COLOR_RED);
        return -1;
    }
	zipFile zf;
	zf = zipOpen(zipFileName, 0);
    if (zf == NULL)
    {
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s", "Error opening ",zipFileName);
		PrintInfo( temp, COLOR_RED );
		free(buf);
        return -1;
    }
    if (password != NULL)
		GetFileCrc(fileInZipPath,buf,size_buf,&crcFile);

    err = zipOpenNewFileInZip3(zf,fileInZipName,&zi,
                     NULL,0,NULL,0,NULL /* comment*/,
                     Z_DEFLATED,  Z_DEFAULT_COMPRESSION ,0,
                     -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                     password,crcFile);
    if (err != ZIP_OK)
	{
		CChar temp[MAX_NAME_SIZE];
        sprintf(temp, "\n%s %s %s", "Error in opening",fileInZipPath, "in zipfile");
		zipCloseFileInZip(zf);
		zipClose(zipOpen, "Vanda Engine 1.1");
		free(buf);
		return -1;
	}
   // else
   // {
   //     fin = fopen(fileInZipPath,"rb");
   //     if (fin==NULL)
   //     {
			//CChar temp[MAX_NAME_SIZE];
			//sprintf(temp, "\n%s %s %s", "Error in opening",fileInZipPath, "for reading");
			//PrintInfo( temp, COLOR_RED );
			//zipCloseFileInZip(zf);
			//zipClose(zf, "Vanda Engine 1.1");
			//free(buf);
			//return -1;
   //     }
   // }
    do
    {
        err = ZIP_OK;
        size_read = (int)fread(buf,1,size_buf,fin);
        if (size_read < (unsigned long)size_buf)
		{
			if (feof(fin)==0)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "\n%s%s", "Error in reading ",fileInZipPath);
				zipCloseFileInZip(zf);
				zipClose(zf, "Vanda Engine 1.1");
				free(buf);
				return -1;
			}
		}

        if (size_read>0)
        {
            err = zipWriteInFileInZip (zf,buf,size_read);
            if (err<0)
            {
				CChar temp[MAX_NAME_SIZE];

                sprintf( temp, "\n%s%s%s", "Error in writing ", fileInZipPath, " in the zipfile");
				zipCloseFileInZip(zf);
				zipClose(zf, "Vanda Engine 1.1");
				free(buf);
				return -1;
            }
        }
    } while ((err == ZIP_OK) && (size_read>0));

    if (fin)
        fclose(fin);
	zipCloseFileInZip(zf);
	zipClose(zf,"Vanda Engine 1.1");
    free(buf);
	return 1;
}

std::string CScene::ReadZipFile(CChar* zipFile, CChar* fileInZip) {
    int err = UNZ_OK;                 // error status
    uInt size_buf = WRITEBUFFERSIZE;  // byte size of buffer to store raw csv data
    void* buf;                        // the buffer  
	std::string sout;                      // output strings
    char filename_inzip[256];         // for unzGetCurrentFileInfo
    unz_file_info file_info;          // for unzGetCurrentFileInfo   
 
    unzFile uf = unzOpen(zipFile); // open zipfile stream
    if (uf==NULL) {
		CChar temp[MAX_URI_SIZE];
        sprintf( temp, "\n%s %s %s", "Cannot open '", zipFile, "'" );
		PrintInfo( temp, COLOR_RED );
        return sout;
    } // file is open
 
    if ( unzLocateFile(uf,fileInZip,1) ) { // try to locate file inside zip
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s %s %s %s %s", "File '", fileInZip, "' not found in '", zipFile, "'" );
		PrintInfo( temp, COLOR_RED );
        // second argument of unzLocateFile: 1 = case sensitive, 0 = case-insensitive
        return sout;
    } // file inside zip found
 
    if (unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0)) {
		CChar temp[MAX_URI_SIZE];
        sprintf( temp, "\n%s %s %s", "Error with zipfile '", zipFile, "' in unzGetCurrentFileInfo()" );
		PrintInfo( temp, COLOR_RED );
        return sout;
    } // obtained the necessary details about file inside zip
 
    buf = (void*)malloc(size_buf); // setup buffer
    if (buf==NULL) {
        PrintInfo( "\nError allocating memory for read buffer", COLOR_RED );
        return sout;
    } // buffer ready
 
	if( Cmp( g_currentPassword, "\n" ) )
		err = unzOpenCurrentFilePassword(uf,NULL); // Open the file inside the zip (password = NULL)
	else
		err = unzOpenCurrentFilePassword(uf, g_currentPassword); // Open the file inside the zip (password = NULL)
    if (err!=UNZ_OK) {
		CChar temp[MAX_URI_SIZE];
        sprintf( temp, "%s %s %s", "Error with zipfile '", zipFile, "' in unzOpenCurrentFilePassword()" );
		PrintInfo( temp, COLOR_RED );
        return sout;
    } // file inside the zip is open
 
    // Copy contents of the file inside the zip to the buffer
	CChar temp[MAX_URI_SIZE];
    sprintf( temp, "\n%s %s %s %s %s", "Extracting '", filename_inzip, "' from '", zipFile, "'");
	PrintInfo( temp );
    do {
        err = unzReadCurrentFile(uf,buf,size_buf);
        if (err<0) {
			CChar temp[MAX_URI_SIZE];
            sprintf( temp, "\n%s %s %s", "Error with zipfile '", zipFile, "' in unzReadCurrentFile()");
			PrintInfo( temp, COLOR_RED );
            sout = ""; // empty output string
            break;
        }
        // copy the buffer to a string
        if (err>0) for (int i = 0; i < (int) err; i++) 
		{
			sout.push_back( *(((char*)buf)+i) );
		}
    } while (err>0);
 
    err = unzCloseCurrentFile (uf);  // close the zipfile
    if (err!=UNZ_OK) {
		CChar temp[MAX_URI_SIZE];
        sprintf( temp, "\n%s %s %s", "Error with zipfile '", zipFile, "' in unzCloseCurrentFile()");
		PrintInfo( temp, COLOR_RED );
        sout = ""; // empty output string
    }
	unzClose( uf );
    free(buf); // free up buffer memory
    return sout;
}

CBool CScene::Load( CChar * fileName, CBool reportWarningsAndErrors, CBool readFromBuffer )
{
	if ( fileName == NULL )
		return CFalse; 
	CChar * nameOnly = GetAfterPath( fileName );
	SetName( nameOnly );

	// Instantiate the reference implementation
	m_collada = new DAE;
	PrintInfo( "\n" );
	PrintInfo( _T("\n========Importing new COLLADA scene========"), COLOR_BLUE ); 
	numErrors = numWarnings = 0;
	domCOLLADA *dom;
	if( readFromBuffer )
	{
		/////////////////////////////////////////////////////////////////
		//zip path
		CChar zipPath[MAX_NAME_SIZE];
		Cpy( zipPath, fileName );
		GetWithoutDot( zipPath );
		Append(zipPath, ".zip" );

		//file name inside zip file
		CChar fileNameInZip[MAX_NAME_SIZE];
		Cpy( fileNameInZip, GetAfterPath( fileName ) );
		//Uncompress zip file
		std::string buffer = ReadZipFile( zipPath, fileNameInZip );
		//res = m_collada->load( "", buffer.c_str() );
		dom = m_collada->openFromMemory( "", buffer.c_str() );
		///////////////////////////////////////////////////////////////
	}
	else
	{
		// load with full path 
		//res = m_collada->load( fileName );
		dom = m_collada->open(fileName);
	}

	//if (res != DAE_OK)
	//{
	//	PrintInfo(_T("\nCScene::Load > Error loading the COLLADA file:") + CString(fileName), COLOR_RED );
	//	delete m_collada;
	//	m_collada = NULL;

		//if( reportWarningsAndErrors )
		//{
		//	char tempReport[MAX_NAME_SIZE];;
		//	sprintf( tempReport, "%s - fatal error (s)", nameOnly );
		//	PrintInfo2( tempReport, COLOR_RED ); 
		//}

	//	return CFalse;
	//}
	PrintInfo( _T("\nCOLLADA_DOM Runtime database initialized from" ) );
	PrintInfo( _T("'") +  CString( fileName ), COLOR_RED_GREEN );
	//PrintInfo( _T("nameOnly: '") + (CString)nameOnly + _T( "'\n" ) );

	//domCOLLADA *dom = m_collada->getDom(nameOnly);
	//if ( !dom )
	//	dom = m_collada->getDom( fileName); 
	if ( !dom )
	{
		PrintInfo( _T("\nCScene::Load > COLLADA File loaded to the dom, but query for the dom assets failed "), COLOR_RED );
		PrintInfo( _T("\nCScene::Load > COLLADA Load Aborted! "), COLOR_RED );
		delete m_collada;	
		m_collada = NULL;
		if( reportWarningsAndErrors )
		{
			char tempReport[MAX_NAME_SIZE];;
			sprintf( tempReport, "%s - Fatal error (s)", nameOnly );
			PrintInfo2( tempReport, COLOR_RED ); 
		}
		return CFalse; 
	}

	CInt ret = 0;
	//PrintInfo("Begin Conditioning\n");
	//ret = kmzcleanup(m_collada, true);
	//if (ret)
	//	PrintInfo("kmzcleanup complete\n");
	ret = Triangulate(m_collada);
	if (ret)
		PrintInfo("\nTriangulate complete");
	//ret = deindexer(m_collada);
	//if (ret)
	//	PrintInfo("deindexer complete\n");

	//PrintInfo("Finish Conditioning\n");

	// Need to now get the asset tag which will determine what vector x y or z is up.  Typically y or z. 
	if ( dom->getAsset()->getUp_axis() )
	{
		domAsset::domUp_axis *up = dom->getAsset()->getUp_axis();
		switch( up->getValue() )
		{
			case UPAXISTYPE_X_UP:
				PrintInfo(_T("\nWarning!X is Up Data and Hiearchies must be converted!") ); 
				PrintInfo(_T("\nConversion to X axis Up isn't currently supported!") ); 
				PrintInfo(_T("\nCOLLADA defaulting to Y Up") ); 
				numWarnings +=1;
				//CRender.SetUpAxis(eCYUp); 
				break; 
			case UPAXISTYPE_Y_UP:
				PrintInfo( _T("\nY Axis is Up for this file...COLLADA set to Y Up ") ); 
				//CRender.SetUpAxis(eCYUp); 
				break;
			case UPAXISTYPE_Z_UP:
				PrintInfo( _T("\nZ Axis is Up for this file ") ); 
				PrintInfo( _T("\nAll Geometries and Hiearchies converted!"), COLOR_YELLOW ) ; 
				numWarnings +=1;
				//CRender.SetUpAxis(eCZUp); 
				break; 
			default:
				break; 
		}
	}
	strcpy( m_fileName, fileName ); 
	strcpy( m_pureFileName, nameOnly ); 

	// Load all the image libraries
	//for ( CUInt i = 0; i < dom->getLibrary_images_array().getCount(); i++)
	//{
	//	ReadImageLibrary( dom->getLibrary_images_array()[i] );			
	//}

	CBool success = CFalse;

	/*
	CChar *cfxBinFilename = ReadCfxBinaryFilename( dom->getExtra_array() );
	
	if ( cfxBinFilename != NULL ) 
	{
		cfxLoader::setBinaryLoadRemotePath( BasePath );
		success = (CBool) cfxLoader::loadMaterialsAndEffectsFromBinFile(cfxBinFilename, cfxMaterials, cfxEffects, cgContext);
		assert(success);
	}
	else
	{
	*/
		//success = ( CBool ) cfxLoader::loadMaterialsAndEffects( m_collada, m_cfxMaterials, m_cfxEffects, m_cgContext );
		//assert(success);
	/*}*/
	
	// Load all the effect libraries
	for ( CUInt i = 0; i < dom->getLibrary_effects_array().getCount(); i++)
	{
		ReadEffectLibrary( dom->getLibrary_effects_array()[i] );			
	}

	// Load all the material libraries
	for ( CUInt i = 0; i < dom->getLibrary_materials_array().getCount(); i++)
	{
 		ReadMaterialLibrary( dom->getLibrary_materials_array()[i] );			
	}

	// Load all the animation libraries
	for ( CUInt i = 0; i < dom->getLibrary_animations_array().getCount(); i++)
	{
		ReadAnimationLibrary( dom->getLibrary_animations_array()[i] );		
		m_hasAnimation = CTrue;
	}
	//Load all animation clips
	for ( CUInt i = 0; i < dom->getLibrary_animation_clips_array().getCount(); i++)
	{
		ReadAnimationClipLibrary( dom->getLibrary_animation_clips_array()[i] );			
	}

	//If there's no clip in COLLADA file, try to create a default clip
	if( m_numClips == 0 && dom->getLibrary_animations_array().getCount() > 0 )
	{
		//Create a default clip and attach all the animations to this clip
		PrintInfo( "\nAdding default animation clip..." );
		CAnimationClip * newAnimClip = CNew(CAnimationClip); 
		//CAssert("No memory\n", newAnimClip!=NULL);
		newAnimClip->SetName( "defaultClip" );
		newAnimClip->SetIndex(0);
		newAnimClip->SetStart(0.0);
		CFloat endTime = 0.0;
		for(CUInt i=0; i<m_animations.size(); i++)
		{
			CAnimation * anim = m_animations[i];

			PrintInfo( "\nAttaching animation ' ", COLOR_WHITE );PrintInfo( anim->GetName(), COLOR_RED_GREEN );
			PrintInfo( " ' to the default animation clip", COLOR_WHITE  );
			
			anim->SetClipTarget( newAnimClip );
			newAnimClip->m_animations.push_back( anim );
			if( anim->GetEndTime() > endTime )
				endTime = anim->GetEndTime();
		}
		newAnimClip->SetEnd((CDouble)endTime);
		m_numClips = 1;
		m_animationClips.push_back( newAnimClip );
	}

	// Find the scene we want
	daeElement* defaultScene = dom->getScene()->getInstance_visual_scene()->getUrl().getElement();
	domAsset::domUp_axis *up = dom->getAsset()->getUp_axis();

	switch( up->getValue() )
	{
		case UPAXISTYPE_X_UP:
			upAxis = eCXUp;
			break; 
		case UPAXISTYPE_Y_UP:
			upAxis = eCYUp;
			break;
		case UPAXISTYPE_Z_UP:
			upAxis = eCZUp;
			break; 
		default:
			break; 
	}

	if(defaultScene)
		ReadScene( (domVisual_scene *)defaultScene, upAxis );
	
	//if( m_loadDefaultCamera )
	//{
	//	PrintInfo( "No camera inside the collada file, switching to the default camera\n", COLOR_WHITE );
	//	// new CCamera
	//	CCamera * default_camera = CNew( CCamera ); 
	//	default_camera->SetName( "default_camera" );	
	//	default_camera->SetDocURI( dom->getDocumentURI()->getURI() );
	//	default_camera->SetZNear(1.0);
	//	default_camera->SetZFar(10000.0);
	//	m_cameras.push_back(default_camera);
	//	
	//	// new CNode
	//	CNode * camNode = CNew( CNode ); 
	//	camNode->SetName( "default_camera_node" ); 
	//	camNode->SetParent( m_sceneRoot );	

	//	m_nodes["default_camera_node"] = camNode;
	//	m_sceneRoot->AddChild( camNode ); 

	//	// new CInstanceCamera
	//	CInstanceCamera *instanceCamera = CNew(CInstanceCamera);
	//	instanceCamera->m_abstractCamera = default_camera;
	//	instanceCamera->m_parent = camNode;

	//	m_cameraInstances.push_back(instanceCamera);
	//	g_render.SetActiveInstanceCamera(instanceCamera);
	//	g_render.m_defaultInstanceCamera = instanceCamera;
	//}

	if (m_collada)
	{
		delete m_collada;
		m_collada = 0;
	}
	if( reportWarningsAndErrors )
	{
		char tempReport[MAX_NAME_SIZE];
		COLORREF color;
		if( numErrors > 0 )
			color = COLOR_RED;
		else if (numWarnings > 0 )
			color = COLOR_YELLOW;
		else
			color = COLOR_GREEN;
		sprintf( tempReport, "%s - %i error (s), %i warning (s)", nameOnly, numErrors, numWarnings );

		PrintInfo2( tempReport, color );

		totalErrors += numErrors;
		totalWarnings += numWarnings;
	}
	return CTrue;
}

CVoid CScene::Destroy()
{
	NxScene* tempScene = gPhysXscene/*gPhysicsSDK->getScene(i)*/;

	for( CUInt i = 0; i < m_instanceGeometries.size(); i++ )
	{
		CInstanceGeometry* m_instanceGeo = m_instanceGeometries[i];
		if( tempScene )
		{
			for( CUInt j = 0; j < tempScene->getNbActors(); j++ )
			{
				CChar actorName[MAX_NAME_SIZE];
				if( !tempScene->getActors()[j]->getName() ) continue;
				Cpy( actorName, tempScene->getActors()[j]->getName() );
				if( !Cmp(m_instanceGeo->m_physXName, "\n" ) && Cmp( actorName, m_instanceGeo->m_physXName ) )
				{
					tempScene->releaseActor( *tempScene->getActors()[j] );
					m_instanceGeo->m_hasPhysX = CFalse;
					Cpy( m_instanceGeo->m_physXName, "\n" );
				}
			}
		}
	}

	//while(!m_cfxMaterials.empty())
	//{
		//std::map<std::string, cfxMaterial*>::iterator iter = m_cfxMaterials.begin();
		//CDelete(iter->second);
		//m_cfxMaterials.erase(iter);
	//}
	//m_cfxEffects.clear();

	//while(!m_cfxEffects.empty())
	//{
		//std::map<std::string, cfxEffect*>::iterator iter = m_cfxEffects.begin();
		//CDelete(iter->second);
		//m_cfxEffects.erase(iter);
	//}
	//m_cfxEffects.clear();
	//delete all the geometries
	m_textureList.clear(); //save functions
	m_VSceneList.clear(); //save functions

	while(!m_geometries.empty())
	{
		CDelete( m_geometries[0] ); 
		m_geometries.erase( m_geometries.begin() );
	}	
	m_geometries.clear();
	m_instanceGeometries.clear();
	//m_instanceControllers.clear();
	while(!m_lightInstances.empty())
	{
		CDelete(m_lightInstances[0]);
		m_lightInstances.erase(m_lightInstances.begin());
	}
	while(!m_lights.empty())
	{
		CDelete( m_lights[0] ); 
		m_lights.erase(m_lights.begin());
	}
	while(!m_cameraInstances.empty())
	{
		CDelete(m_cameraInstances[0]);
		m_cameraInstances.erase(m_cameraInstances.begin());
	}
	g_cameraInstances.clear();

	while(!m_cameras.empty())
	{
		CDelete( m_cameras[0] ); 
		m_cameras.erase(m_cameras.begin());
	}
	g_render.SetActiveInstanceCamera(NULL);
	while(!m_controllers.empty())
	{
		CDelete( m_controllers[0] );
		m_controllers.erase(m_controllers.begin());
	}
	//Delete all the nodes
	CDelete( m_sceneRoot );
	m_nodes.clear();

	//Clear all the images
	//for( std::vector<CImage*>::iterator it = m_images.begin(); it != m_images.end(); it++ )
	//{
	//	CDelete( *it );
	//}
	//Clear the vector objects
	m_images.clear();

	//clear all the effects
	for( std::vector<CEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); it++ )
	{
		CDelete( *it );
	}
	//Clear the vector objects
	m_effects.clear();

	for( std::vector<CMaterial*>::iterator it = m_materials.begin(); it != m_materials.end(); it++ )
	{
		CDelete( *it );
	}
	//Clear the vector objects
	m_materials.clear();

	for( std::vector<CAnimation*>::iterator it = m_animations.begin(); it != m_animations.end(); it++ )
	{
		CDelete( *it );
	}
	//Clear the vector objects
	m_animations.clear();

	for( std::vector<CAnimationClip*>::iterator it = m_animationClips.begin(); it != m_animationClips.end(); it++ )
	{
		CDelete( *it );
	}
	//Clear the vector objects
	m_animationClips.clear();
}
CVoid CScene::RenderForWater(CWater* water)
{
	if ( m_sceneRoot )
	{
		m_sceneRoot->RenderForWater(water); 
	}
}

CVoid CScene::CalculateDistances()
{
	for( CUInt i = 0; i < m_instanceGeometries.size(); i++ )
	{
		CFloat x = g_camera->m_perspectiveCameraPos.x - m_instanceGeometries[i]->m_center.x;
		CFloat y = g_camera->m_perspectiveCameraPos.y - m_instanceGeometries[i]->m_center.y;
		CFloat z = g_camera->m_perspectiveCameraPos.z - m_instanceGeometries[i]->m_center.z;

		m_instanceGeometries[i]->m_distanceFromCamera = sqrt( pow(x, 2) + pow(y, 2) + pow(z, 2) );
	}
}

CVoid CScene::RenderModelsControlledByPhysX(CBool sceneManager)
{
	if ( m_sceneRoot )
	{
		m_sceneRoot->RenderModelsControlledByPhysX(sceneManager, m_sceneRoot); 
	}
}

CVoid CScene::RenderAnimatedModels(CBool sceneManager)
{
	if ( m_sceneRoot )
	{
		m_sceneRoot->RenderAnimatedModels(sceneManager, m_sceneRoot); 
	}
}

CVoid CScene::Render(CBool sceneManager, CChar* parentTreeNameOfGeometries, CBool renderController)
{
	if ( m_sceneRoot )
	{
		m_sceneRoot->Render(sceneManager, parentTreeNameOfGeometries, renderController); 
	}
}

CVoid CScene::RenderGrass(CBool sceneManager, CChar* parentTreeNameOfGeometries)
{
	if ( m_sceneRoot )
	{
		m_sceneRoot->RenderGrass(sceneManager, parentTreeNameOfGeometries); 
	}
}

CVoid CScene::RenderAABBWithLines( CBool animatedGeo )
{
	if ( m_sceneRoot )
	{
		m_sceneRoot->RenderAABBWithLines(animatedGeo); 
	}
}

CVoid CScene::RenderAABBWithQuads()
{
	if( m_sceneRoot )
	{
		m_sceneRoot->RenderAABBWithQuads();
	}
}

CVoid CScene::RenderSelectionMode()
{
	if( m_sceneRoot )
	{
		m_sceneRoot->RenderSelectionMode(m_sceneRoot);
	}
}

//Note: if cycle is true, we blend from animation1 to animation2 and continue from the second animation
//Note: if cycle is false, we blend from animation1 to animation2 and after finishing the second animation,
//we blend from  animation2 to animation1, and continue from the first animation
CVoid CScene::Update( CFloat elapsedTime )
{
	m_sceneRoot->Update( elapsedTime ); 
	UpdateBlendCycleList(elapsedTime);

	CBool updateExecuteActionList = CFalse;
	UpdateExecuteActionList(elapsedTime, updateExecuteActionList);

	if( m_blendCycleList.size() == 0 && !updateExecuteActionList )
		m_updateAnimationLists = CFalse;
}

CVoid CScene::UpdateBlendCycleList( CFloat elapsedTime )
{
	//Blend Cycle List
	std::vector<CInt> index;
	for( CUInt i = 0; i < m_blendCycleList.size(); i++ )
	{
		CChar blendCycleName[MAX_NAME_SIZE];
		Cpy( blendCycleName, m_blendCycleList[i].c_str() );
		CInt clipIndex = 0;
		//Find the animation clip 
		for( CInt ac = 0; ac < GetNumClips(); ac++ )
		{
			if( Cmp( m_animationClips[ac]->GetName(), blendCycleName ) )
			{
				clipIndex = ac;
				break;
			}
		}
		if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_CLEAR_CYCLE && m_animationClips[clipIndex]->GetCurrentWeight() == 0.0f )
		{
			m_animationClips[clipIndex]->SetAnimationStatus( eANIM_NONE );
			index.push_back(i);
			continue;
		}
		//make sure that current weight is not greater than target weight
		if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_BLEND_CYCLE && m_animationClips[clipIndex]->GetCurrentWeight() >= m_animationClips[clipIndex]->GetTargetWeight() )
		{
			m_animationClips[clipIndex]->SetCurrentWeight( m_animationClips[clipIndex]->GetTargetWeight() );
		}
		else
		{
			//compute current delay in or delay out time
			CFloat Dc = 0.0f;
			if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_BLEND_CYCLE )
			{
				m_animationClips[clipIndex]->SetCurrentDelayInTime2( elapsedTime );
				Dc = m_animationClips[clipIndex]->GetCurrentDelayInTime();
			}
			else if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_CLEAR_CYCLE )
			{
				m_animationClips[clipIndex]->SetCurrentDelayOutTime2( elapsedTime );
				Dc = m_animationClips[clipIndex]->GetCurrentDelayOutTime();
			}
			//compute M
			CFloat M = 0.0f;
			if(  m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_BLEND_CYCLE )
				M = (m_animationClips[clipIndex]->GetTargetWeight() - m_animationClips[clipIndex]->GetStartWeight() ) / m_animationClips[clipIndex]->GetTargetDelayIn();
			else
				M = (m_animationClips[clipIndex]->GetTargetWeight() - m_animationClips[clipIndex]->GetStartWeight() ) / m_animationClips[clipIndex]->GetTargetDelayOut();

			//compute current weight
			CFloat Wc = 0.0f;
			Wc = M * Dc + m_animationClips[clipIndex]->GetStartWeight();
			m_animationClips[clipIndex]->SetCurrentWeight( Wc );

			//make sure that W is not less than or greate than target weight
			if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_BLEND_CYCLE && m_animationClips[clipIndex]->GetCurrentWeight() >= m_animationClips[clipIndex]->GetTargetWeight() )
			{
				m_animationClips[clipIndex]->SetCurrentWeight( m_animationClips[clipIndex]->GetTargetWeight() );
			}
			else if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_CLEAR_CYCLE && m_animationClips[clipIndex]->GetCurrentWeight() <= 0.0f )
			{
				m_animationClips[clipIndex]->SetCurrentWeight( 0.0f );
			}
		}

		m_animationClips[clipIndex]->SetCurrentTime2( elapsedTime );
		if( m_animationClips[clipIndex]->GetCurrentTime() > m_animationClips[clipIndex]->GetEnd() )
			m_animationClips[clipIndex]->SetCurrentTime( m_animationClips[clipIndex]->GetCurrentTime() - m_animationClips[clipIndex]->GetEnd() + m_animationClips[clipIndex]->GetStart() );

	}
	for( CUInt i = 0; i < index.size(); i++ )
		m_blendCycleList.erase(m_blendCycleList.begin() + index[i]);
}

CVoid CScene::UpdateExecuteActionList( CFloat elapsedTime, CBool &updateExecuteActionList )
{
	//Execute Action List
	std::vector <CInt> index;
	if( m_executeActionList.size() == 0 )
	{
		updateExecuteActionList = CFalse;
		return;
	}

	for( CUInt i = 0; i < m_executeActionList.size(); i++ )
	{
		CChar executeActionName[MAX_NAME_SIZE];
		Cpy( executeActionName, m_executeActionList[i].c_str() );
		CInt clipIndex = 0;
		//Find the animation clip 
		for( CInt ac = 0; ac < GetNumClips(); ac++ )
		{
			if( Cmp( m_animationClips[ac]->GetName(), executeActionName ) )
			{
				clipIndex = ac;
				break;
			}
		}

		if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_EXECUTE_ACTION && m_animationClips[clipIndex]->GetCurrentTime() >=  m_animationClips[clipIndex]->GetEnd() && !m_animationClips[clipIndex]->GetLock())
		{
			m_animationClips[clipIndex]->SetAnimationStatus( eANIM_NONE );
			index.push_back(i);
			continue;
		}
		if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_EXECUTE_ACTION && m_animationClips[clipIndex]->GetStart() >= m_animationClips[clipIndex]->GetCurrentTime() && m_animationClips[clipIndex]->GetReverse())
		{
			m_animationClips[clipIndex]->SetAnimationStatus( eANIM_NONE );
			index.push_back(i);
			continue;
		}

		if( !(m_animationClips[clipIndex]->GetCurrentTime() >= m_animationClips[clipIndex]->GetEnd() && m_animationClips[clipIndex]->GetLock() && !m_animationClips[clipIndex]->GetReverse()) )
			updateExecuteActionList = CTrue;

		if( m_animationClips[clipIndex]->GetReverse() )
			elapsedTime = -elapsedTime;

		if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_EXECUTE_ACTION && m_animationClips[clipIndex]->GetCurrentTime() >= ( m_animationClips[clipIndex]->GetStart() + m_animationClips[clipIndex]->GetTargetDelayIn() ) && m_animationClips[clipIndex]->GetCurrentTime() <= (m_animationClips[clipIndex]->GetEnd() - m_animationClips[clipIndex]->GetTargetDelayOut() ) )
		{
			m_animationClips[clipIndex]->SetCurrentWeight( m_animationClips[clipIndex]->GetTargetWeight() );
		}
		else if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_EXECUTE_ACTION && m_animationClips[clipIndex]->GetCurrentTime() < ( m_animationClips[clipIndex]->GetStart() + m_animationClips[clipIndex]->GetTargetDelayIn() ) )
		{
			//compute current delay in or delay out time
			CFloat Dc = 0.0f;
			m_animationClips[clipIndex]->SetCurrentDelayInTime2( elapsedTime );
			Dc = m_animationClips[clipIndex]->GetCurrentDelayInTime();
			//compute M
			CFloat M = 0.0f;
			M = (m_animationClips[clipIndex]->GetTargetWeight() - m_animationClips[clipIndex]->GetStartWeight() ) / m_animationClips[clipIndex]->GetTargetDelayIn();

			//compute current weight
			CFloat Wc = 0.0f;
			Wc = M * Dc + m_animationClips[clipIndex]->GetStartWeight();
			m_animationClips[clipIndex]->SetCurrentWeight( Wc );

			if( m_animationClips[clipIndex]->GetCurrentWeight() >= m_animationClips[clipIndex]->GetTargetWeight() )
				m_animationClips[clipIndex]->SetCurrentWeight( m_animationClips[clipIndex]->GetTargetWeight() );
		}
		else if( m_animationClips[clipIndex]->GetAnimationStatus() == eANIM_EXECUTE_ACTION && m_animationClips[clipIndex]->GetCurrentTime() > (m_animationClips[clipIndex]->GetEnd() - m_animationClips[clipIndex]->GetTargetDelayOut() ) )
		{
			//compute current delay in or delay out time
			CFloat Dc = 0.0f;
			m_animationClips[clipIndex]->SetCurrentDelayOutTime2( elapsedTime );
			Dc = m_animationClips[clipIndex]->GetCurrentDelayOutTime();
			//compute M
			CFloat M = 0.0f;
			M = (-m_animationClips[clipIndex]->GetTargetWeight() ) / m_animationClips[clipIndex]->GetTargetDelayOut();
			//compute current weight
			CFloat Wc = 0.0f;
			Wc = M * Dc + m_animationClips[clipIndex]->GetTargetWeight();
			m_animationClips[clipIndex]->SetCurrentWeight( Wc );
			if( m_animationClips[clipIndex]->GetCurrentWeight() <= 0.0f )
				m_animationClips[clipIndex]->SetCurrentWeight( 0.0f );
		}

		if( m_animationClips[clipIndex]->GetReverse() )
		{
			m_animationClips[clipIndex]->SetCurrentTime2( elapsedTime );
			//if( m_animationClips[clipIndex]->GetCurrentTime() < m_animationClips[clipIndex]->GetStart() )
			//	m_animationClips[clipIndex]->SetCurrentTime( m_animationClips[clipIndex]->GetStart() );
		}
		else
		{
			m_animationClips[clipIndex]->SetCurrentTime2( elapsedTime );
			//if( m_animationClips[clipIndex]->GetCurrentTime() > m_animationClips[clipIndex]->GetEnd() )
			//	m_animationClips[clipIndex]->SetCurrentTime( m_animationClips[clipIndex]->GetEnd() );
		}
	}
	for( CUInt j = 0; j < index.size(); j++ )
	{
		m_executeActionList.erase(m_executeActionList.begin() + index[j]);
	}
	index.clear();
}

CVoid CScene::CreateTrigger( CNovodex* nx )
{
	if( m_sceneRoot )
	{	
		m_sceneRoot->CreateTrigger(nx);
	}
}

CVoid CScene::SetupAlphaTexture()
{
	if( m_sceneRoot )
	{	
		m_sceneRoot->SetupAlphaTexture();
	}
}

CVoid CScene::SetupGlossTexture()
{
	if( m_sceneRoot )
	{	
		m_sceneRoot->SetupGlossTexture();
	}
}

CVoid CScene::SetupShadowTexture()
{
	if( m_sceneRoot )
	{	
		m_sceneRoot->SetupShadowTexture();
	}
}

CVoid CScene::SetupDirtTexture()
{
	if( m_sceneRoot )
	{	
		m_sceneRoot->SetupDirtTexture();
	}
}

CVoid CScene::SetupNormalTexture()
{
	if( m_sceneRoot )
	{	
		m_sceneRoot->SetupNormalTexture();
	}
}

CVoid CScene::SetupDiffuseTexture()
{
	if( m_sceneRoot )
	{	
		m_sceneRoot->SetupDiffuseTexture();
	}
}

CInt CScene::Triangulate(DAE * _dae)
{
	CInt error = 0;

	// How many geometry elements are there?
	CInt geometryElementCount = (CInt)(_dae->getDatabase()->getElementCount(NULL, "geometry" ));
//	if(verbose)	cerr<<"There are "<<geometryElementCount<<" geometry elements in this file\n"; 

	for(CInt currentGeometry = 0; currentGeometry < geometryElementCount; currentGeometry++)
	{
		// Find the next geometry element
		domGeometry *thisGeometry;
//		error = _dae->getDatabase()->getElement((daeElement**)&thisGeometry,currentGeometry, NULL, "geometry");
		daeElement * element = 0;
		error = _dae->getDatabase()->getElement(&element,currentGeometry, NULL, "geometry");
		thisGeometry = (domGeometry *) element;

		// Get the mesh out of the geometry
		domMesh *thisMesh = thisGeometry->getMesh();

		if (thisMesh == NULL)
			continue;

		// Loop over all the polygon elements
		for(CInt currentPolygons = 0; currentPolygons < (CInt)(thisMesh->getPolygons_array().getCount()); currentPolygons++)
		{
			// Get the polygons out of the mesh
			// Always get index 0 because every pass through this loop deletes the <polygons> element as it finishes with it
			domPolygons *thisPolygons = thisMesh->getPolygons_array()[currentPolygons];  
			CreateTrianglesFromPolygons( thisMesh, thisPolygons );
			// Remove the polygons from the mesh
//			thisMesh->removeChildElement(thisPolygons);
		}
		while (thisMesh->getPolygons_array().getCount() > 0)
		{
			domPolygons *thisPolygons = thisMesh->getPolygons_array().get(0);  
			// Remove the polygons from the mesh
			thisMesh->removeChildElement(thisPolygons);
		}
		CInt polylistElementCount = (CInt)(thisMesh->getPolylist_array().getCount());
		//if(verbose)	cerr<<"There are "<<polylistElementCount<<" polylist elements in this file\n"; 
		for(CInt currentPolylist = 0; currentPolylist < polylistElementCount; currentPolylist++)
		{
			// Get the polylist out of the mesh
			// Always get index 0 because every pass through this loop deletes the <polygons> element as it finishes with it
			domPolylist *thisPolylist = thisMesh->getPolylist_array()[currentPolylist];  
			CreateTrianglesFromPolylist( thisMesh, thisPolylist );
			// Remove the polylist from the mesh
//			thisMesh->removeChildElement(thisPolylist);
		}
		while (thisMesh->getPolylist_array().getCount() > 0)
		{
			domPolylist *thisPolylist = thisMesh->getPolylist_array().get(0);  
			// Remove the polylist from the mesh
			thisMesh->removeChildElement(thisPolylist);
		}
	}
	return 0;
}

CVoid CScene::CreateTrianglesFromPolylist( domMesh *thisMesh, domPolylist *thisPolylist )
{
	// Create a new <triangles> inside the mesh that has the same material as the <polylist>
	domTriangles *thisTriangles = (domTriangles *)thisMesh->createAndPlace("triangles");
	//thisTriangles->setCount( 0 );
	unsigned CInt triangles = 0;
	thisTriangles->setMaterial(thisPolylist->getMaterial());
	domP* p_triangles = (domP*)thisTriangles->createAndPlace("p");

	// Give the new <triangles> the same <_dae> and <parameters> as the old <polylist>
	for(CInt i=0; i<(CInt)(thisPolylist->getInput_array().getCount()); i++)
	{
		thisTriangles->placeElement( thisPolylist->getInput_array()[i]->clone() );
	}

	// Get the number of inputs and primitives for the polygons array
	CInt numberOfInputs = (CInt)GetMaxOffset(thisPolylist->getInput_array()) + 1;
	CInt numberOfPrimitives = (CInt)(thisPolylist->getVcount()->getValue().getCount());

	unsigned CInt offset = 0;

	// Triangulate all the primitives, this generates all the triangles in a single <p> element
	for(CInt j = 0; j < numberOfPrimitives; j++)
	{	
		CInt triangleCount = (CInt)thisPolylist->getVcount()->getValue()[j] -2;
		// Write out the primitives as triangles, just fan using the first element as the base
		CInt idx = numberOfInputs;
		for(CInt k = 0; k < triangleCount; k++)
		{
			// First vertex
			for(CInt l = 0; l < numberOfInputs; l++)
			{
				p_triangles->getValue().append(thisPolylist->getP()->getValue()[offset + l]);
			}
			// Second vertex
			for(CInt l = 0; l < numberOfInputs; l++)
			{
				p_triangles->getValue().append(thisPolylist->getP()->getValue()[offset + idx + l]);
			}
			// Third vertex
			idx += numberOfInputs;
			for(CInt l = 0; l < numberOfInputs; l++)
			{
				p_triangles->getValue().append(thisPolylist->getP()->getValue()[offset + idx + l]);
			}
			//thisTriangles->setCount(thisTriangles->getCount()+1);
			triangles++;
		}
		offset += (unsigned CInt)thisPolylist->getVcount()->getValue()[j] * numberOfInputs;
	}
	thisTriangles->setCount( triangles );
}


CVoid CScene::CreateTrianglesFromPolygons( domMesh *thisMesh, domPolygons *thisPolygons )
{
	// Create a new <triangles> inside the mesh that has the same material as the <polygons>
	domTriangles *thisTriangles = (domTriangles *)thisMesh->createAndPlace("triangles");
	thisTriangles->setCount( 0 );
	thisTriangles->setMaterial(thisPolygons->getMaterial());
	domP* p_triangles = (domP*)thisTriangles->createAndPlace("p");

	// Give the new <triangles> the same <_dae> and <parameters> as the old <polygons>
	for(CInt i=0; i<(CInt)(thisPolygons->getInput_array().getCount()); i++)
	{
		thisTriangles->placeElement( thisPolygons->getInput_array()[i]->clone() );
	}

	// Get the number of inputs and primitives for the polygons array
	CInt numberOfInputs = (CInt)GetMaxOffset(thisPolygons->getInput_array()) +1;
	CInt numberOfPrimitives = (CInt)(thisPolygons->getP_array().getCount());

	// Triangulate all the primitives, this generates all the triangles in a single <p> element
	for(CInt j = 0; j < numberOfPrimitives; j++)
	{
		// Check the polygons for consistancy (some exported files have had the wrong number of indices)
		domP * thisPrimitive = thisPolygons->getP_array()[j];
		CInt elementCount = (CInt)(thisPrimitive->getValue().getCount());
		if((elementCount%numberOfInputs) != 0)
		{
			//cerr<<"Primitive "<<j<<" has an element count "<<elementCount<<" not divisible by the number of inputs "<<numberOfInputs<<"\n";
		}
		else
		{
			CInt triangleCount = (elementCount/numberOfInputs)-2;
			// Write out the primitives as triangles, just fan using the first element as the base
			CInt idx = numberOfInputs;
			for(CInt k = 0; k < triangleCount; k++)
			{
				// First vertex
				for(CInt l = 0; l < numberOfInputs; l++)
				{
					p_triangles->getValue().append(thisPrimitive->getValue()[l]);
				}
				// Second vertex
				for(CInt l = 0; l < numberOfInputs; l++)
				{
					p_triangles->getValue().append(thisPrimitive->getValue()[idx + l]);
				}
				// Third vertex
				idx += numberOfInputs;
				for(CInt l = 0; l < numberOfInputs; l++)
				{
					p_triangles->getValue().append(thisPrimitive->getValue()[idx + l]);
				}
				thisTriangles->setCount(thisTriangles->getCount()+1);
			}
		}
	}
}

unsigned int CScene::GetMaxOffset( domInputLocalOffset_Array &input_array )
{
	unsigned int maxOffset = 0;
	for ( unsigned int i = 0; i < input_array.getCount(); i++ ) {
		if ( input_array[i]->getOffset() > maxOffset ) {
			maxOffset = (unsigned int)input_array[i]->getOffset();
		}
	}
	return maxOffset;
}

CInt CScene::GetClipIndexForStartup()
{
	return m_clipIndexForStartup;
}
CVoid CScene::SetClipIndexForStartup( CInt index )
{
	m_clipIndexForStartup = index;
}

CVoid CScene::SetClipIndex( CInt index, CBool loopAnimation )
{
	if( m_numClips == 0 )
		return;
	m_currentClipIndex = index;
	if( m_currentClipIndex == m_numClips - 1)
		ex_pBtnNextAnim->EnableWindow( FALSE );
	else if( m_numClips > 1 )
		ex_pBtnNextAnim->EnableWindow( TRUE );

	if( m_currentClipIndex == 0 )
		ex_pBtnPrevAnim->EnableWindow( FALSE );
	else if( m_numClips > 1 )
		ex_pBtnPrevAnim->EnableWindow( TRUE );

	if( loopAnimation )
	{
		for( CInt index = 0; index < m_numClips; index++ )
		{
			if( m_currentClipIndex != index ) 
			{
				ClearCycle( index, 1.0f ); //1.0 second
			}
			else
			{
				BlendCycle( index, 1.0f, 1.0f );
			}
		}
	}
	else
	{
		ExecuteAction( m_currentClipIndex, m_animationClips[m_currentClipIndex]->GetDuration() * 0.1f, m_animationClips[m_currentClipIndex]->GetDuration() * 0.1f);
	}

	PrintInfo3( "\nAnimation '" + (CString)m_animationClips[m_currentClipIndex]->GetName() + "' activated" );
}
CVoid CScene::SetNextAnimation() 
{
	m_currentClipIndex++;
	if( m_currentClipIndex == m_numClips - 1)
		ex_pBtnNextAnim->EnableWindow( FALSE );
	if( m_numClips > 1 )
		ex_pBtnPrevAnim->EnableWindow( TRUE );

	if( m_currentClipIndex == m_numClips)
		m_currentClipIndex = m_numClips - 1;

	for( CInt index = 0; index < m_numClips; index++ )
	{
		if( m_currentClipIndex != index ) 
		{
			ClearCycle( index, 1.0f ); //0.5 second
		}
		else
		{
			BlendCycle( index, 1.0f, 1.0f );
		}
	}

	PrintInfo3( "\nAnimation '" + (CString)m_animationClips[m_currentClipIndex]->GetName() + "' activated" );

}
CVoid CScene::SetPrevAnimation() 
{
	m_currentClipIndex--;
	if( m_currentClipIndex == 0 )
		ex_pBtnPrevAnim->EnableWindow( FALSE );
	if( m_numClips > 1 )
		ex_pBtnNextAnim->EnableWindow( TRUE );

	if( m_currentClipIndex < 0 )
		m_currentClipIndex = 0;

	for( CInt index = 0; index < m_numClips; index++ )
	{
		if( m_currentClipIndex != index ) 
		{
			ClearCycle( index, 1.0f ); //0.5 second
		}
		else
		{
			BlendCycle( index, 1.0f, 1.0f );
		}
	}

	PrintInfo3( "\nAnimation '" + (CString)m_animationClips[m_currentClipIndex]->GetName() + "' activated" );
}

CVoid CScene::SetCurrentClipIndex()
{
	if( m_currentClipIndex == m_numClips - 1)
		ex_pBtnNextAnim->EnableWindow( FALSE );
	else if( m_numClips > 1 )
		ex_pBtnNextAnim->EnableWindow( TRUE );

	if( m_currentClipIndex == 0 )
		ex_pBtnPrevAnim->EnableWindow( FALSE );
	else if( m_numClips > 1 )
		ex_pBtnPrevAnim->EnableWindow( TRUE );
}

CInt CScene::GetCurrentClipIndex()
{
	return m_currentClipIndex;
}

CInt CScene::GetNumClips()
{
	return m_numClips;
}

CBool CScene::BlendCycle(CInt id, CFloat weight, CFloat delay)
{
	if( m_numClips == 0 )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "scene '", GetName(), "' has no animation" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( id > m_numClips - 1 || id < 0 )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Invalid animation ID for scene '", GetName(), "'" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( weight == 0 ) return CFalse;
	if( weight > 1 ) weight = 1;

	if( m_animationClips[id]->GetAnimationStatus() == eANIM_EXECUTE_ACTION) 
		return CFalse;

	if( m_animationClips[id]->GetAnimationStatus() != eANIM_BLEND_CYCLE && m_animationClips[id]->GetAnimationStatus() != eANIM_CLEAR_CYCLE)
		m_blendCycleList.push_back( m_animationClips[id]->GetName() );

	m_animationClips[id]->SetTargetDelayIn( delay );
	m_animationClips[id]->SetTargetDelayOut( 0.0f );
	m_animationClips[id]->SetCurrentDelayInTime( 0.0f );
	m_animationClips[id]->SetCurrentDelayOutTime( 0.0f );
	if( m_animationClips[id]->GetAnimationStatus() != eANIM_BLEND_CYCLE && m_animationClips[id]->GetAnimationStatus() != eANIM_CLEAR_CYCLE)
		m_animationClips[id]->SetCurrentTime( m_animationClips[id]->GetStart() );
	m_animationClips[id]->SetTargetWeight( weight );
	m_animationClips[id]->SetStartWeight( m_animationClips[id]->GetCurrentWeight() );
	m_animationClips[id]->SetAnimationStatus( eANIM_BLEND_CYCLE );
	m_updateAnimationLists = CTrue;

	return CTrue;
}

CBool CScene::ClearCycle(CInt id, CFloat delay)
{
	if( m_numClips == 0 )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "scene '", GetName(), "' has no animation" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( id > m_numClips - 1 || id < 0 )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Invalid animation ID for scene '", GetName(), "'" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( m_animationClips[id]->GetAnimationStatus() != eANIM_BLEND_CYCLE || m_animationClips[id]->GetAnimationStatus() == eANIM_EXECUTE_ACTION)
		return CFalse;

	if( m_animationClips[id]->GetAnimationStatus() != eANIM_BLEND_CYCLE && m_animationClips[id]->GetAnimationStatus() != eANIM_CLEAR_CYCLE)
		m_blendCycleList.push_back( m_animationClips[id]->GetName() );

	m_animationClips[id]->SetTargetDelayIn( 0.0f );
	m_animationClips[id]->SetTargetDelayOut( delay );
	m_animationClips[id]->SetCurrentDelayInTime( 0.0f );
	m_animationClips[id]->SetCurrentDelayOutTime( 0.0f );
	m_animationClips[id]->SetTargetWeight( 0.0f );
	m_animationClips[id]->SetStartWeight( m_animationClips[id]->GetCurrentWeight() );
	m_animationClips[id]->SetAnimationStatus( eANIM_CLEAR_CYCLE );
	m_updateAnimationLists = CTrue;

	return CTrue;
}

CBool CScene::ExecuteAction(CInt id, CFloat delayIn, CFloat delayOut, CFloat weight, CBool LockToLastFrame)
{
	if( m_numClips == 0 )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "scene '", GetName(), "' has no animation" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( id > m_numClips - 1 || id < 0 )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Invalid animation ID for scene '", GetName(), "'" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( weight == 0 ) return CFalse;
	if( weight > 1 ) weight = 1;

	if( delayIn + delayOut  > m_animationClips[id]->GetDuration() )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s%s%s", "scene ' ", GetName(), " ': delayIn + delayOut values are greater than duration of animation ' ", m_animationClips[id]->GetName(), " '" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( m_animationClips[id]->GetAnimationStatus() == eANIM_EXECUTE_ACTION && m_animationClips[id]->GetReverse() )
	{
		m_animationClips[id]->SetReverse(CFalse);
		return CTrue;
	}
	if( m_animationClips[id]->GetAnimationStatus() == eANIM_BLEND_CYCLE || m_animationClips[id]->GetAnimationStatus() == eANIM_CLEAR_CYCLE || m_animationClips[id]->GetAnimationStatus() == eANIM_EXECUTE_ACTION)
		return CFalse;

	m_executeActionList.push_back( m_animationClips[id]->GetName() );

	m_animationClips[id]->SetTargetDelayIn( delayIn );
	m_animationClips[id]->SetTargetDelayOut( delayOut );
	m_animationClips[id]->SetCurrentDelayInTime( 0.0f );
	m_animationClips[id]->SetCurrentDelayOutTime( 0.0f );
	m_animationClips[id]->SetCurrentTime( m_animationClips[id]->GetStart() );
	m_animationClips[id]->SetTargetWeight( weight );
	m_animationClips[id]->SetStartWeight( 0.0f );
	m_animationClips[id]->SetAnimationStatus( eANIM_EXECUTE_ACTION );
	m_animationClips[id]->SetLock( LockToLastFrame );
	m_animationClips[id]->SetReverse(CFalse);
	m_updateAnimationLists = CTrue;
	return CTrue;
}

CBool CScene::ReverseExecuteAction(CInt id )
{
	if( m_numClips == 0 )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "scene '", GetName(), "' has no animation" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( id > m_numClips - 1 || id < 0 )
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf( temp, "\n%s%s%s", "Invalid animation ID for scene '", GetName(), "'" );
		PrintInfo( temp, COLOR_RED );
		return CFalse;
	}

	if( m_animationClips[id]->GetAnimationStatus() == eANIM_BLEND_CYCLE || m_animationClips[id]->GetAnimationStatus() == eANIM_CLEAR_CYCLE )
		return CFalse;

	if( m_animationClips[id]->GetAnimationStatus() != eANIM_EXECUTE_ACTION )
		return CFalse;

	if( m_animationClips[id]->GetReverse() )
		return CFalse;

	m_animationClips[id]->SetReverse(CTrue);
	m_updateAnimationLists = CTrue;
	return CTrue;
}

