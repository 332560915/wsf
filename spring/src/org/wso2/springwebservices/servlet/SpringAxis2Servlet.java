package org.wso2.springwebservices.servlet;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.axis2.Constants;
import org.apache.axis2.context.ConfigurationContext;
import org.apache.axis2.context.ConfigurationContextFactory;
import org.apache.axis2.description.AxisService;
import org.apache.axis2.spring.SpringBeanSystemConfigurator;
import org.apache.axis2.spring.SpringWebServiceBuilder;
import org.apache.axis2.transport.http.AxisServlet;
import org.springframework.context.ApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;
import org.wso2.springwebservices.SpringWebService;
import org.wso2.springwebservices.WebServices;
import org.wso2.springwebservices.beans.MessageReceiverBean;
import org.wso2.springwebservices.util.Utils;




public class SpringAxis2Servlet extends AxisServlet {

    private AxisService axisService;
	
    public void init(ServletConfig servletConfig)
            throws ServletException {
        super.init(servletConfig);
        ServletContext servletContext = servletConfig.getServletContext();
        
        // Spring application context is retrieved from the servlet context
        ApplicationContext applicationContext =
                WebApplicationContextUtils.getWebApplicationContext(servletContext);

        // Retrieve the services bean and get the service list from the app context
        WebServices servicesBean = Utils.getServicesfromAppContext(applicationContext);
        ArrayList<SpringWebService> services = servicesBean.getServices();
        
        if (!(services == null)) {
        	Iterator it = services.iterator();

        	while (it.hasNext()) {
        		SpringWebService springService = (SpringWebService) it.next();
        		
        		//Retrieve data required to build service
        		String beanName = springService.getBeanIDofBeanToExpose();
        		String clazz = applicationContext.getBean(beanName).getClass().getName();
        		
        		//Start building axis service        		
        		axisService = new AxisService(beanName + "SpringService");
        		
        		SpringWebServiceBuilder builder = new SpringWebServiceBuilder(configContext, axisService);
        		
        		try {
        			// populate Spring bean with common parameters
        			
        			springService = populateCommonParameters(springService, clazz);
        			
        			// populate Axis Service
        			axisService = builder.populateService(springService);
        			
        			// add service to axis configuration
        			this.axisConfiguration.addService(axisService);

        		
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace(System.out);
					throw new ServletException("Could not populate Web Service with required Spring bean");
				}
        	}
        }


    }
    
    
    // populate Spring Web Service bean with parameters common to POJOs
    
    public SpringWebService populateCommonParameters(SpringWebService springService, String clazz) {
    	ArrayList<MessageReceiverBean> msgReceivers = springService.getMessageReceivers();
    	
    	// populate common message receivers 
    	MessageReceiverBean inOnlyReceiver = new MessageReceiverBean();
    	inOnlyReceiver.setMep("http://www.w3.org/2004/08/wsdl/in-only");
    	inOnlyReceiver.setClazz("org.apache.axis2.rpc.receivers.RPCInOnlyMessageReceiver");
    	
    	MessageReceiverBean inOutOnlyReceiver = new MessageReceiverBean();
    	inOutOnlyReceiver.setMep("http://www.w3.org/2004/08/wsdl/in-out");
    	inOutOnlyReceiver.setClazz("org.apache.axis2.rpc.receivers.RPCMessageReceiver");
    	
    	if (msgReceivers == null) {
    		msgReceivers = new ArrayList<MessageReceiverBean>();
    	}
    	msgReceivers.add(inOnlyReceiver);
    	msgReceivers.add(inOutOnlyReceiver);
    	
    	// populate common parameters
    	
    	Map parameters = springService.getParameters();
    	
    	if (parameters == null) {
    		parameters = new HashMap();
    	}
    	
    	parameters.put("ServiceObjectSupplier", "org.apache.axis2.extensions.spring.receivers.SpringServletContextObjectSupplier");
    	parameters.put("SpringBeanName", springService.getBeanIDofBeanToExpose());
    	parameters.put("ServiceClass", clazz);
    	
    	springService.setMessageReceivers(msgReceivers);
    	springService.setParameters(parameters);
    	
    	return springService;
    }
    
    
	@Override
	protected ConfigurationContext initConfigContext(ServletConfig config)
			throws ServletException {
		// TODO Auto-generated method stub
		ServletContext servletContext = servletConfig.getServletContext();
        
		// Spring application context is retrieved from the servlet context
		ApplicationContext applicationContext =
                WebApplicationContextUtils.getWebApplicationContext(servletContext);
		
        try {
        // Spring bean system configurator created from the application context
        	SpringBeanSystemConfigurator sbsc = 
        		new SpringBeanSystemConfigurator(Utils.getAxisConfigfromAppContext(applicationContext), config);
        
        // Configuration context created from configurator
            ConfigurationContext configContext =
                    ConfigurationContextFactory
                            .createConfigurationContext(sbsc);
            configContext.setProperty(Constants.CONTAINER_MANAGED, Constants.VALUE_TRUE);
            return configContext;
        } catch (Exception e) {
            throw new ServletException(e);
        }
	}
}