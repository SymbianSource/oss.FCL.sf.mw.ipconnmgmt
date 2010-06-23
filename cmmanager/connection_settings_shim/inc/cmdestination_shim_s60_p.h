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

#ifndef CMDESTINATION_SHIM_P_H
#define CMDESTINATION_SHIM_P_H

// System includes

#include <QString>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations

class RCmDestination;
class CmConnectionMethodShim;

// External data types

// Constants

// Class declaration

class CmDestinationShimPrivate
{

public:

    // Data types

    CmDestinationShimPrivate(RCmDestination &destination);

    ~CmDestinationShimPrivate();
    
    int ConnectionMethodCount() const;
           
    CmConnectionMethodShim *ConnectionMethod(int index) const;
           
    CmConnectionMethodShim *ConnectionMethodByID(uint cmId) const;

    uint Priority(CmConnectionMethodShim *cmItem) const;
           
    QString Name() const;
           
    uint Id() const;

    uint Metadata(CMManagerShim::SnapMetadataField metadataField) const;

    CMManagerShim::CmmProtectionLevel ProtectionLevel() const;
           
    bool IsHidden() const;
           
    int AddConnectionMethod(CmConnectionMethodShim *cm);            
       
    void DeleteConnectionMethod(CmConnectionMethodShim *cm);
       
    void RemoveConnectionMethod(CmConnectionMethodShim *cm);
    
    void ModifyPriority(CmConnectionMethodShim *cm, int index);
    
    void SetName(QString name);
    
    void Update();               
    
    void Refresh();
    
    void DeleteDestination();
    
    QString GetIcon() const;
    
    void SetIcon(QString icon);
    
protected:

private:
                 
    Q_DISABLE_COPY(CmDestinationShimPrivate)

    void PriorityL(CmConnectionMethodShim *cmShim, uint &priority) const;
    
    void NameL(QString &name) const;
           
    void AddConnectionMethodL(CmConnectionMethodShim *cm, int &index);            
       
    void DeleteConnectionMethodL(CmConnectionMethodShim *cm);
       
    void RemoveConnectionMethodL(CmConnectionMethodShim *cm);
             
    void ModifyPriorityL(CmConnectionMethodShim *cm, int index);
           
    void SetNameL(QString name);
    
    void GetIconL(QString &icon) const;
    
    void SetIconL(QString icon);
              
    CMManagerShim::CmmProtectionLevel mapCmmProtectionLevel( 
        CMManager::TProtectionLevel protectionlvl) const;
    
    void TraceIfError(TInt error) const;
    
private: // data
        
    RCmDestination iDestination;  //!< CmManager Destination instance
    
    // Friend classes
    
};

#endif // CMDESTINATION_SHIM_P_H
