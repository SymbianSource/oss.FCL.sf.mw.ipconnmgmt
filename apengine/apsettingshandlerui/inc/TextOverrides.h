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
*     Declares the CTextOverrides class.
*
*/


#ifndef TEXT_OVERRIDES_H
#define TEXT_OVERRIDES_H


// INCLUDES
#include <e32base.h>
#include <commdb.h>

#include <ApSettingsHandlerCommons.h>

// CONST
const TInt  KTxtOverrideGran   = KTTextIdMax+1;


// CLASS DECLARATION
/**
* CTextOverrides class: It is a helper class for the Access Point settings.
*
*/
NONSHARABLE_CLASS( CTextOverrides ) : public CBase
    {

    public: // Constructors and destructor


        /**
        * Two-phased constructor. Leaves on failure.
        * @return The constructed CTextOverrides.
        */
        static CTextOverrides* NewL();


        /**
        * Destructor.
        */
        ~CTextOverrides();


    protected:
        /**
        * Constructor.
        */
        CTextOverrides();


        /**
        * Second-phase constructor.
        */
        void ConstructL();



    public: // New functions
        /**
        * Sets possible overrides of some texts.
        * By default, text is read from resources. This provides a way to 
        * change some of the text to a caller-supplied one. The text must 
        * be read from resource and must be already localized.
        * More than one text can be substituted, they are stored internally 
        * in an array.
        * @param aText2Change TTextID to identify the text to be replaced with
        * @param aNewText Reference to the substitute text.
        */
        void SetTextOverrideL( TTextID aText2Change, const TDesC& aNewText );


        /**
        * Gets overrides of the given text.
        * By default, text is read from resources. This provides a way to 
        * change some of the text to a caller-supplied one. The text must 
        * be read from resource and must be already localized.
        * More than one text can be substituted, they are stored internally 
        * in an array.
        * @param aTextId TTextID type to identify the text to get
        * @return Reference to the overriding text.
        */
        const TDesC& TextOverrideL( TTextID aTextId );

        /**
        * Checks whether a given text is overriden or not.
        * By default, text is read from resources. This provides a way to 
        * change some of the text to a caller-supplied one. The text must 
        * be read from resource and must be already localized.
        * More than one text can be substituted, they are stored internally
        * in an array.
        * @param aTextId TTextID type to identify the text to check
        * @return Boolean indicatiing whether the given text is 
        * overriden or not.
        */
        TBool IsTextOverridenL( TTextID aTextId );

        /*
        * Validates the passed ID. If invalid, leaves with KErrInvalidTextId
        * @param aTextId The text ID to validate
        */
        void ValidateIdL( TTextID aTextId );

    private: //data
        CArrayPtrFlat<HBufC>*   iTxtArray;

    };

#endif

// End of File
