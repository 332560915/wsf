# Copyright 2005 - 2008 WSO2, Inc. http://wso2.com
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

package WSO2::WSF::WSService;

use WSO2::WSF::WSConfig;
use WSO2::WSF::WSRequest;
use WSO2::WSF::Server;
use Data::Dumper;
use CGI;

sub new {
    my $class = shift;
    my $self = {
        -rh => shift,
        -request => new WSO2::WSF::WSRequest ()
    };
    my $request = $self->{-request};
    $request->populate;

    bless $self, $class;
    return $self;
}

# test method is for debugging purposes
sub test {
    my ($self) = @_;
    my ($value, $key);
    my $operation = $self->{-rh};
    my $rh_op = $operation->{'operations'};
    while (($key, $value) = each (%$rh_op)){
        print "key value is $key and value is $value \n";
    }
    print "action ".$operation->{'action'}."\n";
}

sub reply {
    my ($self) = @_;
    my $request = $self->{-request};
    my $worker = WSO2::WSF::Server::wsf_get_worker();
    my $svc_info = WSO2::WSF::Server::wsf_svc_info_t->new ();
    my $req_info = WSO2::WSF::Server::wsf_req_info_t->new ();

    #populating wsf_request_info structure
    WSO2::WSF::Serverc::wsf_req_info_t_content_encoding_set($req_info, $request->{-content_type});
    WSO2::WSF::Serverc::wsf_req_info_t_svr_name_set($req_info, $request->{-server_name});
    WSO2::WSF::Serverc::wsf_req_info_t_svr_port_set($req_info, $request->{-server_port});
    WSO2::WSF::Serverc::wsf_req_info_t_http_protocol_set($req_info, $request->{-server_protocol});
    WSO2::WSF::Serverc::wsf_req_info_t_soap_action_set($req_info, $request->{-soap_action});
    WSO2::WSF::Serverc::wsf_req_info_t_req_data_set($req_info, $request->{-postdata});
    WSO2::WSF::Serverc::wsf_req_info_t_request_method_set($req_info, $request->{-request_method});

    #populating svc_info struct
}


1;
