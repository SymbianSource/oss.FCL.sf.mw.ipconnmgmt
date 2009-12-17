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
* Description:  Declaration of the CApSelect class.
*
*/


// INCLUDE FILES
#include <cdbcols.h>
#include <commdb.h>
#include <eikdef.h>
#include <d32dbms.h>
#include <featmgr.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ProtectDb.h>
#else
#include <comms-infras/commdb/protection/protectdb.h>
#endif

#include <etelpckt.h>
#ifdef __TEST_USE_SHARED_DATA
    #include <sharedDataValues.h>
#endif // __TEST_USE_SHARED_DATA

#include "ApListItem.h"
#include "ApListItemList.h"
#include "ApSelect.h"
#include "ApUtils.h"
#include "ApEngineCommons.h"
#include <ApEngineConsts.h>
#include "ApEngineLogger.h"
#include "ApListItemKey.h"




// CONSTANTS
const TInt KCursorNullPos = 0;
const TInt KItemCountNull = 0;

#ifdef __TEST_USE_SHARED_DATA
    // General Settings UID, Hardcoded
    const TUid KGeneralSettingsUid={0X100058EC};
#endif // __TEST_USE_SHARED_DATA


const TInt KDIALOUT     = 0;
const TInt KGPRSIN      = 1;
const TInt KGPRSOUT     = 2;
const TInt KCDMA_ISP    = 3;
const TInt KVPN         = 4;


const TInt  KUID        = 0;
const TInt  KISPTYPE    = 1;
const TInt  KBEARERTYPE = 2;



// LOCAL CONSTANTS AND MACROS


// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
// ---------------------------------------------------------
// CApSelect::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSelect* CApSelect::NewLC( CCommsDatabase& aDb,
                                     TInt aIspFilter,
                                     TInt aBearerFilter,
                                     TInt aSortType )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::NewLC" ) ) );

    CApSelect* db = CApSelect::NewLC( aDb, aIspFilter, 
                                      aBearerFilter, aSortType, EIPv4 );

    CLOG( ( ESelect, 1, _L( "<- CApSelect::NewLC" ) ) );
    return db;
    }


// Destructor
// ---------------------------------------------------------
// CApSelect::~CApSelect
// ---------------------------------------------------------
//
EXPORT_C CApSelect::~CApSelect()
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::~CApSelect" ) ) );

    if ( iExt )
        {
        if ( iExt->iIsFeatureManagerInitialised )
            {
            FeatureManager::UnInitializeLib();
            }
        }

    if ( iApList )
        {
        iApList->ResetAndDestroy();
        delete iApList;
        }
    
    delete iExt;

    CLOG( ( ESelect, 1, _L( "<- CApSelect::~CApSelect" ) ) );
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApSelect::CApSelect
// ---------------------------------------------------------
//
EXPORT_C CApSelect::CApSelect()
:iCursor( KCursorNullPos ),
iCount( KItemCountNull )
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApSelect::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::ConstructL( CCommsDatabase& aDb,
                                 TInt aIspFilter,
                                 TInt aBearerFilter,
                                 TInt aSortType )

    {
    ConstructL( aDb, aIspFilter, aBearerFilter, 
                aSortType, EIPv4, EVpnFilterBoth );
    }



// ---------------------------------------------------------
// CApSelect::SetFilters
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::SetFiltersL( TInt aIspFilter,
                                    TInt aBearerFilter,
                                    TInt aSortType )
    {
    SetFiltersL( aIspFilter, aBearerFilter, aSortType, 
                 iExt->iReqIpvType, iExt->iVpnFilterType );
                 
    }


// ---------------------------------------------------------
// CApSelect::IspTypeFilter
// ---------------------------------------------------------
//
EXPORT_C TInt CApSelect::IspTypeFilter()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::IspTypeFilter" ) ) );

    return iIsp;
    }


// ---------------------------------------------------------
// CApSelect::BearerFilter
// ---------------------------------------------------------
//
EXPORT_C TInt CApSelect::BearerFilter()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::BearerFilter" ) ) );

    return iBearer;
    }


// ---------------------------------------------------------
// CApSelect::SortOrder
// ---------------------------------------------------------
//
EXPORT_C TInt CApSelect::SortOrder()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::SortOrder" ) ) );

    return iSort;
    }



// ---------------------------------------------------------
// CApSelect::MoveToFirst
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::MoveToFirst()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::MoveToFirst" ) ) );

    iCursor = 0;
    return ( iExt->iMaxIndex >= 0 );
    }


// ---------------------------------------------------------
// CApSelect::MoveNext
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::MoveNext()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::MoveNext" ) ) );

    if ( iCursor < iExt->iMaxIndex )
        {
        iCursor++;
        return ETrue;
        }
    return EFalse;
    }


// ---------------------------------------------------------
// CApSelect::MoveLast
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::MoveLast()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::MoveLast" ) ) );

    iCursor = iExt->iMaxIndex;
    return ( iCursor >= 0 );
    }


// ---------------------------------------------------------
// CApSelect::MovePrev
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::MovePrev()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::MovePrev" ) ) );

    if ( iCursor>0 )
        {
        iCursor--;
        return ETrue;
        }
    return EFalse;
    }


// ---------------------------------------------------------
// CApSelect::Count
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApSelect::Count()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::Count" ) ) );

    return iApList->Count( );
    }


// ---------------------------------------------------------
// CApSelect::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApSelect::Name()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::Name" ) ) );

    return *iApList->At( iCursor )->iName;
    }


// ---------------------------------------------------------
// CApSelect::Uid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApSelect::Uid()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::Uid" ) ) );

    return iApList->At( iCursor )->iUid;
    }


// ---------------------------------------------------------
// CApSelect::Type
// ---------------------------------------------------------
//
EXPORT_C TCommsDbIspType CApSelect::Type()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::Type" ) ) );

    return iApList->At( iCursor )->iIspType;
    }


// ---------------------------------------------------------
// CApSelect::BearerType
// ---------------------------------------------------------
//
EXPORT_C TApBearerType CApSelect::BearerType()
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::BearerType" ) ) );

    return iApList->At( iCursor )->iBearerType;
    }




// ---------------------------------------------------------
// CApSelect::AllListItemDataL
// ---------------------------------------------------------
//
EXPORT_C TInt CApSelect::AllListItemDataL( CApListItemList& aList )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::AllListItemDataL" ) ) );

    aList.ResetAndDestroy();

    CApListItem* item=NULL;

    TInt i;
    TInt count = iApList->Count();
    for ( i=0; i<count; i++ )
        {
        item = CApListItem::NewLC( ( *iApList )[i] );
        aList.AppendL( item );
        CleanupStack::Pop( item ); // item owned by aList !
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::AllListItemDataL" ) ) );

    return aList.Count();
    }



// ---------------------------------------------------------
// CApSelect::IsReadOnly
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::IsReadOnly() const
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::IsReadOnly" ) ) );

    return iApList->At( iCursor )->iExt->iReadOnly;
    }



// ================= OTHER EXPORTED FUNCTIONS ==============

// ================== OTHER MEMBER FUNCTIONS ===============



// ---------------------------------------------------------
// CApSelect::DoUpdate
// ---------------------------------------------------------
//
TInt CApSelect::DoUpdateL()
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::DoUpdateL" ) ) );

    // filtering criteria:
    // Bearer type
    // Isp type
    // HasMandatory
    // + ordering (name or UID...)
    // first filter WAP_ACCESS_POINT table and order it
    TBool OwnTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    if ( !OwnTransaction )
        {
        __ASSERT_DEBUG( EFalse, ApCommons::Panic( ENotSupported ) );
        User::Leave( KErrNotSupported );
        }

    CCommsDbTableView* wapt = NULL;
    CCommsDbTableView* ipbearert = NULL;

    CCommsDbTableView* iapt = NULL;
                                     // csdOut, GprsIn, gprsOut, cdma,  VPN
    CCommsDbTableView* isptables[5] = { NULL,   NULL,   NULL,   NULL,   NULL};


    wapt = GetFilteredSortedWapTableViewLC();
    TInt err = wapt->GotoFirstRecord(); // OK.
    if ( ( err == KErrNone ) || ( err == KErrNotFound ) )
        { // even if it is empty, we must clear the list.
        // otherwise, Leave
        // fill up list. First empty it
        iApList->ResetAndDestroy();
        iCount = 0;
        if ( err == KErrNone )
            { // have some, we can go on...
            // First get ip_bearer table, then iap, then isp & gprs, if needed
            if ( ( iBearer & EApBearerTypeCSD ) ||
                 ( iBearer & EApBearerTypeHSCSD ) ||
                 ( iBearer & EApBearerTypeGPRS ) ||
                 ( iExt->iCdma && ( iBearer & EApBearerTypeCDMA ) ) )
                {
                ipbearert = GetFilteredSortedIpBearerTableViewLC();
                if ( ipbearert )
                    {
                    err = ipbearert->GotoFirstRecord();
                    if ( err == KErrNone )
                        {
                        if ( ( iBearer & EApBearerTypeCSD ) ||
                             ( iBearer & EApBearerTypeHSCSD ) )
                            {
                            isptables[KDIALOUT] =
                                GetFilteredSortedIspOutTableViewLC();
                            }
                        if ( ( iBearer & EApBearerTypeGPRS ) )
                            {
                            isptables[KGPRSIN] =
                                GetFilteredSortedGprsTableViewLC( ETrue );
                            isptables[KGPRSOUT] =
                                GetFilteredSortedGprsTableViewLC( EFalse );
                            }
                        }
                    else
                        {
                        if ( err != KErrNotFound )
                            {
                            User::LeaveIfError( err );
                            }
                        }
                    }
                }
            // get wapapids
               // get smsapids, if defined
            // get ipapids with iap ids
            // get only those iap ids from filtered database
            CArrayFixFlat<TUint32>* wapapid =
                    new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
            CleanupStack::PushL( wapapid );
            GetUintValuesL( wapt, TPtrC(COMMDB_ID), *wapapid );

            // read up access point names, too, just to be as fast as we can...
            CArrayPtrFlat<HBufC>* wapapname =
                    new ( ELeave ) CArrayPtrFlat<HBufC>( 10 );
            CleanupStack::PushL( wapapname );
            GetTextValuesL( wapt, TPtrC(COMMDB_NAME), *wapapname );


            // read up start page, too, just to be as fast as we can...
            CArrayPtrFlat<HBufC>* wapstartpage =
                    new ( ELeave ) CArrayPtrFlat<HBufC>( 10 );
            CleanupStack::PushL( wapstartpage );
            // as it is LONG text...
            GetLongTextValuesL( wapt, TPtrC(WAP_START_PAGE), *wapstartpage );

            //read up Read-only flags
            CArrayFixFlat<TUint32>* protflags =
                    new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
            CleanupStack::PushL( protflags );
            GetProtValuesL( wapt, *protflags );


            CArrayFixFlat<TUint32>* ipapid =
                    new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
            CleanupStack::PushL( ipapid );
            GetUintValuesL( ipbearert, TPtrC(WAP_ACCESS_POINT_ID), *ipapid );

            CArrayFixFlat<TUint32>* ipiapid =
                    new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
            CleanupStack::PushL( ipiapid );
            GetUintValuesL( ipbearert, TPtrC(WAP_IAP), *ipiapid );




            // What comes next:::::
            // for each table:
            // filter separately for different bearers ->
            // we know the bearer type...
            // filter separately for different ISP types ->
            // we know the ISP type...
            // now all we have to do is to 'simply' check filtered IAP with
            // the separate arrays, leave only those that present in
            // both bearer & isp array
            // fill up an array of id & an array of bearer type &
            // an array of isp type
            // after it, narrow down wap with wapbearer,
            // create items in order,
            // check items against remained iap array
            // fill up values in list.
            // bearer arrays : 0: csd, 1:hscsd, 2: gprs, 3: cdma

            TInt i( 0 );

            // an array Isp[CSDOut/GPRSIn/GPRSOut/CDMA/VPN]
            //             [COMMDB_ID, ISP_TYPE, Bearer_type]
            CArrayFixFlat<TUint32[3]>* ispArray[5];
            for (i=0; i<5; i++)
                {
                ispArray[i] = new ( ELeave ) CArrayFixFlat<TUint32[3]>( 10 );
                CleanupStack::PushL( ispArray[i] );
                }

            if ( isptables[KDIALOUT] )
                {
                GetIspValuesL( isptables[KDIALOUT], ispArray[KDIALOUT],
                               TPtrC(ISP_TYPE) );
                }
            if ( isptables[KGPRSIN] )
                {
                GetIspValuesL( isptables[KGPRSIN], ispArray[KGPRSIN],
                               TPtrC(GPRS_AP_TYPE) );
                }
            if ( isptables[KGPRSOUT] )
                {
                GetIspValuesL( isptables[KGPRSOUT], ispArray[KGPRSOUT],
                               TPtrC(GPRS_AP_TYPE) );
                }
            if ( isptables[KCDMA_ISP] )  // NULL if iExt->iCdma==EFalse.
                {
                GetIspValuesL( isptables[KCDMA_ISP], ispArray[KCDMA_ISP], 
                               TPtrC(CDMA_AP_TYPE) );
                }


            // iap arrays, 5 table, needs UID & linked ID for both
            CArrayFixFlat<TUint32>* iapArrays[5][2];
            for (i=0; i<5; i++)
                {
                iapArrays[i][0] = new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
                CleanupStack::PushL( iapArrays[i][0] );

                iapArrays[i][1] = new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
                CleanupStack::PushL( iapArrays[i][1] );
                }
            // get iap table data...
            // needs IAP UID & Iap_service
            if ( ( iBearer & EApBearerTypeCSD ) ||
                 ( iBearer & EApBearerTypeHSCSD ) )
                {
                iapt = OpenViewMatchingTextL( TPtrC(IAP),
                                              TPtrC(IAP_SERVICE_TYPE),
                                              TPtrC(DIAL_OUT_ISP) );
                CleanupStack::PushL( iapt );
                GetUintValuesL( iapt, TPtrC(COMMDB_ID),
                                *iapArrays[KDIALOUT][0] );
                GetUintValuesL( iapt, TPtrC(IAP_SERVICE),
                                *iapArrays[KDIALOUT][1] );
                CleanupStack::PopAndDestroy( iapt ); // iapt;
                iapt = NULL;
                }


            if ( iBearer & EApBearerTypeGPRS )
                {
                iapt = OpenViewMatchingTextL( TPtrC(IAP),
                                              TPtrC(IAP_SERVICE_TYPE),
                                              TPtrC(INCOMING_WCDMA) );
                CleanupStack::PushL( iapt );
                GetUintValuesL( iapt, TPtrC(COMMDB_ID),
                                *iapArrays[KGPRSIN][0] );
                GetUintValuesL( iapt, TPtrC(IAP_SERVICE),
                                *iapArrays[KGPRSIN][1] );
                CleanupStack::PopAndDestroy( iapt ); // iapt;
                iapt = NULL;

                iapt = OpenViewMatchingTextL( TPtrC(IAP),
                                              TPtrC(IAP_SERVICE_TYPE),
                                              TPtrC(OUTGOING_WCDMA) );
                CleanupStack::PushL( iapt );
                GetUintValuesL( iapt, TPtrC(COMMDB_ID),
                                *iapArrays[KGPRSOUT][0] );
                GetUintValuesL( iapt, TPtrC(IAP_SERVICE),
                                *iapArrays[KGPRSOUT][1] );
                CleanupStack::PopAndDestroy( iapt ); // iapt;
                iapt = NULL;
                }

            if ( ( iExt->iVpnFilterType == EVpnFilterVpnOnly ) ||
                ( iExt->iVpnFilterType == EVpnFilterBoth ) )
                {
                iapt = OpenViewMatchingTextL( TPtrC(IAP),
                                              TPtrC(IAP_SERVICE_TYPE),
                                              TPtrC(VPN_SERVICE) );
                CleanupStack::PushL( iapt );
                GetUintValuesL( iapt, TPtrC(COMMDB_ID),
                                *iapArrays[KVPN][0] );
                GetUintValuesL( iapt, TPtrC(IAP_SERVICE),
                                *iapArrays[KVPN][1] );
                CleanupStack::PopAndDestroy( iapt ); // iapt;
                iapt = NULL;
                }



            CArrayFixFlat<TVpnData>* vpnArray = NULL;

            if ( ( iExt->iVpnFilterType == EVpnFilterVpnOnly ) ||
                ( iExt->iVpnFilterType == EVpnFilterBoth ) )
                { // get VPN related data, namely Home IAP and name
                isptables[KVPN] = GetVpnTableViewLC();

                if ( isptables[KVPN] )
                    {

                    vpnArray = new ( ELeave ) CArrayFixFlat<TVpnData>( 10 );
                    CleanupStack::PushL( vpnArray );

                    GetVpnValuesL( isptables[KVPN], vpnArray );
                    }
                }




            // till this point, we have done what we can.
            // now we narrow down arrays.
            // start with iap vs isp arrays, leave what we can in iap array.

            CApListItem* item = NULL;
            CApListItemList* tmpApList = new( ELeave )CApListItemList();
            CleanupStack::PushL( tmpApList );
            TApBearerType bearer (EApBearerTypeAllBearers);

            // iapArrays[5][2]:
            // 5 table (DialOut, GprsIn, Out, Cdma, VPN),2 id's(Uid,ServiceId)
            // iapArrays[0] : Dial In service
            // ispinbearerArrays: 0: csd, 1:hscsd, 2: gprs. 3: cdma

            // 'Merge' iap with isp, create temp. items with bearer types
            // iap/DialIn/ServiceID : iapArrays[0][1]

            // check all iap/DialIn/ServiceID if it is in isparray ->
            // array Gives bearer type & isp type
            TInt j, iappos;
            // check all iap/DialOut/ServiceID if it is
            // in ispoutbearerarray[0,1]
            if ( iapArrays[KDIALOUT][1] )
                {
                for (j=0; j<iapArrays[KDIALOUT][1]->Count(); j++)
                    {
                    bearer = EApBearerTypeAllBearers;
                    // check all iap/DialOut/ServiceID if it is in isparray
                    // check if it is in ispArray[UID, ISP, Bearer]
                    // & return pos.
                    iappos = GetIapPos( ispArray[KDIALOUT],
                                        iapArrays[KDIALOUT][1]->At( j ) );
                    if ( iappos != KErrNotFound )
                        { // DialOut, Isp, Bearer...
                        // create temp. item WITH IAP ID, Bearer type
                        if ( ispArray[KDIALOUT]->At( iappos )[KBEARERTYPE] ==
                             EBearerTypeCSD )
                            {
                            bearer = EApBearerTypeCSD;
                            }
                        else
                            { // HSCSD bearer
                            bearer = EApBearerTypeHSCSD;
                            }
                        // create temp. item WITH IAP ID, Bearer type
                        item = CApListItem::NewLC
                            (
                            (TCommsDbIspType)
                                ispArray[KDIALOUT]->At( iappos )[KISPTYPE],
                            iapArrays[KDIALOUT][0]->At( j ),
                            KEmpty,
                            bearer
                            );
                        tmpApList->AppendL( item );
                        CleanupStack::Pop( item ); // item owned by list !
                        }
                    }
                }



            // check all iap/GprsIn/ServiceID if it is in ispinbearerarray[2]
            if ( iapArrays[KGPRSIN][1] )
                {
                bearer = EApBearerTypeGPRS;
                for (j=0; j<iapArrays[KGPRSIN][1]->Count(); j++)
                    {
                    iappos = GetIapPos( ispArray[KGPRSIN],
                                        iapArrays[KGPRSIN][1]->At( j ) );
                    if ( iappos != KErrNotFound )
                        { // GprsIn, Isp, Bearer...
                        // create temp. item WITH IAP ID, Bearer type
                        item = CApListItem::NewLC
                            (
                            (TCommsDbIspType)
                                ispArray[KGPRSIN]->At( iappos )[KISPTYPE],
                            iapArrays[KGPRSIN][0]->At( j ),
                            KEmpty,
                            bearer
                            );
                        tmpApList->AppendL( item );
                        CleanupStack::Pop( item ); // item owned by list !
                        }
                    }
                }


            // check all iap/GprsOut/ServiceID if it is in isparray
            if ( iapArrays[KGPRSOUT][1] )
                {
                bearer = EApBearerTypeGPRS;
                for (j=0; j<iapArrays[KGPRSOUT][1]->Count(); j++)
                    {
                    iappos = GetIapPos( ispArray[KGPRSOUT],
                                        iapArrays[KGPRSOUT][1]->At( j ) );
                    if ( iappos != KErrNotFound )
                        { // GprsIn, Isp, Bearer...
                        // create temp. item WITH IAP ID, Bearer type
                        item = CApListItem::NewLC
                            (
                            (TCommsDbIspType)
                                ispArray[KGPRSOUT]->At( iappos )[KISPTYPE],
                            iapArrays[KGPRSOUT][0]->At( j ),
                            KEmpty,
                            bearer
                            );
                        tmpApList->AppendL( item );
                        CleanupStack::Pop( item ); // item owned by list !
                        }
                    }
                }


            // check all iap/cdma/ServiceID if it is in ispinbearerarray[2]
            if ( iapArrays[KCDMA_ISP][1] )  // NULL if iExt->iCdma==EFalse.
                {
                bearer = EApBearerTypeCDMA;
                for (j=0; j<iapArrays[KCDMA_ISP][1]->Count(); j++)
                    {
                    iappos = GetIapPos( ispArray[KCDMA_ISP],
                                        iapArrays[KCDMA_ISP][1]->At( j ) );
                    if ( iappos != KErrNotFound )
                        { // Cdma, Isp, Bearer...
                        // create temp. item WITH IAP ID, Bearer type
                        item = CApListItem::NewLC
                            (
                            (TCommsDbIspType)
                                ispArray[KCDMA_ISP]->At( iappos )[KISPTYPE],
                            iapArrays[KCDMA_ISP][0]->At( j ),
                            KEmpty,
                            bearer
                            );
                        tmpApList->AppendL( item );
                        CleanupStack::Pop( item ); // item owned by list !
                        }
                    }
                }


            // Now we have the temporary list filled with
            // IAP UID, bearer type, ISP type,
            // So compare it to WAP part and create the REAL list...
            TInt pos( KErrNotFound );
            TInt itempos( KErrNotFound );
            TBool protsupported = FeatureManager::FeatureSupported(
                                         KFeatureIdSettingsProtection );
            for (i=0; i< wapapid->Count(); i++)
                { // as wapapid is sorted, resulting list will be sorted, too !
                // is it in IP bearer ?
                if ( IsInArray( ipapid, wapapid->At( i ) ) )
                    {
                    // now need to get bearer type & isp type
                    //
                    pos = PosInArray(ipapid, wapapid->At( i ) );
                    if (pos!= KErrNotFound )
                        { // now we have the index in the
                        // ipiapid as they are synchronized...
                        // iapUid = ipiapid[ pos ];
                        // now have to find a CApListItem in tmpApList
                        itempos = GetItemPos( tmpApList,
                                              ipiapid->At( pos ) );
                        if (itempos != KErrNotFound )
                            {
                            TBool prot(EFalse);
                            if (protsupported)
                                {
                                prot = protflags->At( i );
                                }
                            item = CApListItem::NewLC
                                (
                                (TCommsDbIspType)
                                    tmpApList->At( itempos )->Type(),
                                wapapid->At( i ),
                                *wapapname->At( i ),
                                tmpApList->At( itempos )->BearerType(),
                                *wapstartpage->At( i ),
                                prot );
                                
                            iApList->AppendL( item );
                            CleanupStack::Pop( item ); // item owned by aList !
                            }
                        }
                    }
                }

            #ifdef __TEST_LAN_BEARER
                // enable LAN:
                // ADD THEM AFTER NORMAL QUERYING...
                if ( ( iBearer & EApBearerTypeLAN ) 
                    || ( iBearer & EApBearerTypeLANModem ) )
                    {
                    AddLanL( EFalse );
                    }
            #else
                // enable LAN:
                // ADD THEM AFTER NORMAL QUERYING...
                if ( iBearer & EApBearerTypeLANModem ) 
                    {
                    AddLanL( EFalse );
                    }            
            #endif // __TEST_LAN_BEARER
                if ( iBearer & EApBearerTypeWLAN ) 
                    {
                    if ( iExt->iWlanSupported )
                        {
                        // enable WLAN:
                        // ADD THEM AFTER NORMAL QUERYING...
                        AddLanL( ETrue );
                        }
                    }
                
            // now it has only elements which are OK.
            // from ALL CSD/GPRS/CDMA tables, still in sorted order...
            // now it is time to check VPN related stuff, if any
                if ( ( iExt->iVpnFilterType == EVpnFilterVpnOnly ) ||
                    ( iExt->iVpnFilterType == EVpnFilterBoth ) )
                    { // Add VPN access points if needed
                    // Add ALL VPN access points if needed as requested by
                    // Nokia, thus incomplete access points to be listed.
                    TUint32 wapid( 0 );
                    TInt count = vpnArray->Count();
                    
                    CApUtils* util = CApUtils::NewLC( *iDb );
                    for ( i = 0; i < count; i++ )
                        {
                        // get home wap id first
                        pos = PosInArray( ipiapid, 
                                          vpnArray->At( i ).iHomeIapId );
                        TInt foundpos( KErrNotFound );
                        if ( pos!= KErrNotFound )
                            { // now we have the index in the
                            // ipiapid and as they are synchronized...
                            wapid = ipapid->At( pos );
                            // now check if we do have it in array...
                            TInt jcount = iApList->Count();
                            for ( j = 0; j < jcount; j++ )
                                {
                                if ( iApList->At( j )->Uid() == wapid )
                                    {
                                    foundpos = j;
                                    break;
                                    }
                                }
                            }
                        // should add VPN
                        // check if VPN-table COMMDB_ID is 
                        // in iapArrays[VPN][1]
                        pos = PosInArray( 
                                iapArrays[KVPN][1], 
                                vpnArray->At( i ).iVpnIapId );
                        if ( pos != KErrNotFound )
                            { // now get VPN IAP ID
                            TUint32 tempvpniapid = 
                                iapArrays[KVPN][0]->At( pos );
                            TUint32 tempvpnwapid ( 0 );
                            TRAP( err, tempvpnwapid = 
                                    util->WapIdFromIapIdL( 
                                           tempvpniapid ) );

                            if ( err == KErrNone )
                                {
                                if ( foundpos != KErrNotFound )
                                    {
                                    // if not found, it is either an unknown 
                                    // bearer type, for which we can nto show
                                    // icon and UI at all, or it does not 
                                    // fulfill the filtering criteria, so we 
                                    // ignore them.
                                    item = CApListItem::NewLC
                                        (
                                        iApList->At( foundpos )->Type(),
                                        tempvpnwapid,
                                        *vpnArray->At( i ).iName,
                                        iApList->At( foundpos )
                                                ->BearerType()
                                        );
                                    item->SetVpnFlag( ETrue );
                                    iApList->AppendL( item );
                                    // item owned by aList !
                                    CleanupStack::Pop( item ); 
                                    }
                                }
                            }
                        }
                    CleanupStack::PopAndDestroy( util );
                    }

                if ( iExt->iVpnFilterType == EVpnFilterVpnOnly )
                    { // remove non-vpn access points if needed
                    TInt count = iApList->Count();
                    for ( i = 0; i < count; i++ )
                        {
                        if ( !iApList->At( i )->IsVpnAp() )
                            {
                            delete iApList->At( i );
                            iApList->Delete( i );
                            count--;
                            i--; // 'cause it is cycled...
                            }
                        }
                    }


/*
#ifdef __WINS__
                //=============================================================
                // Support for ethernet in emulator, change 1/2
                //

                iapt = OpenViewMatchingTextL( TPtrC(IAP),
                    TPtrC(IAP_SERVICE_TYPE),
                    TPtrC(LAN_SERVICE) );
                CleanupStack::PushL( iapt );
                
                
                TBuf<40> name;
                TUint32 id;
                TInt err = iapt->GotoFirstRecord();
                TInt err2;
                CCommsDbTableView* tmpView = NULL;

                while(err == KErrNone)
                    {
                    iapt->ReadTextL(TPtrC(COMMDB_NAME), name);
                    iapt->ReadUintL(TPtrC(COMMDB_ID), id);
                    
                    tmpView = iDb->OpenViewMatchingUintLC(
                                    TPtrC(WAP_IP_BEARER), TPtrC(WAP_IAP), id);
                    err2 = tmpView->GotoFirstRecord();
                    if(err2 == KErrNone)
                        {
                        tmpView->ReadUintL(TPtrC(WAP_ACCESS_POINT_ID), id);
                        CleanupStack::PopAndDestroy( tmpView );

                        tmpView = iDb->OpenViewMatchingUintLC(
                                TPtrC(WAP_ACCESS_POINT), TPtrC(COMMDB_ID), id);
                        err2 = tmpView->GotoFirstRecord();
                        if ( err2 == KErrNone )
                            {
                            item = CApListItem::NewLC
                                (
                                EIspTypeInternetAndWAP,
                                id,
                                name,
                                EApBearerTypeGPRS
                                );
                            iApList->AppendL( item );
                    
                            CleanupStack::Pop( item );
                            }                        
                        }

                    CleanupStack::PopAndDestroy( tmpView );

                    err = iapt->GotoNextRecord();
                    }
                
                
                CleanupStack::PopAndDestroy( iapt ); // iapt;
                iapt = NULL;
                
                // End of change 1/2
                //=============================================================
                
#endif // __WINS__
*/                
                
                
                iCount = iApList->Count();
                
            CleanupStack::PopAndDestroy( tmpApList ); // tmpApList
            
            if ( vpnArray )
                {	                
                CleanupStack::PopAndDestroy( vpnArray->Count() ); 
                CleanupStack::PopAndDestroy( vpnArray );                 
                }
            if ( isptables[KVPN] )
                {	                
                CleanupStack::PopAndDestroy( isptables[KVPN] ); 
                }

            CleanupStack::PopAndDestroy( 10, iapArrays[0][0] ); // iap arrays
            
            CleanupStack::PopAndDestroy( 5, ispArray[0] ); // isp arrays

            CleanupStack::Pop( 2, ipapid ); // ipiapid, ipapid
            CleanupStack::Pop( protflags ); // protflags
            delete protflags;
            delete ipiapid;
            delete ipapid;

// Note: SMS bearer had been dropped.
            CleanupStack::PopAndDestroy( wapstartpage->Count() );//wapstartpage
            CleanupStack::PopAndDestroy( wapstartpage ); // wapstartpage
            CleanupStack::PopAndDestroy( wapapname->Count() ); // wapapname
            CleanupStack::PopAndDestroy( wapapname ); // wapapname
            CleanupStack::Pop(); // wapapid
            delete wapapid;

            if ( isptables[KCDMA_ISP] )
                {
                CleanupStack::PopAndDestroy( isptables[KCDMA_ISP] );
                }

            if ( isptables[KGPRSOUT] )
                {
                CleanupStack::PopAndDestroy( isptables[KGPRSOUT] );
                }

            if ( isptables[KGPRSIN] )
                {
                CleanupStack::PopAndDestroy( isptables[KGPRSIN] );
                }

            if ( isptables[KDIALOUT] )
                {
                CleanupStack::PopAndDestroy( isptables[KDIALOUT] );
                }

            if ( ipbearert )
                {
                CleanupStack::PopAndDestroy( ipbearert );
                }
            }
        else
            {
            if ( err != KErrNotFound )
                {
                User::LeaveIfError( err );
                }
            }
        }
    else
        {
        if ( err != KErrNotFound )
            {
            User::Leave( err );
            }
        }
    CleanupStack::PopAndDestroy( wapt );  // wapt
    iExt->iMaxIndex = iCount-1;
    if ( OwnTransaction )
        {
        ApCommons::RollbackTransactionOnLeave( iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::DoUpdateL" ) ) );

    return err;
    }





// ---------------------------------------------------------
// CApSelect::GetFilteredSortedWapTableViewLC
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetFilteredSortedWapTableViewLC()
    {
    CLOG( ( ESelect, 0, 
        _L( "-> CApSelect::GetFilteredSortedWapTableViewLC" ) ) );

    CCommsDbTableView* wapt = NULL;
    // Select * from WAP Access Point Table Where
    // WAP_CURRENT_BEARER=WAP_IP_BEARER
    if ( ( iBearer & EApBearerTypeCSD ) ||
         ( iBearer & EApBearerTypeHSCSD ) ||
         ( iBearer & EApBearerTypeGPRS ) ||
         ( iExt->iCdma && ( iBearer & EApBearerTypeCDMA ) ) )
        {
        wapt = iDb->OpenViewMatchingTextLC( TPtrC(WAP_ACCESS_POINT), 
                                            TPtrC(WAP_CURRENT_BEARER), 
                                            TPtrC(WAP_IP_BEARER));
        }
    else
        {
        wapt = iDb->OpenTableLC( TPtrC(WAP_ACCESS_POINT) );
        }
    
    TInt err;
    err = wapt->GotoFirstRecord();
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }

    if ( err != KErrNotFound )
        {
        // here may come additional AND-ed parts
        if ( ( iIsp & KEApIspTypeWAPMandatory ) ||
             ( iIsp & KEApIspTypeMMSMandatory ) )
            { // add AND ( Hasmandatory..)
            if ( ( iBearer & EApBearerTypeCSD ) ||
                 ( iBearer & EApBearerTypeHSCSD ) ||
                 ( iBearer & EApBearerTypeGPRS ) ||
                 ( iExt->iCdma && ( iBearer & EApBearerTypeCDMA ) ) )
                { // Needs IP bearer
                // ( WAP_CURRENT_BEARER = WAP_IP_BEARER AND
                //      WAP_START_PAGE != ""
                //      AND WAP_START_PAGE != "http://"
                // Filter out unneded records
                HBufC* buf;
                do
                    {
                    // Add test cases covering this route
                    buf = wapt->ReadLongTextLC( TPtrC(WAP_START_PAGE) );
                    if ( ( buf->Compare( KEmpty ) == 0 ) 
                       || ( buf->Compare( KInvStartPage ) == 0 ) )
                        {
                        err = wapt->DeleteRecord();
                        }
                    CleanupStack::PopAndDestroy( buf );
                    err = wapt->GotoNextRecord();
                    } 
                    while ( !err );
                }
            }
        }
    CLOG( ( ESelect, 1, 
        _L( "<- CApSelect::GetFilteredSortedWapTableViewLC" ) ) );

    return wapt;
    }





// ---------------------------------------------------------
// CApSelect::GetFilteredSortedIpBearerTableViewLC()
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetFilteredSortedIpBearerTableViewLC()
    {
    CLOG( ( ESelect, 0, 
        _L( "-> CApSelect::GetFilteredSortedIpBearerTableViewLC" ) ) );

    CCommsDbTableView* table = NULL;

    table = iDb->OpenTableLC( TPtrC(WAP_IP_BEARER) );
    TInt err;
    err = table->GotoFirstRecord();

    if ( ( iBearer & EApBearerTypeCSD ) ||
         ( iBearer & EApBearerTypeHSCSD ) ||
         ( iBearer & EApBearerTypeGPRS ) ||
         ( iExt->iCdma && ( iBearer & EApBearerTypeCDMA ) ) )
        { 
        // needs ipbearer table
        // only filtering we can do is about Mandatory
        // because Isptype & Bearertype are up to
        // ISP & GPRS tables...
        if ( iIsp & KEApIspTypeWAPMandatory )
            { // Hasmandatory..
            // SELECT * from WAP_IP_BEARER WHERE
            // WAP_GATEWAY_ADDRESS <> ''
            //      AND WAP_GATEWAY_ADDRESS != "0.0.0.0" )
            // AND HIDDEN...

            if ( err != KErrNotFound )
                {
                User::LeaveIfError( err );

                HBufC* buf;
                do
                    {
                    buf = table->ReadLongTextLC( TPtrC(WAP_GATEWAY_ADDRESS) );
                    if ( ( buf->Compare( KEmpty ) == 0 ) 
                       || ( buf->Compare( KDynIpAddress ) == 0 ) )
                        {
                        err = table->DeleteRecord();
                        }
                    CleanupStack::PopAndDestroy( buf );
                    err = table->GotoNextRecord();
                    } while ( !err );
                }
            }
        }

    CLOG( ( ESelect, 1, 
        _L( "<- CApSelect::GetFilteredSortedIpBearerTableViewLC" ) ) );

    return table;
    }



// ---------------------------------------------------------
// CApSelect::GetFilteredSortedIspOutTableViewLC()
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetFilteredSortedIspOutTableViewLC()
    {
    CLOG( ( ESelect, 0, 
        _L( "-> CApSelect::GetFilteredSortedIspOutTableViewLC" ) ) );
    
    CCommsDbTableView* table = NULL;
    // ISP-type & bearer type...
    if ( ( iBearer & EApBearerTypeCSD ) ||
         ( iBearer & EApBearerTypeHSCSD ) )
        {
        // only needed if CSD or HSCSD
        if ( ( iBearer & EApBearerTypeCSD ) &&
             (!( iBearer & EApBearerTypeHSCSD )) )
            { // needs only CSD
            // WHERE ISP_BEARER_TYPE = EBearerTypeCSD
            table = iDb->OpenViewMatchingUintLC( TPtrC(DIAL_OUT_ISP),
                                                 TPtrC(ISP_BEARER_TYPE),
                                                 EBearerTypeCSD
                                                 );
            }
        else
            {
            if ( ( iBearer & EApBearerTypeHSCSD ) &&
                 (!( iBearer & EApBearerTypeCSD )) )
                { // needs only HSCSD
                // WHERE ISP_BEARER_TYPE = EBearerTypeHSCSD
                table = iDb->OpenViewMatchingUintLC( TPtrC(DIAL_OUT_ISP),
                                                     TPtrC(ISP_BEARER_TYPE),
                                                     EBearerTypeHSCSD
                                                     );
                }
            else
                {
                table = iDb->OpenTableLC( TPtrC(DIAL_OUT_ISP) );
                }
            }
        TInt err;
        err = table->GotoFirstRecord();
        
        if ( err != KErrNotFound )
            {
            User::LeaveIfError( err );
            // now add isp-type filtering
            // if Added = ETrue : AND ( ), else : WHERE
            // KEApIspTypeInternetOnly,
            // KEApIspTypeWAPOnly,
            // KEApIspTypeInternetAndWAP,
            // KEApIspTypeWAPMandatory, KEApIspTypeAll
            TUint32 isp( 0 );
            do
                {
                table->ReadUintL( TPtrC(ISP_TYPE), isp );
                if ( ( iIsp & KEApIspTypeInternetOnly ) 
                     && ( !( iIsp & KEApIspTypeAll ) )
                     )
                    {// ( ( ISP_TYPE == EIspTypeInternetOnly )
                    if ( isp != EIspTypeInternetOnly )
                        {
                        err = table->DeleteRecord();
                        }
                    }
                if ( ( iIsp & KEApIspTypeWAPOnly ) 
                     && ( !( iIsp & KEApIspTypeAll ) )
                     )
                    {
                    if ( isp != EIspTypeWAPOnly )
                        {
                        err = table->DeleteRecord();
                        }
                    }
                if ( ( iIsp & KEApIspTypeInternetAndWAP ) 
                     && ( !( iIsp & KEApIspTypeAll ) )
                     )
                    {
                    if ( isp != EIspTypeInternetAndWAP )
                        {
                        err = table->DeleteRecord();
                        }
                    }
                err = table->GotoNextRecord();
                } while ( !err );
            }
        }

    CLOG( ( ESelect, 1, 
        _L( "<- CApSelect::GetFilteredSortedIspOutTableViewLC" ) ) );

    return table;
    }



// ---------------------------------------------------------
// CApSelect::GetFilteredSortedGprsTableViewLC()
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetFilteredSortedGprsTableViewLC( TBool aIn )
    {
    CLOG( ( ESelect, 0, 
        _L( "-> CApSelect::GetFilteredSortedGprsTableViewLC" ) ) );

    CCommsDbTableView* table = NULL;
    // ISP-type & bearer type...
    if ( iBearer & EApBearerTypeGPRS )
        {
        // as all other criteria has more than 2 possible values
        // and all their combinations should be taken into account,
        // I ask the whole table and then narrow it down
        // myself with removing the unneded records temporarily
        if ( aIn )
            {
            table = iDb->OpenTableLC( TPtrC( INCOMING_WCDMA ) );
            }
        else
            {
            table = iDb->OpenTableLC( TPtrC( OUTGOING_WCDMA ) );
            }
        TInt err;
        err = table->GotoFirstRecord();
        
        if ( err != KErrNotFound )
            {
            User::LeaveIfError( err );
            // only needed if GPRS
            // now add isp-type filtering
            // KEApIspTypeInternetOnly,
            // KEApIspTypeWAPOnly,
            // KEApIspTypeInternetAndWAP,
            // KEApIspTypeWAPMandatory, KEApIspTypeAll
            TUint32 isp( 0 );
            TUint32 pdptype( 0 );
            TBool deleted(EFalse);
            do
                {
                deleted = EFalse;
                // first check if it fits our isp type criteria
                TRAPD( res, table->ReadUintL( TPtrC(GPRS_AP_TYPE), isp ) );
                if ( res != KErrUnknown )
                    {
                    if ( ( iIsp & KEApIspTypeInternetOnly ) 
                         && ( !( iIsp & KEApIspTypeAll ) )
                         )
                        {// ( ( ISP_TYPE == EIspTypeInternetOnly )
                        if ( isp != EIspTypeInternetOnly )
                            {
                            err = table->DeleteRecord();
                            deleted = ETrue;
                            }
                        }
                    if ( ( iIsp & KEApIspTypeWAPOnly ) 
                         && ( !( iIsp & KEApIspTypeAll ) )
                         )
                        {
                        if ( isp != EIspTypeWAPOnly )
                            {
                            err = table->DeleteRecord();
                            deleted = ETrue;
                            }
                        }
                    if ( ( iIsp & KEApIspTypeInternetAndWAP ) 
                         && ( !( iIsp & KEApIspTypeAll ) )
                         )
                        {
                        if ( isp != EIspTypeInternetAndWAP )
                            {
                            err = table->DeleteRecord();
                            deleted = ETrue;
                            }
                        }
                    }
                // now check if it fits IPv4 or IPv6 criteria
                // Add IPv4 or IPv6 query ONLY if it is supported!!!
                if ( ( !deleted ) && ( iExt->iIsIpv6Supported ) )
                    {
                    // if ONLY IPv4 is requested, 
                    // filter out where PDP_type IS IPv6
                    // if both is requested, do nothing
                    // if IPv6 only is requested, filter IN 
                    // where PDP_type IS IPv6
                    if ( ( iExt->iReqIpvType == EIPv4 ) ||
                         ( iExt->iReqIpvType == EIPv6 ) )
                        { // not both, needs filtering
                        table->ReadUintL( TPtrC(GPRS_PDP_TYPE), pdptype );
                        if ( ( iExt->iReqIpvType == EIPv4 ) 
                             && ( pdptype == RPacketContext::EPdpTypeIPv6 )
                             )
                            {
                            err = table->DeleteRecord();
                            deleted = ETrue;
                            }
                        if ( ( iExt->iReqIpvType == EIPv6 ) 
                             && ( pdptype == RPacketContext::EPdpTypeIPv4 )
                             )
                            {
                            err = table->DeleteRecord();
                            deleted = ETrue;
                            }
                        }
                    }
                err = table->GotoNextRecord();
                } while ( !err );
            }
        }
    CLOG( ( ESelect, 1, 
        _L( "<- CApSelect::GetFilteredSortedGprsTableViewLC" ) ) );

    return table;
    }



// ---------------------------------------------------------
// CApSelect::GetUintValuesL
// ---------------------------------------------------------
//
void CApSelect::GetUintValuesL( CCommsDbTableView* aTable,
                                const TDesC& aField,
                                CArrayFixFlat<TUint32>& aArray )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetUintValuesL" ) ) );

    if ( aTable )
        {
        TInt err = aTable->GotoFirstRecord();
        if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            {
            User::Leave( err );
            }
        if ( err != KErrNotFound )
            {
            TUint32 temp( 0 );
            TBool goon( ETrue );
            do
                {
                err = ApCommons::ReadUintL( aTable, TPtrC(COMMDB_ID), temp );
                if ( ( err == KErrNone ) && ( temp ) )
                    {
                    // Here I need to TRAP to be able to use 'old'
                    // database which has no such field !
                    // Also assigns a 'default' for it in that case
                    TRAP(err, ApCommons::ReadUintL( aTable, aField, temp ) );
                    aArray.AppendL( temp );
                    }
                err = aTable->GotoNextRecord();
                if ( err == KErrNotFound )
                    {
                    goon = EFalse;
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
                while ( goon );
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetUintValuesL" ) ) );

    }



// ---------------------------------------------------------
// CApSelect::GetProtValuesL
// ---------------------------------------------------------
//
void CApSelect::GetProtValuesL( CCommsDbTableView* aTable,
                                CArrayFixFlat<TUint32>& aArray )
    {    
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetProtValuesL" ) ) );

    CCommsDbProtectTableView* view =
        (CCommsDbProtectTableView*)aTable;
    if ( aTable )
        {
        TInt err = aTable->GotoFirstRecord();
        if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            {
            User::Leave( err );
            }
        if ( err != KErrNotFound )
            {
            TBool goon( ETrue );
            TInt prot( EFalse );
            do
                {
                err = view->GetRecordAccess( prot );
                if ( ( err == KErrNone ) )
                    {
                    aArray.AppendL( prot );
                    }
                err = aTable->GotoNextRecord();
                if ( err == KErrNotFound )
                    {
                    goon = EFalse;
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
                while ( goon );
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetProtValuesL" ) ) );

    }




// ---------------------------------------------------------
// CApSelect::GetTextValuesL
// ---------------------------------------------------------
//
void CApSelect::GetTextValuesL( CCommsDbTableView* aTable,
                                const TDesC& aField,
                                CArrayPtrFlat<HBufC>& aArray )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetTextValuesL" ) ) );

    if ( aTable )
        {
        TInt err = aTable->GotoFirstRecord();
        if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            {
            User::Leave( err );
            }
        if ( err != KErrNotFound )
            {
            TBool goon( ETrue );
            TInt l;
            TUint32 temp( 0 );
            HBufC* tmp = NULL;
            do
                {
                err = ApCommons::ReadUintL( aTable, TPtrC(COMMDB_ID), temp );
                if ( ( err == KErrNone ) && ( temp ) )
                    {
                    aTable->ReadColumnLengthL( aField, l );
                    tmp = HBufC::NewLC( l );
                    TPtr tmpptr( tmp->Des() );
                    aTable->ReadTextL( aField, tmpptr );
                    aArray.AppendL( tmp );
                    }
                err = aTable->GotoNextRecord();
                if ( err == KErrNotFound )
                    {
                    goon = EFalse;
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
                while ( goon );
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetTextValuesL" ) ) );

    }



// ---------------------------------------------------------
// CApSelect::GetLongTextValuesL
// ---------------------------------------------------------
//
void CApSelect::GetLongTextValuesL( CCommsDbTableView* aTable,
                                    const TDesC& aField,
                                    CArrayPtrFlat<HBufC>& aArray )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetLongTextValuesL" ) ) );

    if ( aTable )
        {
        TInt err = aTable->GotoFirstRecord();
        if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            {
            User::Leave( err );
            }
        if ( err != KErrNotFound )
            {
            TBool goon( ETrue );
            TInt l;
            TUint32 temp( 0 );
            HBufC* tmp = NULL;
            do
                {
                err = ApCommons::ReadUintL( aTable, TPtrC(COMMDB_ID), temp );
                if ( ( err == KErrNone ) && ( temp ) )
                    {
                    aTable->ReadColumnLengthL( aField, l );
                    tmp = aTable->ReadLongTextLC( aField );
                    aArray.AppendL( tmp );
                    }
                err = aTable->GotoNextRecord();
                if ( err == KErrNotFound )
                    {
                    goon = EFalse;
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
                while ( goon );
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetLongTextValuesL" ) ) );

    }



// ---------------------------------------------------------
// CApSelect::IsInArray
// ---------------------------------------------------------
//
TBool CApSelect::IsInArray( CArrayFixFlat<TUint32>* aArray, TUint32 aValue )
    {
    CLOG( ( ESelect, 0, _L( "<-> CApSelect::IsInArray" ) ) );

    return ( PosInArray( aArray, aValue ) >= 0);
    }



// ---------------------------------------------------------
// CApSelect::PosInArray
// ---------------------------------------------------------
//
TInt CApSelect::PosInArray( CArrayFixFlat<TUint32>* aArray, TUint32 aValue )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::PosInArray" ) ) );

    TInt retval( KErrNotFound );
    if ( aArray )
        {
        TInt count = aArray->Count();
        for( TInt i=0; ( ( i<count) && ( retval == KErrNotFound ) ); i++ )
            {
            if ( aArray->At( i ) == aValue )
                {
                retval = i;
                }
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::PosInArray" ) ) );

    return retval;
    }


// ---------------------------------------------------------
// CApSelect::GetItemPos
// ---------------------------------------------------------
//
TInt CApSelect::GetItemPos( CApListItemList* aList, TUint32 aId )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetItemPos" ) ) );

    TInt retval( KErrNotFound );
    if ( aList )
        {
        TInt count = aList->Count();
        for( TInt i=0; ( ( i<count) && ( retval == KErrNotFound ) ); i++ )
            {
            if ( aList->At( i )->Uid() == aId )
                {
                retval = i;
                }
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetItemPos" ) ) );

    return retval;
    }



// ---------------------------------------------------------
// CApSelect::OpenViewMatchingTextL
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::OpenViewMatchingTextL(
                                             const TDesC& aTableName,
                                             const TDesC& aColumnToMatch,
                                             const TDesC16& aValueToMatch )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::OpenViewMatchingTextL" ) ) );

    CCommsDbTableView* table = NULL;
    TRAPD( err, { // this leaves if the table is empty....
                table = iDb->OpenViewMatchingTextLC( aTableName,
                                                     aColumnToMatch,
                                                     aValueToMatch );
                CleanupStack::Pop( table ); // table
                }
        );
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::OpenViewMatchingTextL" ) ) );

    return table;
    }



// ---------------------------------------------------------
// CApSelect::GetIspValuesL
// ---------------------------------------------------------
//
void CApSelect::GetIspValuesL( CCommsDbTableView* aTable,
                               CArrayFixFlat<TUint32[3]>* aArray,
                               const TDesC& aColName )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetIspValuesL" ) ) );

    if ( aTable )
        {
        TInt err = aTable->GotoFirstRecord();
        if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            {
            User::Leave( err );
            }
        if ( err != KErrNotFound )
            {
            TUint32 temp[3];
            TBool goon( ETrue );
            do
                {
                err = ApCommons::ReadUintL( aTable, TPtrC(COMMDB_ID),
                                             temp[KUID] );
                if ( ( err == KErrNone ) && ( temp[KUID] ) )
                    {
                    // Here I need to TRAP to be able to use 'old'
                    // database which has no such field !
                    // Also assigns a 'default' for it in that case
                    TRAP( err, ApCommons::ReadUintL
                                        (
                                        aTable,
                                        aColName,
                                        temp[KISPTYPE]
                                        ) );
                    if ( err == KErrNotFound )
                        {
                        temp[KISPTYPE] = EIspTypeInternetAndWAP;
                        }
                    else
                        {
                        User::LeaveIfError( err );
                        }
                    // if it is CSD/HSCSD, now decide which one
                    if( !aColName.Compare( TPtrC(ISP_TYPE) ) )
                        {
                        err = ApCommons::ReadUintL
                                            (
                                            aTable,
                                            TPtrC(ISP_BEARER_TYPE),
                                            temp[KBEARERTYPE]
                                            );
                        if ( err != KErrNone )
                            { // only possible if it is NULL,
                            // for other errors, it leaves...
                            temp[KBEARERTYPE] = EBearerTypeCSD;
                            }
                        }
                    else
                        {
                        temp[KBEARERTYPE] = 0;
                        }
                    aArray->AppendL( temp );
                    }
                err = aTable->GotoNextRecord();
                if ( err == KErrNotFound )
                    {
                    goon = EFalse;
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
                while ( goon );
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetIspValuesL" ) ) );

    }




// ---------------------------------------------------------
// CApSelect::GetIapPos
// ---------------------------------------------------------
//
TInt CApSelect::GetIapPos( CArrayFixFlat<TUint32[3]>* aArray, TUint32 aValue )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetIapPos" ) ) );

    TInt retval( KErrNotFound );
    if ( aArray )
        {
        TInt count = aArray->Count();
        for( TInt i=0; ( ( i<count) && ( retval == KErrNotFound ) ); i++ )
            {
            if ( aArray->At( i )[KUID] == aValue )
                {
                retval = i;
                }
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetIapPos" ) ) );

    return retval;
    }



// ---------------------------------------------------------
// CApSelect::SortLocalisedNameL()
// ---------------------------------------------------------
//
void CApSelect::SortLocalisedNameL( TBool aAscending )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::SortLocalisedNameL" ) ) );

    TApListItemNameKey Key( aAscending );
    Key.SetPtr( iApList );
    // Sort returns KErrGeneral if stack overflow, otherwise, returns
    // KErrNone. So we will Leave only if stack overflow,
    // but then that really does not matter...
    User::LeaveIfError( iApList->Sort( Key ) );
    CLOG( ( ESelect, 1, _L( "<- CApSelect::SortLocalisedNameL" ) ) );

    }




// ---------------------------------------------------------
// CApSelect::SortUidL()
// ---------------------------------------------------------
//
void CApSelect::SortUidL( TBool aAscending )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::SortUidL" ) ) );

    TApListItemUidKey Key( aAscending );
    Key.SetPtr( iApList );
    // Sort returns KErrGeneral if stack overflow, otherwise, returns
    // KErrNone. So we will Leave only if stack overflow,
    // but then that really does not matter...
    User::LeaveIfError( iApList->Sort( Key ) );
    CLOG( ( ESelect, 1, _L( "<- CApSelect::SortUidL" ) ) );

    }

// ---------------------------------------------------------
// CApSelect::NewLC()
// ---------------------------------------------------------
//
EXPORT_C CApSelect* CApSelect::NewLC( CCommsDatabase& aDb,
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TInt aReqIpvType
                                      )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::NewLC" ) ) );

    CApSelect* db = new( ELeave ) CApSelect;
    CleanupStack::PushL( db );
    db->ConstructL( aDb, aIspFilter, aBearerFilter, 
                    aSortType, aReqIpvType, EVpnFilterBoth );

    CLOG( ( ESelect, 1, _L( "<- CApSelect::NewLC" ) ) );
    return db;
    }



// ---------------------------------------------------------
// CApSelect::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::ConstructL( CCommsDatabase& aDb,
                                 TInt aIspFilter,
                                 TInt aBearerFilter,
                                 TInt aSortType,
                                 TInt aReqIpvType )

    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::ConstructL" ) ) );
    
    ConstructL( aDb, aIspFilter, aBearerFilter, 
                aSortType, aReqIpvType, EVpnFilterBoth );

    CLOG( ( ESelect, 1, _L( "<- CApSelect::ConstructL" ) ) );
    }




// ---------------------------------------------------------
// CApSelect::NewLC()
// ---------------------------------------------------------
//
EXPORT_C CApSelect* CApSelect::NewLC( CCommsDatabase& aDb,
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TInt aReqIpvType,
                                      TVpnFilterType aVpnFilterType
                                      )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::NewLC" ) ) );

    CApSelect* db = new( ELeave ) CApSelect;
    CleanupStack::PushL( db );
    db->ConstructL( aDb, aIspFilter, aBearerFilter, 
                    aSortType, aReqIpvType, aVpnFilterType );

    CLOG( ( ESelect, 1, _L( "<- CApSelect::NewLC" ) ) );
    return db;
    }



// ---------------------------------------------------------
// CApSelect::NewLC()
// ---------------------------------------------------------
//
EXPORT_C CApSelect* CApSelect::NewLC( CCommsDatabase& aDb,
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TInt aReqIpvType,
                                      TVpnFilterType aVpnFilterType,
                                      TBool aIncludeEasyWlan )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::NewLC" ) ) );

    CApSelect* db = new( ELeave ) CApSelect;
    CleanupStack::PushL( db );
    db->ConstructL( aDb, aIspFilter, aBearerFilter, 
                    aSortType, aReqIpvType, aVpnFilterType,
                    aIncludeEasyWlan );

    CLOG( ( ESelect, 1, _L( "<- CApSelect::NewLC" ) ) );
    return db;    
    }


// ---------------------------------------------------------
// CApSelect::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::ConstructL( CCommsDatabase& aDb,
                                 TInt aIspFilter,
                                 TInt aBearerFilter,
                                 TInt aSortType,
                                 TInt aReqIpvType,
                                 TVpnFilterType aVpnFilterType )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::ConstructL" ) ) );
    
    ConstructL( aDb, aIspFilter, aBearerFilter, aSortType, 
                aReqIpvType, aVpnFilterType, EFalse );

    CLOG( ( ESelect, 1, _L( "<- CApSelect::ConstructL" ) ) );
    }





// ---------------------------------------------------------
// CApSelect::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::ConstructL( CCommsDatabase& aDb,
                                 TInt aIspFilter,
                                 TInt aBearerFilter,
                                 TInt aSortType,
                                 TInt aReqIpvType,
                                 TVpnFilterType aVpnFilterType,
                                 TBool aIncludeEasyWlan )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::ConstructL" ) ) );
    
    iExt = new ( ELeave )TSelectExtra;
    iExt->iExtra = 0;
    iExt->iIsIpv6Supported = EFalse;
    iExt->iMaxIndex = 0;
    iExt->iReqIpvType = EIPv4;
    iExt->iIsFeatureManagerInitialised = EFalse;
    iExt->iCdma = EFalse;
    iExt->iWlanSupported = EFalse;
    iExt->iIncludeEasyWlan = aIncludeEasyWlan;

    FeatureManager::InitializeLibL();
    iExt->iIsFeatureManagerInitialised = ETrue;

    iApList = new( ELeave )CApListItemList();
    iExt->iIsIpv6Supported = 
            FeatureManager::FeatureSupported( KFeatureIdIPv6 );
#ifdef __TEST_IPV6_SUPPORT    
    iExt->iIsIpv6Supported = ETrue;
#endif //  __TEST_IPV6_SUPPORT    
    iExt->iCdma = FeatureManager::FeatureSupported( KFeatureIdProtocolCdma );
#ifdef __TEST_CDMA_SUPPORT
    iExt->iCdma = ETrue;
#endif // __TEST_CDMA_SUPPORT

    iExt->iWlanSupported = 
            FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
#ifdef __TEST_WLAN_SUPPORT
    iExt->iWlanSupported = ETrue;
#endif // __TEST_WLAN_SUPPORT
    
    iDb = &aDb;

    SetFiltersL( aIspFilter, aBearerFilter, aSortType, 
                 aReqIpvType, aVpnFilterType );

    CLOG( ( ESelect, 1, _L( "<- CApSelect::ConstructL" ) ) );
    }



// ---------------------------------------------------------
// CApSelect::SetFiltersL
// ---------------------------------------------------------
//
void CApSelect::SetFiltersL( TInt aIspFilter, TInt aBearerFilter,
                             TInt aSortType, TInt aReqIpvType,
                             TInt aVpnFilterType )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::SetFilters" ) ) );

    TInt lcode( KErrNone );
    // as bearer filter might be EApBearerTypeAllBearers, 
    // there is no point to check bearer filter validity
    // but we can still check ISP...
    if ( aIspFilter >= 2*KEApIspTypeMMSMandatory )
        {
        lcode = KErrInvalidIspRequest;
        }

    User::LeaveIfError( lcode );

    if ( !( ( aSortType == KEApSortUidAscending )
        || ( aSortType == KEApSortUidDescending )
        || ( aSortType == KEApSortNameAscending )
        || ( aSortType == KEApSortNameDescending ) )
        )
        {
        // no sorting order specified,
        // assuming KEApSortUidAscending
        aSortType = KEApSortUidAscending;
        }

    if ( !( aBearerFilter & ( EApBearerTypeCSD
                        + EApBearerTypeCDMA
                        + EApBearerTypeGPRS
                        + EApBearerTypeHSCSD
                        + EApBearerTypeWLAN
                    #ifdef __TEST_LAN_BEARER
                        + EApBearerTypeLAN
                    #endif // __TEST_LAN_BEARER
                        + EApBearerTypeLANModem
                    // Check bearertype all case!!!
                        + EApBearerTypeAll ) )
        )
        {
        // no bearer type specified,
        // assuming EApBearerTypeAll
        if ( aBearerFilter != EApBearerTypeAllBearers )
            {
            // unknown bearer, Leave
	        User::Leave( KErrInvalidBearerType );            
            }
        }

    if ( !( aIspFilter & ( KEApIspTypeAll
                         + KEApIspTypeInternetAndWAP
                         + KEApIspTypeInternetOnly
                         + KEApIspTypeWAPOnly
                         + KEApIspTypeMMSMandatory ) )
        )
        {
        // erroneous use, no ISP type specified,
        // or only KEApIspTypeWAPMandatory,
        // both cases assuming KEApIspTypeAll
        aIspFilter += KEApIspTypeAll;
        }

    iIsp = aIspFilter;
    iSort = aSortType;

    if ( aBearerFilter & EApBearerTypeAll )
        {
        aBearerFilter = EApBearerTypeAllBearers;
        }

	iBearer = GetSupportedBearerSet( aBearerFilter );
	
	if ( !iBearer )
	    {
	    User::Leave( KErrInvalidBearerType );
	    }

    iExt->iReqIpvType = aReqIpvType;

    iExt->iVpnFilterType = aVpnFilterType;

    DoUpdateL();

    if ( iSort == KEApSortNameAscending )
        {
        SortLocalisedNameL( ETrue ); // ascending
        }
    else
        {
        if ( iSort == KEApSortNameDescending )
            {
            SortLocalisedNameL( EFalse ); // descending
            }
        else
            {
            if ( iSort == KEApSortUidAscending )
                {
                SortUidL( ETrue );
                }
            else
                {
                if ( iSort == KEApSortUidDescending )
                    {
                    SortUidL( EFalse );
                    }
                }
            }
        }

    CLOG( ( ESelect, 1, _L( "<- CApSelect::SetFilters" ) ) );
    }



// ---------------------------------------------------------
// CApSelect::GetVpnTableViewLC()
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetVpnTableViewLC( )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetVpnTableViewLC" ) ) );

    CCommsDbTableView* table = NULL;
    table = iDb->OpenTableLC( TPtrC(VPN_SERVICE) );
    
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetVpnTableViewLC" ) ) );

    return table;
    }



// ---------------------------------------------------------
// CApSelect::GetVpnValuesL
// ---------------------------------------------------------
//
void CApSelect::GetVpnValuesL( CCommsDbTableView* aTable,
                               CArrayFixFlat<TVpnData>* aVpnArray )
    { // get the VPN values
    CLOG( ( ESelect, 0, _L( "-> CApSelect::GetVpnValuesL" ) ) );

    if ( aTable )
        {
        TInt err = aTable->GotoFirstRecord();
        if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            {
            User::Leave( err );
            }
        if ( err != KErrNotFound )
            {
            TUint32 tempvpn( 0 );
            TUint32 temphome( 0 );
            HBufC*  tempname = NULL;
            TVpnData data;
            TBool goon( ETrue );
            do
                {
                err = ApCommons::ReadUintL( aTable, TPtrC(COMMDB_ID), 
                                            tempvpn );
                if ( ( err == KErrNone ) && ( tempvpn ) )
                    {
                    err = ApCommons::ReadUintL( aTable, 
                                                TPtrC(VPN_SERVICE_IAP),
                                                temphome );
                    if ( ( err == KErrNone ) && ( temphome ) )
                        {
                        // now get the name

                        tempname = 
                            ApCommons::ReadText16ValueLC( aTable, 
                                                          TPtrC(COMMDB_NAME) );
                        
                        // everything is O.K., we can add the item
                        data.iHomeIapId = temphome;
                        data.iVpnIapId = tempvpn;
                        data.iName = tempname;
                        aVpnArray->AppendL( data ); // array owns it...
                        }
                    // Only VPN iaps which point to a real IAP are added to list.
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                err = aTable->GotoNextRecord();
                if ( err == KErrNotFound )
                    {
                    goon = EFalse;
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                }
                while ( goon );
            }
        }
    CLOG( ( ESelect, 1, _L( "<- CApSelect::GetVpnValuesL" ) ) );

    }





// ---------------------------------------------------------
// CApSelect::AddLanL
// ---------------------------------------------------------
//
void CApSelect::AddLanL( TBool aWlan )
    {
    CLOG( ( ESelect, 0, _L( "-> CApSelect::AddLanL" ) ) );

    // First get the list of all VPN-s
    // get their WAP-ID, IAPID,  
    // Also ask their RealIApID, bearertype and filter for it...

    CCommsDbTableView* lant = NULL;

    lant = iDb->OpenViewMatchingTextLC( TPtrC(IAP), 
                                        TPtrC(IAP_SERVICE_TYPE), 
                                        TPtrC(LAN_SERVICE) );
    TInt err = lant->GotoFirstRecord();
    if ( err != KErrNotFound)
        {
        User::LeaveIfError( err );

        CArrayFixFlat<TUint32>* laniapid =
                new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
        CleanupStack::PushL( laniapid );
        GetUintValuesL( lant, TPtrC(COMMDB_ID), *laniapid );

        CArrayFixFlat<TUint32>* lanserviceid =
                new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
        CleanupStack::PushL( lanserviceid );
        GetUintValuesL( lant, TPtrC(IAP_SERVICE), *lanserviceid );

        CArrayFixFlat<TUint32>* iapbearerid =
                new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
        CleanupStack::PushL( iapbearerid );
        GetUintValuesL( lant, TPtrC(IAP_BEARER), *iapbearerid );

        // read up access point names, too, just to be as fast as we can...
        CArrayPtrFlat<HBufC>* lanapname =
                new ( ELeave ) CArrayPtrFlat<HBufC>( 10 );
        CleanupStack::PushL( lanapname );
        GetTextValuesL( lant, TPtrC(COMMDB_NAME), *lanapname );

        // now get WAP ID's
//        CArrayFixFlat<TUint32>* lanwapapid =
//                new ( ELeave ) CArrayFixFlat<TUint32>( 10 );
//        CleanupStack::PushL( lanwapapid );
        CApUtils* utils = CApUtils::NewLC( *iDb );

        TUint32 count( laniapid->Count() );
        TUint32 currlaniapid;
        TUint32 currwapapid( 0 );
        CApListItem* item = NULL;

        for ( TUint32 i = 0; i < count; i++)
            {
            currlaniapid = laniapid->At( i ); 
            TRAP( err, currwapapid = utils->WapIdFromIapIdL( currlaniapid ));
            if ( !err )
                { // get protection for the given record
                TInt prot( EFalse );
                CCommsDbTableView* wt = NULL;

                wt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT), 
                                                TPtrC(COMMDB_ID), 
                                                currwapapid );
                CCommsDbProtectTableView* protview =
                    (CCommsDbProtectTableView*)wt;
                                                
                User::LeaveIfError( wt->GotoFirstRecord() );
                err = protview->GetRecordAccess( prot );
                HBufC* wspage = wt->ReadLongTextLC( TPtrC(WAP_START_PAGE) );
                if ( aWlan )
                    {
                    if ( iExt->iWlanSupported )
                        {
                        // check if it is a WLAN, add ONLY if it IS
                        TUint32 lanid = lanserviceid->At( i );
                        TBool iswlan( EFalse );
                        TRAP( err, iswlan = utils->IsWlanL( lanid ) );
                        if ( !err && iswlan )
                            {
                            TBool hasssid( EFalse );
                            err = KErrNone;
                            TRAP( err, 
                                   hasssid = utils->HasWlanSSIDL( lanid ));
                            if ( iExt->iIncludeEasyWlan )
                                {
                                hasssid = ETrue;
                                }
                            if ( (err == KErrNone) && hasssid )
                                {                                
                                item = CApListItem::NewLC( 
                                       EIspTypeInternetAndWAP,
                                       currwapapid,
                                       *lanapname->At( i ),
                                       EApBearerTypeWLAN,
                                       *wspage,
                                       prot );
                                iApList->AppendL( item );
                                CleanupStack::Pop( item ); // owned by list
                                }
                            }
                        }
                    }
                else
                    {
                    // first check if it is a WLAN, add ONLY if it IS NOT
                    // then check if it is LANModem, 
                    // then check for LAN, if supported
                    // check if it is a WLAN, add ONLY if it IS NOT
                    TUint32 lanid = lanserviceid->At( i );
                    TBool iswlan( EFalse );
                    TRAP( err, iswlan = utils->IsWlanL( lanid ) );                    
                    if ( !err && !iswlan )
                        {
                        // then check if it is LANModem, 
                        CCommsDbTableView* lanbearer;
                        lanbearer = 
                            iDb->OpenViewMatchingUintLC( 
                                        TPtrC(LAN_BEARER),
                                        TPtrC(COMMDB_ID), 
                                        iapbearerid->At(i ) );
                        User::LeaveIfError( 
                                lanbearer->GotoFirstRecord() );
                        // LANMODEM? check commdb_name
                        HBufC* lbuf;
                        lbuf = ApCommons::ReadText16ValueLC( lanbearer,
                                                     TPtrC(COMMDB_NAME) );
                        // if it is KModemBearerLanModem, it is LANModem
                        if ( *lbuf == KModemBearerLANModem )
                            {
                            item = CApListItem::NewLC( EIspTypeInternetAndWAP,
                                                       currwapapid,
                                                       *lanapname->At( i ),
                                                       EApBearerTypeLANModem );
                            iApList->AppendL( item );
                            CleanupStack::Pop( item ); // item owned by list !
                            }
                        else
                            {

#ifdef __TEST_LAN_BEARER
                            // Check for sure it is LAN
                            // we now it is not WLAN, say it is LAN
                            item = CApListItem::NewLC( EIspTypeInternetAndWAP,
                                                       currwapapid,
                                                       *lanapname->At( i ),
                                                       EApBearerTypeLAN,
                                                       *wspage,
                                                       prot );
                            iApList->AppendL( item );
                            CleanupStack::Pop( item ); // item owned by list !
#endif // __TEST_LAN_BEARER
                            }
                        CleanupStack::PopAndDestroy( lbuf );
                        CleanupStack::PopAndDestroy( lanbearer );
                        }
                    }
                CleanupStack::PopAndDestroy( wspage );
                CleanupStack::PopAndDestroy( wt );
                }
            }

        iCount = iApList->Count();
        iExt->iMaxIndex = iCount-1;

        CleanupStack::PopAndDestroy( utils );
//        CleanupStack::PopAndDestroy( lanwapapid );
        // ResetAndDestroy the array:
        CleanupStack::PopAndDestroy( lanapname->Count() ); // lanapname
        CleanupStack::PopAndDestroy( lanapname ); // lanapname
		CleanupStack::PopAndDestroy( iapbearerid );        
        CleanupStack::PopAndDestroy( lanserviceid );
        CleanupStack::PopAndDestroy( laniapid );
        }

    CleanupStack::PopAndDestroy( lant );
    CLOG( ( ESelect, 1, _L( "<- CApSelect::AddLanL" ) ) );
    }


// ---------------------------------------------------------
// CApSelect::GetSupportedBearerSet
// ---------------------------------------------------------
//
TInt CApSelect::GetSupportedBearerSet( TInt aBearerFilter )
    {
    TInt retval = aBearerFilter;

    TBool isCsdSupported = 
          FeatureManager::FeatureSupported( KFeatureIdAppCsdSupport );
#ifdef __TEST_CSD_SUPPORT
    isCsdSupported = ETrue;
#endif // __TEST_CSD_SUPPORT


#ifdef __TEST_USE_SHARED_DATA
    TInt isHscsdSupported = ETrue;
        /*
        ApCommons::IsGivenSharedDataSupportL( KGeneralSettingsUid, 
                                              KGSHSCSDAccessPoints );
                                             */
#else
    TInt isHscsdSupported = ETrue;
#endif // __TEST_USE_SHARED_DATA

#ifdef __TEST_HSCSD_SUPPORT
    isHscsdSupported = ETrue;
#endif // __TEST_HSCSD_SUPPORT

    if ( !isCsdSupported )
        { // remove CSD, HSCSD, do not allow ALL
        if ( aBearerFilter & EApBearerTypeAll )
            {
            retval &= ~EApBearerTypeCSD;
            retval &= ~EApBearerTypeHSCSD;            
            }
        else
            {
            if ( aBearerFilter & EApBearerTypeCSD )
                {
                retval &= ~EApBearerTypeCSD;
                }
            if ( aBearerFilter & EApBearerTypeHSCSD )
                {
                retval &= ~EApBearerTypeHSCSD;
                }
            }
        }

    if ( !isHscsdSupported )
        { // HSCSD
        if ( ( aBearerFilter & EApBearerTypeAll )
        	 || ( aBearerFilter & EApBearerTypeHSCSD ) )
            {
			retval &= ~EApBearerTypeHSCSD;
            }
        }
    return retval;
    }


// End of File
