--TEST--
Float_Nan(NaN)
--FILE--
<?php

try {

    $client = new WSClient(
                  array("wsdl" => "http://131.107.72.15/SoapWsdl_BaseDataTypes_XmlFormatter_Service_Indigo/BaseDataTypesDocLitB.svc?wsdl"));
    
    $proxy = $client->getProxy();
    $val =  $proxy->RetFloat(array(NaN));
    echo $val."\n";

} catch (Exception $e) {

        if ($e instanceof WSFault) {
                printf("Soap Fault: %s\n", $e->Code);
                printf("Soap Reason: %s\n", $e->Reason);
        } else {
                printf("Message = %s\n",$e->getMessage());
        }
}
?>
--EXPECT--
NaN
