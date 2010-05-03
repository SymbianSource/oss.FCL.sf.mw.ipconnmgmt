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

#include <utf.h>

// User includes

#include "cmconnectionmethod_shim_s60_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmconnectionmethod_shim_s60Traces.h"
#endif


/*!
    \class CmConnectionMethodShimPrivate
    \brief Private implementation class for CM Manager Connection Method Shim.

    Wrapper for CM Manager Symbian interface. Symbian leaves are converted to
    standard C++ exceptions.
    
    Wrapper functions are identical to those in CmConnectionMethodShim class,
    so refer to cmconnectionmethod_shim.h for descriptions. Private functions
    in this class are just leaving versions of the same wrapper functions.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
 *  Constructor.
 *  
 *  @param[in] cm CM Manager connection method object.
 */

CmConnectionMethodShimPrivate::CmConnectionMethodShimPrivate(
    RCmConnectionMethod &cm) :
    iCm(cm)
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_CMCONNECTIONMETHODSHIMPRIVATE_ENTRY);
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_CMCONNECTIONMETHODSHIMPRIVATE_EXIT);
}

/*!
 *  Destructor.
 */

CmConnectionMethodShimPrivate::~CmConnectionMethodShimPrivate()
{
    OstTraceFunctionEntry0(DUP1_CMCONNECTIONMETHODSHIMPRIVATE_CMCONNECTIONMETHODSHIMPRIVATE_ENTRY);
    
    iCm.Close();

    OstTraceFunctionExit0(DUP1_CMCONNECTIONMETHODSHIMPRIVATE_CMCONNECTIONMETHODSHIMPRIVATE_EXIT);
}
   
uint CmConnectionMethodShimPrivate::GetIntAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_GETINTATTRIBUTE_ENTRY);
    
    uint value = 0;
    TRAPD(error, value = iCm.GetIntAttributeL(attribute));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_GETINTATTRIBUTE_EXIT);
    return value;
}

bool CmConnectionMethodShimPrivate::GetBoolAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_GETBOOLATTRIBUTE_ENTRY);
    
    bool value = false;
    TRAPD(error, value = iCm.GetBoolAttributeL(attribute));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_GETBOOLATTRIBUTE_EXIT);
    return value;
}

QString CmConnectionMethodShimPrivate::GetStringAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_GETSTRINGATTRIBUTE_ENTRY);
    
    QString value;
    TRAPD(error, GetStringAttributeL(attribute, value));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_GETSTRINGATTRIBUTE_EXIT);
    return value;
}

QString CmConnectionMethodShimPrivate::GetString8Attribute( 
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_GETSTRING8ATTRIBUTE_ENTRY);
    
    QString value;
    TRAPD(error, GetString8AttributeL(attribute, value));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_GETSTRING8ATTRIBUTE_EXIT);
    return value;
}

void CmConnectionMethodShimPrivate::SetIntAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute,
    uint value)
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_SETINTATTRIBUTE_ENTRY);
    
    TRAPD(error, SetIntAttributeL(attribute, value));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_SETINTATTRIBUTE_EXIT);
}

void CmConnectionMethodShimPrivate::SetBoolAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute,
    bool value)
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_SETBOOLATTRIBUTE_ENTRY);
    
    TRAPD(error, SetBoolAttributeL(attribute, value));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_SETBOOLATTRIBUTE_EXIT);
}

void CmConnectionMethodShimPrivate::SetStringAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute,
    QString value)
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_SETSTRINGATTRIBUTE_ENTRY);
    
    TRAPD(error, SetStringAttributeL(attribute, value));
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_SETSTRINGATTRIBUTE_EXIT);
}

void CmConnectionMethodShimPrivate::SetString8Attribute( 
    CMManagerShim::ConnectionMethodAttribute attribute,
    QString value)
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_SETSTRING8ATTRIBUTE_ENTRY);
    
    TRAPD(error, SetString8AttributeL(attribute, value));
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_SETSTRING8ATTRIBUTE_EXIT);
}

bool CmConnectionMethodShimPrivate::DeleteConnectionMethod()
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_DELETECONNECTIONMETHOD_ENTRY);
    
    bool value = false;
    TRAPD(error, value = iCm.DeleteL());
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_DELETECONNECTIONMETHOD_EXIT);
    return value;
}
     
void CmConnectionMethodShimPrivate::Update()
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_UPDATE_ENTRY);
    
    TRAPD(error, iCm.UpdateL());
    TraceIfError(error);
    qt_symbian_throwIfError(error);

    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_UPDATE_EXIT);
}

void CmConnectionMethodShimPrivate::Refresh()
{
    OstTraceFunctionEntry0(CMCONNECTIONMETHODSHIMPRIVATE_REFRESH_ENTRY);
    
    TRAPD(error, iCm.RefreshL());
    TraceIfError(error);
    qt_symbian_throwIfError(error);
    
    OstTraceFunctionExit0(CMCONNECTIONMETHODSHIMPRIVATE_REFRESH_EXIT);
}

void CmConnectionMethodShimPrivate::GetStringAttributeL( 
    CMManagerShim::ConnectionMethodAttribute attribute,
    QString &value) const
{
    HBufC* buf = iCm.GetStringAttributeL(attribute);
    value = QString::fromUtf16(buf->Ptr(), buf->Length());
    delete buf;
}

void CmConnectionMethodShimPrivate::GetString8AttributeL( 
    CMManagerShim::ConnectionMethodAttribute attribute,
    QString &value) const
{
    HBufC8* buf = iCm.GetString8AttributeL(attribute);
    value = QString::fromLatin1((const char*)buf->Ptr(), buf->Length());
    delete buf;
}

void CmConnectionMethodShimPrivate::SetIntAttributeL( 
    CMManagerShim::ConnectionMethodAttribute attribute,
    uint value)
{
    iCm.SetIntAttributeL(attribute, value);
}

void CmConnectionMethodShimPrivate::SetBoolAttributeL( 
    CMManagerShim::ConnectionMethodAttribute attribute,
    bool value)
{
    iCm.SetBoolAttributeL(attribute, value);
}

void CmConnectionMethodShimPrivate::SetStringAttributeL(
    CMManagerShim::ConnectionMethodAttribute attribute,
    QString value)
{
    TPtrC16 valuePtr(reinterpret_cast<const TUint16*>(value.utf16()));
    iCm.SetStringAttributeL(attribute, valuePtr);
}

void CmConnectionMethodShimPrivate::SetString8AttributeL(
    CMManagerShim::ConnectionMethodAttribute attribute,
    QString value)
{
    QByteArray tempValue = value.toLatin1();
    TPtrC8 valuePtr(reinterpret_cast<const unsigned char*>(tempValue.data()));
    iCm.SetString8AttributeL(attribute, valuePtr);
}

/*!
 * Traces given error code if it is not KErrNone. 
 * 
 * @param[in] error Symbian error code.
 */

void CmConnectionMethodShimPrivate::TraceIfError(TInt error) const
{
    if (error != KErrNone) {
        OstTrace1(
            TRACE_WARNING,
            CMCONNECTIONMETHODSHIMPRIVATE_TRACEIFERROR,
            "CmConnectionMethodShimPrivate::TraceIfError;Error code=%d",
            error);
    }
}
