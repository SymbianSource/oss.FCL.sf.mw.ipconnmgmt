/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Represents a connection
*     
*
*/


#ifndef CONNECTIONINFOBASE_H_INCLUDED
#define CONNECTIONINFOBASE_H_INCLUDED

// INCLUDES
#include    <rconnmon.h>
#include    <stdlib.h>
#include    <badesca.h>
#include    <e32std.h>
#include    <stdlib.h>

// CONSTANTS
LOCAL_D const TUint KConnectionListItemText = 64;
LOCAL_D const TUint KDetailsListItemText = 64;

_LIT( KConnectonListItemTextFormat, "%o\t%S\t%S" );
_LIT( KDetailsListItemTextFormat, "%S\t%S" );  

// FORWARD DECLARATIONS

class CActiveWrapper;

// CLASS DECLARATION

enum TConnectionStatus
    {
    EConnectionUninitialized,
    EConnectionCreating,
    EConnectionCreated,
    EConnectionSuspended,
    EConnectionClosing,
    EConnectionClosed
    };

/**
* Order of icons in icon array of containers
*/
enum TIconsOrder
    {
    EGprs = 0,
    ECsd,
    EHscsd,
    EEmpty,
    ESuspended,
    EWlan
    };


/**
* Represents a connection.
*/
class CConnectionInfoBase : public CBase
    {
    public: // Constructors, Desturctor
                
        /**
        * Destructor
        */
        virtual ~CConnectionInfoBase();

    protected:
        /**
        * Constructor
        * @param aConnectionId The id of the given connection.
        * @param aConnectionMonitor for refreshing details
        * @param aConnectionBearerType the bearer type of connection
        * @param aActiveWrapper the for refreshing
        */
        CConnectionInfoBase( TInt aConnectionId,
                             RConnectionMonitor* const aConnectionMonitor,
                             TConnMonBearerType aConnectionBearerType,
                             CActiveWrapper* aActiveWrapper );

        /**
        * Second phase construction
        */
        void ConstructL();

    public: // New functions
    
        /**
        * Give back a copy of connection. 
        */
        virtual CConnectionInfoBase* DeepCopyL() = 0;
    

        /**
        * Refresh required details of connection. 
        */
        virtual void RefreshDetailsL() = 0;

        /**
        * Refresh iListBoxItemText text, connection ListBox has a reference to
        * this text.
        */
        virtual void RefreshConnectionListBoxItemTextL() = 0;

        /**
        * Refresh details array of connection. DetailsListBox has a reference
        * to this array.
        */
        virtual void RefreshDetailsArrayL() = 0;

        /**
        * Called, if the status of connection is changed.
        */ 
        void StatusChangedL();

        /**
        * Gives back the connection is alive or not.
        * @return ETrue if connection is created or suspended, 
        * EFalse otherwise
        */
        TBool IsAlive() const;

        /**
        * Gives back the connection is suspended or not.
        * @return ETrue if connection is suspended, 
        * EFalse otherwise
        */
        TBool IsSuspended() const;
                
        /**
        * @return ETrue if deletion of connection started form CMUI,
        * EFalse otherwise
        */
        TBool GetDeletedFromCMUI() const;
        
        /**
        * Sets status of connection to EConnectionClosing
        */
        void SetAllDeletedFromCMUI();        
        
        /**
        * Sets iDeletedFromCMUI to ETrue, means dletion of connection
        * was established from CMUI
        */
        void SetDeletedFromCMUI();        
        
        /**
        * Gives back a pointer to the DetailsArray of connection.
        * @return CDesCArraySeg* iDetailsArray of connection
        */
        CDesCArrayFlat* GetDetailsArray() const;

        /**
        * Gives back the id of connection
        * @return the id of connection.
        */
        TUint GetConnectionId() const;

        /**
        * Gives back the icon id of connection 
        * @return the icon id of connection 
        */
        virtual TUint GetIconId() const = 0;

        /**
        * Gives back the connection is EasyWLAN or not.
        * @return ETrue if the selected conenction EasyWLAN,
        * EFalse otherwise
        */
        virtual TBool IsEasyWLAN() const { return EFalse; };
        
        /**
        * Gives back current Name of Network ( SSID )
        * @return iNetworkName
        */
        virtual HBufC* GetWlanNetworkNameLC() const { return NULL; };

        /**
        * Gives back the status of connection 
        * @return the status of connection 
        */
        TConnectionStatus GetStatus() const;

        /**
        * Refresh status of connection
        * @param aConnectionStatus new status of connection
        */
        void RefreshConnectionStatus( 
                                   const TInt aConnectionStatus );

        /**
        * Gives back the name of connection 
        * @return the name of connection 
        */
        HBufC* GetNameLC() const;        
        
        /**
        * Gives back the bearertype of connection 
        * @return the bearertype of connection 
        */
        TConnMonBearerType GetBearerType() const;
        
        /**
        * Gives back a pointer the iListBoxItemText of connection 
        * @return the iListBoxItemText of connection 
        */
        TPtrC GetConnectionListBoxItemText() const;

        /**
        * Check if connection name is starts with "mRouter" string or not.
        * ( if this connection is not IR/BT )
        * @retrun ETrue if starts wiht "mRouter", EFalse otherwise.
        */
        TBool CheckMrouterIap();
    
    protected:
        /**
        * Creates a string for DetailsListBoxItem, title came from resource 
        * and value from aValueText
        * @param aResourceId a resource id
        * @param aValueText the value of text
        * @retrun  a string consist of resource and value text
        */
        HBufC*  ToDetailsListBoxItemTextL( const TUint aResourceId, 
                                           const HBufC* aValueText ) const;

        /**
        * Creates a string for DetailsListBoxItem, title came from resource 
        * and value from aValueText, and Appends to iDetailsArray
        * @param aResourceId a resource id
        * @param aValueText the value of text
        */
        void  ToDetailsListBoxItemTextWithAppendL( const TUint aResourceId, 
                                                   const HBufC* aValueText );

        /**
        * Creates a string for DetailsListBoxItem, title came from resource 
        * and value from aValueText, and Replaces the given element with it
        * @param aResourceId a resource id
        * @param aValueText the value of text
        */
        void  ToDetailsListBoxItemTextWithReplaceL( const TUint aResourceId, 
                                                    const HBufC* aValueText,
                                                    TInt aIndex );


        /**
        * Creates an array with requrired details of connection to
        * the listbox which visulaize details of connection. This member
        * is called at initialization.
        */
        virtual void ToArrayDetailsL() = 0;

        /**
        * Refresh status of connection
        */
        void RefreshConnectionStatus();

        /**
        * Refresh amount of sent and received data of connection
        */
        void RefreshSentReceivedDataL();

        /**
        * Refresh the value of transfer speeds (Up/Down) of connection
        */
        void RefreshTransferSpeedsL();
                        
        /**
        * Refresh the duration of connection.
        */
        void RefreshDuration();
        
        /**
        * Refresh the applications names which uses the connection
        * @return the count of applications use connection 
        *         (from the user point of view)
        */
        TInt RefreshAppNamesL();
        
 
     public: // New functions        

        /**
        * Creates a string from bearer of connection
        * @return string representation of bearer of connection.
        */
        HBufC* ToStringBearerLC() const;

        /**
        * Creates a string from status of connection
        * @param aIsDetailsView is ETrue then get localised resources 
        * to details view otherwise to connections view
        * @return string representation of status of connection.
        */
        HBufC* ToStringStatusLC( TBool aIsDetailsView = ETrue ) const;

        /**
        * Creates a string from amount of received data 
        * and sent data combined/summarized.
        * @return string representation  of summarized of 
        * Received and Sent data.
        */
        HBufC* ToStringTotalTransmittedDataLC() const;

        /**
        * Creates a string from amount of Sent data
        * @param aToBeThreeDigit the shown digits numbers limited to three
        * @return string representation of Sent data.
        */
        HBufC* ToStringSentDataLC( TBool aToBeThreeDigit = EFalse ) const;

        /**
        * Creates a string from amount of Received data
        * @param aToBeThreeDigit the shown digits numbers limited to three
        * @return string representation of Received data.
        */
        HBufC* ToStringReceivedDataLC( TBool aToBeThreeDigit = EFalse ) const;

        /**
        * Creates a string from transfer speed Up of connection
        * @return string representation of transfer speed Up of 
        * connection.
        */
        HBufC* ToStringTransferSpeedUpLC() const;

        /**
        * Creates a string from transfer speed Down of connection
        * @return string representation of transfer speed Down of 
        * connection.
        */
        HBufC* ToStringTransferSpeedDownLC() const;

        /**
        * Creates a string from duraion of the connection.
        * @return string representation duraion of the connection.
        */
        HBufC* ToStringDurationLC() const;

        /**
        * Remove directional markers from a string.
        * @param aStringHolder the string which markers has to remove.
        */
        void StringMarkerRemoval( HBufC* aStringHolder );

        /**
        * Remove directional markers from a string.
        * @param aAppIndex the index of Applicaions names array.
        * @return string representation of used app. name 
        */
        HBufC* ToStringAppNameLC( TInt aAppIndex );


    private:
        /**
        * Creates a string from aLoadedData, extended with measure
        * @param aLoadedData the data is converted to string
        * @param aToBeThreeDigit the shown digits numbers limited to three
        * @param aArrowChar the character representation of arrow (Up/Down)
        * @return string representation of loaded data 
        */
        HBufC* ToStringLoadedDataLC( TUint aLoadedData, 
                                     TBool aToBeThreeDigit = EFalse,
                                     const TUint aArrowChar = 0 ) const;


        /**
        * Creates a string from transfer speed of connection
        * @param aSpeed the speed is converted to string
        * @param aArrowChar the character representation of arrow (Up/Down)
        * @return string representation of transfer speed of 
        * connection, extended with measure 
        */
        HBufC* ToStringTransferSpeedLC( div_t aSpeed, 
                                        const TUint aArrowChar ) const;

        /**
        * Called if iConnectionStatus is changed to created. Initializes
        * changable details of connection.
        */
        void InitializeConnectionInfoL();

        /**
         * Compare the provided UID array with the current cached UID array
         * iClientBuf.
         * @param aClients New array of UIDs.
         * @return ETrue if the contents are the same, EFalse otherwise.
         */
        TBool IsTheSameUids( TConnMonClientEnumBuf& aClients );

        /**
         * Copy the provided UIDs into the cached UID array.
         * @param aClients New array of UIDs.
         */
        void CopyUidsToBuf( TConnMonClientEnumBuf& aClients );

    protected:
        
        /**
        * Used for refreshing details of connection.
        */
        RConnectionMonitor* const           iConnectionMonitor;   

        /**
        * The id of connection, given by RConnectionMonitor.
        */
        TUint                               iConnectionId;

        /**
        * The name of connection
        */
        TName                               iConnectionName;    // IAPName
        
        /**
        * BearerType of connection
        */
        TConnMonBearerType                  iConnectionBearerType;
        
        /**
        * The status of connection
        */
        TConnectionStatus                   iConnectionStatus;

        /**
        * The amount of Uploaded data
        */
        TUint                               iUploaded;

        /**
        * The amount of Downloaded data
        */
        TUint                               iDownloaded;

        /**
        * The start time of connection
        */
        TTime                               iStartTime;

        /**
        * The duration of connection
        */
        TTime                               iDuration;

        /**
        * Used for calculating speed data
        */
        TTime                               iLastSpeedUpdate;
 
        /**
        * The speed of transferring Uploaded data
        */
        div_t                               iUpSpeed;

        /**
        * The speed of transferring Downloaded data
        */
        div_t                               iDownSpeed;

        /**
        * The text of list item.
        */
        TBuf<KConnectionListItemText>       iListBoxItemText; 

        /**
        * Descriptor array, contains details of connection. Owned.
        */
        CDesCArrayFlat*                     iDetailsArray;
        
        /**
        * ETrue if deletion of connection started form CMUI, EFalse otherwise
        */
        TBool								iDeletedFromCMUI;
        
        /**
        * The names of applications use connection.Owned.
        */
        CDesCArrayFlat*                     iAppNames;
        
        /**
        * The active object for info. refreshing. NOT Owned.
        */
        CActiveWrapper*                     iActiveWrapper;

        /**
         * Buffer to cache a connection's client application UIDs.
         * These are kept in cache so application names are resolved only when
         * the client list has changed.
         */
        TConnMonClientEnumBuf               iClientBuf;
    };


#endif //CONNECTIONINFOBASE_H_INCLUDED

// End of File