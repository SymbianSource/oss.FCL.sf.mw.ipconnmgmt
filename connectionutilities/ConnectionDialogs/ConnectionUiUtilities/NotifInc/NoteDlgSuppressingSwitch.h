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
* Description:  Declaration of class CNoteDlgSuppressingSwitch.
*
*/


#ifndef __NOTEDLGSUPPRESSINGSWITCH_H__
#define __NOTEDLGSUPPRESSINGSWITCH_H__

// INCLUDES
#include <e32base.h>
#include <aknnotewrappers.h> 



// CLASS DECLARATIONS

/**
 * Class implementing a note dialog suppressing application switch
 */
NONSHARABLE_CLASS( CNoteDlgSuppressingSwitch ) : public CAknNoteDialog
    {
    public:
        /**
        * Constructor of the CNoteDlgSuppressingSwitch class
        * @return -
        */
        CNoteDlgSuppressingSwitch( CEikDialog** aSelfPtr );


        /**
        * Destructor
        */
        virtual ~CNoteDlgSuppressingSwitch();


    private:
        /**
        * PreLayoutDynInitL
        * @param    -
        */
        virtual void PreLayoutDynInitL();
    };


#endif  // __NOTEDLGSUPPRESSINGSWITCH_H__

// End of File
