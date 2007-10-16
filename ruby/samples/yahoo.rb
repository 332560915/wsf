#!/usr/bin/env ruby         
# Copyright 2005,2006,2007 WSO2, Inc. http://wso2.com
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

require 'wsf'

req_payload_string = <<XML
<webSearch><appid>ApacheRestDemo</appid><query>SriLanka</query><form/></webSearch>
XML

begin
  options = Hash.new
  options = {:to => "http://search.yahooapis.com/WebSearchService/V1/webSearch",
             :http_method => "GET",
             :use_soap => false}
  client = WSClient.new(options)

  res_message = client.request(req_payload_string)
  
  puts res_message.payload_to_s

rescue WSFault => wsfault
  puts wsfault.xml
  puts "----------"
  puts wsfault.code
  puts "----------"
  puts wsfault.reason
  puts "----------"
  puts wsfault.role
  puts "----------"
  puts wsfault.detail
  puts "----------"

end
