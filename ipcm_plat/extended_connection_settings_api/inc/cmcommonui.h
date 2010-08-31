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
* Description:  Contains common methods used to show various types of dialogs.
*
*/

#ifndef CMCOMMONUI_H
#define CMCOMMONUI_H

// Deprecation warning
#warning This header file is deprecated and will be removed. See Polonium CR #461 for details.

// INCLUDES
#include <e32def.h>
#include <e32cmn.h>

#include "cmdefconnvalues.h"
// FORWARD DECLARATIONS;

class CCmManagerImpl;
class CGulIcon;

// CLASS DECLARATIONS

/**
 * Helper class containing static methods for displaying various types of
 * dialogs.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TCmCommonUi )
    {
    public:
    
        /**
        * Inner helper class that keeps track of UIDs associated with
        * descriptors (one-to-one). The descriptors are used by a listbox
        * showing bearer-specific information (one bearer one line).
        * A UID, in fact, identifies a bearer (a conn method plug-in).
        *
        * Note: not all methods are overriden by this inherited class, only
        * those that we make use of here. For example, Delete() method is NOT
        * overriden, consequently calling it will result in deletion from the
        * descriptor array, but NOT from the UID array!
        */
        class CCmDesCArray : public CDesCArrayFlat
            {
            public: // public methods
                /**
                * Default constructor.
                */
                CCmDesCArray();

                /**
                * Destructor.
                */
                ~CCmDesCArray();

                /**
                * Appends an item to the array. The descriptor will be appended
                * to the descriptor array, whilst the UID to the uid array.
                * @param aUid UID of the item.
                * @param aItem Descriptor to be added to the array.
                */
                void AppendL( TUint aUid, const TDesC& aItem );

                /**
                * Returns the UID of (aIndex)th item.
                * @param aIndex The index of the item we're looking for.
                * @return UID of the item in question.
                */
                TUint Uid( TInt aIndex ) const;

            private: // data members
                RArray<TUint> iUids;
            };

    public: // enums
        enum TCmNoteType
            {
            ECmOkNote,
            ECmInfoNote,
            ECmWarningNote,
            ECmErrorNote
            };

    public: // public methods
        /**
        * Shows a confirmation query specified by a resource.
        * @param aResId Resource identifier of the text to be displayed
        * @return Result of the query (Yes-No).
        */
        IMPORT_C static TInt ShowConfirmationQueryL( TInt aResId );
        
        /**
        * Shows a confirmation query specified by a resource.
        * @param aResId Resource identifier of the text to be displayed
        * @param aString Replacement string for the first %U found in the
        * resource identified by aResId.
        * @return Result of the query (Yes-No).
        */
        IMPORT_C static TInt ShowConfirmationQueryL( TInt aResId, 
                                                     const TDesC& aString );
        
        /**
        * Shows a note specified by a resource.
        * @param aResId Resource identifier of the text to be displayed
        * @param aNoteType Specifies the type of the note to be displayed.
        * @result
        */
        IMPORT_C static TInt ShowNoteL( TInt aResId, TCmNoteType aNoteType );
        
        /**
        * Shows a note specified by a resource.
        * @param aResId Resource identifier of the text to be displayed
        * @param aString Replacement string for the first %U found in the
        * resource identified by aResId.
        * @param aNoteType Specifies the type of the note to be displayed.
        * @return
        */
        IMPORT_C static TInt ShowNoteL( TInt aResId, const TDesC& aString,
                                        TCmNoteType aNoteType );
                                        
        /**
        * Shows a note containing text passed as argument
        * @param aPrompt Prompt text to show.
        * @param aNoteType Specifies the type of the note to be displayed.
        */
        IMPORT_C static TInt ShowNoteL( const TDesC& aPrompt, TCmNoteType aNoteType );

        /**
        * Shows a confirmation query with an input text.
        * @param aResId Resource ID of the prompt to be displayed on the query.
        * @param aInput String descriptor that will store the text typed in by
        * the user.
        * @return Result of the query (OK-Cancel).
        */
        IMPORT_C static TInt ShowConfirmationQueryWithInputL( TInt aResId,
                                                              TDes& aInput );
        /**
        * Shows a memory full confirmation query. 
        * @return Result of the query. It is always OK. 
        */
        static TInt ShowMemoryFullConfirmationQueryL();


        /**
        * Shows a list with graphic containing the available bearer
        * types (e.g. WLAN, packet data). The input arguments tells the
        * method which bearer type is available. The user is prompted
        * this dialog along with a Select-Cancel softkey pair.
        * @param aAvailableBearers List holding the UIDs of currently available
        * bearers.
        * @param aCmManager Reference to CM Manager implementation used for
        * querying bearer specific data.
        * @return This method returns the UID of selected bearer type or zero
        * if nothing was selected (i.e. user pressed Cancel).
        */
        static TUint32 ShowAvailableMethodsL( 
                                      const RArray<TUint32>& aAvailableBearers,
                                      const CCmManagerImpl& aCmManager );
        /**
        * Shows a list of supported bearers.
        * @param aCmManager Reference to CM Manager implementation used for
        * querying bearer specific data.
        * @return This method returns the UID of selected bearer type or zero
        * if nothing was selected (i.e. user pressed Cancel).
        */
        static TUint32 SelectBearerL( const CCmManagerImpl& aCmManager );
        
        /**
        * Creates and formats a string based on the input arguments. The string
        * can be used by a single graphic style listbox so that the icon
        * index and the first line are always well-formatted.
        * Ownership of the returned string is transferred to the caller.
        * @param aIconIndex Icon index to be appended to the string. This
        * method has nothing to do with any icon arrays, this value is just
        * used for formatting.
        * @param aLineText Line text to be appended to the formatted string.
        * @return Well-formatted string + ownership.
        */
        static HBufC* FormattedTextForListBoxLC( TInt aIconIndex,
                                                 const TDesC& aLineText );
                                                 
        /**
        * Displays a generic pop-up list (CAknPopupList)
        * 
        * @since S60 3.2
        * @param aTitle the title of the popup
        * @param aTextItems the textual items in the popup - ownership is taken over
        * @param aIconArray the icons to show in the list, if NULL icons are not shown
        * @param aSoftKeys the soft key resource ids
        * @param aSelection the result
        * @return ETrue if a selection was made EFalse if cancelled
        */
        IMPORT_C static TBool ShowPopupListL( const TDesC& aTitle,
                                              CDesCArrayFlat* aTextItems,
                                              CArrayPtr<CGulIcon>* aIconArray,
                                              const TUint aSoftKeys,
                                              TInt& aSelection );

        /**
        * Displays a note 'Default destination is set to Always Ask'
        * for use in Destination and CM views when deleting the cm/destination
        * representing the default connection 
        *
        * @since S60 3.2
        * @param aOldDefConn former default connection
        * @param aCmManagerImpl - engine to ask the new default connection from
        */
        TBool static ShowAlwaysAskNoteL( TCmDefConnValue aOldDefConn,
                                        CCmManagerImpl& aCmManagerImpl );

    private: // helper methods
        /**
        * Shows a confirmation query specified by a resource.
        * This method takes over the ownership of the pointer passed and
        * deletes the object at the end of the call.
        * @param aPrompt Prompt text to show.
        * @return Result of the query (Yes-No).
        */
        static TInt ShowConfirmationQueryL( HBufC* aPrompt );
        
        /**
        * Creates and formats a string based on the input arguments. The string
        * can be used by a double graphic style listbox so that the icon
        * index and the two lines are always well-formatted.
        * Ownership of the returned string is transferred to the caller.
        * @param aIconIndex Icon index to be appended to the string. This
        * method has nothing to do with any icon arrays, this value is just
        * used for formatting.
        * @param aFirstLine First line to be appended to the formatted string.
        * @param aSecondLine Second line to be appended to the formatted string.
        * @return Well-formatted string + ownership.
        */
        static HBufC* FormattedTextForListBoxLC( TInt aIconIndex,
                                                 const TDesC& aFirstLine,
                                                 const TDesC& aSecondLine );

        /**
        * Creates and formats a string based on the input arguments. The string
        * can be used by a double large graphic style listbox so that the icon
        * index and the first/second lines are always well-formatted.
        * Ownership of the returned string is transferred to the caller.
        * @param aIconIndex Icon index to be appended to the string. This
        * method has nothing to do with any icon arrays, this value is just
        * used for formatting.
        * @param aFirstLineResId Resource ID of the first that this method will
        * resolve.
        * @param aSecondLineResId Resource ID of the first that this method
        * will resolve.
        * @return Well-formatted string + ownership.
        */
        static HBufC* FormattedTextForListBoxLC( TInt aIconIndex,
                                                TInt aFirstLineResId,
                                                TInt aSecondLineResId );

        /**
        * Creates and formats a string based on the input arguments. The string
        * can be used by a double large graphic style listbox so that the icon
        * index and the first/second lines are always well-formatted.
        * Ownership of the returned string is transferred to the caller.
        * @param aIconIndex Icon index to be appended to the string. This
        * method has nothing to do with any icon arrays, this value is just
        * used for formatting.
        * @param aFirstLineResId Resource ID of the first that this method will
        * resolve.
        * @param aSecondLineResId Resource ID of the first that this method
        * will resolve.
        * @param aReplacementInSecondLine An integer to be inserted in the
        * second line.
        * @return Well-formatted string + ownership.
        */
        static HBufC* FormattedTextForListBoxLC( TInt aIconIndex,
                                               TInt aFirstLineResId,
                                               TInt aSecondLineResId,
                                               TInt aReplacementInSecondLine );

        /**
        * Creates and formats a string based on the input arguments. The string
        * can be used by a double large graphic style listbox so that the icon
        * index and the first/second lines are always well-formatted.
        * Ownership of the returned string is transferred to the caller.
        * @param aIconIndex Icon index to be appended to the string. This
        * method has nothing to do with any icon arrays, this value is just
        * used for formatting.
        * @param aFirstLineResId Resource ID of the first that this method will
        * resolve.
        * @param aSecondLine Second line, a string.
        * @return Well-formatted string + ownership.
        */
        static HBufC* FormattedTextForListBoxL( TInt aIconIndex,
                                                TInt aFirstLineResId,
                                                const TDesC& aSecondLine );
    };

#endif // CMCOMMONUI_H

