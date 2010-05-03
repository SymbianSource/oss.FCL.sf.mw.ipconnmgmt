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

#include <cmdestination.h>
#include <cmmanager.h>
#include <cmconnectionmethod.h>
#include <cmdestination_shim.h>
#include <cmconnectionmethod_shim.h>

// User includes

#include "cmdestination_shim_s60_p.h"
#include "cmconnectionmethod_shim_s60_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmdestination_shim_s60Traces.h"
#endif


/*!
    \class CmDestinationShimPrivate
    \brief Private implementation class for CM Manager Destination Shim.

    Wrapper for CM Manager Symbian interface. Symbian leaves are converted to
    standard C++ exceptions.
    
    Wrapper functions are identical to those in CmDestinationShim class, so
    refer to cmdestination_shim.h for descriptions. Private functions in this
    class are just leaving versions of the same wrapper functions.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
 *  Constructor.
 *  
 *  @param[in] destination CM Manager destination object.
 */

CmDestinationShimPrivate::CmDestinationShimPrivate(
    RCmDestination &destination) :
    iDestination(destination)
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_CMDESTINATIONSHIMPRIVATE_ENTRY);
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_CMDESTINATIONSHIMPRIVATE_EXIT);
}

/*!
 *  Destructor.
 */

CmDestinationShimPrivate::~CmDestinationShimPrivate()
{
    OstTraceFunctionEntry0(DUP1_CMDESTINATIONSHIMPRIVATE_CMDESTINATIONSHIMPRIVATE_ENTRY);
    
    iDestination.Close();

    OstTraceFunctionExit0(DUP1_CMDESTINATIONSHIMPRIVATE_CMDESTINATIONSHIMPRIVATE_EXIT);
}
                 
int CmDestinationShimPrivate::ConnectionMethodCount() const
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_CONNECTIONMETHODCOUNT_ENTRY);
    
    int count = iDestination.ConnectionMethodCount();
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_CONNECTIONMETHODCOUNT_EXIT);
    return count;
}

CmConnectionMethodShim *CmDestinationShimPrivate::ConnectionMethod(
    int index) const
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_CONNECTIONMETHOD_ENTRY);
    
    RCmConnectionMethod cm;
    TRAPD(error, cm = iDestination.ConnectionMethodL(index));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_CONNECTIONMETHOD_EXIT);
    return new CmConnectionMethodShim(cm);
}

CmConnectionMethodShim *CmDestinationShimPrivate::ConnectionMethodByID(
    uint cmId) const
{    
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_CONNECTIONMETHODBYID_ENTRY);
    
    RCmConnectionMethod cm;
    TRAPD(error, cm = iDestination.ConnectionMethodByIDL(cmId));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_CONNECTIONMETHODBYID_EXIT);
    return new CmConnectionMethodShim(cm);
}

uint CmDestinationShimPrivate::Priority(CmConnectionMethodShim *cmItem) const
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_PRIORITY_ENTRY);
    
    uint priority;
    TRAPD(error, PriorityL(cmItem, priority));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_PRIORITY_EXIT);
    return priority;
}

QString CmDestinationShimPrivate::Name() const
{    
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_NAME_ENTRY);
    
    QString name;
    TRAPD(error, NameL(name));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_NAME_EXIT);
    return name;
}

uint CmDestinationShimPrivate::Id() const
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_ID_ENTRY);
    
    uint id = iDestination.Id();
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_ID_EXIT);
    return id;
}

uint CmDestinationShimPrivate::Metadata(
    CMManagerShim::SnapMetadataField metadataField) const
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_METADATA_ENTRY);
    
    uint metadata = 0;
    TRAPD(
        error,
        metadata = iDestination.MetadataL((CMManager::TSnapMetadataField)metadataField));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_METADATA_EXIT);
    return metadata;
}

CMManagerShim::CmmProtectionLevel CmDestinationShimPrivate::ProtectionLevel() const
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_PROTECTIONLEVEL_ENTRY);
    
    CMManager::TProtectionLevel protectionlvl = CMManager::EProtLevel3;
    TRAPD(error, protectionlvl = iDestination.ProtectionLevel());
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_PROTECTIONLEVEL_EXIT);
    return mapCmmProtectionLevel(protectionlvl);
}

bool CmDestinationShimPrivate::IsHidden() const
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_ISHIDDEN_ENTRY);
    
    bool hidden = iDestination.IsHidden(); 
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_ISHIDDEN_EXIT);
    return hidden;
}

int CmDestinationShimPrivate::AddConnectionMethod(
    CmConnectionMethodShim *cm)
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_ADDCONNECTIONMETHOD_ENTRY);
    
    int index;
    TRAPD(error, AddConnectionMethodL(cm, index));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_ADDCONNECTIONMETHOD_EXIT);
    return index;
}

void CmDestinationShimPrivate::DeleteConnectionMethod(
    CmConnectionMethodShim *cm)
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_DELETECONNECTIONMETHOD_ENTRY);
    
    TRAPD(error, DeleteConnectionMethodL(cm));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_DELETECONNECTIONMETHOD_EXIT);
}

void CmDestinationShimPrivate::RemoveConnectionMethod(
    CmConnectionMethodShim *cm)
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_REMOVECONNECTIONMETHOD_ENTRY);
    
    TRAPD(error, RemoveConnectionMethodL(cm));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_REMOVECONNECTIONMETHOD_EXIT);
}

void CmDestinationShimPrivate::ModifyPriority(
    CmConnectionMethodShim *cm,
    int index)
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_MODIFYPRIORITY_ENTRY);
    
    TRAPD(error, ModifyPriorityL(cm, index));
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_MODIFYPRIORITY_EXIT);
}

void CmDestinationShimPrivate::SetName(QString name)
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_SETNAME_ENTRY);
    
    TRAPD(error, SetNameL(name));
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_SETNAME_EXIT);
}

void CmDestinationShimPrivate::Update()
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_UPDATE_ENTRY);
    
    TRAPD(error, iDestination.UpdateL());
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_UPDATE_EXIT);
}

void CmDestinationShimPrivate::Refresh()
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_REFRESH_ENTRY);
    
    TRAPD(error, iDestination.RefreshL());
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_REFRESH_EXIT);
}

void CmDestinationShimPrivate::DeleteDestination()
{
    OstTraceFunctionEntry0(CMDESTINATIONSHIMPRIVATE_DELETEDESTINATION_ENTRY);
    
    TRAPD(error, iDestination.DeleteLD());
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMDESTINATIONSHIMPRIVATE_DELETEDESTINATION_EXIT);
}

void CmDestinationShimPrivate::PriorityL(
    CmConnectionMethodShim *cmShim,
    uint &priority) const
{
    RCmConnectionMethod cm;
    cm = iDestination.ConnectionMethodByIDL(
        cmShim->getIntAttribute(CMManagerShim::CmId));
    priority = iDestination.PriorityL(cm);
}
    
void CmDestinationShimPrivate::NameL(QString &name) const
{
    HBufC* iapName;
    iapName = iDestination.NameLC();
    name = QString::fromUtf16(iapName->Ptr(), iapName->Length());
    CleanupStack::PopAndDestroy(); // iapName
}
           
void CmDestinationShimPrivate::AddConnectionMethodL(
    CmConnectionMethodShim *cmShim,
    int &index)
{
    index = iDestination.AddConnectionMethodL(cmShim->d_ptr->iCm);
    iDestination.UpdateL();
}
       
void CmDestinationShimPrivate::DeleteConnectionMethodL(
    CmConnectionMethodShim *cmShim)
{
    RCmConnectionMethod cm;
    cm = iDestination.ConnectionMethodByIDL(
        cmShim->getIntAttribute(CMManagerShim::CmId));
    iDestination.DeleteConnectionMethodL(cm);
    iDestination.UpdateL();
}
       
void CmDestinationShimPrivate::RemoveConnectionMethodL(
    CmConnectionMethodShim *cmShim)
{
    RCmConnectionMethod cm;
    cm = iDestination.ConnectionMethodByIDL(
        cmShim->getIntAttribute(CMManagerShim::CmId));
    iDestination.RemoveConnectionMethodL(cm);
    iDestination.UpdateL();
}
             
void CmDestinationShimPrivate::ModifyPriorityL(
    CmConnectionMethodShim *cmShim,
    int index)
{
    RCmConnectionMethod cm;
    cm = iDestination.ConnectionMethodByIDL(
        cmShim->getIntAttribute(CMManagerShim::CmId));
    iDestination.ModifyPriorityL(cm, index);
    iDestination.UpdateL();
}
           
void CmDestinationShimPrivate::SetNameL(QString name)
{
    TPtrC16 namePtr(reinterpret_cast<const TUint16*>(name.utf16()));
    iDestination.SetNameL(namePtr);
    iDestination.UpdateL();
}

/*!
 * Maps CM Manager protection levels to those defined by the shim.
 * 
 * @param[in] protectionlvl CM Manager protection level.
 * @return CM Manager Shim protection level.
 */

CMManagerShim::CmmProtectionLevel CmDestinationShimPrivate::mapCmmProtectionLevel( 
    CMManager::TProtectionLevel protectionlvl) const
{
    CMManagerShim::CmmProtectionLevel retval = CMManagerShim::ProtLevel3;
    
    switch (protectionlvl) {
    case CMManager::EProtLevel0:
        retval = CMManagerShim::ProtLevel0;
        break;
        
    case CMManager::EProtLevel1:
        retval = CMManagerShim::ProtLevel1;
        break;
        
    case CMManager::EProtLevel2:
        retval = CMManagerShim::ProtLevel2;
        break;
        
    case CMManager::EProtLevel3:
        retval = CMManagerShim::ProtLevel3;
        break;

#ifndef QT_NO_DEBUG
    default:
        Q_ASSERT( 0 );
        break;
#endif            
    }
    
    return retval;
}

/*!
 * Traces given error code if it is not KErrNone. 
 * 
 * @param[in] error Symbian error code.
 */

void CmDestinationShimPrivate::TraceIfError(TInt error) const
{
    if (error != KErrNone) {
        OstTrace1(
            TRACE_WARNING,
            CMDESTINATIONSHIMPRIVATE_TRACEIFERROR,
            "CmDestinationShimPrivate::TraceIfError;Error code=%d",
            error);
    }
}
