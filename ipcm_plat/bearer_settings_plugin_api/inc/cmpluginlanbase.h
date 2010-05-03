/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of Base Lan interface implementation 
*                for "LAN" plugin
*
*/

#ifndef LAN_PLUGIN_BASE_H
#define LAN_PLUGIN_BASE_H

// INCLUDES
#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>
#include <commsdattypesv1_1.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif


#include <cmpluginbaseeng.h>
#include <cmpluginlanbasedef.h>

/**
 *  Lan based plugin IF implementation class
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
class CCmPluginLanBase : public CCmPluginBaseEng
    {
                    
    public: // From CCmPluginBaseEng
        
        IMPORT_C void LoadServiceSettingL();

    protected:    // Constructors
        /**
        * Second phase constructor. Leaves on failure.
        */
        IMPORT_C void ConstructL();
        
        IMPORT_C virtual ~CCmPluginLanBase();

        /**
        * Constructor. 
        */      
        IMPORT_C CCmPluginLanBase( TCmPluginInitParam* aInitParam );
        
        
        IMPORT_C virtual void ServiceRecordIdLC( HBufC* &aServiceName, 
                                                 TUint32& aRecordId );
        IMPORT_C virtual void BearerRecordIdLC( HBufC* &aBearerName, 
                                                TUint32& aRecordId );

        /**
        * Creates a new service record.
        */
        IMPORT_C virtual void CreateNewServiceRecordL();

    public: // From CCmPluginBaseEng        
        
        /**
        * From CCmPluginBaseEng
        * see base class definition
        *
        * @since S60 3.2
        */
        IMPORT_C void PrepareToUpdateRecordsL();
        
    private:
    
        IMPORT_C CommsDat::CCDLANServiceRecord& ServiceRecord()const;
        
    protected:
    
        TBuf<CommsDat::KMaxTextLength>    iBearerRecName;        
        TUint32                 iReserved[4];
    };

#endif // LAN_PLUGIN_BASE_H
