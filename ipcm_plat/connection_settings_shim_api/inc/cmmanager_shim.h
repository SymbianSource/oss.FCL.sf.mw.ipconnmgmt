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

#ifndef CMMANAGER_SHIM_H
#define CMMANAGER_SHIM_H

// System includes

#include <QString>
#include <cmmanager.h>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations

class CmDestinationShim;
class CmManagerShimPrivate;
class CmConnectionMethodShim;

// External data types

// Constants

// Class declaration

/*!
    \class CmManagerShim
    \brief Implementation class for CM Manager Shim.

    Wrapper for CM Manager Symbian interface.
    
    NOTE! This shim is supposed to be used only in middleware code. Usage
    must be agreed with ipconnmgmt package owner. Applications should use
    the Qt Bearer Management API instead.
*/

class CONNECTIONSETTINGSSHIMDLL_EXPORT CmManagerShim
{

public: 

    // Data types

    /*!
     *  Constructor.
     */
    CmManagerShim();

    /*!
     *  Destructor.
     */
    ~CmManagerShim();
    
    /*!
     * Creates a Destination with the passed name.
     * 
     * @param[in] name The name of the new destination.
     * @return New destination. Ownership is passed to the caller.
     */
    CmDestinationShim *createDestination(QString name);
    
    /*!
     * Creates a connection method that does not belong to any destination.
     * 
     * @param[in] bearerType Bearer type of the new connection method. Types
     *                       are defined in cmmanagerdefines_shim.h.
     * @return New connection method. Ownership is passed to the caller.
     */
    CmConnectionMethodShim *createConnectionMethod(uint bearerType);

    /*!
     * Returns the connection method queried by its ID.
     * 
     * @param[in] connectionMethodId Id of the connection method.
     * @return Found connection method instance.
     */
    CmConnectionMethodShim *connectionMethod(uint connectionMethodId) const;
    
    /*!
     * Returns the list of connection methods that do not belong to any 
     * destination.
     * 
     * @param[out] cmArray On return it is the array of connection method ids.
     * @param[in] legacyOnly When set to true only Legacy connection method
     *                       ids (connection methods which do not belong to
     *                       any destination) will be returned
     */
    void connectionMethod(
        QList<uint> &cmArray,
        bool legacyOnly = true) const;
    
    /*!
     * Returns the destination queried by its ID.
     * 
     * @param[in] destinationId Id of the destination.
     * @return Found network destination.
     */
    CmDestinationShim *destination(uint destinationId) const;
            
    /*!
     * Returns an array of all destination id's.
     * 
     * @param[out] destArray Array of all available network destinations.
     */ 
    void allDestinations(QList<uint> &destArray) const;
    
protected:

private:
    
private: // data
    
    CmManagerShimPrivate *d_ptr;  //!< Private implementation
};

#endif /* CMMANAGER_SHIM_H */
