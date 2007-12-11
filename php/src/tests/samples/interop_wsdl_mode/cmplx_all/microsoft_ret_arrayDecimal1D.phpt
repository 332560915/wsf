--TEST--
RetArrayDecimal1D
--FILE--
<?php

class RetArrayDecimal1DWrapper
{
    public $inArrayDecimal1D;
}


class ArrayOfNullableOfdecimal1DWrapper
{
    public $decimal;
}

class RetArrayDecimal1DResponseWrapper
{
    public $RetArrayDecimal1DResult;
}



$classmap = array("RetArrayDecimal1D" => "RetArrayDecimal1DWrapper",
                  "ArrayOfNullableOfdecimal" => "ArrayOfNullableOfdecimal1DWrapper",
                  "RetArrayDecimal1DResponse" => "RetArrayDecimal1DResponseWrapper");


try{

    $client = new WSClient(array("wsdl"=> "http://131.107.72.15/SoapWsdl_ComplexDataTypes_XmlFormatter_Service_Indigo/ComplexDataTypesDocLitW.svc?wsdl"/*  "./wsdls/ComplexDataTypesDocLitW.wsdl" */,
                                 "classmap" => $classmap));

    $input = new RetArrayDecimal1DWrapper();
    $obj = new ArrayOfNullableOfdecimal1DWrapper();
    $obj->decimal = 0.61803398874989484820458683;

    $input->inArrayDecimal1D = $obj;

    
    $proxy = $client->getProxy();
    $val = $proxy->RetArrayDecimal1D($input);
    print_r($val);

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
RetArrayDecimal1DResponseWrapper Object
(
    [RetArrayDecimal1DResult] => ArrayOfNullableOfdecimal1DWrapper Object
        (
            [decimal] => Array
                (
                    [0] => 0.61803398875
                )

        )

)
