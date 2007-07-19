<?php

include_once '../../soap_ext.php';

try {
    $client = new WSSoapClient(null, 
        array('location' => 'https://localhost:9090/soap/soap_ext/tests/ws_hello_service_nonwsdl.php',
              'uri' => 'http://localhost/axis/helloService',
              'soap_version' => SOAP_1_2 ));

    $a = 1; $b = 2; $c = 3;
    $result =  $client->__soapCall('SomeFunction', array($a, $b, $c), NULL, NULL);

    printf("Result = %s\n", $result);

} catch (Exception $e) {
    printf("Message = %s\n",$e->__toString());
}
?> 

