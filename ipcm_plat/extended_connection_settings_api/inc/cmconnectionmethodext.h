/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Connection method extended interface class.
*
*/

#ifndef CMCONNECTIONMETHODEXT_H
#define CMCONNECTIONMETHODEXT_H

// Deprecation warning
#warning This header file is deprecated and shall not be used anymore. Please use cmconnectionmethod.h instead. See Polonium CR #461 for details.

#include <e32base.h>
#include <cmconnectionmethoddef.h>

// Forward declarations
class RCmDestinationExt;
class CCmConnectionMethodWrapper;


/**
 *  RCmConnectionMethodExt is for getting/setting values of a connection method.
 *  Use RCmManagerExt or RCmDestinationExt classes to query a connection method.
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( RCmConnectionMethodExt )
    {
    public:
        /**
        * Default constructor.
        */
        IMPORT_C RCmConnectionMethodExt();

        /**
        * Copy constructor.
        */
        IMPORT_C RCmConnectionMethodExt(
                const RCmConnectionMethodExt& aConnMethod );

        /**
        * Destructor.
        */
        IMPORT_C ~RCmConnectionMethodExt();

    public:
        /**
        * Close the session.
        */
        IMPORT_C void Close();

        /**
        * Creates a copy of this connection method. UpdateL() has to be called
        * to store new connection method in database. Copy is put to
        * the Uncategorized destination. The ownership is passed.
        */
        IMPORT_C RCmConnectionMethodExt CreateCopyL();

        /**
        * Gets the value for a TInt attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return contains the requested TInt attribute.
        */
        IMPORT_C TUint32 GetIntAttributeL( TUint32 aAttribute ) const;

        /**
        * Gets the value for a TBool attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return contains the requested TBool attribute.
        */
        IMPORT_C TBool GetBoolAttributeL( TUint32 aAttribute ) const;

        /**
        * Gets the value for a String16 attribute.
        * HBuf ownership is passed to the caller
        * @param aAttribute Identifies the attribute to be retrived.
        * @return copy of the requested attribute. Ownership is passed.
        */
        IMPORT_C HBufC* GetStringAttributeL( TUint32 aAttribute ) const;

        /**
        * Gets the value for a String8 attribute.
        * HBuf ownership is passed to the caller
        * @param aAttribute Identifies the attribute to be retrived.
        * @return copy of the requested attribute. Ownership is passed.
        */
        IMPORT_C HBufC8* GetString8AttributeL( const TUint32 aAttribute ) const;

        /**
        * Sets the value for a TInt attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C void SetIntAttributeL( TUint32 aAttribute, TUint32 aValue );

        /**
        * Sets the value for a TBool attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C void SetBoolAttributeL( TUint32 aAttribute, TBool aValue );

        /**
        * Sets the value for a String16 attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C void SetStringAttributeL(
                TUint32 aAttribute,
                const TDesC16& aValue );

        /**
        * Sets the value for a String8 attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C void SetString8AttributeL(
                TUint32 aAttribute,
                const TDesC8& aValue );

        /**
        * Call this function only if this CM is an embedded destination!
        * @return embedded destination
        */
        IMPORT_C RCmDestinationExt DestinationL() const;

        /**
        * Try to delete the connection method. If it is referenced from
        * any destination, then the references are removed and the
        * connection method is deleted.
        * Do NOT call this function if you got this
        * connection method from a destination. Call destination's
        * DeleteConnectionMethodL() function instead.
        * @return ETrue if connection method deleted.
        */
        IMPORT_C TBool DeleteL();

        /**
        * Update in the database.
        * @return None.
        */
        IMPORT_C void UpdateL();

        /**
        * checks if connection methods are the same
        *
        * @since S60 3.2
        * @param aConnMethod the connection method being compared
        * @return ETrue if the destinations are the same
        */
        IMPORT_C TBool operator==( RCmConnectionMethodExt& aConnMethod ) const;

        /**
        * checks if connection methods are not the same
        *
        * @since S60 3.2
        * @param aConnMethod the connection method being compared
        * @return ETrue if the destinations are different
        */
        IMPORT_C TBool operator!=( RCmConnectionMethodExt& aConnMethod ) const;


        /**
        * assignment operator
        *
        * @since S60 3.2
        * @return RCmConnectionMethod
        */
        IMPORT_C RCmConnectionMethodExt& operator=(
                const RCmConnectionMethodExt& aConnMethod );

    private:
        friend class RCmManagerExt;
        friend class RCmDestinationExt;

    private:
        CCmConnectionMethodWrapper* iCmConnectionMethodWrapper;
    };

#endif // CMCONNECTIONMETHODEXT_H

// End of file
