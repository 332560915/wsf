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
package org.wso2.wsf.ide.consumption.core.command;

import java.io.File;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.util.Map;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.osgi.util.NLS;
import org.eclipse.wst.command.internal.env.core.common.StatusUtils;
import org.eclipse.wst.common.environment.IEnvironment;
import org.eclipse.wst.common.environment.IStatusHandler;
import org.eclipse.wst.common.frameworks.datamodel.AbstractDataModelOperation;
import org.wso2.wsf.ide.consumption.core.data.DataModel;
import org.wso2.wsf.ide.consumption.core.messages.WSASConsumptionUIMessages;
import org.wso2.wsf.ide.consumption.core.utils.ContentCopyUtils;
import org.wso2.wsf.ide.consumption.core.utils.WSDL2JavaGenerator;
import org.wso2.wsf.ide.consumption.core.utils.WSDLPropertyReader;
import org.wso2.wsf.ide.core.utils.ClassLoadingUtil;
import org.wso2.wsf.ide.core.utils.FileUtils;

public class WSASClientCodegenCommand extends AbstractDataModelOperation {
	
	  private DataModel model;
		
	  public WSASClientCodegenCommand( DataModel model ){
	    this.model = model;  
	  }

	public IStatus execute(IProgressMonitor monitor, IAdaptable info)
													 throws ExecutionException {
		IStatus status = Status.OK_STATUS;  
		IEnvironment environment = getEnvironment();
		IStatusHandler statusHandler = environment.getStatusHandler();	
		
		String workspaceDirectory = ResourcesPlugin.getWorkspace().getRoot().
													getLocation().toOSString();
		String currentDynamicWebProjectDir = FileUtils.addAnotherNodeToPath(
														workspaceDirectory,
														model.getWebProjectName());
		String matadataDir = FileUtils.addAnotherNodeToPath(
										workspaceDirectory,
										WSASConsumptionUIMessages.DIR_DOT_METADATA);
	    String matadataPluginsDir = FileUtils.addAnotherNodeToPath(
	    								matadataDir,
	    								WSASConsumptionUIMessages.DIR_DOT_PLUGINS);
	    String matadataWSASDir = FileUtils.addAnotherNodeToPath(
	    									matadataPluginsDir, 
	    									WSASConsumptionUIMessages.WSAS_PROJECT);
		String tempCodegenOutputLocation = FileUtils.addAnotherNodeToPath(
											matadataWSASDir, 
											WSASConsumptionUIMessages.CODEGEN_RESULTS);
		File tempCodegenOutputLocationFile = new File(tempCodegenOutputLocation);
		if (tempCodegenOutputLocationFile.exists()){
			FileUtils.deleteDirectories(tempCodegenOutputLocation);
		}
		FileUtils.createDirectorys(tempCodegenOutputLocation);
		WSDL2JavaGenerator generator = new WSDL2JavaGenerator(); 
      
		//AxisService service;
		Object axisServiceInstance;
	try {
		//service = generator.getAxisService(model.getWsdlURI());
		ClassLoadingUtil.setInitByClient(true);
		ClassLoadingUtil.cleanAntClassLoader();
		ClassLoadingUtil.init(model.getWebProjectName());
		axisServiceInstance = generator.getAxisService(model.getWsdlURI());
		Map optionsMap = generator.fillOptionMap(	
			model.isASync(), //async
            model.isSync(), //sync
            false,  //servirside false always
            false,  //services.xml false always
            model.isTestCaseCheck(),
            model.isGenerateAllCheck(),
            (model.getServiceName()!=null)?model.getServiceName():null,
            (model.getPortName()!=null)?model.getPortName():null,
            (model.getDatabindingType().toLowerCase()!=null)
            			?model.getDatabindingType().toLowerCase()
            			:null,
            (model.getWsdlURI()!=null)?model.getWsdlURI():null,
            (model.getPackageText()!=null)?model.getPackageText():null,
            "java",
            tempCodegenOutputLocation,
            model.getNamespaseToPackageMapping(),
            false	//Serverside interface always false
           ); 
      
        //CodeGenConfiguration codegenConfig = new CodeGenConfiguration(service, optionsMap);
		Class CodeGenConfigurationClass = ClassLoadingUtil
				.loadClassFromAntClassLoader("org.apache.axis2.wsdl.codegen.CodeGenConfiguration");

		//-----------------------------------------------------------------------------------//
		//Fix for the Axis2 1.2 
		//Constructor CodeGenConfigurationConstructor = CodeGenConfigurationClass
		//		.getConstructor(new Class[]{axisServiceInstance.getClass(),Map.class});
		//Object CodeGenConfigurationInstance = CodeGenConfigurationConstructor
		//		.newInstance(new Object[]{axisServiceInstance,optionsMap});
		
		Constructor CodeGenConfigurationConstructor = CodeGenConfigurationClass
				.getConstructor(new Class[]{Map.class});
		Object CodeGenConfigurationInstance = CodeGenConfigurationConstructor
				.newInstance(new Object[]{optionsMap});
		
		// codegenConfig.addAxisService(service);
		Method addAxisServiceMethod = CodeGenConfigurationClass
				.getMethod("addAxisService", new Class[]{ axisServiceInstance.getClass()});
		addAxisServiceMethod.invoke(CodeGenConfigurationInstance, 
							new Object[]{axisServiceInstance});
		
        //set the wsdl definision for codegen config for skeleton generarion.
        WSDLPropertyReader reader = new WSDLPropertyReader();
        reader.readWSDL(model.getWebProjectName(),model.getWsdlURI());
        Object wsdlDefinitionInstance = reader.getWsdlDefinitionInstance();
        //Class DefinitionClass = ClassLoadingUtil.loadClassFromAntClassLoader("javax.wsdl.Definition");
        //codegenConfig.setWsdlDefinition(wsdlDefinition);
		Method setWsdlDefinitionMethod = CodeGenConfigurationClass
				.getMethod("setWsdlDefinition", new Class[]{reader.getWsdlDefinitionClass()});
		setWsdlDefinitionMethod.invoke(CodeGenConfigurationInstance, 
					new Object[]{wsdlDefinitionInstance});       
        
		//-----------------------------------------------------------------------------------//
		
        //set the baseURI
        //codegenConfig.setBaseURI(generator.getBaseUri(model.getWsdlURI()));
		
		Method setBaseURIMethod = CodeGenConfigurationClass
				.getMethod("setBaseURI", new Class[]{String.class});
		setBaseURIMethod.invoke(CodeGenConfigurationInstance, 
								new Object[]{generator.getBaseUri(model.getWsdlURI())});
		
		//Get the namespace from the AxisService and set it in the configuration 
		Class AxisServiceClass = ClassLoadingUtil
					.loadClassFromAntClassLoader("org.apache.axis2.description.AxisService");

		Method getTargetNamespaceMethod = AxisServiceClass.getMethod("getTargetNamespace", null);
		Object targetNamespace = getTargetNamespaceMethod.invoke(axisServiceInstance, null);
		
		Class URLProcessorClass = ClassLoadingUtil
					.loadClassFromAntClassLoader("org.apache.axis2.util.URLProcessor");
		Method makePackageNameMethod = URLProcessorClass
					.getMethod("makePackageName", new Class[]{String.class});
		Object stringReturn = makePackageNameMethod.invoke(null, new Object[]{targetNamespace});
		
		model.setPackageText(stringReturn.toString());
		
		Method setPackageNameMethod = CodeGenConfigurationClass
					.getMethod("setPackageName", new Class[]{String.class});
		setPackageNameMethod.invoke(CodeGenConfigurationInstance, new Object[]{stringReturn});
		
        //new CodeGenerationEngine(codegenConfig).generate();
		
		Class CodeGenerationEngineClass = ClassLoadingUtil
				.loadClassFromAntClassLoader("org.apache.axis2.wsdl.codegen.CodeGenerationEngine");
		Constructor CodeGenerationEngineConstructor = CodeGenerationEngineClass
					.getConstructor(new Class[]{CodeGenConfigurationInstance.getClass()});
		Object CodeGenerationEngineInstance  = CodeGenerationEngineConstructor
					.newInstance(new Object[]{CodeGenConfigurationInstance});
		
		//Invoke Codegen Method
		Method generateMethod = CodeGenerationEngineClass.getMethod("generate", null);
		generateMethod.invoke(CodeGenerationEngineInstance, null);
      
		//Copy the existing codegen results to the current project
        ContentCopyUtils contentCopyUtils = new ContentCopyUtils();
	    status = contentCopyUtils.copyDirectoryRecursivelyIntoWorkspace(
	    							tempCodegenOutputLocation, 
	    							currentDynamicWebProjectDir, 
	    							monitor, 
	    							statusHandler);
      
	} catch (Exception e) {
		status = StatusUtils.errorStatus(NLS.bind(
											WSASConsumptionUIMessages.ERROR_CODEGEN_EXCEPTION,
											new String[]{" : "+e.getCause().getMessage()}),
										e);
		environment.getStatusHandler().reportError(status); 
	}
		
		return status;
	}

}
