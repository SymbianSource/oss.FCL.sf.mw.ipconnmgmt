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
* Description:  Declaration of class CFeatureManagerWrapper.
*
*/


#ifndef FEATURE_MANAGER_WRAPPER_H
#define FEATURE_MANAGER_WRAPPER_H

// INCLUDE FILES
#include    <e32base.h>        //CBase
#include    <featmgr.h>

// CLASS DECLARATION

/**
* CFeatureManagerWrapper wraps FeatureManager to eliminate 
* maintenance of existance of FeatureManager.
*/
NONSHARABLE_CLASS( CFeatureManagerWrapper ) :  public CBase
    {  
   
   public:

       /**
       * Destructor. Uninitalize FeatureManager.
       */
       virtual ~CFeatureManagerWrapper();
    
       /**
       * Static constructor.
       */
       static CFeatureManagerWrapper* NewL();        
        
   protected:

       /**
       * Default constructor.
       */
       CFeatureManagerWrapper();

       /**
       * Second-phase constructor. Initialize FeatureManager
       */
       void ConstructL();
   
   private:

       /**
       * Indicates Featuremanger is initialized or not.
       */
       TBool iIsFeatureMgrInitialized; 
    
    };

#endif

// End of file
