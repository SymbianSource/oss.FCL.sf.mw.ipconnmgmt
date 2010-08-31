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

#ifndef CMMANAGER_SHIM_S60_P_H
#define CMMANAGER_SHIM_S60_P_H

// System includes

#include <QString>
#include <cmmanager.h>

// User includes

// Forward declarations

class CmConnectionMethodShim;
class CmDestinationShim;

// External data types

// Constants

// Class declaration

class CmManagerShimPrivate
{

public: 

    // Data types

    CmManagerShimPrivate();

    ~CmManagerShimPrivate();
    
    CmDestinationShim *CreateDestination(QString name);
    
    CmConnectionMethodShim *CreateConnectionMethod(uint bearerType);

    CmConnectionMethodShim* ConnectionMethod(uint connectionMethodId) const;
    
    void ConnectionMethod(
        QList<uint> &cmArray,
        bool legacyOnly) const;
    
    CmDestinationShim *Destination(uint destinationId) const;
            
    void AllDestinations(QList<uint> &destArray) const;
    
    QString GetUncategorizedIcon() const;
    
protected:

private:

    Q_DISABLE_COPY(CmManagerShimPrivate)

    void ConnectionMethodL(
        QList<uint> &cmArray,
        bool legacyOnly) const;
    
    void AllDestinationsL(QList<uint> &destArray) const;
    
    void GetUncategorizedIconL(QString &icon) const;

    void TraceIfError(TInt error) const;

private: // data
    
    RCmManager iCmManager;  //!< Cm Manager instance

    // Friend classes
};

#endif // CMMANAGER_SHIM_S60_P_H
