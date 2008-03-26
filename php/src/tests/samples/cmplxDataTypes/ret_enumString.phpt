--TEST--
RetenumString
--FILE--
<?php

class RetEnumStringWrapper
{
    public $inEnumString;
}


class BitMaskWrapper
{
    public $enumeration;
      
}

class RetEnumStringResponseWrapper
{
    public $RetEnumStringResult;
}


$classmap = array("RetEnumString" => "RetEnumStringWrapper",
                  "RetEnumStringResponse" => "RetEnumStringResponseWrapper",
                  "BitMask" => "BitMaskWrapper");
                  

try{
     $wsdl = file_get_contents('config/cmplxDataTypesWsdlDoclitW.txt');
    $client = new WSClient(array("wsdl"=>$wsdl, "classmap" => $classmap));

    $input = new RetEnumStringWrapper();
    $obj = new BitMaskWrapper();

    $obj->enumeration = "BitTwo";
    
    $input->inEnumString = $obj;
    
    $proxy = $client->getProxy();
    $val = $proxy->RetEnumString($input);
    echo "Result is:"."\n"; 
    var_dump($val);

}catch (Exception $e) {

    if ($e instanceof WSFault) {
        printf("Soap Fault Reason: %s\n", $e->Reason);
        printf("Soap Fault Code: %s \n", $e->Code);

    } else {
        printf("Message = %s\n",$e->getMessage());
    }

}
?>
--EXPECT--
Result is:
object(RetEnumStringResponseWrapper)#16 (1) {
  ["RetEnumStringResult"]=>
  string(6) "BitTwo"
}
