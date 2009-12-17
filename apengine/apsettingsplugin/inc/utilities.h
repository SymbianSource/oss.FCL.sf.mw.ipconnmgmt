/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*        Header file for Utilities class.
*
*/

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

// System includes
#include <e32def.h>

/**
* Utilities class.
*/
class Utilities
    {
    public:

        /**
        * Shows an error note if it can be resolved by the error resolver or
        * a general error note if the given error code cannot be resolved.
        * @param aErrorCode Error code.
        */ 
        static void ShowErrorNote(TInt errorCode);
       
    private:
    
        /**
        * Shows an error note if it can be resolved by the error resolver.
        * @param aErrorCode Error code.
        * @return True if the error note was showned, 
        *         false if not showed.
        */
        static TBool ShowErrorNoteL(TInt aErrorCode);
        
        /**
        * Shows a general error note "System error" (qtn.err.os.general).
        */
        static void ShowGeneralErrorNoteL();
         
    private: //data

    };

#endif //__WLAN_SETTINGS_PLUGIN_H__
//End of file