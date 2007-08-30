--TEST--
Test for ws_message_consturctor_dom_object sample
--FILE--
<?php

$reqPayloadDomDoc = new DOMDocument();
$xmlFile = 'tests/samples/misc_files/echo.xml';

$reqPayloadDomDoc->load($xmlFile);

try {

    $reqMessage = new WSMessage($reqPayloadDomDoc,
        array("to"=>"http://localhost/samples/echo_service_addr.php",
              "action"=>"http://php.axis2.org/samples/echoString",
              "faultTo"=>"http://php.axis2.org/samples/echoString",
              "replyTo"=>"http://localhost/samples/echo_service.php",
              "from"=>"http://localhost/samples/echo_service_addr.php"));
              
    $client = new WSClient(array("useWSA" => TRUE));
				
    $resMessage = $client->request($reqMessage);
    
    printf("Response = %s \n", $resMessage->str);

} catch (Exception $e) {

	if ($e instanceof WSFault) {
		printf("Soap Fault: %s\n", $e->code);
	} else {
		printf("Message = %s\n",$e->getMessage());
	}

}
?>
--EXPECT--
Response = <ns1:echo xmlns:ns1="http://php.axis2.org/samples">
		<text>Hello World!</text>
	</ns1:echo>
