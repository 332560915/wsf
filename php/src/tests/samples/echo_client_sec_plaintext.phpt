--TEST--
Test for echo_client_sec_plaintext sample
--FILE--
<?php

$reqPayloadString = <<<XML
<ns1:echo xmlns:ns1="http://php.axis2.org/samples"><text>Hello World!</text></ns1:echo>
XML;

try {

    $reqMessage = new WSMessage($reqPayloadString,
                                array("to"=>"http://localhost/samples/security/username_token/username_token_plaintText.php",
                                      "action" => "http://php.axis2.org/samples/echoString"));
    
    $sec_array = array("useUsernameToken"=>TRUE );
    $policy = new WSPolicy(array("security"=>$sec_array));
    $sec_token = new WSSecurityToken(array("user" => "Raigama",
                                           "password" => "RaigamaPW",
                                           "passwordType" => "PlainText"));
    
    $client = new WSClient(array("useWSA" => TRUE,
                                 "policy" => $policy,
                                 "securityToken" => $sec_token));
				
    $resMessage = $client->request($reqMessage);
    
    printf("Response = %s \n", $resMessage->str);

} catch (Exception $e) {

	if ($e instanceof WSFault) {
		printf("Soap Fault: %s\n", $e->Reason);
	} else {
		printf("Message = %s\n",$e->getMessage());
	}

}
?>
--EXPECT--
Response = <ns1:echo xmlns:ns1="http://php.axis2.org/samples"><text>Hello World!</text></ns1:echo>
