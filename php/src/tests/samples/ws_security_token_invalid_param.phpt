--TEST--
Test for ws_security_token_invalid_param sample
--FILE--
<?php

$reqPayloadString = <<<XML
<ns1:echo xmlns:ns1="http://php.axis2.org/samples"><text>Hello World!</text></ns1:echo>
XML;

try {
    $rec_cert = ws_get_cert_from_file("C:/Apache2.2/htdocs/samples/security/keys/bob_cert.cert");
    $pvt_key = ws_get_key_from_file("C:/Apache2.2/htdocs/samples/security/keys/alice_key.pem");
    
    $reqMessage = new WSMessage($reqPayloadString,
                                array("to"=>"http://localhost/samples/security/encryption/encrypt_service.php",
                                      "action" => "http://php.axis2.org/samples/echoString"));
    
    $sec_array = array("encrypt"=>TRUE,
                       "algorithmSuite" => "Basic256Rsa15",
                       "securityTokenRefernce" => "IssuerSerial");
    
    $policy = new WSPolicy(array("security"=>$sec_array));
    $sec_token = new WSSecurityToken(10);
    
    $client = new WSClient(array("useWSA" => TRUE,
                                 "policy" => $policy,
                                 "securityToken" => $sec_token));
				
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
Response = <ns1:echo xmlns:ns1="http://php.axis2.org/samples"><text>Hello World!</text></ns1:echo>


