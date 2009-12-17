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
* Description:  Connection Method interface class.
*
*/

#ifndef CMCONNECTIONMETHODINFO_H
#define CMCONNECTIONMETHODINFO_H

#include <cmpluginbaseeng.h>

/**
 *  Acts as a connection method that has no bearer type.
 *  Client applications can query connection method, but non-bearer specific
 *  attributes by calling RCmManager/Ext::GetConnectionMethodInfoXXXL()
 *  functions.
 *  Modification and update of a connetion method is not supported.
 *  Common records of a connection method are loaded, and kept in memory
 *  as long as the client application queries attribute of another connection
 *  method.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmConnectionMethodInfo ) : public CCmPluginBaseEng
    {
    public:
    
        static CCmConnectionMethodInfo* NewL( TCmPluginInitParam *aInitParam );

        static CCmConnectionMethodInfo* NewLC( TCmPluginInitParam *aInitParam );

        virtual ~CCmConnectionMethodInfo();

        virtual CCmPluginBaseEng* 
                CreateInstanceL( TCmPluginInitParam& /*aInitParam*/ ) const;
        virtual CCmPluginBaseEng* CreateCopyL() const;

    public: // new functions
    
        virtual TUint32 GetConnectionInfoIntL( const TUint32 aIapId, 
                                          const TUint32 aAttribute );
                                          
        virtual TBool GetConnectionInfoBoolL( const TUint32 aIapId, 
                                         const TUint32 aAttribute );
                                         
        virtual HBufC* GetConnectionInfoStringL( const TUint32 aIapId, 
                                            const TUint32 aAttribute );
        
        virtual HBufC8* GetConnectionInfoString8L( const TUint32 aIapId, 
                                             const TUint32 aAttribute );
        
        virtual void AdditionalReset(){};

        void ResetIfInMemory( CCmPluginBaseEng* aCM );

    private: // from CCmPluginBaseEng
    
        virtual TInt RunSettingsL();
        virtual TBool InitializeWithUiL( TBool aManuallyConfigure );
        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const;
        virtual TBool CanHandleIapIdL( CommsDat::CCDIAPRecord* aIapRecord ) const;
        virtual void CopyAdditionalDataL( CCmPluginBaseEng* aDestInst );

    private:
    
        virtual void ServiceRecordIdLC( HBufC* &aServiceName, 
                                       TUint32& aRecordId );
        virtual void BearerRecordIdLC( HBufC* &aBearerName, 
                                           TUint32& aRecordId );
        virtual void CreateNewServiceRecordL();
        
        void ResetAndLoadL( TUint32 aIapId );
        
    private:

        CCmConnectionMethodInfo( TCmPluginInitParam *aInitParam );

        void ConstructL();

    private:    // new functions
    
    private: // data
    };


#endif // CMCONNECTIONMETHODINFO_H
