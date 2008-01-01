#include <stdio.h>
#include <WSSOAPClient.h>
#include <OMElement.h>
#include <iostream>
#include <AxisFault.h>
using namespace std;
using namespace wso2wsf;

int main()
{
    WSSOAPClient * sc = new WSSOAPClient("http://localhost:9090/axis2/services/math");
    sc->initializeClient("math_blocking.log", AXIS2_LOG_LEVEL_TRACE);
    sc->engageModule(AXIS2_MODULE_ADDRESSING);
    {
        OMNamespace * ns = new OMNamespace("http://ws.apache.org/axis2/services/math", "ns1");
        OMElement * payload = new OMElement(NULL,"add", ns);
        OMElement * child1 = new OMElement(payload,"param1", NULL);
        child1->setText("40");
        OMElement * child2 = new OMElement(payload,"param2", NULL);
        child2->setText("8");
        printf ((payload->toString()).c_str());
        cout << endl;
        OMElement * response;
        try
        {
            response = sc->request(payload, "http://ws.apache.org/axis2/c/samples/echoString");
            if (response)
            {
                cout << endl << "Response: " << response->toString() << endl;
            }
        }
        catch (AxisFault * e)
        {
            cout << endl << "Response: " << (sc->getLastSOAPFault())->toString() << endl;
        }
        delete payload;
    }
    delete sc;
}

