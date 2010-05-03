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
 * Wrapper for CM Manager Symbian interface.
 */

#ifndef CMDESTINATION_SHIM_H
#define CMDESTINATION_SHIM_H

// System includes

#include <QString>
#include <cmdestination.h>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations

class CmDestinationShim;
class CmDestinationShimPrivate;
class CmConnectionMethodShim;

// External data types

// Constants

// Class declaration

/*!
    \class CmDestinationShim
    \brief Implementation class for CM Manager Destination Shim.

    Wrapper for CM Manager Symbian interface.
    
    NOTE! This shim is supposed to be used only in middleware code. Usage
    must be agreed with ipconnmgmt package owner. Applications should use
    the Qt Bearer Management API instead.
*/

class CONNECTIONSETTINGSSHIMDLL_EXPORT CmDestinationShim
{

public:
    
    // Data types

    /*!
     *  Constructor.
     *  
     *  @param[in] destination CM Manager destination object.
     */
    CmDestinationShim(RCmDestination &destination);

    /*!
     *  Destructor.
     */
    ~CmDestinationShim();
    
    /*!
     * Return the number of connection methods in the destination.
     * 
     * @return Number of the connection methods in this destination.
     */                 
    int connectionMethodCount() const;
           
    /*!
     * Return a reference to the connection method. The index must be less
     * than the return value of connectionMethodCount().
     * 
     * @param[in] index Index of the connection method to be returned.
     * @return Connection method.
     */
    CmConnectionMethodShim *connectionMethod(int index) const;
           
    /*!
     * Returns the connection method with the given ID.
     * 
     * @param cmId Unique id of the requested connection method.
     * @return Connection method.
     */
    CmConnectionMethodShim *connectionMethodByID(uint cmId) const;

    // Getting attributes

    /*!
     * Return the priority of the passed connection method.
     * 
     * @param cmItem Connection method.
     * @return Priority of the queried connection method.
     */
    uint priority(CmConnectionMethodShim *cmItem) const;
           
    /*!
     * Return the name of the destination. 
     * 
     * @return Name of the destination, empty string in error cases.
     */      
    QString name() const;
           
    /*!
     * Return the destination's Id attribute.
     * 
     * @return Id of the destination.
     */
    uint id() const;

    /*!
     * Returns the Metadata.
     *
     * @param metadataField The meta data field to query.
     * @return The value of the requested field.
     */
    uint metadata(CMManagerShim::SnapMetadataField metadataField) const;

    /*!
     * Return the protection level of the destination.
     * 
     * @return Protection level.
     */
    CMManagerShim::CmmProtectionLevel protectionLevel() const;
           
    /*!
     * Return whether the destination is hidden or not.
     * 
     * @return True if the destination is hidden.
     */
    bool isHidden() const;
           
    /*!
     * Adds an existing connection method to a destination.
     * 
     * @param cm Connection method to be added.
     * @return Index in the Connection Method list.
     */
    int addConnectionMethod(CmConnectionMethodShim *cm);            
       
    /*!
     * Remove a connection method from a destination and delete it
     * on update. 
     * Exception: connection method is not deleted, if it's referenced 
     * from any other destination.
     * 
     * @param cm The connection method to be deleted.
     */
    void deleteConnectionMethod(CmConnectionMethodShim *cm);
       
    /*!
     * Remove connection method from the destination.
     * 
     * @param cm Connection method to be removed.
     */
    void removeConnectionMethod(CmConnectionMethodShim *cm);
             
    /*!
     * Set the connection method's priority based on the passed
     * index. Indexing starts from zero(the highest priority).
     * 
     * @param cm The connection method item
     * @param index The new priority of the connection method in the 
     *              destination.
     */
    void modifyPriority(CmConnectionMethodShim *cm, int index);
           
    /*!
     * Set the destination's name.
     * 
     * @param name New name of the destination.
     */
    void setName(QString name);
              
    /*!
     * Update all values of the destination and its connection methods to
     * CommsDat. Nothing is stored, if update leaves due to any reason.
     */
    void update();
    
    /*!
     * Reload the contents of the destination from database.
     * Any current changes made to the destination are lost.
     */
    void refresh();

    /*!
     * Delete destination and its connection methods from CommsDat.
     * Connection methods that belong also to some other destination are
     * not deleted.
     */
    void deleteDestination();

protected:

private:
    
private: // data
    
    CmDestinationShimPrivate *d_ptr;  //!< Private implementation

    // Friend classes
    
};

#endif /* CMDESTINATION_SHIM_H */
