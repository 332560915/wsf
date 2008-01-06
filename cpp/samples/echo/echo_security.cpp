#include <stdio.h>
#include <WSSOAPClient.h>
#include <OMElement.h>
#include <iostream>
#include <AxisFault.h>
using namespace std;
using namespace wso2wsf;

int main(int argc, char *argv[])
{
    string end_point, client_repo, policy_file;
    WSSOAPClient * sc;

    end_point = "http://localhost:9090/axis2/services/echo";
    client_repo = "../client_repo";
    policy_file = "policy.xml";

    if (argc > 1)
    {
        if (string (argv[1]).compare("-h") == 0)
        {
            cout << "Usage : " << argv[0] << " [client_repo] [end_point] [policy_file]" << endl;
            cout << "use -h for help" << endl;
            cout << "default client_repo " << client_repo << endl;
            cout << "default end_point " << end_point << endl;
            cout << "default policy_file " << policy_file << endl;
            cout << "NOTE: command line arguments must appear in given order, with trailing ones being optional" << endl;
            return 0;
        }
        else
            client_repo = argv[1];
    }
    if (argc > 2)
        end_point = argv[2];
    if (argc > 3)
        policy_file = argv[3];

    sc = new WSSOAPClient(client_repo, end_point);
    sc->initializeClient("echo_security.log", AXIS2_LOG_LEVEL_TRACE);
    sc->engageModule(AXIS2_MODULE_ADDRESSING);
    sc->setPolicy(new NeethiPolicy(client_repo + policy_file));
    {
        OMNamespace * ns = new OMNamespace("http://ws.apache.org/rampart/c/samples", "ns1");
        OMElement * payload = new OMElement(NULL,"echoIn", ns);
        OMElement * child = new OMElement(payload,"text", NULL);
        child->setText("Hello");
        cout << endl << "Request: " << payload << endl;
        OMElement * response;
        try
        {
            response = sc->request(payload, "http://example.com/ws/2004/09/policy/Test/EchoRequest");
            if (response)
            {
                cout << endl << "Response: " << sc->getLastResponseSoapEnvelopeString() << endl;
            }
        }
        catch (AxisFault & e)
        {
            if (sc->getLastSOAPFault())
            {
                cout << endl << "Response: " << sc->getLastSOAPFault() << endl;
            }
            else
            {
                cout << endl << "Response: " << e << endl;
            }
        }
        delete payload;
    }
    delete sc;
}

