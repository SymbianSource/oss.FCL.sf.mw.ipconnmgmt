/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CConnectionMonitorPlugin provides an interface for an engine
*                to provide bearer specific information to the Connection
*                Monitor server.
*
*/

#ifndef __CONNMONEXT_H
#define __CONNMONEXT_H

#include <e32base.h>
#include <rconnmon.h>

const TInt KPluginMajorVersionNumber=1;
const TInt KPluginMinorVersionNumber=1;
const TInt KPluginBuildVersionNumber=1;


class CPluginAO;

void Panic( TInt aPanic );

/**
 * MConnectionMonitorPluginInterface
 * MConnectionMonitorPluginInterface is an interface class that must be implemented
 * in order to serve the requests initiated by the Connection Monitor server.
 *
 * @lib CONNMONEXT.LIB
 */
class MConnectionMonitorPluginInterface
    {
public:
    /**
     * GetIntAttribute is called to get the value of a TInt attribute.
     * @param aAttribute Identifies the attribute to be retrived.
     * @param aValue On return, contains the requested TInt attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt GetIntAttribute( const TUint aAttribute, TInt& aValue ) =0;

    /**
     * GetUintAttribute is called to get the value of a TUint attribute.
     * @param aAttribute Identifies the attribute to be retrived.
     * @param aValue On return, contains the requested TUint attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt GetUintAttribute( const TUint aAttribute, TUint& aValue ) =0;

    /**
     * GetBoolAttribute is called to get the value of a TBool attribute.
     * @param aAttribute Identifies the attribute to be retrived.
     * @param aValue On return, contains the requested TBool attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt GetBoolAttribute( const TUint aAttribute, TBool& aValue ) =0;

    /**
     * GetStringAttribute is called to get the value of a TDes attribute.
     * @param aAttribute Identifies the attribute to be retrived.
     * @param aValue On return, contains the requested TDes attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt GetStringAttribute( const TUint aAttribute, TDes& aValue ) =0;

    /**
     * GetPckgAttribute is called to get the value of a packaged attribute.
     * @param aAttribute Identifies the attribute to be retrived.
     * @param aValue On return, contains the requested packaged attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt GetPckgAttribute( const TUint aAttribute, TDes8& aValue ) =0;

    /**
     * SetIntAttribute is called to set the value of a TInt attribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The new value for the TInt attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt SetIntAttribute( const TUint aAttribute, const TInt aValue ) =0;

    /**
     * SetUintAttribute is called to set the value of a TUint attribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The new value for the TUint attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt SetUintAttribute( const TUint aAttribute, const TUint aValue ) =0;

    /**
     * SetBoolAttribute is called to set the value of a TBool attribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The new value for the TBool attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt SetBoolAttribute( const TUint aAttribute, const TBool aValue ) =0;

    /**
     * SetStringAttribute is called to set the value of a TDes attribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The new value for the TDes attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt SetStringAttribute( const TUint aAttribute, const TDes& aValue ) =0;

    /**
     * SetPckgAttribute is called to set the value for a packaged attribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The new value for the packaged attribute.
     * @return KErrNone if successful, a system-wide error code if not.
     */
    virtual TInt SetPckgAttribute( const TUint aAttribute, const TDes8& aValue ) =0;
    };

/**
 * RConnMonPlugin
 * Plug-in subsession.
 * @lib CONNMONEXT.LIB
 */
class RConnMonPlugin : public RSubSessionBase
    {
public:
    /**
     * Opens a plug-in subsession to the Connection Monitor server. It must
     * be called before any of other methods during the subsession.
     * @param aSession Connection Monitor session (RConnectionMonitor) object.
     * @param aConnectionId Bearer specific general connection id that
     * identifies the plug-in. See TConnMonBearerId (rconnmon.h).
     * @return KErrNone if succesfull, a system-wide error code if not.
     */
    TInt Open( RConnectionMonitor &aSession, const TUint aConnectionId );

    /**
     * Closes plug-in subsession from the Connection Monitor server.
     * @return void.
     */
    void Close();

    /**
     * Returns the version information.
     * @return void.
     */
    TVersion Version() const;

    /**
     * Registers an attribute to the Connection monitor server.
     * @param aAttribute Attribute identifier.
     * @return KErrNone if succesfull, a system-wide error code if not.
     */
    TInt RegisterAttribute( const TUint aAttribute ) const;

    /**
     * Cancels the attribute registration from the Connection monitor server.
     * @param aAttribute Attribute identifier.
     * @return KErrNone if succesfull, a system-wide error code if not.
     */
    TInt CancelRegisterAttribute( const TUint aAttribute ) const;

    /**
     * Sends an event to the Connection Monitor server.
     * @param aEventId Event identifier.
     * @param aData Event data.
     * @param aSize Size of the event data. This must not exceed 512 bytes.
     * @return KErrNone if succesfull, a system-wide error code if not.
     */
    TInt SendEvent( const TUint aEventId, const TAny* aData, const TUint aSize ) const;

    /**
     * Gets the next attribute request from the Connection Monitor server.
     * @param aBuffer Query record in buffer.
     * @param aStatus On completion, KErrNone if successfull, a system-wide error code if not.
     * @return void.
     */
    void GetQuery( TDes8& aBuffer, TRequestStatus& aStatus ) const;

     /**
     * Cancels the attribute request from the Connection Monitor server.
     * @return void.
     */
    void CancelGetQuery() const;

    /**
     * Sends an attribute to the Connection Monitor server.
     * @param aType Type of the attribute (TInt, TUint, TBool, TDes, TDes8).
     * @param aAttribute Attribute identifier.
     * @param aRet The plug-in return code.
     * @param aValue The value of the attribute.
     * @param aSize Size of the attribute. This must not exceed 512 bytes.
     * @return KErrNone if succesfull, a system-wide error code if not.
     */
    TInt SendAttribute(
            const TInt aType,
            const TUint aAttribute,
            const TInt aRet,
            const TAny* aValue,
            const TUint aSize ) const;
    };

/**
 * CConnectionMonitorPlugin
 * @lib CONNMONEXT.LIB
 */
class CConnectionMonitorPlugin : public CBase, public MConnectionMonitorPluginInterface
    {
public:
    IMPORT_C CConnectionMonitorPlugin();
    IMPORT_C virtual ~CConnectionMonitorPlugin();

public:
    /**
     * Registers the plug-in to the Connection Monitor Server. It must be called
     * before any of other methods during the plug-in's session.
     * @param aConnectionId Bearer specific general connection id that
     * identifies the plug-in. See TConnMonBearerId (rconnmon.h).
     * @return void
     */
    IMPORT_C void RegisterPluginL( const TUint aConnectionId );

    /**
     * Unregisters the plug-in from the Connection Monitor server.
     * @return void
     */
    IMPORT_C void CancelRegisterPlugin();

    /**
     * Registers a new attribute to the Connection Monitor server.
     * @param aAttribute Attribute identifier.
     * @return void
     */
    IMPORT_C TInt RegisterAttribute( const TUint aAttribute ) const;

    /**
     * Cancels the attribute registration from the Connection Monitor server.
     * @param aAttribute Attribute identifier.
     * @return void
     */
    IMPORT_C void CancelRegisterAttribute( const TUint aAttribute ) const;

    /**
     * Sends an event to the Connection Monitor server.
     * @param aEventId Event identifier.
     * @param aData Event data.
     * @param aSize Size of the event data. This must not exceed 512 bytes.
     * @return void
     */
    IMPORT_C TInt Event( const TUint aEventId, const TAny* aData, const TUint aSize ) const;

    /**
     * Returns the version info.
     * @return Version information.
     */
    IMPORT_C TVersion Version() const;

private:
    RConnectionMonitor iConnMonServer;
    RConnMonPlugin iSession;
    CPluginAO* iPluginAO;
    };

/**
 * TReqInfo
 * @lib CONNMONEXT.LIB
 */
class TReqInfo
    {
public:
    inline void Reset(){ iType = 0; iAttribute = 0; iData = 0; }
public:
    TInt iType;
    TUint iAttribute;
    TInt iData;
    };

/**
 * CPluginAO
 * @lib CONNMONEXT.LIB
 */
NONSHARABLE_CLASS( CPluginAO ) : public CActive
    {
public:
    CPluginAO( RConnMonPlugin& aSession, CConnectionMonitorPlugin& aPlugin );
    void ConstructL();
    virtual ~CPluginAO();

public:
    void ReceiveQuery();

private:
    /**
     * From CActive Cancels the asyncronous request.
     * @return void
     */
    void DoCancel();

    /**
     * From CActive Handles the the request that has arrived from server.
     * @return void
     */
    void RunL();

private:
    RConnMonPlugin& iSession;
    CConnectionMonitorPlugin& iPlugin;

    // Event information
    TReqInfo iReqInfo;

    // Needed in inter-thread reads/writes
    TPtr8 iBuf;
    };

#endif //__CONNMONEXT_H

// End of file
