/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*       This file containes the lookup-information for the ApSettingsDlg.
*       Gives the order of the fields, number of the fields, etc.
*
*/



#ifndef APSETTINGS_LOOKUPS_H
#define APSETTINGS_LOOKUPS_H

// INCLUDE FILES
#include <ApAccessPointItem.h>
#include <ApSetUI.rsg>

#include <ApSettingsHandlerCommons.h>


// CONSTANTS
const TInt KListIndexConnectionName = 0;
const TInt KListIndexBearerType = 1;
const TInt KListIndexCSDAccessNumber = 2;

const TInt KListIndexWLANNetworkName = 2;

const TInt KListIndexWLANSubnetMask = 1;
const TInt KListIndexWLANDefaultGateway = 2;
const TInt KListIndexWLANSecuritySettings = 6;



const TInt KApMember = 0;
const TInt KResourceID = 1;
const TInt KCompulsory = 2;







const TInt Fields_CSD_L2_IPv4[3][2] =
    {
        {
        EApIspIPAddr,                       ///< Phone IP addr.
        EApIP6DNSAddrFromServer             ///< Domain name servers
        },
        {
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_VIEW_SET_DNS_SERVERS_IP      ///< Domain name servers
        },
        {
        0,                                  ///< Phone IP addr.
        0                                   ///< Domain name servers
        }
    };


const TInt Fields_CSD_L2_IPv6[3][1] =
    {
        {
        EApIP6DNSAddrFromServer             ///< Domain name servers
        },
        {
        R_APUI_VIEW_SET_DNS_SERVERS_IP      ///< Domain name servers
        },
        {
        0                                   ///< Domain name servers
        }
    };



//----------------------------------------------------------
// Settings with the KWSPSettingsVisible flag defined
//----------------------------------------------------------
//
const TInt Fields_GPRS_WSP[3][8] =
    {
        {
        EApWapAccessPointName,      ///< Connection name
        EApWapCurrentBearer,        ///< Bearer type

        EApGprsAccessPointName,     ///< Access Point Name
        EApIspIfAuthName,           // username
        EApIspPromptForLogin,       ///< prompt password
        EApIspIfAuthPass,           // password
        EApGprsDisablePlainTextAuth,///< password authentication
        EApWapStartPage             ///< Starting Page
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_AP_NAME,                ///< Access Point Name
        R_APUI_VIEW_USER_NAME,              ///< username
        R_APUI_VIEW_PROMPT_PASSWORD,        ///< prompt password
        R_APUI_VIEW_PASSWORD,               ///< password
        R_APUI_VIEW_PASSWORD_AUTH,          ///< password authentication
        R_APUI_VIEW_STARTING_PAGE           ///< Starting Page
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        0,                                  ///< Access Point Name
        0,                                  ///< username
        0,                                  ///< prompt password
        0,                                  ///< password
        0,                                  ///< password authentication
        0                                   ///< Starting Page
        }
    };



const TInt Fields_GPRS_Advanced_WSP[3][8] =
    {
        {
        EApIspIPAddr,               ///< Phone IP addr.
        EApGprsIPNameServer1,       ///< Primary Name server
        EApGprsIPNameServer2,       ///< Secondary Name server
        EApProxyServerAddress,      ///< Proxy server address
        EApProxyPortNumber,         ///< Proxy port number
        EApWapGatewayAddress,       ///< Wap Gateway IP address
        EApWapSecurity,             ///< WTLS Security
        EApWapWspOption             ///< Connection type
        },
        {
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_VIEW_PRIMARY_NAME_SERVER,    ///< Primary Name server
        R_APUI_VIEW_SECONDARY_NAME_SERVER,  ///< Secondary Name server
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER,      ///< Proxy port number
        R_APUI_VIEW_WAP_GW_IP_ADDR,         ///< Wap Gateway IP address
        R_APUI_VIEW_WTLS_SECURITY,          ///< WTLS Security
        R_APUI_VIEW_CONNECTION_TYPE         ///< Connection type
        },
        {
        0,                                  ///< Phone IP addr.
        0,                                  ///< Primary Name server
        0,                                  ///< Secondary Name server
        0,                                  ///< Proxy server address
        0,                                  ///< Proxy port number
        0,                                  ///< Wap Gateway IP address
        0,                                  ///< WTLS Security
        0                                   ///< Connection type
        }
    };



const TInt Fields_CSD_WSP[3][10] =
    {
        {
        EApWapAccessPointName,          ///< Connection name
        EApWapCurrentBearer,            ///< Bearer type

        EApIspDefaultTelNumber,         ///< Access Number
        EApIspIfAuthName,               // username
        EApIspPromptForLogin,           ///< prompt password
        EApIspIfAuthPass,               // password
        EApGprsDisablePlainTextAuth,    ///< password authentication
        EApWapStartPage,                ///< Starting Page
        EApIspBearerCallTypeIsdn,       ///< Data call type 
                                        ///< (analogue/ISDN110/ISDN120)
        EApIspBearerSpeed               ///< Max. connection speed
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_ACCESS_NUMBER,          ///< Access Number
        R_APUI_VIEW_USER_NAME,              ///< username
        R_APUI_VIEW_PROMPT_PASSWORD,        ///< prompt password
        R_APUI_VIEW_PASSWORD,               ///< password
        R_APUI_VIEW_PASSWORD_AUTH,          ///< password authentication
        R_APUI_VIEW_STARTING_PAGE,          ///< Starting Page
        R_APUI_VIEW_DATA_CALL_TYPE,         ///< Data call type (analogue/ISDN)
        R_APUI_VIEW_MAX_CONN_SPEED          ///< Max. connection speed
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        1,                                  ///< Access Number
        0,                                  ///< username
        0,                                  ///< prompt password
        0,                                  ///< password
        0,                                  ///< password authentication
        0,                                  ///< Starting Page
        0,                                  ///< Data call type (analogue/ISDN)
        0                                   ///< Max. connection speed
        }
    };


const TInt Fields_CSD_Advanced_WSP[3][15] =
    {
        {
        EApIspIPAddr,               ///< Phone IP addr.
        EApGprsIPNameServer1,       ///< Primary Name server
        EApGprsIPNameServer2,       ///< Secondary Name server
        EApProxyServerAddress,      ///< Proxy server address
        EApProxyPortNumber,         ///< Proxy port number
        EApIspIfCallbackEnabled,    ///< use callback
        EApIspIfCallbackType,       ///< call back type
        EApIspIfCallbackInfo,       ///< call back number
        EApIspEnableSWCompression,  ///< Enable SW compression
        EApIspUseLoginScript,       ///< Use login script
        EApIspLoginScript,          ///< The login script
        EApIspInitString,           ///< Modem init string
        EApWapGatewayAddress,           ///< Wap Gateway IP address
        EApWapSecurity,                 ///< WTLS Security
        EApWapWspOption                 ///< Connection type
        },
        {
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_VIEW_PRIMARY_NAME_SERVER,    ///< Primary Name server
        R_APUI_VIEW_SECONDARY_NAME_SERVER,  ///< Secondary Name server
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER,      ///< Proxy port number
        R_APUI_VIEW_USE_CALL_BACK,          ///< use callback
        R_APUI_VIEW_CALL_BACK_TYPE,         ///< call back type
        R_APUI_VIEW_CALL_BACK_NUMBER,       ///< call back number
        R_APUI_VIEW_ENABLE_PPP_COMP,        ///< Enable PPP compression
        R_APUI_VIEW_USE_LOGIN_SCRIPT,       ///< Use login script
        R_APUI_VIEW_LOGIN_SCRIPT,           ///< The login script
        R_APUI_VIEW_MODEM_INIT_STRING,      ///< Modem init string
        R_APUI_VIEW_WAP_GW_IP_ADDR,         ///< Wap Gateway IP address
        R_APUI_VIEW_WTLS_SECURITY,          ///< WTLS Security
        R_APUI_VIEW_CONNECTION_TYPE         ///< Connection type
        },
        {
        0,                                  ///< Phone IP addr.
        0,                                  ///< Primary Name server
        0,                                  ///< Secondary Name server
        0,                                  ///< Proxy server address
        0,                                  ///< Proxy port number
        0,                                  ///< use callback
        0,                                  ///< call back type
        0,                                  ///< call back number
        0,                                  ///< Enable PPP compression
        0,                                  ///< Use login script
        0,                                  ///< The login script
        0,                                  ///< Modem init string
        0,                                  ///< Wap Gateway IP address
        0,                                  ///< WTLS Security
        0                                   ///< Connection type
        }
    };






//----------------------------------------------------------
// Normal settings, no flags defined
//----------------------------------------------------------
//
const TInt Fields_GPRS[3][8] =
    {
        {
        EApWapAccessPointName,      ///< Connection name
        EApWapCurrentBearer,        ///< Bearer type

        EApGprsAccessPointName,     ///< Access Point Name
        EApIspIfAuthName,           // username
        EApIspPromptForLogin,       ///< prompt password
        EApIspIfAuthPass,           // password
        EApGprsDisablePlainTextAuth,///< password authentication
        EApWapStartPage             ///< Starting Page
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_AP_NAME,                ///< Access Point Name
        R_APUI_VIEW_USER_NAME,              ///< username
        R_APUI_VIEW_PROMPT_PASSWORD,        ///< prompt password
        R_APUI_VIEW_PASSWORD,               ///< password
        R_APUI_VIEW_PASSWORD_AUTH,          ///< password authentication
        R_APUI_VIEW_STARTING_PAGE           ///< Starting Page
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        0,                                  ///< Access Point Name
        0,                                  ///< username
        0,                                  ///< prompt password
        0,                                  ///< password
        0,                                  ///< password authentication
        0                                   ///< Starting Page
        }
    };



const TInt Fields_GPRS_Advanced[3][5] =
    {
        {
        EApIspIPAddr,               ///< Phone IP addr.
        EApGprsIPNameServer1,       ///< Primary Name server
        EApGprsIPNameServer2,       ///< Secondary Name server
        EApProxyServerAddress,      ///< Proxy server address
        EApProxyPortNumber          ///< Proxy port number
        },
        {
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_VIEW_PRIMARY_NAME_SERVER,    ///< Primary Name server
        R_APUI_VIEW_SECONDARY_NAME_SERVER,  ///< Secondary Name server
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER       ///< Proxy port number
        },
        {
        0,                                  ///< Phone IP addr.
        0,                                  ///< Primary Name server
        0,                                  ///< Secondary Name server
        0,                                  ///< Proxy server address
        0                                   ///< Proxy port number
        }
    };



const TInt Fields_CSD[3][10] =
    {
        {
        EApWapAccessPointName,          ///< Connection name
        EApWapCurrentBearer,            ///< Bearer type

        EApIspDefaultTelNumber,         ///< Access Number
        EApIspIfAuthName,               // username
        EApIspPromptForLogin,           ///< prompt password
        EApIspIfAuthPass,               // password
        EApGprsDisablePlainTextAuth,    ///< password authentication
        EApWapStartPage,                ///< Starting Page
        EApIspBearerCallTypeIsdn,       ///< Data call type 
                                        ///< (analogue/ISDN110/ISDN120)
        EApIspBearerSpeed               ///< Max. connection speed
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_ACCESS_NUMBER,          ///< Access Number
        R_APUI_VIEW_USER_NAME,              ///< username
        R_APUI_VIEW_PROMPT_PASSWORD,        ///< prompt password
        R_APUI_VIEW_PASSWORD,               ///< password
        R_APUI_VIEW_PASSWORD_AUTH,          ///< password authentication
        R_APUI_VIEW_STARTING_PAGE,          ///< Starting Page
        R_APUI_VIEW_DATA_CALL_TYPE,         ///< Data call type 
                                            ///< (analogue/ISDN)
        R_APUI_VIEW_MAX_CONN_SPEED          ///< Max. connection speed
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        1,                                  ///< Access Number
        0,                                  ///< username
        0,                                  ///< prompt password
        0,                                  ///< password
        0,                                  ///< password authentication
        0,                                  ///< Starting Page
        0,                                  ///< Data call type (analogue/ISDN)
        0                                   ///< Max. connection speed
        }
    };



const TInt Fields_CSD_Advanced[3][12] =
    {
        {
        EApIspIPAddr,               ///< Phone IP addr.
        EApGprsIPNameServer1,       ///< Primary Name server
        EApGprsIPNameServer2,       ///< Secondary Name server
        EApProxyServerAddress,      ///< Proxy server address
        EApProxyPortNumber,         ///< Proxy port number
        EApIspIfCallbackEnabled,    ///< use callback
        EApIspIfCallbackType,       ///< call back type
        EApIspIfCallbackInfo,       ///< call back number
        EApIspEnableSWCompression,  ///< Enable SW compression
        EApIspUseLoginScript,       ///< Use login script
        EApIspLoginScript,          ///< The login script
        EApIspInitString            ///< Modem init string
        },
        {
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_VIEW_PRIMARY_NAME_SERVER,    ///< Primary Name server
        R_APUI_VIEW_SECONDARY_NAME_SERVER,  ///< Secondary Name server
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER,      ///< Proxy port number
        R_APUI_VIEW_USE_CALL_BACK,          ///< use callback
        R_APUI_VIEW_CALL_BACK_TYPE,         ///< call back type
        R_APUI_VIEW_CALL_BACK_NUMBER,       ///< call back number
        R_APUI_VIEW_ENABLE_PPP_COMP,        ///< Enable PPP compression
        R_APUI_VIEW_USE_LOGIN_SCRIPT,       ///< Use login script
        R_APUI_VIEW_LOGIN_SCRIPT,           ///< The login script
        R_APUI_VIEW_MODEM_INIT_STRING       ///< Modem init string
        },
        {
        0,                                  ///< Phone IP addr.
        0,                                  ///< Primary Name server
        0,                                  ///< Secondary Name server
        0,                                  ///< Proxy server address
        0,                                  ///< Proxy port number
        0,                                  ///< use callback
        0,                                  ///< call back type
        0,                                  ///< call back number
        0,                                  ///< Enable PPP compression
        0,                                  ///< Use login script
        0,                                  ///< The login script
        0                                   ///< Modem init string
        }
    };












//----------------------------------------------------------
// Settings with the KWSPSettingsVisible flag defined
// AND IPv6 Support enabled
//----------------------------------------------------------
//
const TInt Fields_GPRS_WSP_IPv6[3][8] =
    {
        {
        EApWapAccessPointName,      ///< Connection name
        EApWapCurrentBearer,        ///< Bearer type

        EApGprsAccessPointName,     ///< Access Point Name
        EApIspIfAuthName,           // username
        EApIspPromptForLogin,       ///< prompt password
        EApIspIfAuthPass,           // password
        EApGprsDisablePlainTextAuth,///< password authentication
        EApWapStartPage             ///< Starting Page
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_AP_NAME,                ///< Access Point Name
        R_APUI_VIEW_USER_NAME,              ///< username
        R_APUI_VIEW_PROMPT_PASSWORD,        ///< prompt password
        R_APUI_VIEW_PASSWORD,               ///< password
        R_APUI_VIEW_PASSWORD_AUTH,          ///< password authentication
        R_APUI_VIEW_STARTING_PAGE           ///< Starting Page
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        0,                                  ///< Access Point Name
        0,                                  ///< username
        0,                                  ///< prompt password
        0,                                  ///< password
        0,                                  ///< password authentication
        0                                   ///< Starting Page
        }
    };



const TInt Fields_GPRS_Advanced_WSP_IPv6[3][8] =
    {
        {
        EApGprsPdpType,             ///< PDP type, IPv4 or IPv6
        EApIspIPAddr,               ///< Phone IP addr.
        EApIP6DNSAddrFromServer,    ///< Domain name servers
        EApProxyServerAddress,      ///< Proxy server address
        EApProxyPortNumber,         ///< Proxy port number
        EApWapGatewayAddress,       ///< Wap Gateway IP address
        EApWapSecurity,             ///< WTLS Security
        EApWapWspOption             ///< Connection type
        },
        {
        R_APUI_VIEW_PDP_TYPE,               ///< PDP type, IPv4 or IPv6
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_VIEW_SET_DNS_SERVERS_IP,     ///< Domain name servers
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER,      ///< Proxy port number
        R_APUI_VIEW_WAP_GW_IP_ADDR,         ///< Wap Gateway IP address
        R_APUI_VIEW_WTLS_SECURITY,          ///< WTLS Security
        R_APUI_VIEW_CONNECTION_TYPE         ///< Connection type
        },
        {
        0,                                  ///< PDP type, IPv4 or IPv6
        0,                                  ///< Phone IP addr.
        0,                                  ///< Domain name servers
        0,                                  ///< Proxy server address
        0,                                  ///< Proxy port number
        0,                                  ///< Wap Gateway IP address
        0,                                  ///< WTLS Security
        0                                   ///< Connection type
        }
    };




const TInt Fields_CSD_WSP_IPv6[3][10] =
    {
        {
        EApWapAccessPointName,          ///< Connection name
        EApWapCurrentBearer,            ///< Bearer type

        EApIspDefaultTelNumber,         ///< Access Number
        EApIspIfAuthName,               // username
        EApIspPromptForLogin,           ///< prompt password
        EApIspIfAuthPass,               // password
        EApGprsDisablePlainTextAuth,    ///< password authentication
        EApWapStartPage,                ///< Starting Page
        EApIspBearerCallTypeIsdn,       ///< Data call type 
                                        ///< (analogue/ISDN110/ISDN120)
        EApIspBearerSpeed               ///< Max. connection speed
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_ACCESS_NUMBER,          ///< Access Number
        R_APUI_VIEW_USER_NAME,              ///< username
        R_APUI_VIEW_PROMPT_PASSWORD,        ///< prompt password
        R_APUI_VIEW_PASSWORD,               ///< password
        R_APUI_VIEW_PASSWORD_AUTH,          ///< password authentication
        R_APUI_VIEW_STARTING_PAGE,          ///< Starting Page
        R_APUI_VIEW_DATA_CALL_TYPE,         ///< Data call type (analogue/ISDN)
        R_APUI_VIEW_MAX_CONN_SPEED          ///< Max. connection speed
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        1,                                  ///< Access Number
        0,                                  ///< username
        0,                                  ///< prompt password
        0,                                  ///< password
        0,                                  ///< password authentication
        0,                                  ///< Starting Page
        0,                                  ///< Data call type (analogue/ISDN)
        0                                   ///< Max. connection speed
        }
    };


const TInt Fields_CSD_Advanced_WSP_IPv6[3][14] =
    {
        {
        EApIpv4Settings,                ///< Ipv4 Settings
        EApIpv6Settings,                ///< Ipv6 Settings
        EApProxyServerAddress,          ///< Proxy server address
        EApProxyPortNumber,             ///< Proxy port number
        EApIspIfCallbackEnabled,        ///< use callback
        EApIspIfCallbackType,           ///< call back type
        EApIspIfCallbackInfo,           ///< call back number
        EApIspEnableSWCompression,      ///< Enable SW compression
        EApIspUseLoginScript,           ///< Use login script
        EApIspLoginScript,              ///< The login script
        EApIspInitString,               ///< Modem init string
        EApWapGatewayAddress,           ///< Wap Gateway IP address
        EApWapSecurity,                 ///< WTLS Security
        EApWapWspOption                 ///< Connection type
        },
        {
        R_APUI_VIEW_IPV4_SETTINGS,          ///< Ipv4 Settings
        R_APUI_VIEW_IPV6_SETTINGS,          ///< Ipv6 Settings
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER,      ///< Proxy port number
        R_APUI_VIEW_USE_CALL_BACK,          ///< use callback
        R_APUI_VIEW_CALL_BACK_TYPE,         ///< call back type
        R_APUI_VIEW_CALL_BACK_NUMBER,       ///< call back number
        R_APUI_VIEW_ENABLE_PPP_COMP,        ///< Enable PPP compression
        R_APUI_VIEW_USE_LOGIN_SCRIPT,       ///< Use login script
        R_APUI_VIEW_LOGIN_SCRIPT,           ///< The login script
        R_APUI_VIEW_MODEM_INIT_STRING,      ///< Modem init string
        R_APUI_VIEW_WAP_GW_IP_ADDR,         ///< Wap Gateway IP address
        R_APUI_VIEW_WTLS_SECURITY,          ///< WTLS Security
        R_APUI_VIEW_CONNECTION_TYPE         ///< Connection type
        },
        {
        0,                                  ///< Ipv4 Settings
        0,                                  ///< Ipv6 Settings
        0,                                  ///< Proxy server address
        0,                                  ///< Proxy port number
        0,                                  ///< use callback
        0,                                  ///< call back type
        0,                                  ///< call back number
        0,                                  ///< Enable PPP compression
        0,                                  ///< Use login script
        0,                                  ///< The login script
        0,                                  ///< Modem init string
        0,                                  ///< Wap Gateway IP address
        0,                                  ///< WTLS Security
        0                                   ///< Connection type
        }
    };









//----------------------------------------------------------
// Normal settings, only Ipv6 defined
//----------------------------------------------------------
//
const TInt Fields_GPRS_IPv6[3][8] =
    {
        {
        EApWapAccessPointName,      ///< Connection name
        EApWapCurrentBearer,        ///< Bearer type

        EApGprsAccessPointName,     ///< Access Point Name
        EApIspIfAuthName,           // username
        EApIspPromptForLogin,       ///< prompt password
        EApIspIfAuthPass,           // password
        EApGprsDisablePlainTextAuth,///< password authentication
        EApWapStartPage             ///< Starting Page
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_AP_NAME,                ///< Access Point Name
        R_APUI_VIEW_USER_NAME,              ///< username
        R_APUI_VIEW_PROMPT_PASSWORD,        ///< prompt password
        R_APUI_VIEW_PASSWORD,               ///< password
        R_APUI_VIEW_PASSWORD_AUTH,          ///< password authentication
        R_APUI_VIEW_STARTING_PAGE           ///< Starting Page
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        0,                                  ///< Access Point Name
        0,                                  ///< username
        0,                                  ///< prompt password
        0,                                  ///< password
        0,                                  ///< password authentication
        0                                   ///< Starting Page
        }
    };



const TInt Fields_GPRS_Advanced_IPv6[3][5] =
    {
        {
        EApGprsPdpType,                     ///< PDP type, IPv4 or IPv6
        EApIspIPAddr,                       ///< Phone IP addr.
        EApIP6DNSAddrFromServer,            ///< Domain name servers
        EApProxyServerAddress,              ///< Proxy server address
        EApProxyPortNumber                  ///< Proxy port number
        },
        {
        R_APUI_VIEW_PDP_TYPE,               ///< PDP type, IPv4 or IPv6
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_VIEW_SET_DNS_SERVERS_IP,     ///< Domain name servers        
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER       ///< Proxy port number
        },
        {
        0,                                  ///< PDP type, IPv4 or IPv6
        0,                                  ///< Phone IP addr.
        0,                                  ///< Domain name servers
        0,                                  ///< Proxy server address
        0                                   ///< Proxy port number
        }
    };



const TInt Fields_CSD_IPv6[3][10] =
    {
        {
        EApWapAccessPointName,              ///< Connection name
        EApWapCurrentBearer,                ///< Bearer type

        EApIspDefaultTelNumber,             ///< Access Number
        EApIspIfAuthName,                   ///< username
        EApIspPromptForLogin,               ///< prompt password
        EApIspIfAuthPass,                   ///< password
        EApGprsDisablePlainTextAuth,        ///< password authentication
        EApWapStartPage,                    ///< Starting Page
        EApIspBearerCallTypeIsdn,           ///< Data call type 
                                            ///< (analogue/ISDN110/ISDN120)
        EApIspBearerSpeed                   ///< Max. connection speed
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_ACCESS_NUMBER,          ///< Access Number
        R_APUI_VIEW_USER_NAME,              ///< username
        R_APUI_VIEW_PROMPT_PASSWORD,        ///< prompt password
        R_APUI_VIEW_PASSWORD,               ///< password
        R_APUI_VIEW_PASSWORD_AUTH,          ///< password authentication
        R_APUI_VIEW_STARTING_PAGE,          ///< Starting Page
        R_APUI_VIEW_DATA_CALL_TYPE,         ///< Data call type (analogue/ISDN)
        R_APUI_VIEW_MAX_CONN_SPEED          ///< Max. connection speed
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        1,                                  ///< Access Number
        0,                                  ///< username
        0,                                  ///< prompt password
        0,                                  ///< password
        0,                                  ///< password authentication
        0,                                  ///< Starting Page
        0,                                  ///< Data call type (analogue/ISDN)
        0                                   ///< Max. connection speed
        }
    };



const TInt Fields_CSD_Advanced_IPv6[3][11] =
    {
        {
        EApIpv4Settings,                    ///< Ipv4 Settings
        EApIpv6Settings,                    ///< Ipv6 Settings
        EApProxyServerAddress,              ///< Proxy server address
        EApProxyPortNumber,                 ///< Proxy port number
        EApIspIfCallbackEnabled,            ///< use callback
        EApIspIfCallbackType,               ///< call back type
        EApIspIfCallbackInfo,               ///< call back number
        EApIspEnableSWCompression,          ///< Enable SW compression
        EApIspUseLoginScript,               ///< Use login script
        EApIspLoginScript,                  ///< The login script
        EApIspInitString                    ///< Modem init string
        },
        {
        R_APUI_VIEW_IPV4_SETTINGS,          ///< Ipv4 Settings
        R_APUI_VIEW_IPV6_SETTINGS,          ///< Ipv6 Settings
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER,      ///< Proxy port number
        R_APUI_VIEW_USE_CALL_BACK,          ///< use callback
        R_APUI_VIEW_CALL_BACK_TYPE,         ///< call back type
        R_APUI_VIEW_CALL_BACK_NUMBER,       ///< call back number
        R_APUI_VIEW_ENABLE_PPP_COMP,        ///< Enable PPP compression
        R_APUI_VIEW_USE_LOGIN_SCRIPT,       ///< Use login script
        R_APUI_VIEW_LOGIN_SCRIPT,           ///< The login script
        R_APUI_VIEW_MODEM_INIT_STRING       ///< Modem init string
        },
        {
        0,                                  ///< Ipv4 Settings
        0,                                  ///< Ipv6 Settings
        0,                                  ///< Proxy server address
        0,                                  ///< Proxy port number
        0,                                  ///< use callback
        0,                                  ///< call back type
        0,                                  ///< call back number
        0,                                  ///< Enable PPP compression
        0,                                  ///< Use login script
        0,                                  ///< The login script
        0                                   ///< Modem init string
        }
    };
















































const TInt Fields_WLAN_IPv6[3][8] =
    {
        {
        EApWapAccessPointName,              ///< Connection name
        EApWapCurrentBearer,                ///< Bearer type
        EApWlanNetworkName,                 ///< The network name
        EApWlanScanSSID,                    ///< need to scan the SSID?
        EApWlanNetworkMode,                 ///< Gives network mode
        EApWlanSecurityMode,                ///< The security mode
        EApWlanSecuritySettings,            ///< The security settings menu
        EApWapStartPage                     ///< Starting Page
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_WLAN_NETWORK_NAME,      ///< The network name
        R_APUI_VIEW_WLAN_HIDDEN_NETWORK,    ///< need to scan the SSID?
        R_APUI_VIEW_WLAN_NETWORK_MODE,      ///< The network mode
        R_APUI_VIEW_WLAN_SECURITY_MODE,     ///< The security mode
        R_APUI_VIEW_WLAN_SECURITY_SETTINGS, ///< The security settings menu
        R_APUI_VIEW_STARTING_PAGE           ///< Starting Page
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        1,                                  ///< The network name
        0,                                  ///< need to scan the SSID
        0,                                  ///< Gives network mode
        0,                                  ///< The security mode
        0,                                  ///< The security settings menu
        0                                   ///< Starting Page
        }
    };



const TInt Fields_WLAN_Advanced_IPv6[3][5] =
    {
        {
        EApIpv4Settings,                    ///< Ipv4 Settings
        EApIpv6Settings,                    ///< Ipv6 Settings
        EApWlanChannelId,                   ///< Ad-hoc channel auto/userdef.
        EApProxyServerAddress,              ///< Proxy server address
        EApProxyPortNumber                  ///< Proxy port number
        },
        {
        R_APUI_VIEW_IPV4_SETTINGS,          ///< Ipv4 Settings
        R_APUI_VIEW_IPV6_SETTINGS,          ///< Ipv6 Settings
        R_APUI_VIEW_AD_HOC,                  ///< Ad-hoc channel auto/userdef.
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER       ///< Proxy port number
        },
        {
        0,                                  ///< Ipv4 Settings
        0,                                  ///< Ipv6 Settings
        0,                                  ///< Ad-hoc channel auto/userdef.
        0,                                  ///< Proxy server address
        0                                   ///< Proxy port number
        }
    };




const TInt Fields_WLAN_IPv4[3][8] =
    {
        {
        EApWapAccessPointName,              ///< Connection name
        EApWapCurrentBearer,                ///< Bearer type
        EApWlanNetworkName,                 ///< The network name
        EApWlanScanSSID,                    ///< need to scan the SSID?        
        EApWlanNetworkMode,                 ///< Gives network mode
        EApWlanSecurityMode,                ///< The security mode
        EApWlanSecuritySettings,            ///< The security settings menu
        EApWapStartPage                     ///< Starting Page
        },
        {
        R_APUI_VIEW_CONNECTION_NAME,        ///< Connection name
        R_APUI_VIEW_BEARER_TYPE,            ///< Bearer type
        R_APUI_VIEW_WLAN_NETWORK_NAME,      ///< The network name
        R_APUI_VIEW_WLAN_HIDDEN_NETWORK,    ///< need to scan the SSID?        
        R_APUI_VIEW_WLAN_NETWORK_MODE,      ///< The network mode
        R_APUI_VIEW_WLAN_SECURITY_MODE,     ///< The security mode
        R_APUI_VIEW_WLAN_SECURITY_SETTINGS, ///< The security settings menu
        R_APUI_VIEW_STARTING_PAGE           ///< Starting Page
        },
        {
        1,                                  ///< Connection name
        0,                                  ///< Bearer type
        1,                                  ///< The network name
        0,                                  ///< need to scan the SSID
        0,                                  ///< Gives network mode
        0,                                  ///< The security mode
        0,                                  ///< The security settings menu
        0                                   ///< Starting Page
        }
    };


const TInt Fields_WLAN_Advanced_IPv4[3][8] =
    {
        {
        EApIspIPAddr,               ///< Phone IP addr.
        EApIspIPNetMask,            ///< Subnet mask
        EApIspIPGateway,            ///< IP address of gateway
        EApGprsIPNameServer1,       ///< Primary Name server
        EApGprsIPNameServer2,       ///< Secondary Name server
        EApWlanChannelId,           ///< Ad-hoc channel auto/userdef.
        EApProxyServerAddress,      ///< Proxy server address
        EApProxyPortNumber          ///< Proxy port number
        },
        {
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_NETMASK,                     ///< Subnet mask
        R_APUI_IP_GATEWAY,                  ///< IP address of gateway        
        R_APUI_VIEW_PRIMARY_NAME_SERVER,    ///< Primary Name server
        R_APUI_VIEW_SECONDARY_NAME_SERVER,  ///< Secondary Name server
        R_APUI_VIEW_AD_HOC,                  ///< Ad-hoc channel auto/userdef.
        R_APUI_VIEW_PROXY_SERVER_ADDR,      ///< Proxy server address
        R_APUI_VIEW_PROXY_PORT_NUMBER       ///< Proxy port number
        },
        {
        0,                                  ///< Phone IP addr.
        1,                                  ///< Subnet mask
        1,                                  ///< IP address of gateway
        0,                                  ///< Primary Name server
        0,                                  ///< Secondary Name server
        0,                                  ///< Ad-hoc channel auto/userdef.
        0,                                  ///< Proxy server address
        0                                   ///< Proxy port number
        }
    };
    
    
    
const TInt Fields_WLAN_L2_IPv4[3][4] =
    {
        {
        EApIspIPAddr,                       ///< Phone IP addr.
        EApIspIPNetMask,                    ///< Subnet mask
        EApIspIPGateway,                    ///< IP address of gateway        
        EApIP6DNSAddrFromServer             ///< Domain name servers
        },
        {
        R_APUI_VIEW_PHONE_IP_ADDR,          ///< Phone IP addr.
        R_APUI_NETMASK,                     ///< Subnet mask
        R_APUI_IP_GATEWAY,                  ///< IP address of gateway
        R_APUI_VIEW_SET_DNS_SERVERS_IP      ///< Domain name servers
        },
        {
        0,                                  ///< Phone IP addr.
        1,                                  ///< Subnet mask
        1,                                  ///< IP address of gateway
        0                                   ///< Domain name servers
        }
    };


#endif

// End of File
