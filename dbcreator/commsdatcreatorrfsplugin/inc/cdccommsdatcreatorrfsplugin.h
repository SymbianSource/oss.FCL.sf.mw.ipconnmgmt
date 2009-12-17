/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declaration of CCdcCommsDatCreatorRfsPlugin class.
*
*/



#ifndef C_CCDCCOMMSDATCREATORRFSPLUGIN_H
#define C_CCDCCOMMSDATCREATORRFSPLUGIN_H

//  INCLUDES
#include <rfsPlugin.h>

// CONSTANTS
const TInt KUidCommsDatCreatorRfsPlugin = 0x10281BC6; 

// FORWARD DECLARATIONS
class CRepository;


// CLASS DECLARATION

/**
*	Concrete class for CRFSPlugin
*	It supplies instantiation & destruction by using
*	the ECom framework, and functional services
*	by using the methods of the actual class.
*/
class CCdcCommsDatCreatorRfsPlugin : public CRFSPlugin
    {
public:

    /**
    * Two-phased constructor.
    * aImplementationUid - Implementation Uid
    */
    static CCdcCommsDatCreatorRfsPlugin * NewL( const TUid& aImplementationUid );

    /** 
	* From CRFSPlugin
	* aType Type
    * @return
	*/
    virtual void RestoreFactorySettingsL( const TRfsReason aType );

    /** 
	* From CRFSPlugin
	* aType - Type
	* aPath - Path
    * @return
	*/
    virtual void GetScriptL( const TRfsReason aType, TDes& aPath );

    /** 
	* From CRFSPlugin
	* aType - Type
	* aCommand - Command
    * @return
	*/
	virtual void ExecuteCustomCommandL( const TRfsReason aType, 
                                        TDesC& aCommand );
                                        
    /**
    * C++ destructor
    */
    virtual ~CCdcCommsDatCreatorRfsPlugin();

                                        
protected:

    /**
    * C++ default Constructor
    */
    CCdcCommsDatCreatorRfsPlugin ();
 
private:

	
    /**
    * Starts the rfs exe.
    */
    void StartProcessL();

private:

    };


#endif // C_CCDCCOMMSDATCREATORRFSPLUGIN_H
