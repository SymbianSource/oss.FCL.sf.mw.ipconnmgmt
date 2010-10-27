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
* Description:  Declaration of class CApMultiLineDataQueryDialog.
*
*/


#ifndef AP_MULTILINE_IP_QUERY_DIALOG_H
#define AP_MULTILINE_IP_QUERY_DIALOG_H

// INCLUDE FILES
#include <AknQueryDialog.h>


// FORWARD DECLARATION


// CLASS DECLARATION

/**
* A new class to extend the functionality of the existing 
* CAKnMultiLineIpQueryDialog to be able to accept 0 length settings...
* Nothing else is changed.
*/
NONSHARABLE_CLASS( CApMultiLineDataQueryDialog ) : 
                    public CAknMultiLineDataQueryDialog
    {
public:
    public:
        /**
        * Multiline data query dialog, 2 lines
        * @param aText1 Text1
        * @param aText2 Text2
        * @param aTone Tone 
        * @return A constructed CApMultiLineDataQueryDialog
        */
        static CApMultiLineDataQueryDialog* NewL(
                            TDes&  aText1, TDes&  aText2, 
                            TTone aTone = ENoTone);


    protected:
        /**
        * If the query text is ok (retrieved form query controls) then 
        * display the left soft key, otherwise hide it.
        */
        void UpdateLeftSoftKeyL();

    private:
        /**
        * Constructor
        * @param aTone Tone
        */
        CApMultiLineDataQueryDialog( const TTone& aTone );

    };


#endif

// End of file
