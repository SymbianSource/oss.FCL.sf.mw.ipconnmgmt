/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  
 * Private implementation of Wrapper for CM Manager Symbian interface.
 */

// System includes

#include <QList>
#include <e32base.h>
#include <cmdestination.h>
#include <cmdestination_shim.h>
#include <cmconnectionmethod_shim.h>

// User includes

#include "cmmanager_shim_s60_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmanager_shim_s60Traces.h"
#endif


/*!
    \class CmManagerShimPrivate
    \brief Private implementation class for CM Manager Shim.

    Wrapper for CM Manager Symbian interface. Symbian leaves are converted to
    standard C++ exceptions.
    
    Wrapper functions are identical to those in CmManagerShim class, so
    refer to cmdestination_shim.h for descriptions. Private functions in this
    class are just leaving versions of the same wrapper functions.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
 *   Constructor.
 */

CmManagerShimPrivate::CmManagerShimPrivate()
{
    OstTraceFunctionEntry0(CMMANAGERSHIMPRIVATE_CMMANAGERSHIMPRIVATE_ENTRY);
    
    TRAPD(error, iCmManager.OpenL());
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMMANAGERSHIMPRIVATE_CMMANAGERSHIMPRIVATE_EXIT);
}

/*!
 *   Destructor.
 */

CmManagerShimPrivate::~CmManagerShimPrivate()
{
    OstTraceFunctionEntry0(DUP1_CMMANAGERSHIMPRIVATE_CMMANAGERSHIMPRIVATE_ENTRY);
    
    iCmManager.Close();
    
    OstTraceFunctionExit0(DUP1_CMMANAGERSHIMPRIVATE_CMMANAGERSHIMPRIVATE_EXIT);
}

CmDestinationShim *CmManagerShimPrivate::CreateDestination(QString name)
{
    OstTraceFunctionEntry0(CMMANAGERSHIMPRIVATE_CREATEDESTINATION_ENTRY);
    
    RCmDestination dest;
    TRAPD(
        error,
        TPtrC16 namePtr(reinterpret_cast<const TUint16*>(name.utf16()));
        dest = iCmManager.CreateDestinationL(namePtr);
        dest.UpdateL());
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMMANAGERSHIMPRIVATE_CREATEDESTINATION_EXIT);
    return new CmDestinationShim(dest);
}
    
CmConnectionMethodShim *CmManagerShimPrivate::CreateConnectionMethod(
    uint bearerType)
{
    OstTraceFunctionEntry0(CMMANAGERSHIMPRIVATE_CREATECONNECTIONMETHOD_ENTRY);
    
    RCmConnectionMethod connectionMethod;
    TRAPD(error, connectionMethod = iCmManager.CreateConnectionMethodL(bearerType));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMMANAGERSHIMPRIVATE_CREATECONNECTIONMETHOD_EXIT);
    return new CmConnectionMethodShim(connectionMethod);
}

CmConnectionMethodShim *CmManagerShimPrivate::ConnectionMethod(
    uint connectionMethodId) const
{
    OstTraceFunctionEntry0(CMMANAGERSHIMPRIVATE_CONNECTIONMETHOD_ENTRY);
    
    RCmConnectionMethod connectionMethod;
    TRAPD(error, connectionMethod = iCmManager.ConnectionMethodL(connectionMethodId));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMMANAGERSHIMPRIVATE_CONNECTIONMETHOD_EXIT);
    return new CmConnectionMethodShim(connectionMethod);
}
    
void CmManagerShimPrivate::ConnectionMethod(
    QList<uint> &cmArray,
    bool legacyOnly) const
{
    OstTraceFunctionEntry0(DUP1_CMMANAGERSHIMPRIVATE_CONNECTIONMETHOD_ENTRY);
    
    TRAPD(error, ConnectionMethodL(cmArray, legacyOnly));
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(DUP1_CMMANAGERSHIMPRIVATE_CONNECTIONMETHOD_EXIT);
}
    
CmDestinationShim *CmManagerShimPrivate::Destination(
    uint destinationId) const
{
    OstTraceFunctionEntry0(CMMANAGERSHIMPRIVATE_DESTINATION_ENTRY);
    
    RCmDestination dest;
    TRAPD(error, dest = iCmManager.DestinationL(destinationId));
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMMANAGERSHIMPRIVATE_DESTINATION_EXIT);
    return new CmDestinationShim(dest);
}
            
void CmManagerShimPrivate::AllDestinations(QList<uint> &destArray) const
{
    OstTraceFunctionEntry0(CMMANAGERSHIMPRIVATE_ALLDESTINATIONS_ENTRY);
    
    TRAPD(error, AllDestinationsL(destArray));
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMMANAGERSHIMPRIVATE_ALLDESTINATIONS_EXIT);
}

QString CmManagerShimPrivate::GetUncategorizedIcon() const
{    
    OstTraceFunctionEntry0(CMMANAGERSHIMPRIVATE_GETUNCATEGORIZEDICON_ENTRY);
    
    QString icon;
    TRAPD(error, GetUncategorizedIconL(icon));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMMANAGERSHIMPRIVATE_GETUNCATEGORIZEDICON_EXIT);
    return icon;
}

void CmManagerShimPrivate::ConnectionMethodL(
    QList<uint> &cmArray,
    bool legacyOnly) const
{
    RArray<TUint32> array;
    iCmManager.ConnectionMethodL(
        array,
        true, // Check bearer type
        legacyOnly,
        false); // No Easy WLAN
    for (TInt i = 0; i < array.Count(); i++) {
        cmArray.append(array[i]);
    }
    // Free memory
    array.Reset();
}
    
void CmManagerShimPrivate::AllDestinationsL(QList<uint> &destArray) const
{
    destArray.clear();
    RArray<TUint32> destinationIdArray;

    destinationIdArray.Reset();
    iCmManager.AllDestinationsL(destinationIdArray);

    for (TInt i = 0; i < destinationIdArray.Count(); i++) {
        destArray.append(destinationIdArray[i]);
    }
    // Free memory
    destinationIdArray.Reset();
}

void CmManagerShimPrivate::GetUncategorizedIconL(QString &icon) const
{
    HBufC* iconName = iCmManager.GetUncategorizedIconL();
    icon = QString::fromUtf16(iconName->Ptr(), iconName->Length());
    delete iconName;
}

/*!
 * Traces given error code if it is not KErrNone. 
 * 
 * @param[in] error Symbian error code.
 */

void CmManagerShimPrivate::TraceIfError(TInt error) const
{
    if (error != KErrNone) {
        OstTrace1(
            TRACE_WARNING,
            CMMANAGERSHIMPRIVATE_TRACEIFERROR,
            "CmManagerShimPrivate::TraceIfError;Error code=%d",
            error);
    }
}
