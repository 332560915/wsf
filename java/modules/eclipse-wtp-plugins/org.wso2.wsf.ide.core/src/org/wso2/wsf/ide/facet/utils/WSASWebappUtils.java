/*
 * Copyright 2005,2006 WSO2, Inc. http://www.wso2.org
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.wso2.wsf.ide.facet.utils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

import org.eclipse.core.runtime.IProgressMonitor;
import org.wso2.wsf.ide.core.plugin.data.ServerModel;
import org.wso2.wsf.ide.core.plugin.messages.WSASCoreUIMessages;
import org.wso2.wsf.ide.core.utils.WSASCoreUtils;
import org.wso2.wsf.ide.core.utils.FileUtils;

public class WSASWebappUtils {

	private static String tempWarLocation = null;
	private static boolean alreadyWarExist = false;

	public static String  copyWSASWar(IProgressMonitor monitor, String wsasHome)
										throws FileNotFoundException, IOException{
		if(!alreadyWarExist){
			File tempWSASDirectory = new File (WSASCoreUtils.tempWSASDirectory());
			if(!tempWSASDirectory.exists()){
				tempWSASDirectory.mkdirs();
			}
			if (tempWSASDirectory.isDirectory()) {
				tempWarLocation = WSASCoreUtils.addAnotherNodeToPath(
						WSASCoreUtils.tempWSASDirectory(),
						WSASCoreUIMessages.DIR_EXPLOADED_TEMPWAR);
				File tempWarLocationFile= new File(tempWarLocation);
				if (tempWarLocationFile.exists()) {
					FileUtils.deleteDirectories(tempWarLocationFile);
				}
				tempWarLocationFile.mkdirs();
				Properties properties = new Properties();
				properties.load(new FileInputStream(WSASCoreUtils.tempWSASWebappFileLocation()));
				if (properties.containsKey(WSASCoreUIMessages.PROPERTY_KEY_PATH)){
					String wsasHomeLocation = (ServerModel.getWsasServerPath()!=null)
									?ServerModel.getWsasServerPath()
									:properties.getProperty(WSASCoreUIMessages.PROPERTY_KEY_PATH);
					String wsasWebappLocation = WSASCoreUtils.addAnotherNodeToPath(
							wsasHomeLocation,
							"webapp");
					String wsasLibFile = WSASCoreUtils.addAnotherNodeToPath(
							wsasHomeLocation,
					"lib");
					String wsasConfFile = WSASCoreUtils.addAnotherNodeToPath(
							wsasHomeLocation,
					"conf");
					String wsasRepositoryFile = WSASCoreUtils.addAnotherNodeToPath(
							wsasHomeLocation,
					"repository");
					String wsasTempWebInfFile = WSASCoreUtils.addAnotherNodeToPath(
							tempWarLocation,
					"WEB-INF");
					String wsasTempWebInfLibFile = WSASCoreUtils.addAnotherNodeToPath(
							wsasTempWebInfFile,
					"lib");
					String wsasTempWebInfConfFile = WSASCoreUtils.addAnotherNodeToPath(
							wsasTempWebInfFile,
					"conf");
					String wsasTempWebBuildFile = WSASCoreUtils.addAnotherNodeToPath(
							tempWarLocation,
					"build.xml");

					//Copy the webapp content 
					FileUtils.copyDirectory(new File(wsasWebappLocation), tempWarLocationFile);
					//delete the build.xml File
					new File(wsasTempWebBuildFile).delete();
					//Copy libs 
					FileUtils.copyDirectory(new File(wsasLibFile),
											new File(wsasTempWebInfLibFile));
					//copy conf/axis2.xml
					FileUtils.copyDirectory(new File(wsasConfFile), 
											new File(wsasTempWebInfConfFile));
					//Copy modules and services 					
					FileUtils.copyDirectory(new File(wsasRepositoryFile), 
											new File(wsasTempWebInfFile));
					alreadyWarExist= true;
				}

			} else {
				alreadyWarExist = false;
				//Throws an error message
			}
		}	
		return tempWarLocation;
	}

}
