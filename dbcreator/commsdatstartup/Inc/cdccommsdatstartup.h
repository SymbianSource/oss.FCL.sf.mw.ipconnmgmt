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
* Description:   Declaration of CCommsDatStartup class.
*
*/




#ifndef CDC_COMMSDAT_STARTUP_H
#define CDC_COMMSDAT_STARTUP_H

// INCLUDE FILES
#include <e32base.h>
#include <bacline.h>
#include <f32file.h>

class CRepository;


// CONSTANTS


// DATA TYPES


// FORWARD DECLARATIONS


// CLASS DECLARATION


/**
* CCdcCommsDatStartup checks the conditions if commsdat creation is enabled.
* If yes starts commsdat creation and indicates the result after launch.
*/
class CCdcCommsDatStartup : public CBase
    {

    public:
    
    /**
    * CCdcCommsDatStartup checks the conditions if commsdat creation is enabled.
    * If yes starts commsdat creation and indicates the result after launch.
    */
    void DoStartL();
        
    ~CCdcCommsDatStartup();
     
          
    private:
    
    /*
    * Checks the creator UID. It leaves if it is not CdcCommsDatRfsPlugin.
    * It is done by security reasons.
    */
    void CheckCreatorUIdL();

    /*
    * Reads the database descriptor input file name
    * @param out: Name of the database descriptor file 
    */
    void GetInputFileNameL( TFileName &aFileName );

    /*
    * Creates repository
    */
    void CreateRepositoryL();

   	/**
    * Resets the commsdat repository. This causes the default Commsdat
    * from z to be copied to c
    */
    void ResetCommsDatToDefaultL();
    
    /**
    * Deletes the EAP DBMS database files from c:\ thus reseting the settings    
    */
    void ResetEAPSettingsL();
    
    /*
    * Sets the startup status
    * @param startup status to be set.
    */
    void SetStartupStatusL( TInt aStatus );
    
    // Repository
    CRepository *iRepository;

    };


#endif // CDC_COMMSDAT_STARTUP_H
            
